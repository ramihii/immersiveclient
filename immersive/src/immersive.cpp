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
#include <fstream>
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
static std::string storage_path;
static bool initialized = false;
const static int BUFFER_SIZE = 128;

//JNIEXPORT jint JNICALL
//Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
//	return a + b;
//}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(JNIEnv *env, jclass clss, jobject assetManager, jstring filePath){

	asset_manager = AAssetManager_fromJava(env, assetManager);

	storage_path = env->GetStringUTFChars(filePath, 0);
	LOGD("The storage path of the program is %s",storage_path.c_str());

	AAssetDir* assetDir = AAssetManager_openDir(asset_manager, "");
	const char* filename;
	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
	   LOGD("Asset directory contains %s",filename);
	}
	AAssetDir_close(assetDir);

	//Temporarily saving assets to file so OSG can read them
	
	assetDir = AAssetManager_openDir(asset_manager, "");
	const char * fileName = NULL;
	while((fileName = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		std::ofstream ofs(fileName,std::ofstream::binary);
		LOGD("Writing file: %s/%s", storage_path.c_str(), fileName);

		AAsset* asset = AAssetManager_open(asset_manager, fileName, AASSET_MODE_STREAMING);
		char buffer[BUFFER_SIZE];
		while( AAsset_read(asset,buffer,BUFFER_SIZE) > 0 )
		{
			ofs << buffer;
		}
	}
	


	AAssetDir_close(assetDir);
	initialized = true;
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(JNIEnv *env, jclass clss){
	int osg_major = OPENSCENEGRAPH_MAJOR_VERSION;
	int osg_minor = OPENSCENEGRAPH_MINOR_VERSION;
	int osg_patch = OPENSCENEGRAPH_PATCH_VERSION;

	const char *osg_ver = osgGetVersion();
	LOGD("%s '%s' %d.%d.%d\n", "CppDraw was called, OSG: ", osg_ver, osg_major, osg_minor, osg_patch);

	//const char * testFileName = "/data/user/0/net.immersive.immersiveclient/files/spiderman_tex_final.obj";
    //osgViewer::Viewer viewer;
    //osg::ref_ptr<osg::Node> model = osgDB::readNodeFile( testFileName );
    //viewer.setSceneData( model.get() );
    
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
