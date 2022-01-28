package sh.ellis.pidgc.serial

import com.fazecast.jSerialComm.SerialPort
import com.fazecast.jSerialComm.SerialPortEvent
import com.fazecast.jSerialComm.SerialPortMessageListener
import sh.ellis.pidgc.state.State
import sh.ellis.pidgc.utils.equalsToTenths
import sh.ellis.pidgc.utils.toBoolean
import java.time.Instant


// Manages communication with external board via serial port
object Serial : Runnable {

    private const val NO_PULSE = 99999L

    private var comPort: SerialPort? = null
    private var lastPacket = Instant.MIN
    private var odoReceived: Int = 0
    private var configReceived: Int = 0

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

                State.serialConnected = false

                // Attempt to close port first, just in case
                try {
                    comPort?.removeDataListener()
                    comPort?.closePort()
                } catch (e: Exception) {}

                try {
                    val comPorts = SerialPort.getCommPorts()
                    val comPortNames = comPorts.map { it.systemPortName }.toList()

                    println("Found COM ports: $comPortNames")
                    println("Opening port ${comPortNames[0]}...")

                    comPort = comPorts[0]

                    if (comPort?.openPort() == false) {
                        println("Failed to open COM port. Retrying in 2 seconds...")
                        Thread.sleep(2000)
                        continue
                    }

                    comPort?.baudRate = 115200
                    comPort?.addDataListener(MessageListener())

                    println("Serial port successfully opened.")

                    // Request odometer values & VSS PPM
                    println("Requested odometer values.")
                    sendToSerial("so")
                    println("Requested config.")
                    sendToSerial("sconfig")
                } catch (e: Exception) {
                    println("Unable to find serial port.")
                }
            }

            Thread.sleep(2000)
        }
    }

    private fun processPacket(packet: String) {
        State.serialConnected = true

        val parts = packet.split(":").toMutableList()

        when(parts[0]) {
            "batt" -> State.battery = parts[1].toDouble()
            "fuel" -> State.fuel = parts[1].toDouble()
            "glite" -> State.gaugeLights = parts[1].toInt().toBoolean()
            "hi" -> State.highBeam = parts[1].toInt().toBoolean()
            "left" -> State.left = parts[1].toInt().toBoolean()
            "lo" -> State.lowBeam = parts[1].toInt().toBoolean()
            "log" -> State.addLogMessage(parts[1])
            "mil" -> State.mil = parts[1].toInt().toBoolean()
            "odo" -> setOdometerState(parts[1])
            "config" -> setConfig(parts[1].split(',').toMutableList())
            "pulses" -> handlePulses(parts[1])
            "rev" -> State.reverse = parts[1].toInt().toBoolean()
            "right" -> State.right = parts[1].toInt().toBoolean()
        }
    }

    private fun setConfig(values: List<String>) {
        println("Received config values $values from Serial.")

        if (values.size != 4) {
            println("Invalid config size of ${values.size}")
            return
        }

        val ppm = values[0].toInt()
        val blinkerSound = values[1].toInt().toBoolean()
        val chimeSound = values[2].toInt().toBoolean()
        val screenDimming = values[3].toInt()

        if (ppm == -1) {
            println("Could not read PPM from EEPROM. Defaulting to 8000.")
            State.vssPulsesPerMile = 8000
        } else if (ppm != State.vssPulsesPerMile) {
            println("Setting vssPulsesPerMile to $ppm")
            State.vssPulsesPerMile = ppm
        }

        println("Setting blinkerSound to $blinkerSound")
        State.blinkerSound = blinkerSound

        println("Setting chimeSound to $chimeSound")
        State.chimeSound = chimeSound

        if (screenDimming < 0 || screenDimming > 100) {
            println("Invalid value for screenDimming, defaulting to 20.")
            State.screenDimming = 20
        } else {
            println("Setting screenDimming to $screenDimming")
            State.screenDimming = screenDimming
        }

        configReceived = -1
    }

    private fun setOdometerState(values: String) {
        val parts = values.split(",")

        if (parts.size != 2)
            println("Odometer value from uC is not two parts")

        odoReceived = -1

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

    fun writeOdometers(tripOdometer: Double, odometer: Double) {
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

    fun writeConfig(configString: String) {
        sendToSerial("write_config:$configString")
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

        // Don't write odometers if they report as 0
        if (State.tripOdometer > 0.0 && State.odometer > 0.0)
            writeOdometers(State.tripOdometer, State.odometer)

        if (pulseSeparationMicros != NO_PULSE && pulseSeparationMicros > 0L) {
            // Calculate MPH
            val oneMphInMicros = 3_600_000_000.0 / State.vssPulsesPerMile.toDouble()
            val mph = oneMphInMicros / pulseSeparationMicros.toDouble()
            State.mph.add(mph)
        } else {
            State.mph.add(0.0)
        }

        if (configReceived != -1)
            configReceived++

        if (odoReceived != -1)
            odoReceived++

        if (configReceived >= 10) {
            sendToSerial("sppm")
            configReceived = 0
        }

        if (odoReceived >= 10) {
            sendToSerial("so")
            odoReceived = 0
        }
    }
}