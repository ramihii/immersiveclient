#include <string.h>
#include "gles2test.h"

static const char *vert_shader = 
	"attribute vec3 aPosition;"
	"attribute vec3 aColor;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  vColor = aColor;"
	"  gl_Position = vec4(aPosition, 1.0);"
	"}";

static const char *frag_shader = 
	"precision mediump float;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  gl_FragColor = vec4(vColor, 1.0);"
	"}";

static const float bufdata[] = {
	// positions
	-1.0, -0.50, 0.0,
	 0.0,  0.75, 0.0,
	 1.0, -0.50, 0.0,

	// colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
};

#ifdef __cplusplus
extern "C" {
#endif

// Returns non-zero (true) on success
int gles2t_init(gles2t *self, void(*log)(const char *)) {

// Convenience macro for logging a message and jumping to error handling code
#define GOFAIL(handler, msg) do { log(msg); goto handler; } while(0)

	// Set "background" color; the color glClear() fills the screen with
	glClearColor(1.0, 0.5, 1.0, 0.0);

	// Draw both back and front faces
	glDisable(GL_CULL_FACE);

	// Always draw on top of everything
	glDisable(GL_DEPTH_TEST);

	// Create buffer name(s)
	glGenBuffers(1, self->buffers);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_noclean, "Unable to gen buffers");

	// Bind our buffer 0 as ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[0]);

	// Copy our vertex data from 'bufdata' to the bound ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(bufdata), bufdata, GL_STATIC_DRAW);

	if(glGetError() != GL_NO_ERROR) 
		GOFAIL(fail_buffers, "Unable to set buffer data");

	// Create shader program and the shader components
	self->program = glCreateProgram();
	self->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
	self->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);

	if(glGetError() != GL_NO_ERROR) 
		GOFAIL(fail_buf_shdr, "Failed to gen shaders");

	// Length of the shader source code string
	GLint source_len;

	// Set the shaders' source codes
	source_len = strlen(vert_shader);
	glShaderSource(self->vert_shdr, 1, &vert_shader, &source_len);
	source_len = strlen(frag_shader);
	glShaderSource(self->frag_shdr, 1, &frag_shader, &source_len);

	// Compile shaders
	glCompileShader(self->vert_shdr);
	glCompileShader(self->frag_shdr);

	// Attach shader components to the shader program
	glAttachShader(self->program, self->vert_shdr);
	glAttachShader(self->program, self->frag_shdr);

	// Link the shader program
	glLinkProgram(self->program);

	if(glGetError() != GL_NO_ERROR) 
		GOFAIL(fail_buf_shdr, "Unable to create shader program");

	// Find the vertex attribute locations (position and color)
	self->aPosition = glGetAttribLocation(self->program, "aPosition");
	self->aColor = glGetAttribLocation(self->program, "aColor");

	if(self->aPosition == -1 || self->aColor == -1)
		GOFAIL(fail_buf_shdr, "Unable to find attribute locations");

	// Unbind ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 1;

fail_buf_shdr:
	log("Cleaning shaders due to fail");
	glDeleteShader(self->vert_shdr);
	glDeleteShader(self->frag_shdr);
	glDeleteProgram(self->program);
fail_buffers:
	log("Cleaning buffers due to fail");
	glDeleteBuffers(1, self->buffers);
fail_noclean:
	return 0;
#undef GOFAIL
}

int gles2t_draw(gles2t *self, void(*log)(const char *)) {
	// Fill the screen with the clear color
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable our shader program
	glUseProgram(self->program);

	// Bind vertex data buffer (position vectors and colors)
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[0]);

	// Setup buffer pointers (shader attributes)
	glVertexAttribPointer(self->aPosition, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	glEnableVertexAttribArray(self->aPosition);

	glVertexAttribPointer(self->aColor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(3 * 3 * 4));
	glEnableVertexAttribArray(self->aColor);

	// Render 3 vertices as triangle(s) starting at offset 0
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if(glGetError() != GL_NO_ERROR) {
		log("GLES2 draw failed");
		return 0;
	} else {
		log("GLES2 draw succeeded");
		return 1;
	}

	// Ensure the drawing commands will be run (probably not needed)
	glFlush();
}

#ifdef __cplusplus
}
#endif
