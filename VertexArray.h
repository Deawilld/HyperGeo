#pragma once

#include <vector>
#include "Vector.h"

using Volume = Vector<4, int>;

class VertexArray
{
protected:
	std::vector<VectorND> points;
	std::vector<std::vector<int>> lines;
	std::vector<Volume> volumes;

public:
	VertexArray() {}

	VertexArray(VertexArray& arr) {
		addAll(arr);
	}

	VertexArray(VertexArray& arr, int start, int end) {
		add(arr, start, end);
	}

	void add(VectorND point) {
		points.push_back(point);
		lines.resize(points.size());
	}

	VectorND& get(int point_index) {
		return points.at(point_index);
	}

	//does not copy volumes!
	//No bounds checking!
	void add(VertexArray& arr, int start, int end) {
		int old_length = getSize();

		for (int i = start; i < end; i++) {
			points.push_back(VectorND(arr.get(i)));
			lines.push_back(std::vector<int>(arr.getConnections(i)));
		}
	}

	void addAll(VertexArray& arr) {
		int old_length = getSize();
		add(arr, 0, arr.getSize());
		for (auto volume : arr.volumes) {
			Volume cvolume(volume);
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

	std::vector <Volume>& getVolumes() {
		return volumes;
	}

	void addVolume(Volume v) {
		volumes.push_back(v);
	}

	void connect(int point1, int point2) {
		lines.at(point1).push_back(point2-point1);
	}

	void move(unsigned int axis, double value) {
		if (axis >= MAX_DIMS) return;
		for (int i = 0; i < points.size(); i++) {
			points.at(i)[axis] += value;
		}
	}

	void rotate(unsigned int axis1, unsigned int axis2, double angle) {
		for (int i = 0; i < points.size(); i++) {
			double &a = points.at(i)[axis1];
			double &b = points.at(i)[axis2];
			double range = sqrt(a * a + b * b);
			if (range == 0) continue;
			double cur_angle = acos(a / range) * (signbit(b) ? -1 : 1);
			cur_angle += angle;
			a = range * cos(cur_angle);
			b = range * sin(cur_angle);
		}
	}

	void scale(double factor) {
		for (VectorND& point : points) {
			point *= factor;
		}
	}

	void scale(int axis, double factor) {
		for (VectorND& point : points) {
			point[axis] *= factor;
		}
	}

	void erase(int index) {
		points.erase(points.begin()+index);
		lines.erase(lines.begin()+index);
	}
};