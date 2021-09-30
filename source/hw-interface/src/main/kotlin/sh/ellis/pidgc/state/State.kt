package sh.ellis.pidgc.state

import com.fasterxml.jackson.annotation.JsonIgnore
import sh.ellis.pidgc.config.Config
import sh.ellis.pidgc.model.Mph

object State {
    @set:Synchronized @get:Synchronized
    var battery: Double = 0.0

    @JsonIgnore
    @set:Synchronized @get:Synchronized
    var barometricPressure: Double = 0.0

    @set:Synchronized @get:Synchronized
    var boost: Double = 0.0

    @set:Synchronized @get:Synchronized
    var coolant: Int = 0

    @set:Synchronized @get:Synchronized
    var fuel: Double = 0.0

    @set:Synchronized @get:Synchronized
    var highBeam: Boolean = false

    @set:Synchronized @get:Synchronized
    var left: Boolean = false

    @set:Synchronized @get:Synchronized
    var lowBeam: Boolean = false

    @set:Synchronized @get:Synchronized
    var mil: Boolean = false

    val mph: Mph<Double> = Mph(Config.speedodometerSmoothing)

    @set:Synchronized @get:Synchronized
    var oilPressure: Double = 0.0

    @JsonIgnore
    @set:Synchronized @get:Synchronized
    var lastSavedOdometer: Double = 0.0

    @set:Synchronized @get:Synchronized
    var odometer: Double = 0.0

    @JsonIgnore
    @set:Synchronized @get:Synchronized
    var vssPulsesPerMile: Int = 0

    @set:Synchronized @get:Synchronized
    var reverse: Boolean = false

    @set:Synchronized @get:Synchronized
    var right: Boolean = false

    @set:Synchronized @get:Synchronized
    var rpm: Int = 0

    @JsonIgnore
    @set:Synchronized @get:Synchronized
    var lastSavedTripOdometer: Double = 0.0

    @JsonIgnore
    @set:Synchronized @get:Synchronized
    var totalPulses: Long = 0L

    @set:Synchronized @get:Synchronized
    var tripOdometer: Double = 0.0

    private val logs = mutableListOf<String>()

    fun getLogMessages(): List<String> {
        synchronized(logs) {
            val logMessages = logs.toMutableList()
            logs.clear()
            return logMessages
        }
    }

    fun addLogMessage(logMessage: String) {
        println(logMessage)

        synchronized(logs) {
            logs.add(logMessage)
        }
    }
}