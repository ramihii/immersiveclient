#include <immersive_config.h>

#define GLES2TEST
//#define DO_ZBAR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zbar.h>

#include <GL/glut.h>
#include <opencv2/videoio/videoio_c.h>

#include "gles2test.h"

#ifdef __cplusplus

#ifdef DO_ZBAR
using namespace zbar;
#endif

extern "C" {
#endif

// Global gles2test state
static gles2t gletest;

// Running natively on development computer
static void platform_log_func(const char *str) {
	printf("%s\n", str);
}

#ifdef DO_ZBAR
static unsigned char to_grayscale(unsigned char *ptr) {
	unsigned char red = 0.2989 * ptr[0];
	unsigned char green = 0.5870 * ptr[1];
	unsigned char blue = 0.1140 * ptr[2];

	unsigned char val = red + green + blue;

	return val > 255 ? 255 : val;
}
#endif

static bool initialized = false;

static int immersive_init(unsigned int frame_width,
						  unsigned int frame_height) {
	int ret = 1;

	if(initialized) {
		platform_log_func("ERR: already initialized");
		return 0;
	}

#ifdef GLES2TEST
	ret = gles2t_init(&gletest, frame_width, frame_height, platform_log_func);

	if(ret) {
		platform_log_func("GLES2 Init successful");
		initialized = true;
	} else {
		platform_log_func("GLES2 Init failed");
		initialized = false;
	}

	return ret;
#endif

	initialized = true;

	platform_log_func("native initialized");

	return ret;
}

static int immersive_render(unsigned char *camera_rgb,
							unsigned int frame_width,
							unsigned int frame_height) {

	const int bytes_per_pixel = 3;
	const int buffer_format = 1;

	int ret = 1;

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
	ret = gles2t_draw(&gletest, camera_rgb, frame_width, frame_height, 1, 3, platform_log_func);
	return ret;
#endif
}

#ifdef __cplusplus
}
#endif

struct {
	int width;
	int height;
	int fwidth;
	int fheight;
	unsigned char *dummy_buf;
	CvCapture *capture;
} state;
		
static void do_keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 27: // Escape
		exit(0);
		break;
	}
}

static void do_display(void) {
	//glClear(GL_DEPTH_BUFFER_BIT);

	IplImage *frame = cvQueryFrame(state.capture);

	int fwidth = frame->width;
	int fheight = frame->height;

	/*
	printf("width: %d, height: %d, wstep: %d, channels: %d, depth: ", fwidth, fheight, frame->widthStep, frame->nChannels);
	switch(frame->depth) {
	case IPL_DEPTH_8U:
		printf("8u\n");
		break;
	case IPL_DEPTH_8S:
		printf("8u\n");
		break;
	case IPL_DEPTH_16S:
		printf("8u\n");
		break;
	case IPL_DEPTH_32S:
		printf("8u\n");
		break;
	case IPL_DEPTH_32F:
		printf("8u\n");
		break;
	case IPL_DEPTH_64F:
		printf("8u\n");
		break;
	default:
		printf("\n");
		break;
	}
	*/

	if(frame->depth == IPL_DEPTH_8U && frame->widthStep == fwidth * 3) {
		// Only support one depth for now
		if(!state.dummy_buf || fwidth != state.fwidth || fheight != state.fheight) {
			free(state.dummy_buf);
			state.dummy_buf = (unsigned char *) malloc(sizeof(unsigned char) * fwidth * fheight * 3);
			state.fwidth = fwidth;
			state.fheight = fheight;
			printf("fwidth: %d, fheight: %d\n", fwidth, fheight);
		}
			
		if(state.dummy_buf) {
			memcpy(state.dummy_buf, frame->imageData, fwidth * fheight * 3);
		} else {
			printf("dummy_buf null\n");
		}
	} else {
		printf("Invalid frame format\n");
	}

	// TODO: code
	immersive_render(state.dummy_buf, state.fwidth, state.fheight);

	glutSwapBuffers();
}

static void do_reshape(GLint width, GLint height) {
	state.width = width;
	state.height = height;

	/*
	int bufsize = sizeof(unsigned char) * 3 * state.fwidth * state.fheight;
	state.dummy_buf = (unsigned char *) malloc(bufsize);

	printf("Created dummy_buf of size %d x %d\n", state.fwidth, state.fheight);

	int i;
	for(i = 0; i < bufsize; i += 3) {
		state.dummy_buf[i + 0] = 0xFF;
		state.dummy_buf[i + 1] = 0x00;
		state.dummy_buf[i + 2] = 0xFF;
	}
	*/

	glViewport(0, 0, width, height);
}

#define FRAME_DELAY 100

static void do_rerender(int val) {
	glutPostRedisplay();
	glutTimerFunc(FRAME_DELAY, do_rerender, val);
}

int main(int argc, char **argv) {
	char cparam_name[] = "Data/camera_para.dat";
	char vconf[] = "";
	char patt_name[]  = "Data/hiro.patt";

	int windowed = 1;
	int windowWidth = 800;
	int windowHeight = 600;
	int windowDepth = 1;
	int windowRefresh = 10;

	state.width = windowWidth;
	state.height = windowHeight;
	state.dummy_buf = NULL;

	state.capture = cvCreateCameraCapture(CV_CAP_ANY);
	if(!state.capture) {
		fprintf(stderr, "Error opening camera capture\n");
		return 1;
	}

	IplImage *frame = cvQueryFrame(state.capture);
	state.fwidth = frame->width;
	state.fheight = frame->height;

	printf("Frame size: %d x %d\n", state.fwidth, state.fheight);

	do_reshape(windowWidth, windowHeight);

	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow(argv[0]);

	// Init graphics
	immersive_init(state.fwidth, state.fheight);

	// Register callbacks
	glutDisplayFunc(do_display);
	glutReshapeFunc(do_reshape);
	glutKeyboardFunc(do_keyboard);
	glutTimerFunc(FRAME_DELAY, do_rerender, 0);

	printf("Hello, world2\n");
	glutMainLoop();

	cvReleaseCapture(&state.capture);
	return 0;
}
