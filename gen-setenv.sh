FILENAME=setenv.sh

if [ -f "$FILENAME" ]; then
	echo 'file "$FILENAME" exists already and will be overwritten!'
	echo "Continue? (y/N)"

	read yesno || exit 127

	if [ "$yesno" != "y" ]; then
		exit 0
	fi
fi

echo "# Script file generated by gen-setenv.sh" > $FILENAME
echo >> $FILENAME
echo "# Customize this script to match locations where your android crosscompilation toolchains are located." >> $FILENAME
echo "# This script should be sourced before using the Makefile." >> $FILENAME
echo >> $FILENAME
echo "# The directory where all following TOOLDIR's are assumed to be rooted in." >> $FILENAME
echo 'export TOOLCHAIN_HOME="CHANGEME"' >> $FILENAME
echo >> $FILENAME
echo "# The following variables are subdirectory names for each toolchain." >> $FILENAME
echo "# If you don't have one of these, just comment the line out." >> $FILENAME
echo "export TOOLDIR_ARM=arm-api15" >> $FILENAME
echo "export TOOLDIR_ARMv7=arm-api15" >> $FILENAME
echo "export TOOLDIR_ARM64=arm64-api21" >> $FILENAME
echo "export TOOLDIR_i686=x86-api15" >> $FILENAME
echo "export TOOLDIR_x86_64=x86_64-api21" >> $FILENAME
echo >> $FILENAME
echo 'if [ ! -d "$TOOLCHAIN_HOME" ]; then' >> $FILENAME
echo '    echo "ERROR: Path TOOLCHAIN_HOME=$TOOLCHAIN_HOME does not exist!"' >> $FILENAME
echo '    export TOOLCHAIN_HOME=""' >> $FILENAME
echo 'fi' >> $FILENAME
echo >> $FILENAME
echo "echo 'TOOLCHAIN_HOME and TOOLDIR_* env variables loaded'" >> $FILENAME
echo >> $FILENAME
echo 'if [ ! -z "$SENV_PATH" ]; then' >> $FILENAME
echo "    echo 'Adding tools to PATH...'" >> $FILENAME
echo >> $FILENAME
echo '    [ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARM" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARM/bin"' >> $FILENAME
echo '    [ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARMv7" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARMv7/bin"' >> $FILENAME
echo '    [ -d "$TOOLCHAIN_HOME/$TOOLDIR_ARM64" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_ARM64/bin"' >> $FILENAME
echo '    [ -d "$TOOLCHAIN_HOME/$TOOLDIR_i686" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_i686/bin"' >> $FILENAME
echo '    [ -d "$TOOLCHAIN_HOME/$TOOLDIR_x86_64" ] && export PATH="$PATH:$TOOLCHAIN_HOME/$TOOLDIR_x86_64/bin"' >> $FILENAME
echo 'else' >> $FILENAME
echo "    echo 'Not setting PATH, use \"SENV_PATH=yes source $FILENAME\" to automatically set PATH aswell'" >> $FILENAME
echo "fi" >> $FILENAME

echo "Wrote $FILENAME"
