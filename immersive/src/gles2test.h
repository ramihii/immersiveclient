#ifndef GLES2_TESTI_H
#define GLES2_TESTI_H

#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	GLuint buffers[1];
	GLuint program;
	GLuint vert_shdr;
	GLuint frag_shdr;
	GLint  aPosition;
	GLint  aColor;
} gles2t;

// Returns non-zero (true) on success
int gles2t_init(gles2t *self, void(*log)(const char *));

// Returns non-zero (true) on success
int gles2t_draw(gles2t *self, void(*log)(const char *));
#ifdef __cplusplus
}
#endif

#endif
