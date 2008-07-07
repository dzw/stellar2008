#pragma once
#ifndef UTIL_MAP_H
#define UTIL_MAP_H
//------------------------------------------------------------------------------
/**
    
*/    
#include <map>

//------------------------------------------------------------------------------
namespace Util
{
template<class KEYTYPE, class VALUETYPE> class Map
{
public:
	Map();
	~Map();
	void operator=(const Map<KEYTYPE, VALUETYPE>& rhs);
	const VALUETYPE& operator[](const KEYTYPE& key)const;
	VALUETYPE& operator[](const KEYTYPE& key);
	/// 
	void Clear();
	bool IsEmpty()const;
	bool Add(const KEYTYPE& key, const VALUETYPE& val);
	void Erase(const KEYTYPE& key);
	bool Find(const KEYTYPE& key)const;
	bool Contains(const KEYTYPE& key)const;

	/// 遍历元素
	VALUETYPE& GetFirstValue();
	VALUETYPE& GetNextValue();
protected:
	typedef std::map<KEYTYPE, VALUETYPE> TypeMap;
	typename typedef TypeMap::iterator TypeIter;
	typename typedef TypeMap::const_iterator ConstTypeIter;

	TypeMap pairs;
	/// 遍历所有元素时会用到
	TypeIter curIter;
};

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
Map<KEYTYPE, VALUETYPE>::Map()
{
	this->curIter = NULL;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE>
Map<KEYTYPE, VALUETYPE>::~Map()
{
	this->curIter.clear();
}


//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline void 
Map<KEYTYPE, VALUETYPE>::Clear()
{
	pairs.clear();
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline bool 
Map<KEYTYPE, VALUETYPE>::IsEmpty()const
{
	return this->pairs.size() > 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline void 
Map<KEYTYPE, VALUETYPE>::Erase(const KEYTYPE& key)
{
	TypeIter it = this->pairs.find(key);
	if (it != this->pairs.end())
		this->pairs.erase(it);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> void
Map<KEYTYPE, VALUETYPE>::operator=(const Map<KEYTYPE, VALUETYPE>& rhs)
{
	this->pairs = rhs.pairs;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> const VALUETYPE&
Map<KEYTYPE, VALUETYPE>::operator[](const KEYTYPE& key) const
{
	ConstTypeIter it = this->pairs.find(key);
	if (it == this->pairs.end())
		n_assert(0);
	
	return it->second;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> VALUETYPE&
Map<KEYTYPE, VALUETYPE>::operator[](const KEYTYPE& key)
{
	TypeIter it = this->pairs.find(key);
	if (it == this->pairs.end())
		n_assert(0);

	return it->second;
}

//------------------------------------------------------------------------------
/**
	返回值：插入成功返回true否则返回false
*/
template<class KEYTYPE, class VALUETYPE> inline bool 
Map<KEYTYPE, VALUETYPE>::Add(const KEYTYPE& key, const VALUETYPE& val)
{
	std::pair<TypeIter, bool> p = this->pairs.insert(TypeMap::value_type(key, val));
	if (p.second)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline bool 
Map<KEYTYPE, VALUETYPE>::Find(const KEYTYPE& key)const
{
	ConstTypeIter it = pairs.find(key);
	if (it != pairs.end())
		return true;
	return false;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline bool 
Map<KEYTYPE, VALUETYPE>::Contains(const KEYTYPE& key)const
{
	return this->Find(key);
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline VALUETYPE& 
Map<KEYTYPE, VALUETYPE>::GetFirstValue()
{
	n_assert(this->IsEmpty() || this->curIter != NULL);
	this->curIter = this->pairs.begin();
	return this->curIter->second;
}

//------------------------------------------------------------------------------
/**
*/
template<class KEYTYPE, class VALUETYPE> inline VALUETYPE& 
Map<KEYTYPE, VALUETYPE>::GetNextValue()
{
	n_assert(this->IsEmpty() || this->curIter == NULL);
	this->curIter++;
	if (this->curIter != this->pairs.end())
		return this->curIter->second;

	this->curIter = NULL;
	return (VALUETYPE)0;
}

} // namespace Util
//------------------------------------------------------------------------------
#endif