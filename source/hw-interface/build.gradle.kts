import org.jetbrains.kotlin.gradle.tasks.KotlinCompile
import com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar

plugins {
    java
    kotlin("jvm") version "1.5.31"
    application
    id("com.github.johnrengelman.shadow") version "7.0.0"
}

group = "sh.ellis"
//version = "1.0"

application {
    mainClass.set("sh.ellis.pidgc.ApplicationKt")
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("org.jetbrains.kotlin:kotlin-stdlib:1.5.21")
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk8:1.5.21")

    implementation("io.ktor:ktor-network:1.6.3")
    implementation("tel.schich:javacan:2.3.0")
    implementation("org.ini4j:ini4j:0.5.4")
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk8")
    implementation("com.fazecast:jSerialComm:2.7.0")
    implementation("com.fasterxml.jackson.module:jackson-module-kotlin:2.13.+")
    implementation("ch.qos.logback:logback-classic:1.2.5")
}

tasks.withType<KotlinCompile>() {
    kotlinOptions.jvmTarget = "11"
}

tasks {
    named<ShadowJar>("shadowJar") {
        archiveBaseName.set("hw-interface")
        mergeServiceFiles()
        manifest {
            attributes(mapOf("Main-Class" to "sh.ellis.pidgc.ApplicationKt"))
        }
    }
}

tasks {
    build {
        dependsOn(shadowJar)
    }
}