#ifndef MEDIAMANAGERUNIT_HPP
#define MEDIAMANAGERUNIT_HPP

#include "MediaManager.hpp"
#include "TextureUnit.hpp"
#include "Mesh.hpp"
#include "SoundUnit.hpp"
#include "ModelUnit.hpp"

class TFileMediaManager : public TGenericMediaManager {
public:
    TFileMediaManager();
    
    // Asset caching helpers
    TTexture* LoadTexture(const std::string& name, const std::string& path);
    TMesh* LoadMesh(const std::string& name, const std::string& path);
    TSound* LoadSound(const std::string& name, const std::string& path);
    TModel* LoadModel(const std::string& name, const std::string& path);
    
    // High-level wrapper for game initialization
    void LoadGameAssets();

    // Lazy loading overrides
    virtual TMesh* getMeshByName(const std::string& name) override;
    virtual TTexture* getTextureByName(const std::string& name) override;
    virtual TSound* getSoundByName(const std::string& name) override;
    virtual TModel* getModelByName(const std::string& name) override;
};

#endif