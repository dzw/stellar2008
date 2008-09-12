#pragma once
#ifndef MODELS_ATTRIBUTES_H
#define MODELS_ATTRIBUTES_H
//------------------------------------------------------------------------------
/**
    @file models/attributes.h
    
    Define Models subsystem attributes.
    
    (C) 2007 Radon Labs GmbH
*/
#include "attr/attribute.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareFloat4(BoxCenter, 'BOXC', ReadWrite);
    DeclareFloat4(BoxExtents, 'BOXE', ReadWrite);

    // Models::TransformNode
    DeclareFloat4(Position, 'POSN', ReadWrite);
    DeclareFloat4(Rotate, 'ROTA', ReadWrite);
    DeclareFloat4(Scale, 'SCAL', ReadWrite);
    DeclareFloat4(RotatePivot, 'RPVT', ReadWrite);
    DeclareFloat4(ScalePivot, 'SPVT', ReadWrite);

    // Models::StateNode
    DeclareString(Shader, 'SHDR', ReadWrite);

    // Models::ShapeNode
    DeclareString(MeshResourceId, 'MSRI', ReadWrite);
    DeclareInt(MeshGroupIndex, 'MSGI', ReadWrite);

    // legacy Nebula2 shader params
    DeclareString(DiffMap0, 'DIM0', ReadWrite);
    DeclareString(EmsvMap0, 'EVM0', ReadWrite);
    DeclareString(SpecMap0, 'SCM0', ReadWrite);
    DeclareString(BumpMap0, 'BMM0', ReadWrite);
    DeclareInt(CullMode, 'CULM', ReadWrite);
    DeclareFloat(MatEmissiveIntensity, 'MESI', ReadWrite);
    DeclareFloat(MatSpecularIntensity, 'MSCI', ReadWrite);
    DeclareFloat(MatSpecularPower, 'MSCP', ReadWrite);
    DeclareFloat(BumpScale, 'BMSC', ReadWrite);

	// kok shader params
	DeclareInt(SrcAlphaBlend, 'SABD', ReadWrite);
	DeclareInt(DestAlphaBlend, 'DABD', ReadWrite);

    // Models::CharacterNode
    DeclareString(AnimFilename, 'ANFN', ReadWrite);
    DeclareBlob(JointIndices, 'JIND', ReadWrite);
    DeclareBlob(JointFragments, 'JFRA', ReadWrite);
    DeclareBlob(Clips, 'CLPS', ReadWrite);

	//Models::ParticleSystem

	// WOW中地形的混合纹理
	DeclareString(TexBlend0, 'TEB0', ReadWrite);
	// WOW中地形四层纹理设置
	DeclareString(DiffMap1, 'DIM1', ReadWrite);
	DeclareString(DiffMap2, 'DIM2', ReadWrite);
	DeclareString(DiffMap3, 'DIM3', ReadWrite);

    DeclareString(LoadChunk, 'LCHK', ReadWrite);
}
//------------------------------------------------------------------------------
#endif    