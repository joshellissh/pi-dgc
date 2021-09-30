package sh.ellis.pidgc.model

// Data class used to transmit JSON via websocket to frontend
data class StatusMessage(
    val mph: Double,            // Serial
    val rpm: Int,               // OBD2
    val boost: Double,          // OBD2
    val coolant: Int,           // OBD2
    val fuel: Double,           // Serial
    val mil: Boolean,           // OBD2
    val oilPressure: Double,    // OBD2
    val lowBeam: Boolean,       // Serial
    val highBeam: Boolean,      // Serial
    val left: Boolean,          // Serial
    val reverse: Boolean,       // Serial
    val right: Boolean,         // Serial
    val voltage: Double,        // Serial
    val odometer: Double,       // Serial + Config
    val temperature: Double,    // Serial + Config
    val totalPulses: Long,      // Serial
    val tripOdometer: Double    // Serial + Config
)