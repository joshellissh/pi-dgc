package sh.ellis.pidgc.comms

import com.fasterxml.jackson.module.kotlin.jacksonObjectMapper
import io.ktor.network.selector.*
import io.ktor.network.sockets.*
import io.ktor.utils.io.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import sh.ellis.pidgc.state.State
import java.lang.Thread.sleep
import java.net.ConnectException
import java.net.InetSocketAddress

class CommsClient : Runnable {

    private val mapper = jacksonObjectMapper()

    override fun run() {
        while (true) {
            var input: ByteReadChannel
            val output: ByteWriteChannel

            try {
                // Connect to display server
                runBlocking {
                    val socket = aSocket(ActorSelectorManager(Dispatchers.IO))
                        .tcp()
                        .connect(InetSocketAddress("127.0.0.1", 4025))

                    input = socket.openReadChannel()
                    output = socket.openWriteChannel(autoFlush = true)

                    println("Connected to display server.")
                }
            } catch (e: ConnectException) {
                println("Unable to connect to display server, retrying in 500ms...")
                Thread.sleep(500)
                continue
            }

            while (true) {
                try {
                    runBlocking {
                        output.writeStringUtf8(mapper.writeValueAsString(State))

                        val inputLine = input.readUTF8Line(2048)

                        when (inputLine) {
                            "OK" -> {}
                            "RESET_TRIP" -> State.tripOdometer = 50.0
                            else -> println("Unable to confirm receipt of packet.")
                        }

                        sleep(15)
                    }
                } catch (e: Exception) {
                    println("Lost connection to display server. Reconnecting...")
                    break
                }
            }
        }
    }
}