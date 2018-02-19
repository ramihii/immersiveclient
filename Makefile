all:
	echo 'Please specify accurate target:'
	echo 'make run:  build and run the application natively on local machine'
	echo 'make android: build the application for android (armeabi)'

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

# expects to find arm-linux-androideabi-* on PATH
cmake-armeabi:
	mkdir -p immersive/build-armeabi
	cd immersive/build-armeabi; cmake -DFOR_ANDROID=ON -DIMMERSIVE_ROOT=$(PWD) -DTGT_ABI=armeabi -DCMAKE_TOOLCHAIN_FILE=../toolchains/armeabi.cmake -DAS_EXECUTABLE=OFF ..

# alias
android: armeabi

armeabi: cmake-armeabi
	make -C immersive/build-armeabi

copy-armeabi: armeabi
	cp immersive/build-armeabi/libimmersive.so ImmersiveClient/app/immersive-lib/bin/armeabi/libimmersive.so
