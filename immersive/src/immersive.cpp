#include <immersive_config.h>

#ifdef FOR_ANDROID
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Version>

#include <string>
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

static AAssetManager* asset_manager;

//JNIEXPORT jint JNICALL
//Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
//	return a + b;
//}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(JNIEnv *env, jclass clss, jobject assetManager ){
	asset_manager = AAssetManager_fromJava(env, assetManager);

	AAssetDir* assetDir = AAssetManager_openDir(asset_manager, "");
	const char* filename;
	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
	   LOGD("Asset directory contains %s",filename);
	}

	AAssetDir_close(assetDir);
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(JNIEnv *env, jclass clss){
	int osg_major = OPENSCENEGRAPH_MAJOR_VERSION;
	int osg_minor = OPENSCENEGRAPH_MINOR_VERSION;
	int osg_patch = OPENSCENEGRAPH_PATCH_VERSION;

	const char *osg_ver = osgGetVersion();
	LOGD("%s '%s' %d.%d.%d\n", "Testi!", osg_ver, osg_major, osg_minor, osg_patch);


//    osgViewer::Viewer viewer;
//    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile( filename );
//    viewer.setSceneData( model.get() );
//    
//    viewer.run();
}
#endif

const char *get_hello_world() {
	return "Hello, world_imm\n";
}

#ifdef __cplusplus
}
#endif

#ifdef AS_EXECUTABLE
//#include <
int main(int argc, char **argv) {
	char glutGamemode[32];
	char cparam_name[] = "Data/camera_para.dat";
	char vconf[] = "";
	char patt_name[]  = "Data/hiro.patt";

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!windowed) {
		if(windowRefresh)
			sprintf(glutGamemode, "%ix%i:%i@%i", windowWidth, windowHeight, windowDepth, windowRefresh);
		else
			sprintf(glutGamemode, "%ix%i:%i", windowWidth, windowHeight, windowDepth);

		glutGameModeString(glutGamemode);
		glutEnterGameMode();
	} else {
		glutInitWindowSize(windowWidth, windowHeight);
		glutCreateWindow(argv[0]);
	}

	printf("Hello, world2\n");
	return 0;
}
#endif
