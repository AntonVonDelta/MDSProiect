#include "Grafica.h"

Grafica::Grafica() {
	buffer = new char[width * height * 4];
}

Grafica::~Grafica() {
	delete[] buffer;
}
Grafica::Grafica(const Grafica& other) {
	buffer = new char[width * height * 4];
	move_amount = other.move_amount;
	rotate_amount = other.rotate_amount;
	width = other.width;
	height = other.height;
	object_definition = other.object_definition;

	if (other.init_succesful) {
		init();
		drawScene();
	}
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
	// Reset the internal rotation/translation matrixes 
	glLoadIdentity();

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


void Grafica::drawScene(void) {
	for (const Triangle& triangle : object_definition) {
		glBegin(GL_TRIANGLES);
		if (triangle.hasNormal) glNormal3f(triangle.normals[0].x, triangle.normals[0].y, triangle.normals[0].z);
		glVertex3f(triangle.v[0].x, triangle.v[0].y, triangle.v[0].z);
		if (triangle.hasNormal) glNormal3f(triangle.normals[1].x, triangle.normals[1].y, triangle.normals[1].z);
		glVertex3f(triangle.v[1].x, triangle.v[1].y, triangle.v[1].z);
		if (triangle.hasNormal) glNormal3f(triangle.normals[2].x, triangle.normals[2].y, triangle.normals[2].z);
		glVertex3f(triangle.v[2].x, triangle.v[2].y, triangle.v[2].z);
		glEnd();
	}
}

void Grafica::nextScene() {
	if (!init_succesful) return;
	
	// Select current context
	glfwMakeContextCurrent(window);

	//Before drawing
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();

	// Copy pixel memory to buffer
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	// Return to onscreen rendering:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Swap the buffers
	//glutSwapBuffers();
}

void Grafica::moveScene(int direction) {
	// Select current context
	glfwMakeContextCurrent(window);

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
	// Select current context
	glfwMakeContextCurrent(window);

	switch (direction) {
		case 0:
			glRotatef(rotate_amount, 0.0, 1.0, 0.0);
			break;

		case 1:
			glRotatef(-rotate_amount, 1.0, 0.0, 0.0);
			break;
	}
}

void Grafica::loadObject(string input) {
	// Process data and fill vertexes in memory
	vector<Vertex> corners;
	vector<Normal> normals;
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
		if (mode == "vn") {
			// Process the vertex
			Normal temp;
			line_stream >> temp.x;
			line_stream >> temp.y;
			line_stream >> temp.z;

			if (line_stream.fail() || line_stream.bad()) throw runtime_error(string("Error on reading normal data: ") + line);
			normals.push_back(temp);
		}
		if (mode == "f") {
			Triangle temp;
			int vertex_index;
			
			temp.hasNormal = false;

			try {
				for (int i = 0; i < 3; i++) {
					string coord_group;
					line_stream >> coord_group;
					istringstream coord_stream(coord_group);
					int vertex_index=-1, texture_index=-1, normal_index=-1;

					char junk;
					coord_stream >> vertex_index>>junk;

					if (junk == '/') {
						if (coord_stream.peek() != '/') {
							coord_stream >> texture_index;
							if (coord_stream.eof()) texture_index = -1;
						}
						coord_stream >> junk;

						if (junk == '/' && coord_stream.peek() != ' ') {
							coord_stream >> normal_index;
							if (coord_stream.eof()) normal_index = -1;
						}
					}
					

					temp.v[i] = corners.at(vertex_index-1);

					if (normal_index != -1) {
						temp.normals[i] = normals.at(normal_index-1);
						temp.hasNormal = true;
					}
				}

			} catch (out_of_range e) {
				throw runtime_error(string("Index out of bounds on reading triangle data: ") + line);
			}

			if (line_stream.fail() || line_stream.bad()) throw runtime_error(string("Error on reading triangle data: ") + line);

			scene_data.push_back(temp);
		}

		continue;
	}

	if (scene_data.size() == 0) throw runtime_error("Error on final size: 0 triangles");

	object_definition = scene_data;
}