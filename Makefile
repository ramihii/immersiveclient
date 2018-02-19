all:
	echo 'Please specify accurate target:'
	echo 'make run:  build and run the application natively on local machine'
	echo 'make android: build the application for android (armeabi)'

# Development target, not needed for building
javah:
	mkdir -p /tmp/imm-javah
	mkdir -p immersive/src/javah/
	cp -r ImmersiveClient/app/src/main/java/net/ /tmp/imm-javah
	cd /tmp/imm-javah; javac net/immersive/immersiveclient/Hello.java
	cd /tmp/imm-javah; javah net.immersive.immersiveclient.Hello
	cp /tmp/imm-javah/*.h immersive/src/javah
	rm -r /tmp/imm-javah

native:
	mkdir -p immersive/build-native
	cd immersive/build-native; cmake -DAS_EXECUTABLE=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=native ..
	make -C immersive/build-native

run: native
	./immersive/build-native/immersive

# Mobile targets below

# expects to find arm-linux-androideabi-* on PATH
cmake-armeabi:
	mkdir -p immersive/build-armeabi
	cd immersive/build-armeabi; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=armeabi -DCMAKE_TOOLCHAIN_FILE=../toolchains/armeabi.cmake -DAS_EXECUTABLE=OFF ..

armeabi: cmake-armeabi
	make -C immersive/build-armeabi

copy-armeabi: armeabi
	mkdir -p ImmersiveClient/app/immersive-lib/bin/armeabi/
	cp immersive/build-armeabi/libimmersive.so ImmersiveClient/app/immersive-lib/bin/armeabi/libimmersive.so

# expects to find i686-linux-android-* on PATH
cmake-i686:
	mkdir -p immersive/build-i686
	cd immersive/build-i686; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=i686 -DCMAKE_TOOLCHAIN_FILE=../toolchains/i686.cmake -DAS_EXECUTABLE=OFF ..

i686: cmake-i686
	make -C immersive/build-i686

copy-i686: i686
	mkdir -p ImmersiveClient/app/immersive-lib/bin/i686/
	cp immersive/build-i686/libimmersive.so ImmersiveClient/app/immersive-lib/bin/i686/libimmersive.so

# expects to find x86_64-linux-android-* on PATH
cmake-x86_64:
	mkdir -p immersive/build-x86_64
	cd immersive/build-x86_64; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=x86_64 -DCMAKE_TOOLCHAIN_FILE=../toolchains/x86_64.cmake -DAS_EXECUTABLE=OFF ..

x86_64: cmake-x86_64
	make -C immersive/build-x86_64

copy-x86_64: x86_64
	mkdir -p ImmersiveClient/app/immersive-lib/bin/x86_64/
	cp immersive/build-x86_64/libimmersive.so ImmersiveClient/app/immersive-lib/bin/x86_64/libimmersive.so
