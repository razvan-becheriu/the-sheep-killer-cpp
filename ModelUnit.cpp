#include "ModelUnit.hpp"
#include "XmlParser.hpp"
#include "MsxMath.hpp"
#include "Types3D.hpp"
#include "MediaManager.hpp"
#include "TextureUnit.hpp"
#include <algorithm>
#include <iostream>
#include <cstddef>

// --- TTransformation Implementation ---

TTransformation::TTransformation() : pos({0,0,0}), rot({0,0,0}), sca({1,1,1}) {}

bool TTransformation::isZero() const {
    return (pos.x == 0 && pos.y == 0 && pos.z == 0 &&
            rot.x == 0 && rot.y == 0 && rot.z == 0 &&
            sca.x == 0 && sca.y == 0 && sca.z == 0);
}

bool TTransformation::mustToggleFaces() const {
    return sca.x * sca.y * sca.z < 0;
}

TTransformation TTransformation::sum(const TTransformation& other) const {
    TTransformation result;
    result.pos = Add(this->pos, other.pos);
    // Pascal source adds Euler angle components for diff transformations.
    result.rot = Add(this->rot, other.rot);
    result.sca = Add(this->sca, other.sca);
    return result;
}

TTransformation TTransformation::clone() const {
    TTransformation result;
    result.pos = this->pos;
    result.rot = this->rot;
    result.sca = this->sca;
    return result;
}

// --- TModelTreeNode Implementation ---

TModelTreeNode::TModelTreeNode() : texture(nullptr) {
    color = {1.0f, 1.0f, 1.0f};
    graphicalElement.type = geNone;
    graphicalElement.mesh = nullptr;
}

TModelTreeNode::~TModelTreeNode() {}

bool TModelTreeNode::load(void* xmlNodePtr, TGenericMediaManager* man) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;

    name = xml->query("NAME")->textContent;

    TXmlNode* textureNode = xml->query("TEXTURE");
    if (textureNode && textureNode->textContent != "nil") {
        texture = man->getTextureByName(textureNode->textContent);
    } else {
        texture = nullptr;
    }

    if (auto* node = xml->query("POS")) {
        transf.pos.x = std::stof(node->query("X")->textContent);
        transf.pos.y = std::stof(node->query("Y")->textContent);
        transf.pos.z = std::stof(node->query("Z")->textContent);
    }
    if (auto* node = xml->query("ROT")) {
        transf.rot.x = std::stof(node->query("X")->textContent);
        transf.rot.y = std::stof(node->query("Y")->textContent);
        transf.rot.z = std::stof(node->query("Z")->textContent);
    }
    if (auto* node = xml->query("SCA")) {
        transf.sca.x = std::stof(node->query("X")->textContent);
        transf.sca.y = std::stof(node->query("Y")->textContent);
        transf.sca.z = std::stof(node->query("Z")->textContent);
    }

    if (auto* node = xml->query("COLOR")) {
        color.r = node->query("RED")->getNumber() / 255.0f;
        color.g = node->query("GREEN")->getNumber() / 255.0f;
        color.b = node->query("BLUE")->getNumber() / 255.0f;
    }

    if (auto* geNode = xml->query("GRAPHICALELEMENT")) {
        std::string typeStr = geNode->query("TYPE")->textContent;
        if (typeStr == "mesh") {
            graphicalElement.type = geMesh;
            graphicalElement.mesh = man->getMeshByName(geNode->query("MESH")->textContent);
        } else if (typeStr == "model") {
            graphicalElement.type = geModel;
            if (auto* mNode = geNode->query("MODEL")) {
                graphicalElement.model = man->getModelByName(mNode->textContent);
            }
        } else if (typeStr == "reference") {
            graphicalElement.type = geReference;
            graphicalElement.reference = geNode->query("REFERENCE")->getNumber();
        } else {
            graphicalElement.type = geNone;
        }
    }

    for (const auto& childXml : xml->children) {
        if (childXml->name == "NODE") {
            auto childNode = std::make_unique<TModelTreeNode>();
            if (childNode->load(childXml.get(), man)) {
                children.push_back(std::move(childNode));
            }
        }
    }
    return true;
}

void* TModelTreeNode::save() const { return nullptr; }

std::unique_ptr<TModelTreeNode> TModelTreeNode::cloneDeep() const {
    auto newNode = std::make_unique<TModelTreeNode>();
    newNode->name = name;
    newNode->transf = transf.clone();
    newNode->color = color;
    newNode->graphicalElement = graphicalElement;
    newNode->texture = texture;
    for (const auto& child : children) {
        newNode->children.push_back(child->cloneDeep());
    }
    return newNode;
}

// --- TPoseItem Implementation ---

TPoseItem::TPoseItem() {}
TPoseItem::~TPoseItem() {}

