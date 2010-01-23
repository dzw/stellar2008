#ifndef M2_CHARSKELETON_H
#define M2_CHARSKELETON_H
//------------------------------------------------------------------------------
/**
    @class M2CharSkeleton
    @ingroup Character

    @brief Implements a character skeleton made of M2CharJoint objects.

    (C) 2002 RadonLabs GmbH
*/

#include "wow/m2/m2charjoint.h"
#include "wow/m2/modelheaders.h"
#include "wow/m2/m2charjoint.h"

namespace WOW
{
//------------------------------------------------------------------------------
class M2CharSkeleton
{
public:
    /// constructor
    M2CharSkeleton();
    /// destructor
    ~M2CharSkeleton();
    /// copy constructor
    M2CharSkeleton(const M2CharSkeleton& src);
    /// assignment operator
    void operator=(const M2CharSkeleton& src);
    /// clear content
    void Clear();
    /// begin adding joints
    void BeginJoints(int num);
    /// add a joint to the joint skeleton
    void SetJoint(int index, unsigned char *f, ModelBoneDef &b, int *global);
    /// finish adding joints
    void EndJoints();
    /// get number of joint
    int GetNumJoints() const;
    /// get joint by index
    M2CharJoint& GetJointAt(int index) const;
    /// get joint by index (faster but unsafe, no index checking done)
    const M2CharJoint& GetJointAtUnsafe(int index) const;
    /// get joint index by name
    int GetJointIndexByName(const Util::String& name) const;
    /// evaluate all character joints
    void Evaluate(int anim, int time);
        
private:
    /// update the parent joint pointers from their indices
    void UpdateParentJointPointers();

    Util::FixedArray<M2CharJoint> jointArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
M2CharSkeleton::Evaluate(int anim, int time)
{
    // first pass: clear uptodate flag
    int i;
    int num = this->jointArray.Size();
    for (i = 0; i < num; i++)
    {
        this->jointArray[i].ClearUptodateFlag();
    }
    // seconds pass: evaluate joints
    for (i = 0; i < num; i++)
    {
        this->jointArray[i].Evaluate(&jointArray[0], anim, time);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
M2CharSkeleton::M2CharSkeleton()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Update the parent joint pointers in the joint array.
*/
inline
void
M2CharSkeleton::UpdateParentJointPointers()
{
    /*uint i;
    for (i = 0; i < this->jointArray.Size(); i++)
    {
        int parentJointIndex = this->jointArray[i].GetParentJointIndex();
        if (parentJointIndex != -1)
        {
            jointArray[i].SetParentJoint(&(this->jointArray[parentJointIndex]));
        }
    }*/
}

//------------------------------------------------------------------------------
/**
*/
inline
M2CharSkeleton::M2CharSkeleton(const M2CharSkeleton& src)
{
    this->jointArray = src.jointArray;
    this->UpdateParentJointPointers();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
M2CharSkeleton::operator=(const M2CharSkeleton& src)
{
    n_assert(&src != this);
    this->jointArray = src.jointArray;
    this->UpdateParentJointPointers();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
M2CharSkeleton::Clear()
{
    this->jointArray.SetSize(0);
}

//------------------------------------------------------------------------------
/**
*/
inline
M2CharSkeleton::~M2CharSkeleton()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Start joint skeleton definition.

    @param  num     number of joints in skeleton
*/
inline
void
M2CharSkeleton::BeginJoints(int num)
{
    n_assert(num > 0);
    this->jointArray.SetSize(num);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the skeleton.
*/
inline
void
M2CharSkeleton::SetJoint(int index, unsigned char *f, ModelBoneDef &b, int *global)
{
    M2CharJoint newJoint;
    newJoint.Init(f, b, global);
    this->jointArray[index] = newJoint;
}

//------------------------------------------------------------------------------
/**
    Finish defining joints.
*/
inline
void
M2CharSkeleton::EndJoints()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
M2CharSkeleton::GetNumJoints() const
{
    return this->jointArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
M2CharJoint&
M2CharSkeleton::GetJointAt(int index) const
{
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
/**
*/
__forceinline const M2CharJoint&
M2CharSkeleton::GetJointAtUnsafe(int index) const
{
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
M2CharSkeleton::GetJointIndexByName(const Util::String& name) const
{
    /*uint index;
    for (index = 0; index < this->jointArray.Size(); index++)
    {
        if (this->jointArray[index].GetName() == name)
        {
            return index;
        }
    }
    return -1;*/
}
}; // namespace Nebula2
//------------------------------------------------------------------------------
#endif
