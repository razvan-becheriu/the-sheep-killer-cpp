#include "Mesh.hpp"
#include "MsxMath.hpp" // For Cross, Normalize
#include "Logger.hpp"
#include <fstream>
#include "tinyxml2.h"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

// Placeholder for XML parsing. In a real project, you'd use a library like TinyXML2.
// For now, we'll assume a simple format or hardcode some data for demonstration.
// This is a significant simplification and would need a proper XML parser implementation.
// For the purpose of this conversion, I'll provide a basic structure that would interact
// with a hypothetical XML parser.

// --- TMesh Implementation ---

TMesh::TMesh() : textureID(0), displayList(0), hasDisplayList(false) {
    // Constructor initializes members
}

TMesh::~TMesh() {
    if (hasDisplayList) {
        glDeleteLists(displayList, 1);
    }
}

void TMesh::CalculateNormals() {
    // Reset vertex normals
    for (auto& v : vertices) {
        v.nx = v.ny = v.nz = 0.0f;
    }

    // Calculate face normals and accumulate for vertex normals
    for (auto& face : faces) {
        // Get vertices of the face
        const TVertex& v0 = vertices[face.point[0]];
        const TVertex& v1 = vertices[face.point[1]];
        const TVertex& v2 = vertices[face.point[2]];

        // Calculate two edges of the face
        TVector edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        TVector edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        // Calculate cross product to get face normal
        TVector faceNormal = Cross(edge1, edge2);
        Normalize(faceNormal);

        face.nx = faceNormal.x;
        face.ny = faceNormal.y;
        face.nz = faceNormal.z;

        // Accumulate face normal to its vertices for smooth shading
        if (face.smooth) {
            vertices[face.point[0]].nx += faceNormal.x;
            vertices[face.point[0]].ny += faceNormal.y;
            vertices[face.point[0]].nz += faceNormal.z;

            vertices[face.point[1]].nx += faceNormal.x;
            vertices[face.point[1]].ny += faceNormal.y;
            vertices[face.point[1]].nz += faceNormal.z;

            vertices[face.point[2]].nx += faceNormal.x;
            vertices[face.point[2]].ny += faceNormal.y;
            vertices[face.point[2]].nz += faceNormal.z;
        }
    }

    // Normalize vertex normals
    for (auto& v : vertices) {
        TVector vec = {v.nx, v.ny, v.nz};
        Normalize(vec);
        v.nx = vec.x;
        v.ny = vec.y;
        v.nz = vec.z;
    }
}

void TMesh::BuildDisplayList() {
    if (hasDisplayList) {
        glDeleteLists(displayList, 1);
    }

    displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);

    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        // Pascal uses 1-based indexing for face.point and face.uv, C++ uses 0-based
        // Adjusting to 0-based for C++ vector access
        for (int i = 0; i < 3; ++i) {
            const TVertex& v = vertices[face.point[i]];
            if (face.smooth) {
                glNormal3f(v.nx, v.ny, v.nz);
            } else {
                glNormal3f(face.nx, face.ny, face.nz);
            }
            glTexCoord2f(face.uv[i].u, face.uv[i].v); // Assuming face.uv is 0-indexed in C++
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();

    glEndList();
    hasDisplayList = true;
}

bool TMesh::LoadFromFile(const std::string& filename) {
    Log("Loading mesh from: " + filename);

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) return false;

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) return false;

    vertices.clear();
    faces.clear();

    int pointCount = 0;
    if (const auto* elem = root->FirstChildElement("POINTCOUNT")) {
        elem->QueryIntText(&pointCount);
    }
    vertices.resize(pointCount);

    auto pointsElem = root->FirstChildElement("POINTS");
    if (pointsElem) {
        for (auto vElem = pointsElem->FirstChildElement("VERTEX"); vElem; vElem = vElem->NextSiblingElement("VERTEX")) {
            int id = vElem->IntAttribute("id");
            if (id >= 0 && id < pointCount) {
                TVertex& v = vertices[id];
                if (auto* e = vElem->FirstChildElement("X")) e->QueryFloatText(&v.x);
                if (auto* e = vElem->FirstChildElement("Y")) e->QueryFloatText(&v.y);
                if (auto* e = vElem->FirstChildElement("Z")) e->QueryFloatText(&v.z);
                if (auto* e = vElem->FirstChildElement("NX")) e->QueryFloatText(&v.nx);
                if (auto* e = vElem->FirstChildElement("NY")) e->QueryFloatText(&v.ny);
                if (auto* e = vElem->FirstChildElement("NZ")) e->QueryFloatText(&v.nz);
                
                if (auto* e = vElem->FirstChildElement("U")) {
                    e->QueryFloatText(&v.tu);
                    if (auto* ev = vElem->FirstChildElement("V")) ev->QueryFloatText(&v.tv);
                }
            }
        }
    }

    int faceCount = 0;
    if (const auto* elem = root->FirstChildElement("FACECOUNT")) {
        elem->QueryIntText(&faceCount);
    }
    faces.resize(faceCount);

    auto facesElem = root->FirstChildElement("FACES");
    if (facesElem) {
        for (auto fElem = facesElem->FirstChildElement("FACE"); fElem; fElem = fElem->NextSiblingElement("FACE")) {
            int id = fElem->IntAttribute("id");
            if (id >= 0 && id < faceCount) {
                TFace& f = faces[id];
                if (auto* e = fElem->FirstChildElement("A")) e->QueryIntText(&f.point[0]);
                if (auto* e = fElem->FirstChildElement("B")) e->QueryIntText(&f.point[1]);
                if (auto* e = fElem->FirstChildElement("C")) e->QueryIntText(&f.point[2]);
                
                if (auto* e = fElem->FirstChildElement("AU")) {
                    e->QueryFloatText(&f.uv[0].u);
                    if (auto* ev = fElem->FirstChildElement("AV")) ev->QueryFloatText(&f.uv[0].v);
                    if (auto* ev = fElem->FirstChildElement("BU")) ev->QueryFloatText(&f.uv[1].u);
                    if (auto* ev = fElem->FirstChildElement("BV")) ev->QueryFloatText(&f.uv[1].v);
                    if (auto* ev = fElem->FirstChildElement("CU")) ev->QueryFloatText(&f.uv[2].u);
                    if (auto* ev = fElem->FirstChildElement("CV")) ev->QueryFloatText(&f.uv[2].v);
                }
                
                int smooth = 1;
                if (const auto* elem = fElem->FirstChildElement("SMOOTH")) {
                    elem->QueryIntText(&smooth);
                }
                f.smooth = (smooth != 0);
            }
        }
    }

    CalculateNormals();
    BuildDisplayList();
    return true;
}

void TMesh::Render() {
    // Draw the mesh using the display list
    if (hasDisplayList) {
        glCallList(displayList);
    } else {
        // Fallback if display list not built (shouldn't happen if LoadFromFile is successful)
        BuildDisplayList(); // Try to build it now
        if (hasDisplayList) {
            glCallList(displayList);
        }
    }
}