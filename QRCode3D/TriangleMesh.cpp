#include "TriangleMesh.h"

bool TriangleMesh::writeToStl(std::ostream& os) const
{
    for (auto& t : triangles) {
        os << "facet normal 0.0 0.0 0.0\n";
        os << "  outer loop\n";
        for (int i = 0; i < 3; i++) {
            os << "    vertex";
            auto& v = vertices[t[i]];
            os << " " << v.x() << " " << v.y() << " " << v.z() << "\n";
        }
        os << "  endloop\n";
        os << "endfacet\n";
    }
    return true;
}

bool TriangleMesh::writeTo(boost::property_tree::ptree& tree) const
{
    boost::property_tree::ptree vt;
    for (auto& v : vertices) {
        boost::property_tree::ptree vvt;
        boost::property_tree::ptree att;
        att.add("x", v.x());
        att.add("y", v.y());
        att.add("z", v.z());
        vvt.add_child("<xmlattr>", att);
        vt.add_child("vertex", vvt);
    }
    tree.add_child("<vertices>", vt);
    boost::property_tree::ptree tt;
    for (auto& t : triangles) {
        for (int i = 0; i < 3; i++) {
            boost::property_tree::ptree ttt;
            boost::property_tree::ptree att;
            att.add("v0", t[0]);
            att.add("v1", t[1]);
            att.add("v2", t[2]);
            ttt.add_child("<xmlattr>", att);
            tt.add_child("triangle", ttt);
        }
    }
    tree.add_child("<triangles>", tt);
    return true;
}
