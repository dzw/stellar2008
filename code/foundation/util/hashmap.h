#pragma once
#ifndef UTIL_HASHMAP_H
#define UTIL_HASHMAP_H
//------------------------------------------------------------------------------
/**
    
*/    
#include <hash_map>
#include <xhash>
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Util
{

typedef stdext::hash_compare<DWORD> DwordCompare;

template<class KEYTYPE, class VALUETYPE, class COMPARE> class HashMap
{
public:
	HashMap();
	~HashMap();
	void operator=(const HashMap<KEYTYPE, VALUETYPE, COMPARE>& rhs);
	const VALUETYPE& operator[](const KEYTYPE& key)const;
	
	/// 
	void Clear();
	bool IsEmpty()const;
	bool Add(const KEYTYPE& key, const VALUETYPE& val);
	void Erase(const KEYTYPE& key);
	bool Find(const KEYTYPE& key, VALUETYPE& val)const;
	bool Contains(const KEYTYPE& key)const;
	SizeT Size();

	/// 遍历元素
	VALUETYPE& GetFirstValue();
	VALUETYPE& GetNextValue();

	Array<VALUETYPE> GetContents();
protected:
	typedef stdext::hash_map<KEYTYPE, VALUETYPE, COMPARE> TypeMap;
	typename typedef TypeMap::iterator TypeIter;
	typename typedef TypeMap::const_iterator ConstTypeIter;

	TypeMap pairs;
	/// 遍历所有元素时会用到
	TypeIter curIter;
};

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE>
HashMap<KEYTYPE, VALUETYPE, COMPARE>::HashMap()
{
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE>
HashMap<KEYTYPE, VALUETYPE, COMPARE>::~HashMap()
{
	this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline void 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Clear()
{
	this->pairs.clear();
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline bool 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::IsEmpty()const
{
	return this->pairs.empty();
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline void 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Erase(const KEYTYPE& key)
{
	TypeIter it = this->pairs.find(key);
	if (it != this->pairs.end())
		this->pairs.erase(it);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> void
HashMap<KEYTYPE, VALUETYPE, COMPARE>::operator=(const HashMap<KEYTYPE, VALUETYPE, COMPARE>& rhs)
{
	this->pairs = rhs.pairs;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> const VALUETYPE&
HashMap<KEYTYPE, VALUETYPE, COMPARE>::operator[](const KEYTYPE& key) const
{
	ConstTypeIter it = this->pairs.find(key);
	if (it == this->pairs.end())
		n_assert(0);
	
	return it->second;
}

//------------------------------------------------------------------------------
/**
	返回值：插入成功返回true否则返回false
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline bool 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Add(const KEYTYPE& key, const VALUETYPE& val)
{
	std::pair<TypeIter, bool> p = this->pairs.insert(TypeMap::value_type(key, val));
	if (p.second)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------
/**
	找到返回true，并且val有找到的值，否则返回false
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline  bool
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Find(const KEYTYPE& key, VALUETYPE& val)const
{
	ConstTypeIter it = this->pairs.find(key);
	if (it != this->pairs.end())
	{
		val = it->second;
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline bool 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Contains(const KEYTYPE& key)const
{
	ConstTypeIter it = this->pairs.find(key);
	if (it != this->pairs.end())
		return true;
	return false;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline VALUETYPE& 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::GetFirstValue()
{
	if (!this->IsEmpty())
	{
		this->curIter = this->pairs.begin();
		return this->curIter->second;
	}
	else
		return (VALUETYPE)0;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline VALUETYPE& 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::GetNextValue()
{
	n_assert(this->IsEmpty());
	this->curIter++;
	if (this->curIter != this->pairs.end())
		return this->curIter->second;

	return (VALUETYPE)0;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE, class COMPARE> inline Array<VALUETYPE>
HashMap<KEYTYPE, VALUETYPE, COMPARE>::GetContents()
{
	Array<VALUETYPE> ret((SizeT)this->pairs.size(), (SizeT)this->pairs.size());
	TypeIter it;
	for (it = this->pairs.begin(); it != this->pairs.end(); it++)
	{
		ret.Append(it->second);
	}
	return ret;
}

template<class KEYTYPE, class VALUETYPE, class COMPARE> inline SizeT 
HashMap<KEYTYPE, VALUETYPE, COMPARE>::Size()
{
	return (SizeT)this->pairs.size();
}

} // namespace Util
//------------------------------------------------------------------------------
#endif