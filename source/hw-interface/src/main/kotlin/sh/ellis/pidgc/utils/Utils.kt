package sh.ellis.pidgc.utils

fun isWindows(): Boolean {
    val os = System.getProperty("os.name").toLowerCase()

    return when {
        os.contains("win") -> true
        else -> false
    }
}

// This has got to be the most inefficient way of doing this :)
fun Double.equalsToTenths(other: Double): Boolean {
    val thisStr = String.format("%.1f", this)
    val otherStr = String.format("%.1f", other)

    return thisStr == otherStr
}

fun Int.toBoolean() = this == 1