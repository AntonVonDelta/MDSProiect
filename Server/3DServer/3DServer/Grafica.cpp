#include "Grafica.h"

Grafica::Grafica() {
	GLuint fbo, render_buf;
	glReadPixels();

	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &render_buf);
	glBindRenderbuffer(render_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_BGRA8, width, height);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);
}

Grafica::~Grafica() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &render_buf);
}