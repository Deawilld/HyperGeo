#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "VertexArray.h"
#include "Viewport.h"
#include <chrono>
#include "Vector.h"

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

sf::RenderWindow window;

VertexArray testArray;

Viewport viewport;

bool mousedown = false;
int mouseX = 0, mouseY = 0;
float rotateSpeed = 1;
bool rotation = false;

void initGLWindow();
void drawScene();
void mouseDown(sf::Event::MouseButtonEvent);
void mouseMove(sf::Event::MouseMoveEvent);

void buildTestCube() {
	double size = 1;
	double *p = new double[DIMS];
	for (int i = 0; i < DIMS; i++) {
		if (i < 3) {
			p[i] = -size / 2;
		}else{
			p[i] = 0;
		}
	}

	testArray.add(p);
	for (int dim = 0; dim < DIMS; dim++) {
		VertexArray copy(testArray);
		copy.move(dim, size);
		for (int i = 0; i < testArray.getSize(); i++) {
			testArray.connect(i, i + testArray.getSize());
		}
		testArray.addAll(copy);
	}
}

double* point() {
	double* p = new double[DIMS];
	for (int i = 0; i < DIMS; i++) {
		p[i] = 0;
	}
	return p;
}

void buildTetraedr() {
	float a = 2, m = a/2, h;
	double* p = point();
	p[0] = a / 2;
	testArray.add(p);
	p = point();
	p[0] = -a / 2;
	testArray.add(p);
	testArray.connect(0, 1);
	for (int i = 1; i < DIMS; i++) {
		h = sqrt(a * a - m * m);
		p = point();
		p[i] = h;
		testArray.add(p);
		for (int j = 0; j < testArray.getSize() - 1; j++) {
			testArray.connect(j, testArray.getSize() - 1);
		}
		m = (h * h + m * m) / (2 * h);
		testArray.move(i, m-h);
	}
}

int main(int argc, char** argv) {
	Vector3 v;
	std::cout << v[0];
	window.create(sf::VideoMode(800, 600), "HyperGeo", sf::Style::Default);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	viewport.setup();
	viewport.resize(3, 1, (float)window.getSize().x / window.getSize().y);

	buildTetraedr();
	viewport.shape = &testArray;

	viewport.draw();
	bool doPainting = true;

	auto timer = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds delay(20);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				viewport.resize(3, 1, (float)window.getSize().x / window.getSize().y);
				break;
			case sf::Event::MouseButtonPressed:
				mouseDown(event.mouseButton);
				break;
			case sf::Event::MouseMoved:
				mouseMove(event.mouseMove);
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) mousedown = false;
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Space) {
					rotation = !rotation;
				}
			}
			doPainting = true;
		}

		auto now = std::chrono::high_resolution_clock::now();
		if (now - timer >= delay) {
			timer = now;

			if (rotation) {
				testArray.rotate(0, 3, -0.005);
			}
			doPainting = true;
		}

		if (doPainting) {
			viewport.draw();
			window.display();
			doPainting = false;
		}
	}

	return 0;
}

void initGLWindow() {
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5, 5, -5, 5, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -3);

	glClearColor(0.0, 0.0, 1.0, 1.0);
}

/*void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4b(255, 255, 255, 255);
	for (int i = 0; i < testArray.getSize(); i++) {
		for (int j = 0; j < testArray.getConnections(i).size(); j++) {
			double* point = testArray.get(i);
			glVertex3d(point[0], point[1], point[2]);
			point = testArray.get(testArray.getConnections(i).at(j)+i);
			glVertex3d(point[0], point[1], point[2]);
		}
	}
	glEnd();
}*/

void mouseDown(sf::Event::MouseButtonEvent event) {
	if (event.button == sf::Mouse::Left) {
		mouseX = event.x;
		mouseY = event.y;
		mousedown = true;
	}
}

void mouseMove(sf::Event::MouseMoveEvent event) {
	if (mousedown) {
		int dx = event.x - mouseX;
		int dy = event.y - mouseY;
		mouseX = event.x;
		mouseY = event.y;

		viewport.yaw += dx * rotateSpeed;
		viewport.pitch += dy * rotateSpeed;
	}
}