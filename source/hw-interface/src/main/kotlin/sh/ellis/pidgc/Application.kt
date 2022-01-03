package sh.ellis.pidgc

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import sh.ellis.pidgc.canbus.CanbusManager
import sh.ellis.pidgc.comms.CommsClient
import sh.ellis.pidgc.serial.Serial
import sh.ellis.pidgc.utils.isWindows

fun main() {
    println("Starting hardware interface for PiDGC...")

    Thread(Serial).start()

    if (!isWindows()) {
        Thread(CanbusManager()).start()
    }

    runBlocking { CommsClient.run() }

    println("Hardware interface running!")
}