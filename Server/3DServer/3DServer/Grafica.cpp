#include "Grafica.h"

Grafica::Grafica() {
	buffer = new char[width * height * 4];
}

Grafica::~Grafica() {
	delete[] buffer;
}

void Grafica::destroy() {
	if (init_succesful) {
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffers(2, render_buf);

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

bool Grafica::init() {
	bool error = false;

	// Make sure it wasn't already init'ed
	if (init_succesful) return false;

	if (!glfwInit()) {
		// Initialization failed
		return false;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, "Hidden window", NULL, NULL);
	if (!window) {
		// Window or OpenGL context creation failed
		glfwTerminate();
	}

	// Select current context
	glfwMakeContextCurrent(window);

	// Create Frame buffers
	glewInit();

	glGenFramebuffers(1, &fbo);			// Frame buffer used for drawing on
	glGenRenderbuffers(2, render_buf);	// We need two render buffers for color and depth

	glBindRenderbuffer(GL_RENDERBUFFER, render_buf[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buf[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf[0]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buf[1]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		error = true;
	}

	if (!error) {
		initScene();
		init_succesful = true;
	}

	return !error;
}

void Grafica::setSize(int window_width, int window_height) {
	width = window_width;
	height = window_height;

	// Update internal buffer
	delete[] buffer;
	buffer = new char[width * height * 4];

	if (!init_succesful) return;
	glfwSetWindowSize(window, width, height);
}

void Grafica::initScene() {


	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */

	/* Adjust cube position to be asthetic angle. */
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(-20, 0.0, 0.0, 1.0);
}


void Grafica::drawBox(void) {
	int i;

	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void Grafica::nextScence() {
	if (!init_succesful) return;

	//Before drawing
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawBox();

	// Copy pixel memory to buffer
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	// Return to onscreen rendering:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Swap the buffers
	//glutSwapBuffers();
}

void Grafica::moveScene(int direction) {
	switch (direction) {
		case 0:
			glTranslatef(0.0, 0.0, move_amount);
			break;

		case 1:
			glTranslatef(0.0, 0.0, -move_amount);
			break;

		case 2:
			glTranslatef(move_amount, 0.0, 0.0);
			break;

		case 3:
			glTranslatef(-move_amount, 0.0, 0.0);
			break;

		case 4:
			glTranslatef(0.0, move_amount, 0.0);
			break;

		case 5:
			glTranslatef(0.0, -move_amount, 0.0);
			break;
	}

}

void Grafica::rotateScene(int direction) {
	switch (direction) {
		case 0:
			glRotatef(rotate_amount, 1.0, 0.0, 0.0);
			break;

		case 1:
			glRotatef(-rotate_amount, 1.0, 0.0, 0.0);
			break;
	}
}

void Grafica::loadObject(string input) {
	// Process data and fill vertexes in memory
	vector<Vertex> corners;
	vector<Triangle> scene_data;

	istringstream stream(input);
	string line;


	while (getline(stream, line)) {
		istringstream line_stream(line);
		string mode;
		line_stream >> mode;

		if (mode == "v") {
			// Process the vertex
			Vertex temp;
			line_stream >> temp.x;
			line_stream >> temp.y;
			line_stream >> temp.z;

			if (line_stream.fail() || line_stream.bad()) throw runtime_error(string("Error on reading vertex data: ") + line);
			corners.push_back(temp);
		}
		if (mode == "f") {
			Triangle temp;
			int vertex_index;

			line_stream >> vertex_index;
			temp.v[0] = corners[vertex_index - 1];

			line_stream >> vertex_index;
			temp.v[1] = corners[vertex_index - 1];

			line_stream >> vertex_index;
			temp.v[2] = corners[vertex_index - 1];

			if (line_stream.fail() || line_stream.bad()) throw runtime_error(string("Error on reading triangle data: ") + line);

			scene_data.push_back(temp);
		}

		continue;
	}

	if (scene_data.size() == 0) throw runtime_error("Error on final size: 0 triangles");

	object_definition = scene_data;
}