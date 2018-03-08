#ifndef GLES2_TEST_H
#define GLES2_TEST_H

#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int valid;
	GLuint buffers[4]; // 0 = vertex buffer, 1 = index buffer, 2 = cube vertices, 3 = cube indices
	GLuint textures[1];

	GLuint program;
	GLuint vert_shdr;
	GLuint frag_shdr;

	GLint  aPosition;
	GLint  aColor;
	GLint  aUV;
	GLint  uTexture;

	GLuint cube_program;
	GLuint cube_vert;
	GLuint cube_frag;

	GLint  aCubePosition;
	GLint  aCubeColor;
	GLint  uCubeRot;

	float cube_rot;

	unsigned char *local_frame;
} gles2t;

// Returns non-zero (true) on success
int gles2t_init(gles2t *self, int buffer_width, int buffer_height, void(*log)(const char *));

// Returns non-zero (true) on success
int gles2t_draw(gles2t *self, unsigned char *buffer, int buffer_width, int buffer_height, int buffer_format, int bytes_per_pixel, void(*log)(const char *));
#ifdef __cplusplus
}
#endif

#endif
