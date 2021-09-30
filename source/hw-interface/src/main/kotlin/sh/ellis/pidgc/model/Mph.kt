package sh.ellis.pidgc.model

import com.fasterxml.jackson.annotation.JsonValue

class Mph<T>(private val limit: Int) {

    private var container = mutableListOf<T>()

    fun add(element: T) {
        synchronized(this) {
            if (container.size >= limit) {
                container = shiftLeft(container, 1)
                container[container.size - 1] = element
            } else {
                container.add(element)
            }
        }
    }

    fun getAll(): MutableList<T> {
        synchronized(this) {
            return container.toMutableList()
        }
    }

    @JsonValue
    fun getAverage(): Double {
        synchronized(this) {
            if (container.size == 0)
                return 0.0

            var average = 0.0

            container.forEach {
                average += it as Double
            }

            return average / container.size.toDouble()
        }
    }

    fun clear() {
        synchronized(this) {
            container.clear()
        }
    }

    private fun shiftLeft(list: List<T>, count: Int): MutableList<T> {
        val newList = list.toMutableList()
        var shift = count

        if (shift > list.size) {
            shift %= list.size
        }

        list.forEachIndexed { index, value ->
            val newIndex = (index + (list.size - shift)) % list.size
            newList[newIndex] = value
        }

        return newList
    }
}