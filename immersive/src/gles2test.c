#include <immersive_config.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "gles2test.h"

#ifdef AS_EXECUTABLE
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES2/gl2.h>
#endif

static const char *vert_shader =
	"#version 100\n"
	"attribute vec3 aPosition;"
	"attribute vec3 aColor;"
	"attribute vec2 aUV;"
	""
	"varying vec3 vColor;"
	"varying vec2 vUV;"
	""
	"void main() {"
	"  vColor = aColor;"
	"  vUV = aUV;"
	"  gl_Position = vec4(aPosition, 1.0);"
	"}";

static const char *frag_shader =
	"#version 100\n"
	"precision mediump float;"
	""
	"uniform sampler2D uTexture;"
	""
	"varying vec3 vColor;"
	"varying vec2 vUV;"
	""
	"void main() {"
	//"  gl_FragColor = vec4(vColor, 1.0);"
	"  gl_FragColor = texture2D(uTexture, vUV) * vec4(vColor, 1.0);"
	"}";

static const char *cube_vert =
	"#version 100\n"
	"attribute vec3 aPosition;"
	"attribute vec3 aColor;"
	""
	"uniform mediump float uRot;"
	//"uniform float uRot;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  vColor = aColor;"
	"  vec2 rotated = vec2(aPosition.x * cos(uRot) - aPosition.z * sin(uRot), aPosition.z * cos(uRot) + aPosition.x * sin(uRot));"
	"  gl_Position = vec4(rotated.x, aPosition.y, rotated.y, 1.0);"
	"}";

static const char *cube_frag =
	"#version 100\n"
	"precision mediump float;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  gl_FragColor = vec4(vColor, 1.0);"
	"}";

//#define wid (1920.0 / 2048.0)
//#define hed (1080.0 / 2048.0)

#define wid (640.0 / 1024.0)
#define hed (480.0 / 1024.0)

static float bufdata[] = {
	// positions
	-1.0, -1.0, 0.9, // left-bot
	 1.0, -1.0, 0.9, // right-bot
	 1.0,  1.0, 0.9, // right-top
	-1.0,  1.0, 0.9, // left-top

	// colors
	1.0, 1.0, 1.0, // white
	1.0, 1.0, 1.0, // white
	1.0, 1.0, 1.0, // white
	1.0, 1.0, 1.0, // white

	// UV's
	//0.0, 0.0,
	//0.5, 1.0,
	//1.0, 0.0,

#ifdef FOR_ANDROID
	wid, hed,
	wid, 0.0,
	0.0, 0.0,
	0.0, hed,
#else
	0.0, hed,
	wid, hed,
	wid, 0.0,
	0.0, 0.0,
#endif
};

static const float cubedata[] = {
	/// positions
	// front
	-0.5, -0.5, 0.0, // left-bot
	 0.5, -0.5, 0.0, // right-bot
	 0.5,  0.5, 0.0, // right-top
	-0.5,  0.5, 0.0, // left-top
	// back
	 0.5, -0.5, 0.5, // left-bot
	-0.5, -0.5, 0.5, // right-bot
	-0.5,  0.5, 0.5, // right-top
	 0.5,  0.5, 0.5, // left-top

	/// colors
	// front
	1.0, 0.0, 0.0, // red
	0.0, 1.0, 0.0, // green
	0.0, 0.0, 1.0, // blue
	1.0, 1.0, 0.0, // yellow
	// back
	1.0, 0.0, 0.0, // red
	0.0, 1.0, 0.0, // green
	0.0, 0.0, 1.0, // blue
	1.0, 1.0, 0.0, // yellow
};

static const unsigned short cubeinds[] = {
	// Front
	0,1,2,
	2,3,0,
	// Right
	1,4,7,
	7,2,1,
	// Back
	4,5,6,
	6,7,4,
	// Left
	5,0,3,
	3,6,5,
	// Top
	3,2,7,
	7,6,3,
	// Bottom
	5,4,1,
	1,0,5,
};

#undef wid
#undef hed