bool TPoseItem::load(void* xmlNodePtr) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;
    movedNode = xml->query("MOVEDNODE")->textContent;
    if (auto* n = xml->query("POS")) {
        diff.pos.x = std::stof(n->query("X")->textContent);
        diff.pos.y = std::stof(n->query("Y")->textContent);
        diff.pos.z = std::stof(n->query("Z")->textContent);
    }
    if (auto* n = xml->query("ROT")) {
        diff.rot.x = std::stof(n->query("X")->textContent);
        diff.rot.y = std::stof(n->query("Y")->textContent);
        diff.rot.z = std::stof(n->query("Z")->textContent);
    }
    if (auto* n = xml->query("SCA")) {
        diff.sca.x = std::stof(n->query("X")->textContent);
        diff.sca.y = std::stof(n->query("Y")->textContent);
        diff.sca.z = std::stof(n->query("Z")->textContent);
    }
    return true;
}

void* TPoseItem::save() const { return nullptr; }

// --- TPose Implementation ---

TPose::TPose() : name("A pose") {}
TPose::~TPose() {}

TTransformation* TPose::getTransfForNode(const std::string& nodename, bool createIfNotFound) {
    for (const auto& item : items) {
        if (item->movedNode == nodename) return &item->diff;
    }
    if (createIfNotFound) {
        auto newItem = std::make_unique<TPoseItem>();
        newItem->movedNode = nodename;
        items.push_back(std::move(newItem));
        return &items.back()->diff;
    }
    return nullptr;
}

const TTransformation* TPose::getTransfForNode(const std::string& nodename) const {
    for (const auto& item : items) {
        if (item->movedNode == nodename) return &item->diff;
    }
    return nullptr;
}

bool TPose::isMoved(const std::string& nodename) const {
    const auto* t = getTransfForNode(nodename);
    return t && !t->isZero();
}

bool TPose::load(void* xmlNodePtr) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;
    name = xml->query("NAME")->textContent;
    for (const auto& childXml : xml->children) {
        if (childXml->name == "POSEITEM") {
            auto item = std::make_unique<TPoseItem>();
            if (item->load(childXml.get())) items.push_back(std::move(item));
        }
    }
    return true;
}

void* TPose::save() const { return nullptr; }

// --- TAnimationKey Implementation ---

TAnimationKey::TAnimationKey() : time(0), trigger(false) {}
TAnimationKey::~TAnimationKey() {}

bool TAnimationKey::load(void* xmlNodePtr) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;
    poseName = xml->query("POSENAME")->textContent;
    time = xml->query("TIME")->getNumber();
    if (auto* n = xml->query("TRIGGER")) {
        trigger = (n->textContent == "true");
    }
    return true;
}

void* TAnimationKey::save() const { return nullptr; }

// --- TAnimation Implementation ---

TAnimation::TAnimation() : name("Animation") {}
TAnimation::~TAnimation() {}

bool TAnimation::load(void* xmlNodePtr) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;

    name = xml->query("NAME")->textContent;
    keys.clear();
    for (const auto& child : xml->children) {
        if (child->name == "ANIMITEM") {
            auto key = std::make_unique<TAnimationKey>();
            if (key->load(child.get())) {
                keys.push_back(std::move(key));
            }
        }
    }
    return true;
}

void* TAnimation::save() const { return nullptr; }

// --- TModel Implementation ---

TModel::TModel() : name("Amodel") {
    root = std::make_unique<TModelTreeNode>();
}

bool TModel::load(const std::string& filename, TGenericMediaManager* man) {
    TXmlDocument doc;
    try {
        doc.load(filename);
        return load(doc.rootNodeWrapper.get(), man);
    } catch (const std::exception& e) {
        std::cerr << "Error loading TModel XML: " << e.what() << std::endl;
        return false;
    }
}

bool TModel::load(void* xmlNodePtr, TGenericMediaManager* man) {
    TXmlNode* xml = static_cast<TXmlNode*>(xmlNodePtr);
    if (!xml) return false;

    name = xml->query("NAME")->textContent;

    if (auto* rootXml = xml->query("NODE")) {
        root = std::make_unique<TModelTreeNode>();
        root->load(rootXml, man);
    }

    poses.clear();
    for (const auto& child : xml->children) {
        if (child->name == "POSE") {
            auto pose = std::make_unique<TPose>();
            if (pose->load(child.get())) {
                poses.push_back(std::move(pose));
            }
        }
    }

    animations.clear();
    for (const auto& child : xml->children) {
        if (child->name == "ANIMATION") {
            auto anim = std::make_unique<TAnimation>();
            if (anim->load(child.get())) {
                animations.push_back(std::move(anim));
            }
        }
    }
    return true;
}

bool TModel::hasPose(const std::string& poseName) const {
    return indexOfPose(poseName) != -1;
}

