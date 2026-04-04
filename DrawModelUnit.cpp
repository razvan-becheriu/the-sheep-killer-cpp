#include "DrawModelUnit.hpp"
#include "OrthoUnit.hpp"
#include "TextureUnit.hpp"
#include <GL/gl.h>

void ApplyTrans(const TTransformation& tran) {
    glTranslatef(tran.pos.x, tran.pos.y, tran.pos.z);
    
    glRotatef(tran.rot.x, 1, 0, 0);
    glRotatef(tran.rot.y, 0, 1, 0);
    glRotatef(tran.rot.z, 0, 0, 1);
    
    glScalef(tran.sca.x, tran.sca.y, tran.sca.z);
}

void ToggleFaceMode() {
    GLint frontFace;
    glGetIntegerv(GL_FRONT_FACE, &frontFace);
    if (frontFace == GL_CCW) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);
}

void RenderTreeNode(const TModelTreeNode* node) {
    if (!node) return;

    glPushMatrix();
    ApplyTrans(node->transf);

    // Check for negative scaling which flips faces
    bool toggle = node->transf.mustToggleFaces();
    if (toggle) ToggleFaceMode();

    if (node->texture == nullptr) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        node->texture->bind();
    }

    glColor3f(node->color.r, node->color.g, node->color.b);

    if (node->graphicalElement.type == geMesh) {
        if (node->graphicalElement.mesh) {
            node->graphicalElement.mesh->Render();
        }
    } else if (node->graphicalElement.type == geModel) {
        if (node->graphicalElement.model && node->graphicalElement.model->root) {
            RenderTreeNode(node->graphicalElement.model->root.get());
        }
    }

    for (const auto& child : node->children) {
        RenderTreeNode(child.get());
    }

    if (toggle) ToggleFaceMode();
    glPopMatrix();
}

void DrawRealTimeModel(TRealTimeModel* animator) {
    if (animator && animator->getActualPose()) {
        RenderTreeNode(animator->getActualPose());
    }
}