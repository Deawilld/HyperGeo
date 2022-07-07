#pragma once

#include <vector>

static unsigned int DIMS = 4;

class VertexArray
{
protected:
	std::vector<double*> points;
	std::vector<std::vector<int>> lines;
	std::vector<int*> volumes;

public:
	VertexArray() {}

	VertexArray(VertexArray& arr) {
		addAll(arr);
	}

	void add(double* point) {
		points.push_back(point);
		lines.resize(points.size());
	}

	double* get(int point_index) {
		return points.at(point_index);
	}

	void addAll(VertexArray& arr) {
		for (int i = 0; i < arr.getSize(); i++) {
			points.push_back(copyPoint(arr.get(i)));
			lines.push_back(std::vector<int>(arr.getConnections(i)));
		}

		int old_length = getSize();
		for (auto volume : arr.volumes) {
			int* cvolume = new int[4];
			std::copy(volume, volume + 4, cvolume);
			for (int i = 0; i < 4; i++) {
				cvolume[i] += old_length;
			}
			volumes.push_back(cvolume);
		}
	}


	std::vector<int> getConnections(int point_index) {
		return lines.at(point_index);
	}

	int getSize() {
		return points.size();
	}

	std::vector <int*> getVolumes() {
		return volumes;
	}

	void connect(int point1, int point2) {
		lines.at(point1).push_back(point2-point1);
	}

	void move(unsigned int axis, double value) {
		if (axis >= DIMS) return;
		for (int i = 0; i < points.size(); i++) {
			points.at(i)[axis] += value;
		}
	}

	void rotate(unsigned int axis1, unsigned int axis2, double angle) {
		for (int i = 0; i < points.size(); i++) {
			double &a = points.at(i)[axis1];
			double &b = points.at(i)[axis2];
			double range = sqrt(a * a + b * b);
			double cur_angle = acos(a / range) * (signbit(b) ? -1 : 1);
			cur_angle += angle;
			a = range * cos(cur_angle);
			b = range * sin(cur_angle);
		}
	}

	~VertexArray() {
		for (int i = 0; i < points.size(); i++) {
			delete[] points.at(i);
		}
		for (int i = 0; i < volumes.size(); i++) {
			delete[] volumes.at(i);
		}
	}

	static double* copyPoint(double* point) {
		double* newPoint = new double[DIMS];
		for (int i = 0; i < DIMS; i++) {
			newPoint[i] = point[i];
		}
		return newPoint;
	}
};