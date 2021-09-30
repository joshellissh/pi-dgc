package sh.ellis.pidgc

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

    Thread(CommsClient()).start()

    println("Hardware interface running!")
}