// Top-level build file where you can add configuration options common to all sub-projects/modules.
buildscript {
    repositories {
        maven {
            url = uri("https://developer.hihonor.com/repo")
        }
    }
    dependencies {
        classpath("com.hihonor.mcs:magic-android:34.38.1.200")
    }
}


plugins {
    id("com.android.application") version "8.2.1" apply false
}