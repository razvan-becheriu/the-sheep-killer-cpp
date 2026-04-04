#ifndef MODEL_HPP
#define MODEL_HPP

#include "Types3D.hpp"
#include "MsxMath.hpp"
#include <vector>
#include <string>

class TModel {
public:
    std::vector<TVertex> vertices;
    std::vector<TFace> faces;
    unsigned int textureID;

    TModel();
    ~TModel();

    bool LoadFromFile(const std::string& filename);
    void Render(const TVector& pos, const TQuaternion& rot, TFloat scale);
    
private:
    void CalculateNormals();
    void BuildDisplayList();
    unsigned int displayList;
    bool hasDisplayList;
};

#endif // MODEL_HPP