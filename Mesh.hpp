#ifndef MODEL_HPP
#define MODEL_HPP

#include "Types3D.hpp"
#include "MsxMath.hpp"
#include <vector>
#include <string>

class TMesh {
public:
    std::vector<TVertex> vertices;
    std::vector<TFace> faces;
    unsigned int textureID;

    TMesh();
    ~TMesh();

    bool LoadFromFile(const std::string& filename);
    void Render(); // Render a single mesh, transformations applied externally
    
private:
    void CalculateNormals();
    void BuildDisplayList();
    unsigned int displayList;
    bool hasDisplayList;
};

#endif // MESH_HPP