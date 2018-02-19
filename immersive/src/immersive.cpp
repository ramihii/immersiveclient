#include <immersive_config.h>

#ifdef FOR_ANDROID
#include <jni.h>
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FOR_ANDROID
JNIEXPORT jint JNICALL
Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
	return a + b;
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(JNIEnv *env, jclass clss){
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(JNIEnv *env, jclass clss){
	printf("%s","Testi!\n");
}
#endif

const char *get_hello_world() {
	return "Hello, world_imm\n";
}

#ifdef __cplusplus
}
#endif

#ifdef AS_EXECUTABLE
int main(int argc, char **argv) {
	printf("Hello, world2\n");
	return 0;
}
#endif
