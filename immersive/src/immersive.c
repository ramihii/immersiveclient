#ifdef ANDROID
#include <jni.h>
#endif

#include <immersive_config.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ANDROID
JNIEXPORT jint JNICALL
Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
	return a + b;
}

JNIEXPORT jint JNICALL
Java_net_immersive_immersiveclient_Hello_mulInts(JNIEnv *env, jobject obj, jint a, jint b) {
	return a * b;
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
