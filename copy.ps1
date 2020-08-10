& "$PSScriptRoot/build.ps1"

adb shell am force-stop com.beatgames.beatsaber

adb push libs/arm64-v8a/libbs-utils.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libbs-utils_0_2_0.so
cp obj/local/arm64-v8a/libbs-utils.so C:\Users\michaelzoller\source\repos\Quest-Trick-Saber\extern\libbs-utils_0_2_0.so
