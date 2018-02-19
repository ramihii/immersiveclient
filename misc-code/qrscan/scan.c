#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zbar.h>
#include <png.h>

static void fail(const char *txt) {
	printf("%s\n", txt);
	exit(20);
}

static unsigned long stoli(const char *str) {
	unsigned long ret = 0;
	int i;

	for(i = 0; str[i]; i++)
		ret |= str[i] << (i * 8);
	
	return ret;
}

static unsigned char to_grayscale(png_bytep ptr) {
	unsigned char red = 0.2989 * ptr[0];
	unsigned char green = 0.5870 * ptr[1];
	unsigned char blue = 0.1140 * ptr[2];

	unsigned char val = red + green + blue;

	return val > 255 ? 255 : val;
}

static const char *symtype_to_str(zbar_symbol_type_t type) {
	switch(type) {
		case ZBAR_NONE:    return "None";
		case ZBAR_PARTIAL: return "Intermediate; partial";
		case ZBAR_EAN8:    return "EAN-8";
		case ZBAR_UPCE:    return "UPC-E";
		case ZBAR_ISBN10:  return "ISBN-10 (from EAN-13)";
		case ZBAR_UPCA:    return "UPC-A";
		case ZBAR_EAN13:   return "EAN-13";
		case ZBAR_ISBN13:  return "ISBN-13 (from EAN-13)";
		case ZBAR_I25:     return "Interleaved 2 of 5";
		case ZBAR_CODE39:  return "Code 39";
		case ZBAR_PDF417:  return "PDF417";
		case ZBAR_QRCODE:  return "QR Code";
		case ZBAR_CODE128: return "Code 128";
		case ZBAR_SYMBOL:  return "mask for... SYMBOL";
		case ZBAR_ADDON2:  return "ADDON2";
		case ZBAR_ADDON5:  return "ADDON5";
		case ZBAR_ADDON:   return "ADDON";
		default: return "?";
	}
}

static unsigned char *read_image(const char *path, unsigned int *out_width, unsigned int *out_height) {
	FILE *fp = fopen(path, "rb");
	if(!fp)
		fail("unable to open file");

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) {
		fclose(fp);
		fail("unable to create read struct");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!png_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fail("unable to create info struct");
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		fail("unable to init io");
	}

	png_init_io(png_ptr, fp);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);
	unsigned int width = png_get_image_width(png_ptr, info_ptr);
	unsigned int height = png_get_image_height(png_ptr, info_ptr);

	unsigned int bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	unsigned int channels = png_get_channels(png_ptr, info_ptr);
	unsigned int color_type = png_get_color_type(png_ptr, info_ptr);

	printf("width: %u, height: %u, bitdepth: %u, channels: %u, coltype: %u\n", width, height, bitdepth, channels, color_type);

	if(bitdepth != 8)
		fail("invalid bitdepth, expected 8");

	if(channels != 3 && channels != 1)
		fail("invalid # of channel, expected 1 or 3");

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	unsigned char *data = (unsigned char *) malloc(sizeof(unsigned char) * width * height);

	int x, y;
	if(channels == 3) {
		printf("copying bytes and converting to greyscale\n");
		for(y = 0; y < height; y++) {
			png_bytep row = row_pointers[y];
			for(x = 0; x < width; x++) {
				png_bytep pix = row + x * 3; // FIXME: assuming 3 bytes per pixel
				data[y * width + x] = to_grayscale(pix);
			}
		}
	} else {
		for(y = 0; y < height; y++) {
			memcpy(data + y * width, row_pointers[y], width * sizeof(unsigned char));
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	*out_width = width;
	*out_height = height;
	return data;
}

int main(int argc, char **argv) {
	if(argc != 2)
		fail("Expected an image filename");
	
	const char *imgpath = argv[1];
	printf("reading file from: %s\n", imgpath);

	// Test by printing version
	unsigned int major, minor;
	int ret = zbar_version(&major, &minor);
	printf("zbar major: %u, minor: %u, ret: %d\n", major, minor, ret);

	zbar_image_scanner_t *zbar = zbar_image_scanner_create();
	if(!zbar)
		fail("failed to construct zbar");
	
	zbar_image_t *zimg = zbar_image_create();
	/* zbar_image_set_format(zimg, stoli("GRAY")); */
	/* zbar_image_set_format(zimg, 0x47524159); */
	zbar_image_set_format(zimg, 0x59455247);

	unsigned int img_width = 0, img_height = 0;

	unsigned char *image = read_image(imgpath, &img_width, &img_height);
	if(!image)
		fail("failed to read image");

	unsigned long img_bytes = img_width * img_height;

	zbar_image_set_size(zimg, img_width, img_height);

	zbar_image_set_data(zimg, image, img_bytes, NULL);

	zbar_image_write(zimg, "testimg.bin");
	
	ret = zbar_scan_image(zbar, zimg);

	const zbar_symbol_set_t *symbols = zbar_image_scanner_get_results(zbar);
	if(!symbols)
		fail("failed to get symbols");


	const zbar_symbol_t *sym = zbar_symbol_set_first_symbol(symbols);
	if(!sym)
		printf("No symbols found\n");

	while(sym) {
		const char *data = zbar_symbol_get_data(sym);
		if(!data)
			fail("failed to get data from symbol");

		zbar_symbol_type_t type = zbar_symbol_get_type(sym);
		printf("  - symbol type: %s\n", symtype_to_str(type));

		unsigned int npoints = zbar_symbol_get_loc_size(sym);
		unsigned int i;

		for(i = 0; i < npoints; i++) {
			printf("  * point %d (x, y): (%3d, %3d)\n", i,
					zbar_symbol_get_loc_x(sym, i),
					zbar_symbol_get_loc_y(sym, i));
		}
		
		printf("data within symbol: %s\n", data);
		
		sym = zbar_symbol_next(sym);
	}

	zbar_image_destroy(zimg);
	zbar_image_scanner_destroy(zbar);

	return 0;
}
