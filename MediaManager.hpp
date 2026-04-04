#ifndef MEDIAMANAGER_HPP
#define MEDIAMANAGER_HPP

#include <string>
#include <map>
#include <memory>
#include <iostream> // For logging

// Forward declarations for assets
class TMesh; // Defined in Mesh.hpp
class TTexture; // Defined in TextureUnit.hpp
class TSound;   // Defined in SoundUnit.hpp
class TModel;   // Defined in ModelUnit.hpp

// TGenericMediaManager (from mediamanagerunit.pas)
class TGenericMediaManager {
public:
    std::map<std::string, std::unique_ptr<TMesh>> meshes;
    std::map<std::string, std::unique_ptr<TTexture>> textures;
    std::map<std::string, std::unique_ptr<TSound>> sounds;
    std::map<std::string, std::unique_ptr<TModel>> models;

    TGenericMediaManager();
    virtual ~TGenericMediaManager();

    virtual TMesh* getMeshByName(const std::string& name) = 0;
    virtual TTexture* getTextureByName(const std::string& name) = 0;
    virtual TSound* getSoundByName(const std::string& name) = 0;
    virtual TModel* getModelByName(const std::string& name) = 0;
};

#endif // MEDIAMANAGER_HPP