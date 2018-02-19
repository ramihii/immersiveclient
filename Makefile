all:
	echo 'Please specify accurate target:'
	echo 'make run:  build and run the application natively on local machine'
	echo 'make android: build the application for android (armeabi)'

native:
	mkdir -p immersive/build-native
	cd immersive/build-native; cmake -DAS_EXECUTABLE=ON ..
	make -C immersive/build-native

run: native
	./immersive/build-native/immersive

cmake-armeabi:
	mkdir -p immersive/build-armeabi
	cd immersive/build-armeabi; cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/armeabi.cmake -DAS_EXECUTABLE=OFF ..

# alias
android: armeabi

armeabi: cmake-armeabi
	make -C immersive/build-armeabi
