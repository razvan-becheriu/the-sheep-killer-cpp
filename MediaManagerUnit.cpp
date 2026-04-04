#include "MediaManagerUnit.hpp"
#include "Logger.hpp"

TGenericMediaManager::TGenericMediaManager() {}
TGenericMediaManager::~TGenericMediaManager() {}

TFileMediaManager::TFileMediaManager() {}

TTexture* TFileMediaManager::LoadTexture(const std::string& name, const std::string& path) {
    auto tex = std::make_unique<TTexture>();
    if (tex->LoadFrom(path)) {
        tex->name = name;
        textures[name] = std::move(tex);
        return textures[name].get();
    }
    return nullptr;
}

TMesh* TFileMediaManager::LoadMesh(const std::string& name, const std::string& path) {
    auto mesh = std::make_unique<TMesh>();
    if (mesh->LoadFromFile(path)) {
        meshes[name] = std::move(mesh);
        return meshes[name].get();
    }
    return nullptr;
}

TSound* TFileMediaManager::LoadSound(const std::string& name, const std::string& path) {
    auto sound = std::make_unique<TSound>(path);
    sound->SetName(name);
    sounds[name] = std::move(sound);
    return sounds[name].get();
}

TModel* TFileMediaManager::LoadModel(const std::string& name, const std::string& path) {
    auto model = std::make_unique<TModel>();
    if (model->load(path, this)) {
        models[name] = std::move(model);
        return models[name].get();
    }
    return nullptr;
}

TMesh* TFileMediaManager::getMeshByName(const std::string& name) {
    auto it = meshes.find(name);
    if (it != meshes.end()) return it->second.get();

    TMesh* m = LoadMesh(name, "media/" + name + ".mesh");
    if (m) return m;

    // Fallback: create empty mesh to prevent crashes and repeat loading attempts
    auto empty = std::make_unique<TMesh>();
    TMesh* ptr = empty.get();
    meshes[name] = std::move(empty);
    return ptr;
}

TTexture* TFileMediaManager::getTextureByName(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) return it->second.get();
    return LoadTexture(name, "media/" + name + ".png");
}

TSound* TFileMediaManager::getSoundByName(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) return it->second.get();
    return LoadSound(name, "media/" + name + ".wav");
}

TModel* TFileMediaManager::getModelByName(const std::string& name) {
    auto it = models.find(name);
    if (it != models.end()) return it->second.get();

    TModel* m = LoadModel(name, "media/" + name + ".model");
    if (m) return m;

    // Fallback: create empty model to prevent crashes
    auto empty = std::make_unique<TModel>();
    TModel* ptr = empty.get();
    models[name] = std::move(empty);
    return ptr;
}

void TFileMediaManager::LoadGameAssets() {
    // Core UI textures should be pre-loaded
    getTextureByName("font");
    getTextureByName("numbers");
    getTextureByName("heart");
    getTextureByName("point");
    getTextureByName("tile1");
    getTextureByName("loading");
    getTextureByName("title");
    getTextureByName("gameover");
    getTextureByName("intro");

    // Common sounds
    getSoundByName("x2");
    getSoundByName("hit0");
    getSoundByName("swhoosh0");
}