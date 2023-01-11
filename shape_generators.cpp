#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "math.h"
#include "VertexArray.h"


void buildTestCube(VertexArray& array, const int dims) {
	double size = 1;
	VectorND p;
	for (int i = 0; i < dims; i++) {
		if (i < 3) {
			p[i] = -size / 2;
		}
	}

	array.add(p);
	for (int dim = 0; dim < dims; dim++) {
		VertexArray copy(array);
		copy.move(dim, size);
		for (int i = 0; i < array.getSize(); i++) {
			array.connect(i, i + array.getSize());
		}
		int volSize = array.getVolumes().size();
		array.addAll(copy);

		if (dim == 2) {
			array.addVolume(Volume{ 0, 3, 5, 6 });
			array.addVolume(Volume{ 4, 0, 5, 6 });
			array.addVolume(Volume{ 1, 0, 3, 5 });
			array.addVolume(Volume{ 2, 0, 3, 6 });
			array.addVolume(Volume{ 7, 3, 6, 5 });
		}
		else if (dim > 2) {
			for (int i = 0; i < volSize; i++) {
				//Each side of a tetrahedron is a triangle.
				//Each triandle when shifted forms a triangle prism.
				//For each side we fill the prism with a pre-counted way.
				for (uint8_t j = 0; j < 4; j++) {
					int map[3];
					for (int k = 0; k < 3; k++) {
						map[k] = k < j ? k : k + 1;
					}

					Volume v = array.getVolumes().at(i);
					Volume u = array.getVolumes().at(i + volSize);
					array.addVolume(Volume{ v[map[0]], v[map[1]], v[map[2]], u[map[0]] });
					array.addVolume(Volume{ v[map[1]], u[map[1]], u[map[2]], u[map[0]] });
					array.addVolume(Volume{ v[map[2]], u[map[2]], v[map[1]], u[map[0]] });
				}
			}
		}
	}
}

void buildTetraedr(VertexArray& array, const int dims) {
	float a = 2, m = a / 2, h;
	VectorND p;
	p[0] = a / 2;
	array.add(p);
	p = VectorND();
	p[0] = -a / 2;
	array.add(p);
	array.connect(0, 1);
	for (int i = 1; i < dims; i++) {
		h = sqrt(a * a - m * m);
		p = VectorND();
		p[i] = h;

		const int arrsize = array.getSize();
		array.add(p);

		for (int a = 0; a < arrsize; a++) {
			array.connect(a, arrsize);
			for (int b = a + 1; b < arrsize; b++) {
				for (int c = b + 1; c < arrsize; c++) {
					array.addVolume(Volume{ arrsize, a, b, c });
				}
			}
		}
		m = (h * h + m * m) / (2 * h);
		array.move(i, m - h);
	}
}

void buildSphere(VertexArray& array, int dims) {
	array.add(VectorND());
	float R = 1;
	VectorND A;
	A[0] = R;
	VectorND B;
	B[0] = -R;
	array.add(A);
	array.add(B);
	int steps = 3;
	float stepsize = R / steps;
	for (int d = 1; d < dims; d++) {
		VertexArray copy(array);
		copy.erase(0);
		int baseSize = copy.getSize();
		for (int step = 1; step < steps; step++) {
			VertexArray ring(copy);
			double angle = M_PI_2 * step / steps;
			float rscale = cos(angle);
			ring.scale(rscale);
			ring.move(d, R * sin(angle));
			int i = array.getSize();
			array.addAll(ring);
			for (; i < array.getSize(); i++) {
				array.connect(i - baseSize, i);
			}
		}
		VectorND P;
		P[d] = R;
		array.add(P);
		for (int i = array.getSize() - baseSize - 1; i < array.getSize()-1; i++) {
			array.connect(i, array.getSize() - 1);
		}

		VertexArray mirror(array, baseSize + 1, array.getSize());
		mirror.scale(d, -1);
		int p = array.getSize();
		array.addAll(mirror);
		for (int i = 0; i < baseSize; i++) {
			array.connect(i + 1, p);
			p++;
		}
	}
}