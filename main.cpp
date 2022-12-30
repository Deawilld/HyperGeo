#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "VertexArray.h"
#include "Viewport.h"
#include <chrono>
#include "Vector.h"
#include "CommandParser.h"
#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <math.h>

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

sf::RenderWindow window;

VertexArray testArray;

Viewport viewport;

CommandParser cmdparser(&viewport);

sf::Font font;

bool mousedown = false;
int mouseX = 0, mouseY = 0;
float rotateSpeed = 1;

void initGLWindow();
void drawScene();
void mouseDown(sf::Event::MouseButtonEvent);
void mouseMove(sf::Event::MouseMoveEvent);

//shape_generators.cpp
void buildTestCube(VertexArray& array, const int dims);
void buildTetraedr(VertexArray& array, const int dims);
void buildSphere(VertexArray& array, const int dims);


int main(int argc, char** argv) {
	if (!font.loadFromFile("calibri.ttf")) {
		std::cout << "Failed to load the font!";
	}

	window.create(sf::VideoMode(800, 600), "HyperGeo", sf::Style::Default);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	viewport.setup();
	viewport.resize(3, 1, (float)window.getSize().x / window.getSize().y);

	sf::Text text_command_line;
	text_command_line.setFont(font);
	text_command_line.setFillColor(sf::Color::White);
	sf::Text text_command_out = text_command_line;
	sf::Text text_vp_status = text_command_line;
	text_command_line.setPosition(20, 40);
	text_command_out.setPosition(20, 75);
	text_vp_status.setPosition(20, 5);

	buildSphere(testArray, 4);
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
				case sf::Event::KeyReleased:
					using namespace sf;
					switch (event.key.code) {
					case Keyboard::Up:
					case Keyboard::Down:
						viewport.movement_direction = Keyboard::isKeyPressed(Keyboard::Up) - Keyboard::isKeyPressed(Keyboard::Down);
						break;

					case Keyboard::Right:
					case Keyboard::Left:
						viewport.rotation_direction = -Keyboard::isKeyPressed(Keyboard::Left) + Keyboard::isKeyPressed(Keyboard::Right);
						break;
					}
					break;
					
				case sf::Event::TextEntered:
					char c;
					sf::Utf32::encodeAnsi(event.text.unicode, &c);
					if (c != 0) {
						cmdparser.input(c);
					}
					break;
			}
			doPainting = true;
		}

		auto now = std::chrono::high_resolution_clock::now();
		if (now - timer >= delay) {
			timer = now;
			
			if (viewport.movement_direction != 0) {
				viewport.moveArray(0.01);
				doPainting = true;
			}
			if (viewport.rotation_direction != 0) {
				viewport.rotateArray(M_PI*0.005);
				doPainting = true;
			}
		}

		if (doPainting) {
			viewport.draw();

			text_command_out.setString(cmdparser.output_bar);
			text_command_line.setString(cmdparser.cmdline_bar);
			text_vp_status.setString(viewport.vp_status_bar);

			window.pushGLStates();
			
			window.draw(text_command_line);
			window.draw(text_command_out);
			window.draw(text_vp_status);
			
			window.popGLStates();

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