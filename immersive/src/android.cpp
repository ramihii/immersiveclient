#include <immersive_config.h>

#define GLES2TEST
//#define DO_ZBAR

/*
 * Android-specific wrapper functions
 */
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#ifdef DO_OSG
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osg/Version>
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/GL>
#include <osg/GLExtensions>
#endif

#include <string>
#include <fstream>

#include "gles2test.h"

#ifdef __cplusplus

#ifdef DO_ZBAR
using namespace zbar;
#endif

extern "C" {
#endif

#define LOG_TAG "ImmersiveLib"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#ifdef DO_OSG
// Handles logging osg notifications
#include "osgAndroidNotifier.h"
static immersive::OsgAndroidNotifier* _osgAndroidNotifier;

static osgViewer::Viewer _viewer;

static AAssetManager* asset_manager;
static std::string storage_path;

const static int BUFFER_SIZE = 128;
#endif

// Global gles2test state
static gles2t gletest;

static bool initialized = false;

// Must be defined for each platform
static void platform_log_func(const char *str) {
	LOGD("%s", str);
}

JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppInit(
		JNIEnv *env,
		jclass clss,
		jobject assetManager,
		jstring filePath,
		jint bufferWidth,
		jint bufferHeight) {

	if(initialized) {
		platform_log_func("ERR: already initialized");
		return;
	}

#ifdef GLES2TEST
	int ret = gles2t_init(&gletest, bufferWidth, bufferHeight, platform_log_func);

	if(ret) {
		platform_log_func("GLES2 Init successful");
		initialized = true;
	} else {
		platform_log_func("GLES2 Init failed");
		initialized = false;
	}

#endif

	initialized = true;

	platform_log_func("native initialized");

#ifdef DO_OSG
	if(!initialized) {
		// TODO: make non-platform specific if we plan to use OSG.
		platform_log_func("Initializing native");

		const char *osg_ver = osgGetVersion();
		platform_log_func("OSG version: %s\n", osg_ver);

		_osgAndroidNotifier = new immersive::OsgAndroidNotifier();
		_osgAndroidNotifier->setTag("OSG NOTIFIER");
		osg::setNotifyHandler(_osgAndroidNotifier);

		asset_manager = AAssetManager_fromJava(env, assetManager);

		storage_path = env->GetStringUTFChars(filePath, 0);
		platform_log_func("The storage path of the program is %s",storage_path.c_str());

		AAssetDir* assetDir = AAssetManager_openDir(asset_manager, "");
		//const char* filename;
		//while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
		//{
		//   platform_log_func("Asset directory contains %s",filename);
		//}
		//AAssetDir_close(assetDir);

		//Temporarily saving assets to file so OSG can read them

		//assetDir = AAssetManager_openDir(asset_manager, "");
		const char * fileName = NULL;
		while((fileName = AAssetDir_getNextFileName(assetDir)) != NULL)
		{
			std::string outputPath = storage_path + "/" + fileName;
			std::ofstream ofs(outputPath.c_str(),std::ofstream::binary);
			platform_log_func("Writing file: %s", outputPath.c_str());

			AAsset* asset = AAssetManager_open(asset_manager, fileName, AASSET_MODE_STREAMING);
			char buffer[BUFFER_SIZE];
			while( AAsset_read(asset,buffer,BUFFER_SIZE) > 0 )
			{
				ofs << buffer;
			}
		}

		AAssetDir_close(assetDir);

		const char * testFileName = "/data/user/0/net.immersive.immersiveclient/files/cat.obj";

		//osgDB::setCurrentWorkingDirectory("/data/user/0/net.immersive.immersiveclient/files/");

		osg::ref_ptr<osg::Node> model = osgDB::readRefNodeFile( testFileName );
		if(NULL != model){
			platform_log_func("OSG SUCCESFULLY LOADED MODEL %s",testFileName);
			ret = 1;
		} else {
			platform_log_func("OSG FAILED TO LOAD MODEL %s",testFileName);
			ret = 0;
		}
		_viewer.setSceneData( model.get() );

		_viewer.setUpViewerAsEmbeddedInWindow(0, 0, 1000, 1000);
		_viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

		_viewer.realize();

		_viewer.addEventHandler(new osgViewer::StatsHandler);
		_viewer.addEventHandler(new osgViewer::ThreadingHandler);
		_viewer.addEventHandler(new osgViewer::LODScaleHandler);
		//_viewer.run();

		platform_log_func("OSG initialized");
	}
#endif
}

static void convert_nv21_frame(unsigned char *ptr, int width, int height, unsigned char *buffer) {
	int nR, nG, nB, nY, nU, nV, i, j, id2, jd2, offset;

	unsigned char *pY = ptr;
	unsigned char *pUV = ptr + width * height;

	offset = 0;

	// YUV 4:2:0
	for(i = 0; i < height; i++) {
		id2 = i >> 1; // Divide by two

		for(j = 0; j < width; j++) {
			jd2 = j >> 1; // Divide by two

			nY = *(pY + i*width + j);
			nV = *(pUV + id2*width + 2*jd2);
			nU = *(pUV + id2*width + 2*jd2 + 1);

			// Yuv Convert
			nY = MAX(nY - 16, 0);
			nU -= 128;
			nV -= 128;

			nB = 1192 * nY + 2066 * nU;
			nG = 1192 * nY - 833 * nV - 400 * nU;
			nR = 1192 * nY + 1634 * nV;

			nR = MIN(262143, MAX(0, nR));
			nG = MIN(262143, MAX(0, nG));
			nB = MIN(262143, MAX(0, nB));

			nR >>= 10; nR &= 0xff;
			nG >>= 10; nG &= 0xff;
			nB >>= 10; nB &= 0xff;

			buffer[offset++] = (unsigned char)nR;
			buffer[offset++] = (unsigned char)nG;
			buffer[offset++] = (unsigned char)nB;
		}
	}
}

// public static native void cppDraw(int bufferWidth, int bufferHeight, int format, int bytesPerPixel, ByteBuffer buffer);
JNIEXPORT void JNICALL
Java_net_immersive_immersiveclient_Immersive_cppDraw(
		JNIEnv *env,
		jclass cls,
		jint buffer_width,
		jint buffer_height,
		jint buffer_format,
		jint bytes_per_pixel,
		jbyteArray jframe) {

	// If not initialized, abort rendering
	if(!initialized)
		return;

	unsigned char *buffer = (unsigned char *) env->GetByteArrayElements(jframe, NULL);
	convert_nv21_frame(buffer, buffer_width, buffer_height, gletest.local_frame);
	env->ReleaseByteArrayElements(jframe, (jbyte *)buffer, JNI_ABORT);

#ifdef DO_ZBAR
	unsigned char *grey = (unsigned char *) malloc(sizeof(unsigned char) * frame_width * frame_height);
	if(!grey) {
		LOGE("failed to malloc()\n");
		return;
	}

	int x, y;
	for(y = 0; y < frame_width; y++) {
		for(x = 0; x < frame_height; x++) {
			grey[y * frame_width + x] = to_grayscale(buffer + y * frame_width * bytes_per_pixel + x * bytes_per_pixel);
		}
	}

	unsigned int major, minor;
	int ret = zbar_version(&major, &minor);
	LOGD("zbar major: %u, minor: %u, ret: %d\n", major, minor, ret);

	zbar_image_scanner_t *zbar = zbar_image_scanner_create();
	if(!zbar) {
		LOGE("failed to construct zbar\n");
		free(grey);
		return;
	}

	zbar_image_t *zimg = zbar_image_create();
	if(!zimg) {
		LOGE("failed to construct zbar image\n");
		free(grey);
		return;
	}

	zbar_image_set_format(zimg, 0x59455247);
	zbar_image_set_size(zimg, frame_width, frame_height);
	zbar_image_set_data(zimg, grey, frame_width * frame_height, NULL);

	ret = zbar_scan_image(zbar, zimg);

	const zbar_symbol_set_t *symbols = zbar_image_scanner_get_results(zbar);
	if(!symbols) {
		LOGE("failed to get symbols\n");
		free(grey);
		return;
	}

	const zbar_symbol_t *sym = zbar_symbol_set_first_symbol(symbols);
	if(!sym) {
		LOGE("No symbols found\n");
		free(grey);
		return;
	}

	while(sym) {
		const char *data = zbar_symbol_get_data(sym);
		if(!data) {
			LOGW("failed to get data from symbol");
		} else {
			unsigned int npoints = zbar_symbol_get_loc_size(sym);
			unsigned int i;

			for(i = 0; i < npoints; i++) {
				LOGD("  * point %d (x, y): (%3d, %3d)\n", i,
						zbar_symbol_get_loc_x(sym, i),
						zbar_symbol_get_loc_y(sym, i));
			}

			LOGD("data within symbol: %s\n", data);
		}

		sym = zbar_symbol_next(sym);
	}

	zbar_image_destroy(zimg);
	zbar_image_scanner_destroy(zbar);
#endif

#ifdef GLES2TEST
	gles2t_draw(&gletest, gletest.local_frame, buffer_width, buffer_height, 1, 3, platform_log_func);
	return;
#endif

#ifdef DO_OSG
	_viewer.frame();
#endif
}
//#endif


#ifdef __cplusplus
}
#endif
