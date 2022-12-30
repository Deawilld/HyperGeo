#include "VertexArray.h"
#include "SFML/OpenGL.hpp"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#include <iostream>
#endif

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

	Projection_type projection = CUT;

	float yaw=0, pitch=0;
	
	int8_t rotation_direction = 0, movement_direction = 0; //take values from -1 to 1
	std::string vp_status_bar = "";

protected:
	float rotation_history = 0;
	float movement_history = 0;
	int rotation_axis[2]{ 0, 1 };
	int movement_axis = 3;

public:
	void setup() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1, 1, -1, 1, 1, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0, 0, -2);
		glPushMatrix();

		glClearColor(0.0, 0.0, 0.0, 1.0);

		updateStatusBar();
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
		Vector3D* points = new Vector3D[shape->getSize()];
		for (int i = 0; i < shape->getSize(); i++) {
			switch (MAX_DIMS) {
			case 3:
				points[i] = shape->get(i).cast<3>();
				break;
			case 4:
			default:
				points[i] = (Vector3D)shape->get(i);
				if (projection == BIHYPERBOLIC) {
					for (int j = 0; j < 3; j++) {
						points[i][j] /= (fabs(shape->get(i)[3]) + 1);
					}
				}

				std::swap(points[i][1], points[i][2]); //swap Y and Z
				if (shape->get(i)[3] > maxw) maxw = shape->get(i)[3];
				if (shape->get(i)[3] < minw) minw = shape->get(i)[3];
				break;
			}
		}

		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3ub(0, 255, 0);
		for (int v1 = 0; v1 < shape->getSize(); v1++) {
			for (int i = 0; i < shape->getConnections(v1).size(); i++) {
				Vector3D& p1 = points[v1];
				Vector3D& p2 = points[v1 + shape->getConnections(v1).at(i)];
				pickColor(shape->get(v1), minw, maxw);
				glVertex3d(p1[0], p1[1], p1[2]);
				pickColor(shape->get(v1 + shape->getConnections(v1).at(i)), minw, maxw);
				glVertex3d(p2[0], p2[1], p2[2]);
			}
		}
		glEnd();

		applyTransform();
	}

	bool liesIn3D(VectorND vec) {
		bool in3D = true;
		for (int j = 3; j < MAX_DIMS; j++) {
			if (vec[j] != 0) {
				in3D = false;
				break;
			}
		}
		return in3D;
	}

	void drawCut() {
		glClear(GL_COLOR_BUFFER_BIT);

		std::vector<Vector3D> faces;
		std::vector<Volume>& volumes = shape->getVolumes();
		for (const Volume& vol : volumes) {
			std::vector<Vector3D> dots;
			bool all3D = true;
			for (int i = 0; i < 4; i++) {
				const VectorND& p1 = shape->get(vol[i]);
				if (liesIn3D(p1)) {
					dots.push_back(p1.cast<3>());
					continue;
				}
				else {
					all3D = false;
					if (p1[3] > 0) continue;
				}
				for (int j = 0; j < 4; j++) {
					const VectorND& p2 = shape->get(vol[j]);
					if (p2[3] <= 0) continue;

					VectorND d = p2 - p1;
					//if (d[3] == 0) continue;

					VectorND r = d * (-(p1[3] / d[3])) + p1;
					//if (liesIn3D(r)) {
						dots.push_back((Vector3D)r);
					//}
				}
			}
			//now we've picked all points that are volume's intersection with our 3D hypersurface

			switch (dots.size()) {
			case 3:
				faces.insert(faces.end(), dots.begin(), dots.end());
				break;
			case 4:
				if (all3D) {
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							if (j == i)continue;
							faces.push_back(dots.at(j));
						}
					}
				}
				else {
					faces.push_back(dots[0]);
					faces.push_back(dots[1]);
					faces.push_back(dots[3]);
					faces.push_back(dots[0]);
					faces.push_back(dots[3]);
					faces.push_back(dots[2]);
				}
			}
		}

		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3ub(0, 0, 255);
		glBegin(GL_TRIANGLES);
		for (Vector3D& vertex : faces) {
			glVertex3dv(vertex.components);
		}
		glEnd();

		/*//This can draw simply triangle polygons. I'll leave it here for some time
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor4ub(200, 200, 0, 64);
		glBegin(GL_TRIANGLES);
		for (Vector3D& vertex : faces) {
			glVertex3dv(vertex.components);
		}
		glEnd();
		*/
		for (int i = 0; i < faces.size(); i += 3) {
			Vector3D face[3]{ faces.at(i), faces.at(i + 1), faces.at(i + 2) };
			Vector3D center = (face[0] + face[1] + face[2]) * (double(1)/3);
			for (int i = 0; i < 3; i++) {
				face[i] = center + (face[i] - center) * 0.85;
			}
			
			glPointSize(1);
			glColor4ub(220, 220, 0, 192);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_POINT);
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < 4; j++) {
				int k = j % 3;
				glVertex3dv(face[k].components);
				glVertex3dv(faces.at(i + k).components);
			}
			glEnd();

			glColor4ub(150, 150, 0, 192);
			glPolygonMode(GL_FRONT, GL_POINT);
			glPolygonMode(GL_BACK, GL_FILL);
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < 4; j++) {
				int k = j % 3;
				glVertex3dv(face[k].components);
				glVertex3dv(faces.at(i + k).components);
			}
			glEnd();
			
		}

		applyTransform();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void draw() {
		switch (projection) {
		case ORTHO:
		case BIHYPERBOLIC:
			drawProjection();
			break;
		case CUT:
			drawCut();
			break;
		}
	}

	void pickColor(VectorND point, double min, double max) {
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

	void updateStatusBar() {
		vp_status_bar = "movement [" + std::to_string(movement_axis) + "] : " + std::to_string(movement_history) + "   "
			+ "rotation [" + std::to_string(rotation_axis[0]) + ", " + std::to_string(rotation_axis[1]) + "] : "
			+ std::to_string(rotation_history);
	}

	void setMoveAxis(int axis) {
		movement_axis = axis;
		movement_history = 0;
		updateStatusBar();
	}

	void setRotationAxis(int a1, int a2) {
		rotation_axis[0] = a1;
		rotation_axis[1] = a2;
		rotation_history = 0;
		updateStatusBar();
	}

	float getRotationHistory() {
		return rotation_history;
	}

	float getMovementHistory() {
		return movement_history;
	}

	void moveArray(double value) {
		value *= movement_direction;
		shape->move(movement_axis, value);
		movement_history += value;
		updateStatusBar();
	}

	void rotateArray(double value) {
		value *= rotation_direction;
		shape->rotate(rotation_axis[0], rotation_axis[1], value);
		rotation_history += value;
		if (rotation_history > M_PI) {
			rotation_history -= 2*M_PI;
		}
		else if (rotation_history < -M_PI) {
			rotation_history += 2*M_PI;
		}
		updateStatusBar();
	}
};

