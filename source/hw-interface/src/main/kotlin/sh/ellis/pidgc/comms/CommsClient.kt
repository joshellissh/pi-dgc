package sh.ellis.pidgc.comms

import com.fasterxml.jackson.module.kotlin.jacksonObjectMapper
import io.ktor.network.selector.*
import io.ktor.network.sockets.*
import io.ktor.utils.io.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import sh.ellis.pidgc.serial.Serial
import sh.ellis.pidgc.state.State
import java.net.ConnectException
import java.net.InetSocketAddress

object CommsClient {

    private val mapper = jacksonObjectMapper()

    suspend fun run() {
        while (true) {
            var input: ByteReadChannel
            val output: ByteWriteChannel

            try {
                // Connect to display server
                val socket = aSocket(ActorSelectorManager(Dispatchers.IO))
                    .tcp()
                    .connect(InetSocketAddress("127.0.0.1", 4025))

                input = socket.openReadChannel()
                output = socket.openWriteChannel(autoFlush = true)

                println("Connected to display server.")
            } catch (e: ConnectException) {
                println("Unable to connect to display server, retrying in 500ms...")
                delay(500)
                continue
            }

            while (true) {
                try {
                    output.writeStringUtf8(mapper.writeValueAsString(State))

                    if (input.availableForRead > 0) {
                        input.readUTF8Line()?.let {
                            if (it.contains(':')) {
                                // Key:Value pair
                                val parts = it.split(":").toMutableList()

                                when (parts[0]) {
                                    "WRITE_CONFIG" -> {
                                        val partValues = parts[1].trim('\n').split(",").toMutableList()

                                        val newPPM = partValues[0].toInt()
                                        val blinkerSound = partValues[1].toBoolean()
                                        val chimeSound = partValues[2].toBoolean()
                                        val screenDimming = partValues[3].toInt()

                                        println("Setting PPM to $newPPM")
                                        println("Setting Blinker Sound to $blinkerSound")
                                        println("Setting Chime Sound to $chimeSound")
                                        println("Setting Screen Dimming to $screenDimming")

                                        State.blinkerSound = blinkerSound
                                        State.chimeSound = chimeSound
                                        State.screenDimming = screenDimming
                                        State.vssPulsesPerMile = newPPM

                                        Serial.writeConfig(parts[1].trim('\n'))
                                    }
                                    "RESET_TRIP" -> {
                                        State.tripOdometer = 0.0
                                        Serial.writeOdometers(State.tripOdometer, State.odometer)
                                        println("Sent trip reset to Serial.")
                                    }
                                    else -> println("Unknown socket command `$it`")
                                }
                            }
                        }
                    }

                    delay(15)
                } catch (e: Exception) {
                    println("Lost connection to display server. Reconnecting...")
                    break
                }
            }
        }
    }
}