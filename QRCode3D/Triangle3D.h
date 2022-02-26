#pragma once

#include "Point3D.h"

#include <vector>


class Triangle3D
{
public:
	Triangle3D(std::vector<Point3D> points, std::initializer_list<size_t> v, bool check = false)
	{
		auto i = v.begin();
		this->v[0] = *i++;
		this->v[1] = *i++;
		this->v[2] = *i++;
	}

public:
	size_t& operator[](size_t n) { return v[n]; }
	size_t const & operator[](size_t n) const { return v[n]; }

private:
	size_t v[3];
};

