# Immersive mobile AR - project

## Setup

Download ARToolKit5-bin-5.3.2-Android.zip from:

https://artoolkit.org/download-artoolkit-sdk

Install Android Studio:

https://developer.android.com/studio/index.html

Install LLDB, CMake and NDK within Android studio
from Tools > Android > SDK Manager > tab: SDK Tools

Place OpenSourceGraph compiled binaries (include and lib files) in build/armeabi/

Export a standalone android toolchain with the
`make_standalone_toolchain` tool provided in the
NDK. The tool can be found at
`Android/Sdk/ndk-bundle/build/tools/make_standalone_toolchain.py`.

```Bash
./make_standalone_toolchain.py --arch arm --api 15 --install-dir <directory to install toolchain to>
```

Make sure the `<exported toolchain>/bin` directory
can be found on path before attempting to build for
android.

Example of toolchain placement:
If you want to place your toolchain in: $HOME/immersivetoolchain/arm-api15
you should run:

```Bash
$HOME/Android/Sdk/ndk-bundle/build/tools/make_standalone_toolchain.py --arch arm --api 15 --install-dir $HOME/immersivetoolchain/arm-api15
```

Now you can add this line to your $HOME/.bashrc:

```Bash
export PATH=$PATH:$HOME/immersivetoolchain/arm-api15/bin
```

Setting build environment
Run
```Bash
./gen-setenv.sh
```
This generates the setenv.sh file. Open the file and replace CHANGEME with the toolchain dir you specified earlier.

Now you can try running (gives you instructions)
```Bash
make copy-armeabi
```

When you have succesfully compiled the project through make, you can build and run it on Android Studio.

## LICENSE
This software is licensed under the MIT License

## TODO:
* This README
