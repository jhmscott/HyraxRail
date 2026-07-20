plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
    alias(libs.plugins.kotlin.compose)
    id("org.qtproject.qt.gradleplugin") version "1.+"
}

android {
    namespace = "ca.justinlab.hyraxrail"
    compileSdk {
        version = release(36)
    }

    defaultConfig {
        applicationId = "ca.justinlab.hyraxrail"
        minSdk = 28
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    kotlinOptions {
        jvmTarget = "11"
    }
    buildFeatures {
        compose = true
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.compose.ui)
    implementation(libs.androidx.compose.ui.graphics)
    implementation(libs.androidx.compose.ui.tooling.preview)
    implementation(libs.androidx.compose.material3)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.compose.ui.test.junit4)
    debugImplementation(libs.androidx.compose.ui.tooling)
    debugImplementation(libs.androidx.compose.ui.test.manifest)
}
tasks.named("QtBuildTask") {
    dependsOn("checkDebugAarMetadata")
    dependsOn("generateDebugResValues")
    dependsOn("processDebugNavigationResources")
    dependsOn("mergeDebugNativeDebugMetadata")
    dependsOn("compileDebugNavigationResources")
    dependsOn("mapDebugSourceSetPaths")
    dependsOn("generateDebugResources")
    dependsOn("extractDeepLinksDebug")
    dependsOn("javaPreCompileDebug")
    dependsOn("mergeDebugShaders")
    dependsOn("checkDebugDuplicateClasses")
    dependsOn("desugarDebugFileDependencies")
    dependsOn("mergeDebugJniLibFolders")
    dependsOn("validateSigningDebug")
    dependsOn("writeDebugAppMetadata")
    dependsOn("writeDebugSigningConfigVersions")
    dependsOn("mergeDebugResources")
    dependsOn("packageDebugResources")
    dependsOn("createDebugCompatibleScreenManifests")
    dependsOn("processDebugMainManifest")
    dependsOn("mergeDebugAssets")
    dependsOn("mergeExtDexDebug")
    dependsOn("mergeDebugNativeLibs")
    dependsOn("processDebugResources")
    dependsOn("compressDebugAssets")
    dependsOn("mergeLibDexDebug")
    dependsOn("parseDebugLocalResources")
    dependsOn("processDebugManifest")
    dependsOn("stripDebugDebugSymbols")

}