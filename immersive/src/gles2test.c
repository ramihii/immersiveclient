#include <GLES2/gl2.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "gles2test.h"

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

static const char *vert_shader =
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
	"attribute vec3 aPosition;"
	"attribute vec3 aColor;"
	""
	"uniform mediump float uRot;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  vColor = aColor;"
	"  vec2 rotated = vec2(aPosition.x * cos(uRot) - aPosition.z * sin(uRot), aPosition.z * cos(uRot) + aPosition.x * sin(uRot));"
	"  gl_Position = vec4(rotated.x, aPosition.y, rotated.y, 1.0);"
	"}";

static const char *cube_frag =
	"precision mediump float;"
	""
	"varying vec3 vColor;"
	""
	"void main() {"
	"  gl_FragColor = vec4(vColor, 1.0);"
	"}";

#define wid (1920.0 / 2048.0)
#define hed (1080.0 / 2048.0)

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

	wid, hed,
	wid, 0.0,
	0.0, 0.0,
	0.0, hed,
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
	bufdata[2 * 4 * 3 + 0] = wid;
	bufdata[2 * 4 * 3 + 1] = hed;
	bufdata[2 * 4 * 3 + 2] = wid;
	bufdata[2 * 4 * 3 + 7] = hed;

	self->local_frame = (unsigned char *) malloc(sizeof(unsigned char) * buffer_width * buffer_height * 3);
	if(!self->local_frame)
		GOFAIL(fail_noclean, "Unable to allocate RGB buffer");

	int i;
	//memset(self->local_frame, 0, buffer_width * buffer_height * 3);
	for(i = 0; i < buffer_width * buffer_height * 3; i += 3) {
	//for(i = 0; i < 2048 * 2048 * 3; i += 3) {
		self->local_frame[i] = 255;
		self->local_frame[i + 1] = 0;
		self->local_frame[i + 2] = 255;
	}

	// Set "background" color; the color glClear() fills the screen with
	//glClearColor(1.0, 0.5, 1.0, 0.0);
	glClearColor(0.0, 0.0, 1.0, 1.0);

	// Draw only front faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Use depth testing when rendering
	glEnable(GL_DEPTH_TEST);

	// Create buffer name(s)
	glGenBuffers(4, self->buffers);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_frame, "Unable to gen buffers");

	// Create texture name(s)
	glGenTextures(1, self->textures);
	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buffers, "Unable to gen textures");

	glBindTexture(GL_TEXTURE_2D, self->textures[0]);

	// Set a couple texture parameters, if unspecified the texture won't be rendered
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Allocate texture, NULL -> uninitialized
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pot_width, pot_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_width, buffer_height, GL_RGB, GL_UNSIGNED_BYTE, self->local_frame);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Unable to allocate frame texture");

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Bind our buffer 0 as ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[0]);

	// Copy our vertex data from 'bufdata' to the bound ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(bufdata), bufdata, GL_STATIC_DRAW);

	// Bind our buffer 1 as ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[1]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indbufdata), indbufdata, GL_STATIC_DRAW);

	// Setup cube buffers
	glBindBuffer(GL_ARRAY_BUFFER, self->buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubedata), cubedata, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->buffers[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeinds), cubeinds, GL_STATIC_DRAW);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Unable to set buffer data");

	// Create shader programs and the shader components
	self->program = glCreateProgram();
	self->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
	self->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);

	self->cube_program = glCreateProgram();
	self->cube_vert = glCreateShader(GL_VERTEX_SHADER);
	self->cube_frag = glCreateShader(GL_FRAGMENT_SHADER);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_tex, "Failed to gen shaders");

	// Length of the shader source code string
	GLint source_len;

	// Set the shaders' source codes
	source_len = strlen(vert_shader);
	glShaderSource(self->vert_shdr, 1, &vert_shader, &source_len);
	source_len = strlen(frag_shader);
	glShaderSource(self->frag_shdr, 1, &frag_shader, &source_len);

	source_len = strlen(cube_vert);
	glShaderSource(self->cube_vert, 1, &cube_vert, &source_len);
	source_len = strlen(cube_frag);
	glShaderSource(self->cube_frag, 1, &cube_frag, &source_len);

	// Compile shaders
	glCompileShader(self->vert_shdr);
	glCompileShader(self->frag_shdr);

	glCompileShader(self->cube_vert);
	glCompileShader(self->cube_frag);

	// Attach shader components to the shader program
	glAttachShader(self->program, self->vert_shdr);
	glAttachShader(self->program, self->frag_shdr);

	glAttachShader(self->cube_program, self->cube_vert);
	glAttachShader(self->cube_program, self->cube_frag);

	// Link the shader program
	glLinkProgram(self->program);
	glLinkProgram(self->cube_program);

	if(glGetError() != GL_NO_ERROR)
		GOFAIL(fail_buf_shdr, "Unable to create shader program");

	// Find the vertex attribute locations (position and color)
	self->aPosition = glGetAttribLocation(self->program, "aPosition");
	self->aColor = glGetAttribLocation(self->program, "aColor");
	self->aUV = glGetAttribLocation(self->program, "aUV");

	if(self->aPosition == -1 || self->aColor == -1 || self->aUV == -1)
		GOFAIL(fail_buf_shdr, "Unable to find attribute locations");

	self->uTexture = glGetUniformLocation(self->program, "uTexture");

	if(self->uTexture == -1)
		GOFAIL(fail_buf_shdr, "Unable to find uniform locations");

	self->aCubePosition = glGetAttribLocation(self->cube_program, "aPosition");
	self->aCubeColor = glGetAttribLocation(self->cube_program, "aColor");

	if(self->aCubePosition == -1 || self->aCubeColor == -1)
		GOFAIL(fail_buf_shdr, "Unable to find cube attribute locations");

	self->uCubeRot = glGetUniformLocation(self->cube_program, "uRot");

	if(self->uCubeRot == -1)
		GOFAIL(fail_buf_shdr, "Unable to find uniform locations");

	// Unbind ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

int gles2t_draw(gles2t *self, unsigned char *buffer, int buffer_width, int buffer_height, int buffer_format, int bytes_per_pixel, void(*log)(const char *)) {
	// Convert NV21 to RGB
	convert_nv21_frame(buffer, buffer_width, buffer_height, self->local_frame);

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
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_width, buffer_height, GL_RGB, GL_UNSIGNED_BYTE, self->local_frame);
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
		log("GLES2 draw succeeded");
		return 1;
	}
}

#ifdef __cplusplus
}
#endif
