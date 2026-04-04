#ifndef MODELUNIT_HPP
#define MODELUNIT_HPP

#include "Mesh.hpp" // For TMesh
#include "MsxMath.hpp"
#include "Types3D.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

// Forward declarations
class TModel;
class TGenericMediaManager; // From MediaManager.hpp
class TTexture; // From MediaManager.hpp

// --- TTransformation ---
// Represents a transformation of a tree node.
class TTransformation {
public:
    TVector pos;
    TVector rot;
    TVector sca;

    TTransformation();
    bool isZero() const;
    bool mustToggleFaces() const;
    TTransformation sum(const TTransformation& other) const;
    TTransformation clone() const;
};

// --- TModelTreeNode ---
// The tree node that forms a model.
enum TGraphicElementType {
    geNone,
    geMesh,
    geModel,
    geReference
};

struct TGraphicElement {
    TGraphicElementType type;
    union {
        TMesh* mesh; // Pointer to a TMesh object
        TModel* model; // Reference to another model (geModel)
        int reference; // Index reference (geReference)
    };

    TGraphicElement() : type(geNone), mesh(nullptr) {}
};

class TModelTreeNode {
public:
    std::string name;
    TTransformation transf;
    TRGB color;
    TGraphicElement graphicalElement;
    TTexture* texture; // Pointer to a TTexture object

    std::vector<std::unique_ptr<TModelTreeNode>> children;

    TModelTreeNode();
    ~TModelTreeNode();

    // Placeholder for loading from XML node
    bool load(void* xmlNode, TGenericMediaManager* man); // Use void* for TXmlNode for now
    // Placeholder for saving to XML node
    void* save() const; // Return void* for TXmlNode for now

    std::unique_ptr<TModelTreeNode> cloneDeep() const;
};

// --- TPoseItem ---
// Represents a node of the model tree that is moved in this pose.
class TPoseItem {
public:
    std::string movedNode;
    TTransformation diff;

    TPoseItem();
    ~TPoseItem();
    // Placeholder for loading from XML node
    bool load(void* xmlNode);
    // Placeholder for saving to XML node
    void* save() const;
};

// --- TPose ---
// Represents a pose of the model.
class TPose {
public:
    std::string name;
    std::vector<std::unique_ptr<TPoseItem>> items;

    TPose();
    ~TPose();
    TTransformation* getTransfForNode(const std::string& nodename, bool createIfNotFound);
    const TTransformation* getTransfForNode(const std::string& nodename) const;
    bool isMoved(const std::string& nodename) const;
    // Placeholder for loading from XML node
    bool load(void* xmlNode);
    // Placeholder for saving to XML node
    void* save() const;
};

// --- TAnimationKey ---
// A key in an animation, defining what pose should be used at a given time.
class TAnimationKey {
public:
    std::string poseName;
    int time; // In some arbitrary units (e.g., 0-99)
    bool trigger;

    TAnimationKey();
    ~TAnimationKey();
    // Placeholder for loading from XML node
    bool load(void* xmlNode);
    // Placeholder for saving to XML node
    void* save() const;
};

// --- TAnimation ---
// Animation data of a model.
class TAnimation {
public:
    std::string name;
    std::vector<std::unique_ptr<TAnimationKey>> keys;

    TAnimation();
    ~TAnimation();
    // Placeholder for loading from XML node
    bool load(void* xmlNode);
    // Placeholder for saving to XML node
    void* save() const;
};

// --- TModel ---
// A 3D Model with hierarchical structure, poses, and animations.
class TModel {
public:
    std::string name;
    std::unique_ptr<TModelTreeNode> root;
    std::vector<std::unique_ptr<TPose>> poses;
    std::vector<std::unique_ptr<TAnimation>> animations;

    TModel();
    ~TModel() = default;

    // Placeholder for loading from file/stream/XML node
    bool load(const std::string& filename, TGenericMediaManager* man);
    bool load(void* xmlNode, TGenericMediaManager* man); // Use void* for TXmlNode for now

    // Helper functions
    bool hasPose(const std::string& poseName) const;
    int indexOfPose(const std::string& poseName) const;

    // Placeholder for saving to XML node
    void* save() const;
};

// --- TRealTimeAnimationKey ---
struct TRealTimeAnimationKey {
    int poseIndex;
    int time;
};

// --- TRealTimeAnimation ---
struct TRealTimeAnimation {
    std::vector<TRealTimeAnimationKey> keys;
    int triggerTime; // Time in animation (0-99) when a trigger event occurs
};

// --- TTransfTreeNode ---
// A tree node with only Transformations, useful for animations.
class TTransfTreeNode {
public:
    TTransformation transf;
    std::vector<std::unique_ptr<TTransfTreeNode>> children;

    TTransfTreeNode();
    ~TTransfTreeNode() = default;
};

// --- TRealTimeModel ---
// Represents a model with animation to be used in game, optimized for speed.
class TRealTimeModel {
private:
    std::unique_ptr<TModelTreeNode> fActualPose;
    std::vector<std::unique_ptr<TTransfTreeNode>> fPoseRoots;
    std::vector<TRealTimeAnimation> fAnimations;

    // Helper functions for building and interpolating
    std::unique_ptr<TTransfTreeNode> createTreeForPose(const TPose* pose, const TModel* model);
    std::unique_ptr<TTransfTreeNode> cloneTreeWithPose(const TPose* pose, const TModelTreeNode* node);
    void interpolateTree(const TTransfTreeNode* tree1, const TTransfTreeNode* tree2, TModelTreeNode* actual, TFloat value);
    void makeAnimation(TRealTimeAnimation& rta, const TAnimation* anim, const TModel* model);
    void interpolatePose(int pose1Index, int pose2Index, TFloat value);

public:
    TRealTimeModel();
    ~TRealTimeModel() = default;

    void build(const TModel* model);
    const TModelTreeNode* getActualPose() const { return fActualPose.get(); }

    void interpolate(int animationIndex, TFloat value);
    bool triggerPassed(int animationIndex, TFloat value) const;

    bool load(const std::string& filename, TGenericMediaManager* man);
};

#endif // MODELUNIT_HPP