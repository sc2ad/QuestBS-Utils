& "$PSScriptRoot/build.ps1"

adb shell am force-stop com.beatgames.beatsaber

adb push libs/arm64-v8a/libbs-utils.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libbs-utils.so
