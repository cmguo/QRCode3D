#pragma once

#include "Triangle3D.h"

#include <boost/property_tree/ptree.hpp>

class TriangleMesh
{
public:
	size_t addVerticals(std::initializer_list<Point3D> vertices) {
		size_t n = this->vertices.size();
		for (auto p : vertices) {
			this->vertices.push_back(p);
		}
		return n;
	}

	void addTriangles(std::initializer_list<std::initializer_list<size_t>> triangles) {
		for (auto t : triangles) {
			this->triangles.push_back(Triangle3D(vertices, t));
		}
	}

public:
	bool writeToStl(std::ostream & os) const;

	bool writeTo(boost::property_tree::ptree& tree) const;

	bool readFrom(boost::property_tree::ptree const & tree);

private:
	std::vector<Point3D> vertices;
	std::vector<Triangle3D> triangles;
};

