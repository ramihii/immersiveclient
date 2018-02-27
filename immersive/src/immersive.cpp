#include <immersive_config.h>

#ifdef FOR_ANDROID
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osg/Version>
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/GL>
#include <osg/GLExtensions>

#include <zbar.h>

#include <string>
#include <fstream>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
using namespace zbar;
extern "C" {
#endif

static unsigned char to_grayscale(unsigned char *ptr) {
	unsigned char red = 0.2989 * ptr[0];
	unsigned char green = 0.5870 * ptr[1];
	unsigned char blue = 0.1140 * ptr[2];

	unsigned char val = red + green + blue;

	return val > 255 ? 255 : val;
}

static AAssetManager* asset_manager;
static std::string storage_path;
static bool initialized = false;
const static int BUFFER_SIZE = 128;

#include "osgAndroidNotifier.h" //Handles logging osg notifications
static immersive::OsgAndroidNotifier* _osgAndroidNotifier;

static osgViewer::Viewer _viewer;
//JNIEXPORT jint JNICALL
//Java_net_immersive_immersiveclient_Hello_addInts(JNIEnv *env, jobject obj, jint a, jint b) {
//	return a + b;
//}

#ifdef FOR_ANDROID

#define LOG_TAG "ImmersiveLib"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(JNIEnv *env, jclass clss, jobject assetManager, jstring filePath){
	if(!initialized){
	LOGD("Initializing native");

	const char *osg_ver = osgGetVersion();
	LOGD("OSG version: %s\n", osg_ver);

	_osgAndroidNotifier = new immersive::OsgAndroidNotifier();
    _osgAndroidNotifier->setTag("OSG NOTIFIER");
    osg::setNotifyHandler(_osgAndroidNotifier);

	asset_manager = AAssetManager_fromJava(env, assetManager);

	storage_path = env->GetStringUTFChars(filePath, 0);
	LOGD("The storage path of the program is %s",storage_path.c_str());

	AAssetDir* assetDir = AAssetManager_openDir(asset_manager, "");
	//const char* filename;
	//while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
	//{
	//   LOGD("Asset directory contains %s",filename);
	//}
	//AAssetDir_close(assetDir);

	//Temporarily saving assets to file so OSG can read them
	
	//assetDir = AAssetManager_openDir(asset_manager, "");
	const char * fileName = NULL;
	while((fileName = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		std::string outputPath = storage_path + "/" + fileName;
		std::ofstream ofs(outputPath.c_str(),std::ofstream::binary);
		LOGD("Writing file: %s", outputPath.c_str());

		AAsset* asset = AAssetManager_open(asset_manager, fileName, AASSET_MODE_STREAMING);
		char buffer[BUFFER_SIZE];
		while( AAsset_read(asset,buffer,BUFFER_SIZE) > 0 )
		{
			ofs << buffer;
		}
	}

	AAssetDir_close(assetDir);

	//OSG CODE ----------------------------------
	
	const char * testFileName = "/data/user/0/net.immersive.immersiveclient/files/cat.obj";

	//osgDB::setCurrentWorkingDirectory("/data/user/0/net.immersive.immersiveclient/files/");

    osg::ref_ptr<osg::Node> model = osgDB::readRefNodeFile( testFileName );
	if(NULL != model){
		LOGD("OSG SUCCESFULLY LOADED MODEL %s",testFileName);
	} else {
		LOGD("OSG FAILED TO LOAD MODEL %s",testFileName);
	}
    _viewer.setSceneData( model.get() );

    _viewer.setUpViewerAsEmbeddedInWindow(0, 0, 1000, 1000);
    _viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

	_viewer.realize();

    _viewer.addEventHandler(new osgViewer::StatsHandler);
    _viewer.addEventHandler(new osgViewer::ThreadingHandler);
    _viewer.addEventHandler(new osgViewer::LODScaleHandler);
	//_viewer.run();
	
	LOGD("Native initialized");
	initialized = true;
	}//end initialization block
	else 
	{
	LOGD("Already initialized!");
	}
}

// public static native void cppDraw(int bufferWidth, int bufferHeight, int format, int bytesPerPixel, ByteBuffer buffer);
JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(JNIEnv *env, jclass cls, jint buffer_width, jint buffer_height, jint buffer_format, jint bytes_per_pixel, jobject jbuffer) {

	_viewer.frame();

	//LOGD("cppDraw call");
	
	///* jobject buffer is of Java type java/nio/ByteBuffer*/
	//unsigned char *buffer = (unsigned char *) env->GetDirectBufferAddress(jbuffer);

	//unsigned char *grey = (unsigned char *) malloc(sizeof(unsigned char) * buffer_width * buffer_height);
	//if(!grey) {
	//	LOGE("failed to malloc()\n");
	//	return;
	//}
	//
	//int x, y;
	//for(y = 0; y < buffer_width; y++) {
	//	for(x = 0; x < buffer_height; x++) {
	//		grey[y * buffer_width + x] = to_grayscale(buffer + y * buffer_width * bytes_per_pixel + x * bytes_per_pixel);
	//	}
	//}
    //
	//unsigned int major, minor;
	//int ret = zbar_version(&major, &minor);
	//LOGD("zbar major: %u, minor: %u, ret: %d\n", major, minor, ret);

	//zbar_image_scanner_t *zbar = zbar_image_scanner_create();
	//if(!zbar) {
	//	LOGE("failed to construct zbar\n");
	//	free(grey);
	//	return;
	//}

	//zbar_image_t *zimg = zbar_image_create();
	//if(!zimg) {
	//	LOGE("failed to construct zbar image\n");
	//	free(grey);
	//	return;
	//}

	//zbar_image_set_format(zimg, 0x59455247);
	//zbar_image_set_size(zimg, buffer_width, buffer_height);
	//zbar_image_set_data(zimg, grey, buffer_width * buffer_height, NULL);

	//ret = zbar_scan_image(zbar, zimg);

	//const zbar_symbol_set_t *symbols = zbar_image_scanner_get_results(zbar);
	//if(!symbols) {
	//	LOGE("failed to get symbols\n");
	//	free(grey);
	//	return;
	//}

	//const zbar_symbol_t *sym = zbar_symbol_set_first_symbol(symbols);
	//if(!sym) {
	//	LOGE("No symbols found\n");
	//	free(grey);
	//	return;
	//}

	//while(sym) {
	//	const char *data = zbar_symbol_get_data(sym);
	//	if(!data) {
	//		LOGW("failed to get data from symbol");
	//	} else {
	//		unsigned int npoints = zbar_symbol_get_loc_size(sym);
	//		unsigned int i;

	//		for(i = 0; i < npoints; i++) {
	//			LOGD("  * point %d (x, y): (%3d, %3d)\n", i,
	//					zbar_symbol_get_loc_x(sym, i),
	//					zbar_symbol_get_loc_y(sym, i));
	//		}
	//		
	//		LOGD("data within symbol: %s\n", data);
	//	}
	//	
	//	sym = zbar_symbol_next(sym);
	//}

	//zbar_image_destroy(zimg);
	//zbar_image_scanner_destroy(zbar);
}
#endif

#ifdef __cplusplus
}
#endif

#ifdef AS_EXECUTABLE
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
