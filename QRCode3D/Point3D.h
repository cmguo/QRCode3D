#pragma once

#include <initializer_list>

class Point3D
{
public:
	Point3D(std::initializer_list<double> v) {
		auto i = v.begin();
		this->v[0] = *i++;
		this->v[1] = *i++;
		this->v[2] = *i++;
	}

	Point3D(double x, double y, double z = 0) {
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}

public:
	double& x() {
		return v[0];
	}

	double& y() {
		return v[1];
	}

	double& z() {
		return v[2];
	}

	double const & x() const {
		return v[0];
	}

	double const& y() const {
		return v[1];
	}

	double const& z() const {
		return v[2];
	}

private:
	double v[3];
};

