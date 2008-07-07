#pragma once
#ifndef IO_ASSIGN_H
#define IO_ASSIGN_H
//------------------------------------------------------------------------------
/**
    @class IO::Assign
    
    An assign associates an intuitive name with a real-world 
    filesystem path.
    
    (C) 2006 Radon Labs GmbH
*/
#include "util/keyvaluepair.h"

//------------------------------------------------------------------------------
namespace IO
{
class Assign : public Util::KeyValuePair<Util::String, Util::String>
{
public:
    /// default constructor
    Assign();
    /// constructor with assign name and path
    Assign(const Util::String& name, const Util::String& path);
    /// get name of assign
    const Util::String& GetName() const;
    /// get path of assign
    const Util::String& GetPath() const;
};

//------------------------------------------------------------------------------
/**
*/
inline
Assign::Assign()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
Assign::Assign(const Util::String& name, const Util::String& path) :
    KeyValuePair(name, path)
{
    n_assert(name.IsValid());
    n_assert(path.IsValid());
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
Assign::GetName() const
{
    n_assert(this->key.IsValid());
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
Assign::GetPath() const
{
    n_assert(this->value.IsValid());
    return this->value;
}

} // namespace IO
//------------------------------------------------------------------------------
#endif    