package sh.ellis.pidgc.canbus

import sh.ellis.pidgc.state.State
import tel.schich.javacan.*
import java.nio.ByteBuffer
import java.time.Duration

// Manages all interactions with OBD2 via canbus
class CanbusManager: Runnable {

    @ExperimentalUnsignedTypes
    override fun run() {
        CanChannels.newRawChannel().use {
            it.bind(NetworkDevice.lookup("can0"))
            it.configureBlocking(true)
            it.setOption(CanSocketOptions.SO_RCVTIMEO, Duration.ofMillis(2))

            var lastHiRes: Long = System.currentTimeMillis()
            var lastLowRes = lastHiRes

            while(true) {
                val currentTime: Long = System.currentTimeMillis()

                if (currentTime - lastHiRes >= 50) {
                    // Request engine RPM
                     it.write(makeCanRequest(0x0C))
                     readResponse(it)

                    lastHiRes = currentTime
                }

                if (currentTime - lastLowRes >= 500) {
                    // Request coolant temp
                    it.write(makeCanRequest(0x05))
                    readResponse(it)

                    // Request Barometric Pressure
                    it.write(makeCanRequest(0x33))
                    readResponse(it)

                    // Request MAP
                    it.write(makeCanRequest(0x0B))
                    readResponse(it)

                    lastLowRes = currentTime
                }

                // Hopefully catch any timed out responses
                readResponse(it)
            }
        }
    }

    @ExperimentalUnsignedTypes
    private fun readResponse(canChannel : RawCanChannel) {
        try {
            processResponse(canChannel.read())
        } catch (e: Exception) {
            // This is fine, the read timeout is just being hit.
        }
    }

    @ExperimentalUnsignedTypes
    private fun processResponse(frame: CanFrame) {
        // Store frame data
        val data: ByteBuffer = ByteBuffer.allocate(frame.dataLength)
        frame.getData(data)

        // Get message length
        val length: Int = data[0].toInt()

        // Handle mode 41
        if (frame.id == 0x7E8 && data[1].toInt() == 0x41) {
            // Handle response PID
            when (data[2].toInt()) {

                // RPM
                0x0C -> {
                    if (length != 4) {
                        println("RPM message length != 4")
                        return
                    }

                    State.rpm = ((256 * data[3].toUByte().toInt()) + data[4].toUByte().toInt()) / 4
                }

                // Coolant
                0x05 -> {
                    if (length != 3) {
                        println("Coolant message length != 3")
                        return
                    }

                    State.coolant = (((data[3].toUByte().toInt().toDouble() - 40.0) * 1.8) + 32.0).toInt()
                }

                // MAP
                0x0B -> {
                    if (length != 3) {
                        println("MAP message length != 3")
                        return
                    }

                    State.boost = (data[3].toUByte().toInt().toDouble() * 0.145038) - State.barometricPressure;
                }

                // Barometric pressure
                0x33 -> {
                    State.barometricPressure = data[3].toUByte().toInt().toDouble() * 0.145038
                }
            }
        } else if (frame.id == 0x18FEEF00) {
            // Engine Fluid Level/Pressure 1

            // Oil pressure
            State.oilPressure = data[3].toUByte().toDouble() * 4.0 * 0.145038
        }
    }

    private fun makeCanRequest(activeByte: Byte): CanFrame {
        return CanFrame.createRaw(
            0x7E0,
            0,
            byteArrayOf(
                0x02,
                0x01,
                activeByte
            ),
            0,
            3
        )
    }

    @ExperimentalUnsignedTypes
    val UByte.msb get() = (this.toInt() shr 4 and 0b1111).toUByte()
    @ExperimentalUnsignedTypes
    val UByte.lsb get() = (this.toInt() and 0b1111).toUByte()
}