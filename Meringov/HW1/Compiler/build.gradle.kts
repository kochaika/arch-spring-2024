plugins {
    kotlin("jvm") version "1.9.21"
    antlr
    application
}

group = "org.example"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation("org.jetbrains.kotlin:kotlin-test")
    antlr("org.antlr:antlr4:4.13.1")
}

tasks.test {
    useJUnitPlatform()
}
kotlin {
    jvmToolchain(17)
}
//
//tasks.compileKotlin {
//    dependsOn(tasks.generateGrammarSource)
//}

sourceSets {
    main {
        java {
            srcDir(tasks.generateGrammarSource)
        }
    }
}
tasks.generateGrammarSource {
    // your configuration
    arguments.add("-visitor")
    outputDirectory = file("${project.projectDir}/build/generated/sources/main/java/antlr")
}
application {
    mainClass.set("org.example.MainKt")
}