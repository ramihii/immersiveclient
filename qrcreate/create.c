#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <qrencode.h>
#include <png.h>

static int dbg_counter = 0;

#define PPC 10

#define NDEBUG

#define WHITE 0xFFFFFF
#define BLACK 0x000000

#ifdef DEBUG
#define CHECKDBG printf("%d\n", dbg_counter++)
#else
#define CHECKDBG 
#endif

extern int errno ;

void imgtest(unsigned char *data, int width, int ppc);

void _abort(const char *txt) {
	fprintf(stderr, "%s\n", txt);
	exit(127);
}

int main(int argc, char **argv) {
	/* QRinput *qr = QRinput_new(); */

	/* if(!qr) { */
	/* 	printf("Error occurred!\n"); */
	/* 	printf("errno: %d\n", errno); */
	/* 	return 1; */
	/* } */

	const char *hello = "Hello World";

	const char *text = hello;
	if(argc == 2)
		text = argv[1];

	printf("to encode: %s\n", text);

	QRcode *code = QRcode_encodeString(text, 0, QR_ECLEVEL_H, QR_MODE_8, 1);

	if(!code) {
		int ecode = errno;
		printf("ERR %d ", ecode);
		switch(ecode) {
			case EINVAL:
				printf("Invalid\n");
				break;
			case ENOMEM:
				printf("No mem\n");
				break;
			case ERANGE:
				printf("Range\n");
				break;
		}
		return 1;
	}

	printf("version: %d, width: %d, data: %c\n", code->version, code->width, code->data[0]);

	int y, x;
	for(y = 0; y < code->width; y++) {
		for(x = 0; x < code->width; x++) {
			unsigned char c = code->data[y * code->width + x];
			printf(" %c", c & 1 ? '#' : '.');
		}

		printf("\n");
	}

	imgtest(code->data, code->width, PPC);

	QRcode_free(code);
	printf("End of main\n");

	printf("Calling imgtest\n");

	return 0;
}

// quiet zone = 3 * ppc
void imgtest(unsigned char *data, int width, int ppc) {
	int realwidth = width + 6;

	FILE *fp = fopen("qrout.png", "wb");
	if(!fp)
		_abort("File could not be opened for writing");

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if(!png_ptr)
		_abort("png_create_write_struct() failed");
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
		_abort("png_create_info_struct() failed");
	
	if(setjmp(png_jmpbuf(png_ptr)))
		_abort("error during init_io");
	
	png_init_io(png_ptr, fp);

	if(setjmp(png_jmpbuf(png_ptr)))
		_abort("error during writing header");
	
	png_set_IHDR(png_ptr, info_ptr, realwidth * ppc, realwidth * ppc, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	if(setjmp(png_jmpbuf(png_ptr)))
		_abort("Error during writing bytes");
	
	// allocate row table
	png_bytep *row_ptrs = (png_bytep*) malloc(sizeof(png_bytep) * realwidth * ppc);
	printf("row_ptrs: %p\n", row_ptrs);

	png_size_t rowbs = png_get_rowbytes(png_ptr, info_ptr);
	if(rowbs != 3 * realwidth * ppc)
		_abort("rowbs is of unexpected value: \n");

	printf("rowbs: %u %u %u\n", rowbs, rowbs / 3, rowbs / 3 / ppc);

	int white = WHITE;
	int black = BLACK;

	int y, x, sx, sy;

	// init image
	for(y = 0; y < realwidth * ppc; y++) {
		// allocate row
		row_ptrs[y] = (png_byte*) malloc(rowbs);
		/* printf("row_ptrs[%2d]: %p\n", y, row_ptrs[y]); */

		// default to white
		memset(row_ptrs[y], 0xFF, rowbs);
	}

	CHECKDBG;

	int quiet = 3;

	for(y = 0; y < width; y++) {
		for(sy = 0; sy < ppc; sy++) {
			// fill with color
			for(x = 0; x < width; x++) {
				for(sx = 0; sx < ppc; sx++) {
					/* printf("access\n"); */
					int col = (data[y * width + x] & 1) ? black : white;

					/* printf("set\n"); */

					// RGB
					row_ptrs[(y + quiet) * ppc + sy][(x + quiet) * ppc * 3 + sx * 3 + 0] = (col >> 16) & 0xFF;
					row_ptrs[(y + quiet) * ppc + sy][(x + quiet) * ppc * 3 + sx * 3 + 1] = (col >>  8) & 0xFF;
					row_ptrs[(y + quiet) * ppc + sy][(x + quiet) * ppc * 3 + sx * 3 + 2] =  col        & 0xFF;
				}
			}
		}
	}

	CHECKDBG;

	if(setjmp(png_jmpbuf(png_ptr)))
		_abort("Error during writing bytes");

	png_write_image(png_ptr, row_ptrs);

	CHECKDBG;

	if(setjmp(png_jmpbuf(png_ptr)))
		_abort("Error during end of write");

	CHECKDBG;
	
	png_write_end(png_ptr, NULL);

	CHECKDBG;

	for(y = 0; y < realwidth * ppc; y++)
		free(row_ptrs[y]);
	free(row_ptrs);

	CHECKDBG;

	fclose(fp);
}