static const unsigned short indbufdata[] = {
	0, 1, 2, // right-bot
	2, 3, 0, // left-top
};

#ifdef __cplusplus
extern "C" {
#endif

// Convert non-power-of-two integer to next power of two
// if the number is already a power of two, it's returned as-is.
// if the number is greater than 0x80000000, it's also returned as-is.
static unsigned int npot2pot(unsigned int npot) {
	unsigned int pot = 1;

	if(npot > 0x80000000)
		return npot;

	while(npot > pot)
		pot <<= 1; // multiply by two

	return pot;
}

#ifdef AS_EXECUTABLE
static void print_gl_error(const char *filename, int linenumber) {
	GLenum err = glGetError();
	if(err == GL_NO_ERROR) {
		printf("OK %s:%d\n", filename, linenumber);
		return;
	}

	printf("GLU error in %s:%d: %d %s\n", filename, linenumber, err, gluErrorString(err));
}
#endif

//#define GLDBG print_gl_error(__FILE__, __LINE__)
#define GLDBG

// Returns non-zero (true) on success
int gles2t_init(gles2t *self, int buffer_width, int buffer_height, void(*log)(const char *)) {

// Convenience macro for logging a message and jumping to error handling code
#define GOFAIL(handler, msg) do { log(msg); goto handler; } while(0)

	// Allocate buffer for RGB data
	//self->local_frame = (unsigned char *) malloc(sizeof(unsigned char) * buffer_width * buffer_height * 3);

	unsigned int pot_width = npot2pot(buffer_width);
	unsigned int pot_height = npot2pot(buffer_height);

	// Rewrite texture coordinates stupidly
	// TODO: not do it so stupidly
	float wid = (float)buffer_width / (float)pot_width;
	float hed = (float)buffer_height / (float)pot_height;
	printf("wid: %f, hed: %f, bw: %d, bh: %d, pw: %d, ph: %d\n", wid, hed, buffer_width, buffer_height, pot_width, pot_height);
#ifdef FOR_ANDROID
	bufdata[2 * 4 * 3 + 0] = wid;
	bufdata[2 * 4 * 3 + 1] = hed;
	bufdata[2 * 4 * 3 + 2] = wid;
	bufdata[2 * 4 * 3 + 7] = hed;
#else
	bufdata[2 * 4 * 3 + 1] = hed;
	bufdata[2 * 4 * 3 + 2] = wid;
	bufdata[2 * 4 * 3 + 3] = hed;
	bufdata[2 * 4 * 3 + 4] = wid;
#endif

	self->local_frame = (unsigned char *) malloc(sizeof(unsigned char) * buffer_width * buffer_height * 3);
	if(!self->local_frame)
		GOFAIL(fail_noclean, "Unable to allocate RGB buffer");

	int i;
	//memset(self->local_frame, 0, buffer_width * buffer_height * 3);
	for(i = 0; i < buffer_width * buffer_height * 3; i += 3) {
	//for(i = 0; i < 2048 * 2048 * 3; i += 3) {
		self->local_frame[i + 0] = 255;
		self->local_frame[i + 1] = 0;
		self->local_frame[i + 2] = 255;
	}

	// Set "background" color; the color glClear() fills the screen with
	//glClearColor(1.0, 0.5, 1.0, 0.0);
	glClearColor(0.0, 0.0, 1.0, 1.0);GLDBG;

	// Draw only front faces
	glEnable(GL_CULL_FACE);GLDBG;
	glCullFace(GL_BACK);GLDBG;

	// Use depth testing when rendering
	glEnable(GL_DEPTH_TEST);GLDBG;

	// Create buffer name(s)
	glGenBuffers(4, self->buffers);GLDBG;

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_frame, "Unable to gen buffers");

	// Create texture name(s)
	glGenTextures(1, self->textures);GLDBG;
	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buffers, "Unable to gen textures");

	glBindTexture(GL_TEXTURE_2D, self->textures[0]);GLDBG;

	// Set a couple texture parameters, if unspecified the texture won't be rendered
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);GLDBG;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);GLDBG;

	// Allocate texture, NULL -> uninitialized
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pot_width, pot_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);GLDBG;
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_width, buffer_height, GL_RGB, GL_UNSIGNED_BYTE, self->local_frame);GLDBG;

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Unable to allocate frame texture");

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);GLDBG;

	// Bind our buffer 0 as ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[0]);GLDBG;

	// Copy our vertex data from 'bufdata' to the bound ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(bufdata), bufdata, GL_STATIC_DRAW);GLDBG;

	// Bind our buffer 1 as ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[1]);GLDBG;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indbufdata), indbufdata, GL_STATIC_DRAW);GLDBG;

	// Setup cube buffers
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[2]);GLDBG;
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubedata), cubedata, GL_STATIC_DRAW);GLDBG;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[3]);GLDBG;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeinds), cubeinds, GL_STATIC_DRAW);GLDBG;

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Unable to set buffer data");

	// Create shader programs and the shader components
	self->program = glCreateProgram();GLDBG;
	self->vert_shdr = glCreateShader(GL_VERTEX_SHADER);GLDBG;
	self->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);GLDBG;

	self->cube_program = glCreateProgram();GLDBG;
	self->cube_vert = glCreateShader(GL_VERTEX_SHADER);GLDBG;
	self->cube_frag = glCreateShader(GL_FRAGMENT_SHADER);GLDBG;

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Failed to gen shaders");

	// Length of the shader source code string
	GLint source_len;

	// Set the shaders' source codes
	source_len = strlen(vert_shader);
	glShaderSource(self->vert_shdr, 1, &vert_shader, &source_len);GLDBG;
	source_len = strlen(frag_shader);
	glShaderSource(self->frag_shdr, 1, &frag_shader, &source_len);GLDBG;

	source_len = strlen(cube_vert);
	glShaderSource(self->cube_vert, 1, &cube_vert, &source_len);GLDBG;
	source_len = strlen(cube_frag);
	glShaderSource(self->cube_frag, 1, &cube_frag, &source_len);GLDBG;

	char linklog[1000];

	// Compile shaders
	glCompileShader(self->vert_shdr);GLDBG;
	glGetShaderInfoLog(self->vert_shdr, 1000, NULL, linklog);
	//printf("vert shader log: %s\n", linklog);

	glCompileShader(self->frag_shdr);GLDBG;
	glGetShaderInfoLog(self->frag_shdr, 1000, NULL, linklog);
	//printf("frag shader log: %s\n", linklog);

	glCompileShader(self->cube_vert);GLDBG;
	glGetShaderInfoLog(self->cube_vert, 1000, NULL, linklog);
	//printf("vert shader log: %s\n", linklog);

	glCompileShader(self->cube_frag);GLDBG;
	glGetShaderInfoLog(self->cube_frag, 1000, NULL, linklog);
	//printf("frag shader log: %s\n", linklog);

	// Attach shader components to the shader program
	glAttachShader(self->program, self->vert_shdr);GLDBG;
	glAttachShader(self->program, self->frag_shdr);GLDBG;

	glAttachShader(self->cube_program, self->cube_vert);GLDBG;
	glAttachShader(self->cube_program, self->cube_frag);GLDBG;

	// Link the shader program
	glLinkProgram(self->program);GLDBG;
	glGetProgramInfoLog(self->program, 1000, NULL, linklog);
	//printf("link log: %s\n", linklog);

	glLinkProgram(self->cube_program);GLDBG;
	glGetProgramInfoLog(self->cube_program, 1000, NULL, linklog);
	//printf("link log: %s\n", linklog);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_shdr, "Unable to create shader program");

	// Find the vertex attribute locations (position and color)
	self->aPosition = glGetAttribLocation(self->program, "aPosition");GLDBG;
	self->aColor = glGetAttribLocation(self->program, "aColor");GLDBG;
	self->aUV = glGetAttribLocation(self->program, "aUV");GLDBG;

	if(self->aPosition == -1 || self->aColor == -1 || self->aUV == -1)
		GOFAIL(fail_buf_shdr, "Unable to find attribute locations");

	self->uTexture = glGetUniformLocation(self->program, "uTexture");GLDBG;

	if(self->uTexture == -1)
		GOFAIL(fail_buf_shdr, "Unable to find uniform locations");

	self->aCubePosition = glGetAttribLocation(self->cube_program, "aPosition");GLDBG;
	self->aCubeColor = glGetAttribLocation(self->cube_program, "aColor");GLDBG;

	if(self->aCubePosition == -1 || self->aCubeColor == -1)
		GOFAIL(fail_buf_shdr, "Unable to find cube attribute locations");

	self->uCubeRot = glGetUniformLocation(self->cube_program, "uRot");GLDBG;

	if(self->uCubeRot == -1)
		GOFAIL(fail_buf_shdr, "Unable to find uniform locations");

	// Unbind ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);GLDBG;

	// Unbind ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);GLDBG;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);GLDBG;

	self->valid = 1;
	self->cube_rot = 0;
	return 1;

