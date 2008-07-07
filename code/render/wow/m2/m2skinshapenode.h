#pragma once
#ifndef WOW_M2SKINSHAPENODE_H
#define WOW_M2SKINSHAPENODE_H
//------------------------------------------------------------------------------
/**
*/    
#include "models/nodes/Skinshapenode.h"
#include "addons/nebula2/nebula2wrapper.h"
#include "attr/attribute.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2SkinShapeNode : public Models::SkinShapeNode
{
    DeclareClass(M2SkinShapeNode);
public:
    /// constructor
    M2SkinShapeNode();
    /// destructor
    virtual ~M2SkinShapeNode();

    /// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;    
    /// apply state shared by all my SkinShapeNodeInstances
    virtual void ApplySharedState();

    /// begin defining mesh fragments
    void BeginFragments(int num);
    /// set mesh group index of a skin fragment
    void SetFragGroupIndex(int fragIndex, int primGroupIndex);
    /// get mesh group index of a skin fragment
    int GetFragGroupIndex(int fragIndex) const;
    /// begin defining the joint palette of a fragment
    void BeginJointPalette(int fragIndex, int numJoints);
    /// add up to 8 joint indices to the fragments joint palette
    void SetJointIndices(int fragIndex, int paletteIndex, int ji0, int ji1, int ji2, int ji3, int ji4, int ji5, int ji6, int ji7);
    /// add a single joint index to the fragments joint palette
    void SetJointIndex(int fragIndex, int paletteIndex, int jointIndex);
    /// finish adding joints to the joint palette
    void EndJointPalette(int fragIndex);
    /// finish defining fragments
    void EndFragments();
    /// get number of fragments
    int GetNumFragments() const;
    /// get fragment array
    const Util::FixedArray<Char::CharFragment>& GetFragmentArray() const;
    /// get number of joints in a fragment's joint palette
    int GetJointPaletteSize(int fragIndex) const;
    /// get a joint index from a fragment's joint palette
    int GetJointIndex(int fragIndex, int paletteIndex) const;
    /// request load resources, if not loaded yet
    void RequestLoadResources();
    /// request to unload our resources
    void RequestUnloadResources();
   
protected:   
    /// called when resources should be loaded
    virtual void LoadResources();    

private:    
    Util::FixedArray<Char::CharFragment> fragmentArray;
    int resourceRefCount;
};

//------------------------------------------------------------------------------
/**
    Get number of fragments.
*/
inline int
M2SkinShapeNode::GetNumFragments() const
{
    return this->fragmentArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get fragments.
*/
inline const Util::FixedArray<Char::CharFragment>& 
M2SkinShapeNode::GetFragmentArray() const
{
    return this->fragmentArray;
}

//------------------------------------------------------------------------------
/**
    Get joint palette size of a skin fragment.
*/
inline int
M2SkinShapeNode::GetJointPaletteSize(int fragIndex) const
{
    return this->fragmentArray[fragIndex].GetJointPalette().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get a joint index from a fragment's joint index.
*/
inline int
M2SkinShapeNode::GetJointIndex(int fragIndex, int paletteIndex) const
{
    return this->fragmentArray[fragIndex].GetJointPalette().GetJointIndexAt(paletteIndex);
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
