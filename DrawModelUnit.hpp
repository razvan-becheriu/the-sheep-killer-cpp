#ifndef DRAWMODELUNIT_HPP
#define DRAWMODELUNIT_HPP

#include "ModelUnit.hpp"

// Renders the hierarchy of a TRealTimeModel using OpenGL
void DrawRealTimeModel(TRealTimeModel* animator);

// Internal recursive helper
void RenderTreeNode(const TModelTreeNode* node);
void ApplyTrans(const TTransformation& tran);

#endif