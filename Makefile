all:
	@echo 'Please specify accurate target:'
	@echo 'make run:          build and run the application natively on local machine'
	@echo 'make armeabi:      build the application for android (armeabi, armv5, softfloat)'
	@echo 'make armeabi-v7a:  build the application for android (armeabi-v7a, hardfloat)'
#	@echo 'make arm64-v8a:    build the application for android (aarch64)'
#	@echo 'make i686:         build the application for android (i686)'
	@echo 'make x86_64:       build the application for android (x86_64)'
	@echo 'make copy-armeabi: copy built files for android (armeabi)'
	@echo 'make copy-i686:    copy built files for android (i686)'
	@echo 'make copy-x86_64:  copy built files for android (x86_64)'

# Development target, not needed for building
javah:
	mkdir -p /tmp/imm-javah
	mkdir -p immersive/src/javah/
	cp -r ImmersiveClient/app/src/main/java/net/ /tmp/imm-javah
	cd /tmp/imm-javah; javac net/immersive/immersiveclient/Immersive.java
	cd /tmp/imm-javah; javah net.immersive.immersiveclient.Immersive
	cp /tmp/imm-javah/*.h immersive/src/javah
	rm -r /tmp/imm-javah

native:
	mkdir -p immersive/build-native
	cd immersive/build-native; cmake -DAS_EXECUTABLE=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=native ..
	make -C immersive/build-native

run: native
	./immersive/build-native/immersive

android: copy-armeabi
	cd ImmersiveClient; ./gradlew installDebug
	adb shell am start -n net.immersive.immersiveclient/net.immersive.immersiveclient.MainActivity
	@echo 'Press Enter to stop the application'
	@read
	adb shell am force-stop net.immersive.immersiveclient

crossenv-check:
ifeq (,${TOOLCHAIN_HOME})
	@echo 'TOOLCHAIN_HOME not set; please source setenv.sh'
	@exit 1
endif

## Mobile targets below

# expects to find arm-linux-androideabi-* on PATH
cmake-armeabi: crossenv-check
ifeq (,${TOOLDIR_ARM})
	@exit 1
endif
	mkdir -p immersive/build-armeabi
	cd immersive/build-armeabi; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=armeabi -DCMAKE_TOOLCHAIN_FILE=../toolchains/armeabi.cmake -DAS_EXECUTABLE=OFF ..

armeabi: cmake-armeabi
	make -C immersive/build-armeabi

copy-armeabi: armeabi
	mkdir -p ImmersiveClient/app/immersive-lib/bin/armeabi/
	cp immersive/build-armeabi/libimmersive.so ImmersiveClient/app/immersive-lib/bin/armeabi/libimmersive.so

# expects to find i686-linux-android-* on PATH
cmake-i686: crossenv-check
ifeq (,${TOOLDIR_i686})
	@exit 1
endif
	mkdir -p immersive/build-i686
	cd immersive/build-i686; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=i686 -DCMAKE_TOOLCHAIN_FILE=../toolchains/i686.cmake -DAS_EXECUTABLE=OFF ..

i686: cmake-i686
	make -C immersive/build-i686

copy-i686: i686
	mkdir -p ImmersiveClient/app/immersive-lib/bin/i686/
	cp immersive/build-i686/libimmersive.so ImmersiveClient/app/immersive-lib/bin/i686/libimmersive.so

# expects to find x86_64-linux-android-* on PATH
cmake-x86_64: crossenv-check
ifeq (,${TOOLDIR_x86_64})
	@exit 1
endif
	mkdir -p immersive/build-x86_64
	cd immersive/build-x86_64; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=x86_64 -DCMAKE_TOOLCHAIN_FILE=../toolchains/x86_64.cmake -DAS_EXECUTABLE=OFF ..

x86_64: cmake-x86_64
	make -C immersive/build-x86_64

copy-x86_64: x86_64
	mkdir -p ImmersiveClient/app/immersive-lib/bin/x86_64/
	cp immersive/build-x86_64/libimmersive.so ImmersiveClient/app/immersive-lib/bin/x86_64/libimmersive.so

clean:
	rm -rf immersive/build-armeabi/
	rm -rf immersive/build-i686/
	rm -rf immersive/build-x86_64/
