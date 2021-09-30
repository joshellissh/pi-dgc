package sh.ellis.pidgc.serial

import com.fazecast.jSerialComm.SerialPort
import com.fazecast.jSerialComm.SerialPortEvent
import com.fazecast.jSerialComm.SerialPortMessageListener
import sh.ellis.pidgc.config.Config
import sh.ellis.pidgc.state.State
import sh.ellis.pidgc.utils.equalsToTenths
import sh.ellis.pidgc.utils.isWindows
import sh.ellis.pidgc.utils.toBoolean
import java.time.Instant


// Manages communication with external board via serial port
object Serial : Runnable {

    private const val NO_PULSE = 99999L

    private var comPort: SerialPort? = null
    private var lastPacket = Instant.MIN

    private class MessageListener : SerialPortMessageListener {
        override fun getListeningEvents(): Int {
            return SerialPort.LISTENING_EVENT_DATA_RECEIVED
        }

        override fun getMessageDelimiter(): ByteArray {
            return byteArrayOf(0x0A)
        }

        override fun delimiterIndicatesEndOfMessage(): Boolean {
            return true
        }

        override fun serialEvent(event: SerialPortEvent) {
            lastPacket = Instant.now()

            val kvString = event.receivedData.map {
                it.toInt().toChar()
            }
                .joinToString("")
                .replace("\n", "")
                .replace("\r", "")

            processPacket(kvString)
        }
    }

    override fun run() {
        // Loop forever
        while(true) {
            if (Instant.now().minusMillis(2500) > lastPacket) {
                State.addLogMessage("Serial port seems to be stalled or closed. Reopening.")

                // Attempt to close port first, just in case
                try {
                    comPort?.removeDataListener()
                    comPort?.closePort()
                } catch (e: Exception) {}

                val portString = if (isWindows()) "COM6" else "/dev/ttyACM0"
                comPort = SerialPort.getCommPort(portString)
                comPort?.openPort()
                comPort?.setRTS()
                Thread.sleep(1000)
                comPort?.clearRTS()
                comPort?.baudRate = 115200
                comPort?.addDataListener(MessageListener())

                // Request odometer values & VSS PPM
                sendToSerial("so")
                sendToSerial("sppm")

                State.addLogMessage("Serial port successfully opened.")
            }

            Thread.sleep(2000)
        }
    }

    private fun processPacket(packet: String) {
        val parts = packet.split(":").toMutableList()

        when(parts[0]) {
            "batt" -> State.battery = parts[1].toDouble()
            "fuel" -> State.fuel = parts[1].toDouble()
            "hi" -> State.highBeam = parts[1].toInt().toBoolean()
            "left" -> State.left = parts[1].toInt().toBoolean()
            "lo" -> State.lowBeam = parts[1].toInt().toBoolean()
            "log" -> State.addLogMessage(parts[1])
            "odo" -> setOdometerState(parts[1])
            "ppm" -> setVssPpm(parts[1].toInt())
            "pulses" -> handlePulses(parts[1])
            "rev" -> State.reverse = parts[1].toInt().toBoolean()
            "right" -> State.right = parts[1].toInt().toBoolean()
        }
    }

    private fun setVssPpm(ppm: Int) {
        if (ppm == -1) {
            println("Could not read PPM from EEPROM. Defaulting to 8000.")
            State.vssPulsesPerMile = 8000
            return
        }

        println("Initializing vssPulsesPerMile to $ppm")
        State.vssPulsesPerMile = ppm
    }

    private fun setOdometerState(values: String) {
        val parts = values.split(",")

        if (parts.size != 2)
            println("Odometer value from uC is not two parts")

        State.tripOdometer = parts[0].toDouble()
        State.lastSavedTripOdometer = parts[0].toDouble()

        State.odometer = parts[1].toDouble()
        State.lastSavedOdometer = parts[1].toDouble()

        println("Initializing trip odo to ${State.tripOdometer} and odo to ${State.odometer}")
    }

    private fun sendToSerial(message: String) {
        var sendString = message

        if (!sendString.endsWith('\n'))
            sendString += '\n'

        comPort?.writeBytes(sendString.toByteArray(), sendString.length.toLong())
    }

    private fun writeOdometers(tripOdometer: Double, odometer: Double) {
        // Only save to EEPROM if either of our tenths values have changed
        if (State.lastSavedOdometer.equalsToTenths(odometer) &&
            State.lastSavedTripOdometer.equalsToTenths(tripOdometer)) {
            return
        }

        val command = String.format("wo:%.1f,%.1f", tripOdometer, odometer)
        sendToSerial(command)

        State.lastSavedOdometer = odometer
        State.lastSavedTripOdometer = tripOdometer
    }

    fun writePpm(ppm: Int) {
        sendToSerial("wppm:$ppm")
    }

    private fun handlePulses(value: String) {
        val parts = value.split(",")

        // Update speedometer & odometer
        val numPulses = parts[0].toLong()
        val pulseSeparationMicros = parts[1].toLong()

        State.totalPulses += numPulses

        // Pulses counted / pulses per mile = distance travelled.
        val distance = numPulses / State.vssPulsesPerMile.toDouble()

        State.odometer += distance
        State.tripOdometer += distance

        writeOdometers(State.tripOdometer, State.odometer)

        if (pulseSeparationMicros != NO_PULSE && pulseSeparationMicros > 0L) {
            // Calculate MPH
            val oneMphInMicros = 3_600_000_000.0 / State.vssPulsesPerMile.toDouble()
            val mph = oneMphInMicros / pulseSeparationMicros.toDouble()
            State.mph.add(mph)
        } else {
            State.mph.add(0.0)
        }
    }
}