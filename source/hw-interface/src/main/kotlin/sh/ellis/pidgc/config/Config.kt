package sh.ellis.pidgc.config

import org.ini4j.Wini
import java.io.File

object Config {
    private val ini = Wini(File("./config.ini"))

    var batteryMin: Double = 12.0
    var batteryMax: Double = 15.0
    var speedodometerSmoothing: Int = 10
    var tempCompensation = -5.0

    init {
        batteryMin = ini.get("battery", "min_voltage", Double::class.javaPrimitiveType)
        batteryMax = ini.get("battery", "max_voltage", Double::class.javaPrimitiveType)
        speedodometerSmoothing = ini.get("speedometer", "smoothing", Int::class.javaPrimitiveType)
        tempCompensation = ini.get("BMP180", "tempCompensation", Double::class.javaPrimitiveType)
    }
}