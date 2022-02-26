#include "QRCodeModel.h"

QRCodeModel::QRCodeModel(std::string const& name)
    : name(name)
{
}

void QRCodeModel::generate(std::vector<std::vector<unsigned char>> const& bytes, size_t padding, double blockSize, double height)
{
    this->padding = padding;
    this->blockSize = blockSize;
    size_t widthX = bytes.front().size() + padding * 2;
    size_t widthY = bytes.size() + padding * 2;
    this->radiusX = widthX * blockSize / 2;
    this->radiusY = widthY * blockSize / 2;
    this->height = height;
    prepareBlocks(bytes);
    generateBorder();
    generateBackFace();
    generateFrontFace();
}

// bytes: non-zero for black
void QRCodeModel::prepareBlocks(std::vector<std::vector<unsigned char>> const& bytes)
{
    //std::vector<std::vector<unsigned char>> bytes = {
    //  { 1,1,1,1,1,1,1,0,1,0,0,0,1,0,1,0,1,0,1,1,1,1,1,1,1, },
    //  { 1,0,0,0,0,0,1,0,1,1,1,0,0,1,0,1,1,0,1,0,0,0,0,0,1, },
    //  { 1,0,1,1,1,0,1,0,0,1,0,1,0,0,0,1,0,0,1,0,1,1,1,0,1, },
    //  { 1,0,1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,0,1,0,1,1,1,0,1, },
    //  { 1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,1,0,1,1,1,0,1, },
    //  { 1,0,0,0,0,0,1,0,0,1,1,1,0,1,0,1,1,0,1,0,0,0,0,0,1, },
    //  { 1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1, },
    //  { 0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0, },
    //  { 0,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,0,1,1,0,0,1,0,1,1, },
    //  { 1,0,1,1,0,1,0,1,0,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,0, },
    //  { 1,0,1,1,0,0,1,0,1,1,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0, },
    //  { 0,1,1,0,1,0,0,1,0,0,0,1,0,0,1,0,1,1,0,1,1,0,0,0,0, },
    //  { 1,0,1,1,0,0,1,0,0,1,0,0,0,1,1,1,1,1,0,0,1,1,0,1,0, },
    //  { 1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,1,1,1, },
    //  { 1,1,0,1,1,0,1,0,1,0,0,0,0,1,1,0,1,1,0,1,1,0,0,1,0, },
    //  { 1,1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0, },
    //  { 1,1,0,0,1,0,1,0,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1, },
    //  { 0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,1,0,0,0,1,0,0,1,1, },
    //  { 1,1,1,1,1,1,1,0,1,1,0,0,1,1,0,1,1,0,1,0,1,0,0,0,0, },
    //  { 1,0,0,0,0,0,1,0,1,0,1,1,0,1,1,0,1,0,0,0,1,1,1,0,1, },
    //  { 1,0,1,1,1,0,1,0,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,0,0, },
    //  { 1,0,1,1,1,0,1,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,0, },
    //  { 1,0,1,1,1,0,1,0,1,0,0,0,0,1,1,0,1,0,0,1,0,1,1,0,0, },
    //  { 1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1, },
    //  { 1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,0,0,1,1,0,0,0,1,0,1, },
    //};
    //std::vector<std::vector<unsigned char>> bytes = {
    //  { 1,1,1,1,1,1,1 },
    //  { 1,0,0,0,0,0,1 },
    //  { 1,0,1,1,1,0,1 },
    //  { 1,0,1,1,1,0,1 },
    //  { 1,0,1,1,1,0,1 },
    //  { 1,0,0,0,0,0,1 },
    //  { 1,1,1,1,1,1,1 },
    //};
    //std::vector<std::vector<unsigned char>> bytes = {
    //  { 1,1,1 },
    //  { 1,1,1 },
    //  { 1,1,1 },
    //};
    //std::vector<std::vector<unsigned char>> bytes = {
    //  { 1 },
    //};
    size_t nx = bytes.front().size();
    size_t ny = bytes.size();
    Block block{ {0, 0, 0, 0}, 2 };  // padding is not black, first group
    std::vector<Block> line(nx + padding * 2, block); // padding is not black, first group
    blocks.resize(ny + padding * 2, line);
    for (size_t r = 0; r < ny; ++r) {
        std::vector<Block>& line = blocks[r + padding];
        std::vector<unsigned char> const& row = bytes[r];
        for (size_t c = 0; c < nx; ++c)
            line[c + padding].group = row[c] ? 1 : 0;
    }
    nx += padding * 2 - 1;
    ny += padding * 2 - 1;
    blocks[0][0].points[4] = 4;
    blocks[0][nx].points[5] = 5;
    blocks[ny][0].points[6] = 6;
    blocks[ny][nx].points[7] = 7;
}