int TModel::indexOfPose(const std::string& poseName) const {
    for (size_t i = 0; i < poses.size(); ++i) {
        if (poses[i]->name == poseName) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void* TModel::save() const { return nullptr; }

// --- TTransfTreeNode Implementation ---

TTransfTreeNode::TTransfTreeNode() {}

// --- TRealTimeModel Implementation ---

TRealTimeModel::TRealTimeModel() : fActualPose(nullptr) {}

std::unique_ptr<TTransfTreeNode> TRealTimeModel::createTreeForPose(const TPose* pose, const TModel* model) {
    return cloneTreeWithPose(pose, model->root.get());
}

std::unique_ptr<TTransfTreeNode> TRealTimeModel::cloneTreeWithPose(const TPose* pose, const TModelTreeNode* node) {
    auto newNode = std::make_unique<TTransfTreeNode>();
    const TTransformation* poseDiff = pose ? pose->getTransfForNode(node->name) : nullptr;

    if (poseDiff) {
        newNode->transf = node->transf.sum(*poseDiff);
    } else {
        newNode->transf = node->transf.clone();
    }

    for (const auto& child : node->children) {
        newNode->children.push_back(cloneTreeWithPose(pose, child.get()));
    }
    return newNode;
}

void TRealTimeModel::interpolateTree(const TTransfTreeNode* tree1, const TTransfTreeNode* tree2, TModelTreeNode* actual, TFloat value) {
    ::Interpolate(tree1->transf.pos, tree2->transf.pos, value, actual->transf.pos);
    ::Interpolate(tree1->transf.rot, tree2->transf.rot, value, actual->transf.rot);
    ::Interpolate(tree1->transf.sca, tree2->transf.sca, value, actual->transf.sca);

    for (size_t i = 0; i < tree1->children.size(); ++i) {
        interpolateTree(tree1->children[i].get(), tree2->children[i].get(), actual->children[i].get(), value);
    }
}

void TRealTimeModel::interpolatePose(int pose1Index, int pose2Index, TFloat value) {
    if (pose1Index < 0 || (size_t)pose1Index >= fPoseRoots.size() ||
        pose2Index < 0 || (size_t)pose2Index >= fPoseRoots.size()) {
        return;
    }
    interpolateTree(fPoseRoots[pose1Index].get(), fPoseRoots[pose2Index].get(), fActualPose.get(), value);
}

void TRealTimeModel::makeAnimation(TRealTimeAnimation& rta, const TAnimation* anim, const TModel* model) {
    rta.triggerTime = 50;
    rta.keys.resize(anim->keys.size());
    for (size_t i = 0; i < anim->keys.size(); ++i) {
        if (anim->keys[i]->trigger) {
            rta.triggerTime = anim->keys[i]->time;
        }
        rta.keys[i].poseIndex = model->indexOfPose(anim->keys[i]->poseName);
        rta.keys[i].time = anim->keys[i]->time;
    }
    std::sort(rta.keys.begin(), rta.keys.end(), [](const TRealTimeAnimationKey& a, const TRealTimeAnimationKey& b) {
        return a.time < b.time;
    });
}

void TRealTimeModel::build(const TModel* model) {
    fPoseRoots.clear();
    for (const auto& pose : model->poses) {
        fPoseRoots.push_back(createTreeForPose(pose.get(), model));
    }
    fActualPose = model->root->cloneDeep();
    fAnimations.resize(model->animations.size());
    for (size_t i = 0; i < model->animations.size(); ++i) {
        makeAnimation(fAnimations[i], model->animations[i].get(), model);
    }
}

void TRealTimeModel::interpolate(int animationIndex, TFloat value) {
    if (animationIndex < 0 || (size_t)animationIndex >= fAnimations.size()) return;
    const TRealTimeAnimation& anim = fAnimations[animationIndex];
    if (anim.keys.empty()) return;
    if (anim.keys.size() == 1) {
        interpolatePose(anim.keys[0].poseIndex, anim.keys[0].poseIndex, 0.0f);
        return;
    }

    int val = static_cast<int>(value * 100);
    size_t p = anim.keys.size() - 1;
    for (size_t i = 1; i < anim.keys.size(); ++i) {
        if (anim.keys[i].time > val) {
            p = i;
            break;
        }
    }

    int key1Time = anim.keys[p - 1].time;
    int key2Time = anim.keys[p].time;
    int pose1Index = anim.keys[p - 1].poseIndex;
    int pose2Index = anim.keys[p].poseIndex;

    TFloat duration = static_cast<TFloat>(key2Time - key1Time);
    TFloat interpValue = (duration > 0) ? (val - key1Time) / duration : 0.0f;

    interpolatePose(pose1Index, pose2Index, interpValue);
}

bool TRealTimeModel::triggerPassed(int animationIndex, TFloat value) const {
    if (animationIndex < 0 || (size_t)animationIndex >= fAnimations.size()) {
        return false;
    }
    const TRealTimeAnimation& anim = fAnimations[animationIndex];
    return (anim.triggerTime / 100.0f) <= value;
}

bool TRealTimeModel::load(const std::string& filename, TGenericMediaManager* man) {
    std::cout << "Loading TRealTimeModel from: " << filename << std::endl;
    TModel tempModel;
    if (tempModel.load(filename, man)) {
        build(&tempModel);
        return true;
    }
    return false;
}