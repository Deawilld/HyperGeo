#pragma once

template<int size, typename T = double>
class Vector
{
public:
	T components[size];

	Vector() {
		for (int i = 0; i < size; i++) {
			components[i] = 0;
		}
	}

	Vector(const Vector& vec) {
		for (int i = 0; i < size; i++) {
			components[i] = vec[i];
		}
	}

	Vector(std::initializer_list<T> vals) {
		auto it = vals.begin();
		for (int i = 0; i < size; i++) {
			components[i] = i < vals.size() ? *it : 0;
			it++;
		}
	}

	const T operator[](int index) const {
		return (index < size) ? components[index] : 0;
	}

	T& operator[](int index) {
		return components[index];
	}

	friend Vector operator+(const Vector& v1, const Vector& v2) {
		Vector res;
		for (int i = 0; i < size; i++) {
			res[i] = v1[i] + v2[i];
		}
		return res;
	}

	Vector& operator+=(const Vector& vec) {
		for (int i = 0; i < size; i++) {
			components[i] += vec[i];
		}
		return *this;
	}

	Vector operator-() const {
		Vector res;
		for (int i = 0; i < size; i++) {
			res[i] = -components[i];
		}
		return res;
	}

	friend Vector operator-(const Vector& v1, const Vector& v2) {
		return v1 + (-v2);
	}

	Vector& operator-=(const Vector& vec) {
		for (int i = 0; i < size; i++) {
			components[i] -= vec[i];
		}
		return *this;
	}

	friend Vector operator*(const Vector& vec, const T k) {
		Vector res;
		for (int i = 0; i < size; i++) {
			res[i] = vec[i] * k;
		}
		return res;
	}

	Vector& operator*=(const T k) {
		for (int i = 0; i < size; i++) {
			components[i] *= k;
		}
		return *this;
	}

	T length() const {
		T sqsum = 0;
		for (int i = 0; i < size; i++) {
			sqsum += pow(components[i], 2);
		}
		return sqrt(sqsum);
	}

	Vector toUnitVec() const {
		Vector vec = *this;
		T len = length();
		for (int i = 0; i < size; i++) {
			vec[i] /= len;
		}
		return vec;
	}

	template<int toSize>
	Vector<toSize, T> cast() const {
		Vector<toSize, T> result;
		for (int i = 0; i < std::min(size, toSize); i++) {
			result[i] = components[i];
		}
		return result;
	}
	template<int toSize>
	operator Vector<toSize, T>() const {
		Vector<toSize, T> result;
		for (int i = 0; i < std::min(size, toSize); i++) {
			result[i] = components[i];
		}
		return result;
	}
};
using Vector3D = Vector<3>;
static const int MAX_DIMS = 5;
using VectorND = Vector<MAX_DIMS>;