# customize this script to match locations where your android crosscompilation toolchains are located
# this script should be sourced before using the Makefile

# The directory where all following TOOLDIR's are assumed to be rooted in
export TOOLCHAIN_HOME=$HOME/code/crosscc/android/

# The following variables are subdirectory names for each toolchain
# if you don't have one of these, just comment the line out
export TOOLDIR_ARM=arm-api15
export TOOLDIR_ARMv7=arm-api15
export TOOLDIR_ARM64=arm64-api21
export TOOLDIR_i686=x86-api15
export TOOLDIR_x86_64=x86_64-api21

echo 'TOOLCHAIN_HOME and TOOLDIR_* env variables loaded'

if [ ! -z "$SENV_PATH" ]; then
	echo 'Adding tools to PATH...'

	[ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARM" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARM/bin"
	[ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARMv7" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARMv7/bin"
	[ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARM64" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARM64/bin"
	[ -d "$TOOLCHAIN_HOME/$TOOLDIR_i686" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_i686/bin"
	[ -d "$TOOLCHAIN_HOME/$TOOLDIR_x86_64" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_x86_64/bin"
else
	echo "Not setting PATH, use 'SENV_PATH=yes source setenv.sh' to automatically set PATH aswell"
fi