void QRCodeModel::groupBlocks()
{
    // group neighbours
    size_t nx = blocks.front().size() - padding * 2;
    size_t ny = blocks.size() - padding * 2;
    size_t next_0 = 4;
    size_t next_1 = 3;
    groups.resize(2, { size_t(-1), size_t(-1) });
    groups.push_back({ 0, 0 }); // first group
    for (size_t r = padding; r < ny + padding; ++r) {
        std::vector<Block> const& prev = blocks[r - 1];
        std::vector<Block>& line = blocks[r];
        for (size_t c = padding; c < nx + padding; ++c) {
            bool join_up = ((line[c].group ^ prev[c].group) & 1) == 0;
            bool join_left = ((line[c].group ^ line[c - 1].group) & 1) == 0;
            if (join_up && join_left) {
                line[c].group = std::min(prev[c].group, line[c - 1].group);
                groups[std::max(prev[c].group, line[c - 1].group)] = groups[line[c].group];
            }
            else if (join_up) {
                line[c] = prev[c];
            }
            else if (join_left) {
                line[c] = line[c - 1];
            }
            else if (line[c].group == 0) {
                line[c].group = next_0;
                groups.resize(next_0, { size_t(-1), size_t(-1) });
                next_0 += 2;
                groups.push_back({ r, c });
            }
            else {
                line[c].group = next_1;
                groups.resize(next_1, { size_t(-1), size_t(-1) });
                next_1 += 2;
                groups.push_back({ r, c });
            }
        }
    }
}

//   4 +--------+5
//   0/+-------/+1
//   //       //
//  //       //
//7+--------+/6
//3+--------+2

void QRCodeModel::generateBorder()
{
    mesh.addVerticals({
        {-radiusX, -radiusY, 0}, // 0
        {radiusX, -radiusY, 0}, // 1
        {radiusX, radiusY, 0}, // 2
        {-radiusX, radiusY, 0}, // 3
        {-radiusX, -radiusY, height}, // 4
        {radiusX, -radiusY, height},// 5
        {radiusX, radiusY, height}, // 6
        {-radiusX, radiusY, height} // 7
        });

    mesh.addTriangles({
        {0, 1, 5},
        {0, 5, 4},
        {1, 6, 5},
        {1, 2, 6},
        {3, 6, 2},
        {3, 7, 6},
        {0, 7, 3},
        {0, 4, 7},
        });
}

void QRCodeModel::generateBackFace()
{
    mesh.addTriangles({
        {0, 3, 2},
        {0, 2, 1},
        });
}

void QRCodeModel::generateFrontFace()
{
    //mesh.addTriangles({
    //    {4, 6, 7},
    //    {4, 5, 6},
    //});
    size_t nx = blocks.front().size();
    size_t ny = blocks.size();
    for (size_t r = 0; r < ny; ++r) {
        std::vector<Block>& line = blocks[r];
        for (size_t c = 0; c < nx; ++c)
            if (line[c].group == 1)
                generateBlackBlock(r, c);
            else
                generateWhiteBlock(r, c);
    }
}

size_t QRCodeModel::blockPoint(size_t row, size_t col, size_t index)
{
    size_t& p = blocks[row][col].points[index];
    if (p == 0) {
        auto n = index & 3;
        if (n == 0 && row > 0 && col > 0)
            p = blockPoint(row - 1, col - 1, index + 2);
        else if (n == 0 && row > 0)
            p = blockPoint(row - 1, col, index + 3);
        else if (n == 0 && col > 0)
            p = blockPoint(row, col - 1, index + 1);
        else if (n == 1 && (row > 0 || !++col))
            p = blockPoint(row - 1, col, index + 1);
        else if (n == 3 && (col > 0 || !++row))
            p = blockPoint(row, col - 1, index - 1);
        else if (n != 2 || (++row && ++col))
            p = mesh.addVerticals({ { col * blockSize - radiusX, row * blockSize - radiusY, index >= 4 ? height : height / 2 } });
    }
    return p;
}

void QRCodeModel::generateWhiteBlock(size_t row, size_t col)
{
    size_t points[] = { blockPoint(row, col, 4), blockPoint(row, col, 5), blockPoint(row, col, 6), blockPoint(row, col, 7) };
    mesh.addTriangles({
        {points[0], points[2], points[3]},
        {points[0], points[1], points[2]},
    });
}

void QRCodeModel::generateBlackBlock(size_t row, size_t col)
{
    bool l = blocks[row][col - 1].group == 1; // if left neighbour is black
    bool r = blocks[row][col + 1].group == 1;
    bool t = blocks[row - 1][col].group == 1;
    bool b = blocks[row + 1][col].group == 1;
    size_t points[] = {
        blockPoint(row, col, 0), blockPoint(row, col, 1), blockPoint(row, col, 2), blockPoint(row, col, 3),
        (l && t) ? 0 : blockPoint(row, col, 4),
        (r && t) ? 0 : blockPoint(row, col, 5),
        (r && b) ? 0 : blockPoint(row, col, 6),
        (l && b) ? 0 : blockPoint(row, col, 7),
    };
    mesh.addTriangles({
        {points[0], points[2], points[3]},
        {points[0], points[1], points[2]},
        });
    if (!l)
        mesh.addTriangles({
            {points[0], points[3], points[7]},
            {points[0], points[7], points[4]},
            });
    if (!t)
        mesh.addTriangles({
            {points[0], points[5], points[1]},
            {points[0], points[4], points[5]},
            });
    if (!r)
        mesh.addTriangles({
            {points[1], points[5], points[6]},
            {points[1], points[6], points[2]},
            });
    if (!b)
        mesh.addTriangles({
            {points[3], points[2], points[6]},
            {points[3], points[6], points[7]},
            });
}

bool QRCodeModel::writeToStl(std::ostream& os) const
{
    os << "solid " << name << " QRCode\n";
    mesh.writeToStl(os);
    os << "endsolid " << name << " QRCode\n";
    return true;
}

bool QRCodeModel::writeTo(boost::property_tree::ptree& tree) const
{
    return false;
}
