#pragma once

#include "TriangleMesh.h"

class QRCodeModel
{
public:
	QRCodeModel(std::string const& name);
		
public:
	void generate(std::vector<std::vector<unsigned char>> const& bytes, size_t padding, double blockSize, double thickness, bool concave);

private:
	void prepareBlocks(std::vector<std::vector<unsigned char>> const& bytes);

	void groupBlocks();

	void generateBorder();

	void generateBackFace();

	void generateFrontFace();

	struct Block {
		size_t points[8];
		size_t group;
	};

	struct Group {
		size_t row;
		size_t col;
	};

	size_t blockPoint(size_t row, size_t col, size_t index);

	void generateWhiteBlock(size_t row, size_t col);

	void generateBlackBlock(size_t row, size_t col);

	void generateFace(std::vector<Point3D> const & polygon);

public:
	bool writeToStl(std::ostream& os) const;

	bool writeTo(boost::property_tree::ptree& tree) const;

	bool readFrom(boost::property_tree::ptree const& tree);

private:
	std::string name;
	size_t padding = 0;
	double blockSize = 0;
	double radiusX = 0;
	double radiusY = 0;
	double thickness = 0;
	bool concave = false;

private:
	TriangleMesh mesh;
	std::vector<std::vector<Block>> blocks;
	std::vector<Group> groups;
};

