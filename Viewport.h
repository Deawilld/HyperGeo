#include "VertexArray.h"
#include "SFML/OpenGL.hpp"

#pragma once

enum Projection_type {
	ORTHO,
	BIHYPERBOLIC,
	CUT,

};

class Viewport
{
public:
	VertexArray* shape;

	Projection_type projection = ORTHO;

	float yaw, pitch;

	void setup() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1, 1, -1, 1, 1, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0, 0, -2);
		glPushMatrix();

		glClearColor(0.0, 0.0, 0.0, 1.0);
	}

	void resize(float range, float edge, float x_to_y) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (x_to_y >= 1) {
			glFrustum(-edge*x_to_y, edge*x_to_y, -edge, edge, 1, 10);
		}
		else {
			glFrustum(-edge, edge, -edge / x_to_y, edge / x_to_y, 1, 10);
		}
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glLoadIdentity();
		glTranslatef(0, 0, -range);
		glPushMatrix();
	}

	void drawProjection() {
		glClear(GL_COLOR_BUFFER_BIT);
		
		double minw=shape->get(0)[3], maxw=shape->get(0)[3];
		double** points = new double*[shape->getSize()];
		for (int i = 0; i < shape->getSize(); i++) {
			switch (DIMS) {
			case 3:
				points[i] = shape->get(i);
				break;
			case 4:
			default:
				points[i] = new double[3];
				for (int j = 0; j < 3; j++) {
					points[i][j] = shape->get(i)[j];
					if (projection == BIHYPERBOLIC) {
						points[i][j] /= (fabs(shape->get(i)[3]) + 1);
					}
				}
				std::swap(points[i][1], points[i][2]); //swap Y and Z
				if (shape->get(i)[3] > maxw) maxw = shape->get(i)[3];
				if (shape->get(i)[3] < minw) minw = shape->get(i)[3];
				break;
			}
		}

		glBegin(GL_LINES);
		glColor3ub(0, 255, 0);
		for (int v1 = 0; v1 < shape->getSize(); v1++) {
			for (int i = 0; i < shape->getConnections(v1).size(); i++) {
				double*& p1 = points[v1];
				double*& p2 = points[v1 + shape->getConnections(v1).at(i)];
				pickColor(shape->get(v1), minw, maxw);
				glVertex3d(p1[0], p1[1], p1[2]);
				pickColor(shape->get(v1 + shape->getConnections(v1).at(i)), minw, maxw);
				glVertex3d(p2[0], p2[1], p2[2]);
			}
		}
		glEnd();

		applyTransform(); 
	}

	void drawCut() {
		glClear(GL_COLOR_BUFFER_BIT);

		//every element of *faces* is a pointer to an array that represents a triangle of 3-dimensional points.
		//the array is 9 elements long. A row of 3 values represents a 3D point * 3 points in each array.
		std::vector<double*> faces; 
		std::vector<int*> volumes = shape->getVolumes();
		for (int* vol : volumes) {
		}
	}

	void draw() {
		drawProjection();
	}

	void pickColor(double* point, double min, double max) {
		uint8_t color;
		color = 255 * (point[3] - min) / (max - min);
		glColor3ub(color, 0, 255 - color);
	}

	void applyTransform() {
		yaw = loopContract(yaw, 0, 360);
		pitch = contract(pitch, -90, 90);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPushMatrix();

		glRotatef(pitch, 1, 0, 0);
		glRotatef(yaw, 0, 1, 0);	
	}

	float contract(float val, float min, float max) {
		if (val > max) {
			val = max;
		}
		else if (val < min) {
			val = min;
		}
		return val;
	}

	float loopContract(float val, float min, float max) {
		if (val > max) {
			val = min;
		}
		else if (val < min) {
			val = max;
		}
		return val;
	}
};

