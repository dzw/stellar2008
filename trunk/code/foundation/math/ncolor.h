#pragma once
#ifndef MATH_NCOLOR_H
#define MATH_NCOLOR_H
//------------------------------------------------------------------------------
/**
	@class Math::nColor

	(C) 2008 cTuo
*/
#include "math/d3dx9/d3dx9_float4.h"

//------------------------------------------------------------------------------
namespace Math
{
class nColor : public float4
{
public:
	/// default constructor
	nColor();
	explicit nColor(DWORD dw);
	nColor(scalar r, scalar g, scalar b, scalar a);
	/// construct from float4
	nColor(const float4& rhs);
	/// copy constructor
	nColor(const nColor& rhs);
	/// assignment operator
	void operator=(const nColor& rhs);
	/// flip sign
	nColor operator-() const;
	/// add nColor inplace
	void operator+=(const nColor& rhs);
	/// subtract nColor inplace
	void operator-=(const nColor& rhs);
	/// scale nColor inplace
	void operator*=(scalar s);
	/// add 2 vectors
	nColor operator+(const nColor& rhs) const;
	/// subtract 2 vectors
	nColor operator-(const nColor& rhs) const;
	/// scale nColor
	nColor operator*(scalar s) const;
	/// equality operator
	bool operator==(const nColor& rhs) const;
	/// inequality operator
	bool operator!=(const nColor& rhs) const;
	/// set components
	void set(scalar r, scalar g, scalar b, scalar a);
};

//------------------------------------------------------------------------------
/**
*/
__forceinline
	nColor::nColor() :
float4(0.0f, 0.0f, 0.0f, 0.0f)
{
	// empty
}

__forceinline
nColor::nColor(DWORD dw)
{
	const scalar f = 1.0f / 255.0f;
	R = f * (scalar)(unsigned char)(dw >> 16);
	G = f * (scalar)(unsigned char)(dw >> 8);
	B = f * (scalar)(unsigned char)(dw >> 0);
	A = f * (scalar)(unsigned char)(dw >> 24);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline
	nColor::nColor(scalar r, scalar g, scalar b, scalar a) :
float4(r, g, b, a)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
__forceinline
	nColor::nColor(const float4& rhs) :
float4(rhs)
{
}

//------------------------------------------------------------------------------
/**
*/
__forceinline
	nColor::nColor(const nColor& rhs) :
float4(rhs.R, rhs.G, rhs.B, rhs.A)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
__forceinline void
	nColor::operator=(const nColor& rhs)
{
	this->R = rhs.R;
	this->G = rhs.G;
	this->B = rhs.B;
	this->A = rhs.A;
}

//------------------------------------------------------------------------------
/**
*/
__forceinline nColor
	nColor::operator-() const
{
	return nColor(-this->R, -this->G, -this->B, -this->A);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline void
	nColor::operator+=(const nColor& rhs)
{
	this->R += rhs.R;
	this->G += rhs.G;
	this->B += rhs.B;
	this->A += rhs.A;
}

//------------------------------------------------------------------------------
/**
*/
__forceinline void
	nColor::operator-=(const nColor& rhs)
{
	this->R -= rhs.R;
	this->G -= rhs.G;
	this->B -= rhs.B;
	this->A -= rhs.A;
}

//------------------------------------------------------------------------------
/**
*/
__forceinline void
	nColor::operator*=(scalar s)
{
	this->R *= s;
	this->G *= s;
	this->B *= s;
	this->A *= s;
}

//------------------------------------------------------------------------------
/**
*/
__forceinline nColor
	nColor::operator+(const nColor& rhs) const
{
	return nColor(this->R + rhs.R, this->G + rhs.G, this->B + rhs.B, this->A + rhs.A);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline nColor
	nColor::operator-(const nColor& rhs) const
{
	return nColor(this->R - rhs.R, this->G - rhs.G, this->B - rhs.B, this->A - rhs.A);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline nColor
	nColor::operator*(scalar s) const
{
	return nColor(this->R * s, this->G * s, this->B * s, this->A * s);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline bool
	nColor::operator==(const nColor& rhs) const
{
	return (this->R == rhs.R) && (this->G == rhs.G) && (this->B == rhs.B) && (this->A == rhs.A);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline bool
	nColor::operator!=(const nColor& rhs) const
{
	return (this->R != rhs.R) || (this->G != rhs.G) || (this->B != rhs.B) || (this->A != rhs.A);
}    

//------------------------------------------------------------------------------
/**
*/
__forceinline void
	nColor::set(scalar r, scalar g, scalar b, scalar a)
{
	float4::set(r, g, b, a);
}

} // namespace Math
//------------------------------------------------------------------------------
#endif
