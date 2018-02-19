#include <immersive_config.h>

#ifdef FOR_ANDROID
#include <jni.h>
#include <android/log.h>
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FOR_ANDROID

#define  LOG_TAG    "someTag"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

JNIEXPORT jint JNICALL
Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
	return a + b;
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(JNIEnv *env, jclass clss){
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(JNIEnv *env, jclass clss){
	LOGD("%s","Testi!\n");
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