fail_buf_shdr:
	log("Cleaning shaders due to fail");
	glDeleteShader(self->vert_shdr);
	glDeleteShader(self->frag_shdr);
	glDeleteProgram(self->program);

	glDeleteShader(self->cube_vert);
	glDeleteShader(self->cube_frag);
	glDeleteProgram(self->cube_program);
fail_buf_tex:
	log("Cleaning textures due to fail");
	glDeleteTextures(1, self->textures);
fail_buffers:
	log("Cleaning buffers due to fail");
	glDeleteBuffers(4, self->buffers);
fail_frame:
	free(self->local_frame);
fail_noclean:
	self->valid = 0;
	return 0;
#undef GOFAIL
}


int gles2t_draw(gles2t *self, unsigned char *buffer, int buffer_width, int buffer_height, int buffer_format, int bytes_per_pixel, void(*log)(const char *)) {
	// Convert NV21 to RGB
	//convert_nv21_frame(buffer, buffer_width, buffer_height, self->local_frame);

	// Fill the screen with the clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable our shader program
	glUseProgram(self->program);

	// Bind vertex data buffer (position vectors and colors)
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[0]);

	// Bind index buffer (vertex indices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[1]);

	// Setup buffer pointers (shader attributes)
	glVertexAttribPointer(self->aPosition, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	glEnableVertexAttribArray(self->aPosition);

	glVertexAttribPointer(self->aColor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(4 * 3 * 4));
	glEnableVertexAttribArray(self->aColor);

	glVertexAttribPointer(self->aUV, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(8 * 3 * 4));
	glEnableVertexAttribArray(self->aUV);

	// Setup texture/sampler
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->textures[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_width, buffer_height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glUniform1i(self->uTexture, 0);

	// Render 6 vertices as triangle(s) starting at offset 0
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	// Setup rendering for cube
	glUseProgram(self->cube_program);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[3]);

	glVertexAttribPointer(self->aCubePosition, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	glEnableVertexAttribArray(self->aCubePosition);

	glVertexAttribPointer(self->aCubeColor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(8 * 3 * 4));
	glEnableVertexAttribArray(self->aCubeColor);

	self->cube_rot += 0.1;
	while(self->cube_rot > 360.0)
		self->cube_rot -= 360.0;
	while(self->cube_rot < 0.0)
		self->cube_rot = self->cube_rot + 360.0;

	glUniform1f(self->uCubeRot, self->cube_rot);

	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 3 * 2 * 6, GL_UNSIGNED_SHORT, 0);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)(1 * 6 * 2));
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)(3 * 6 * 2));
	glDisable(GL_DEPTH_TEST);

	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	if(glGetError() != GL_NO_ERROR) {
		log("GLES2 draw failed");
		return 0;
	} else {
		//log("GLES2 draw succeeded");
		return 1;
	}
}

#ifdef __cplusplus
}
#endif
