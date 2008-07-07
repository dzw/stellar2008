#pragma once
#ifndef MATH__VECTOR3_H
#define MATH__VECTOR3_H
//------------------------------------------------------------------------------
/**
    @class Math::vector3

    Generic vector3 class.

    (C) 2002 RadonLabs GmbH
*/
//#include "math/math.h"
#include <float.h>
#include "math/scalar.h"

//------------------------------------------------------------------------------
namespace Math
{
class vector3
{
public:
    /// constructor 1
    vector3();
    /// constructor 2
    vector3(const float _x, const float _y, const float _z);
    /// constructor 3
    vector3(const vector3& vec);
    /// set elements 1
    void set(const float _x, const float _y, const float _z);
    /// set elements 2 
    void set(const vector3& vec);
    /// return length
    float len() const;
    /// return length squared
    float lensquared() const;
    /// normalize
    void norm();
    /// inplace add
    void operator +=(const vector3& v0);
    /// inplace sub
    void operator -=(const vector3& v0);
    /// inplace scalar multiplication
    void operator *=(float s);
    /// true if any of the elements are greater
    bool operator >(const vector3& rhs);
    /// true if any of the elements are smaller
    bool operator <(const vector3& rhs);
    /// true if all elements are equal
    bool operator ==(const vector3& v0) const;
    /// true if any of the elements is not equal
    bool operator !=(const vector3& v0) const;
    /// fuzzy compare, return true/false
    bool isequal(const vector3& v, float tol) const;
    /// fuzzy compare, returns -1, 0, +1
    int compare(const vector3& v, float tol) const;
    /// rotate around axis
    void rotate(const vector3& axis, float angle);
    /// inplace linear interpolation
    void lerp(const vector3& v0, float lerpVal);
    /// returns a vector orthogonal to self, not normalized
    vector3 findortho() const;
    /// saturate components between 0 and 1
    void saturate();
    /// clamp components between min and max
    void clamp(float min, float max);
    /// dot product
    float dot(const vector3& v0) const;
    /// distance between 2 vector3's
    static float distance(const vector3& v0, const vector3& v1);
    /// returns the angle between 2 vectors
    static float angle(const vector3& v0, const vector3& v1);
    /// returns a new vector which is the lerp-result between v0 and v1
    static vector3 lerp(const vector3& v0, const vector3& v1, float l);

    float x, y, z;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector3::vector3() :
    x(0.0f),
    y(0.0f),
    z(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3::vector3(const float _x, const float _y, const float _z) :
    x(_x),
    y(_y),
    z(_z)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3::vector3(const vector3& vec) :
    x(vec.x),
    y(vec.y),
    z(vec.z)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::set(const float _x, const float _y, const float _z)
{
    x = _x;
    y = _y;
    z = _z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::set(const vector3& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
vector3::len() const
{
    return (float) n_sqrt(x * x + y * y + z * z);
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
vector3::lensquared() const
{
    return x * x + y * y + z * z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::norm()
{
    float l = len();
    if (l > TINY) 
    {
        x /= l;
        y /= l;
        z /= l;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::operator +=(const vector3& v0)
{
    x += v0.x;
    y += v0.y;
    z += v0.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::operator -=(const vector3& v0)
{
    x -= v0.x;
    y -= v0.y;
    z -= v0.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::operator *=(float s)
{
    x *= s;
    y *= s;
    z *= s;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector3::isequal(const vector3& v, float tol) const
{
    if (fabs(v.x - x) > tol)      return false;
    else if (fabs(v.y - y) > tol) return false;
    else if (fabs(v.z - z) > tol) return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
vector3::compare(const vector3& v, float tol) const
{
    if (fabs(v.x - x) > tol)      return (v.x > x) ? +1 : -1; 
    else if (fabs(v.y - y) > tol) return (v.y > y) ? +1 : -1;
    else if (fabs(v.z - z) > tol) return (v.z > z) ? +1 : -1;
    else                          return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::rotate(const vector3& axis, float angle)
{
    // rotates this one around given vector. We do
    // rotation with matrices, but these aren't defined yet!
    float rotM[9];
    float sa, ca;

    sa = (float) sin(angle);
    ca = (float) cos(angle);

    // build a rotation matrix
    rotM[0] = ca + (1 - ca) * axis.x * axis.x;
    rotM[1] = (1 - ca) * axis.x * axis.y - sa * axis.z;
    rotM[2] = (1 - ca) * axis.z * axis.x + sa * axis.y;
    rotM[3] = (1 - ca) * axis.x * axis.y + sa * axis.z;
    rotM[4] = ca + (1 - ca) * axis.y * axis.y;
    rotM[5] = (1 - ca) * axis.y * axis.z - sa * axis.x;
    rotM[6] = (1 - ca) * axis.z * axis.x - sa * axis.y;
    rotM[7] = (1 - ca) * axis.y * axis.z + sa * axis.x;
    rotM[8] = ca + (1 - ca) * axis.z * axis.z;

    // "handmade" multiplication
    vector3 help(rotM[0] * this->x + rotM[1] * this->y + rotM[2] * this->z,
                  rotM[3] * this->x + rotM[4] * this->y + rotM[5] * this->z,
                  rotM[6] * this->x + rotM[7] * this->y + rotM[8] * this->z);
    *this = help;
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
vector3 operator +(const vector3& v0, const vector3& v1) 
{
    return vector3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
vector3 operator -(const vector3& v0, const vector3& v1) 
{
    return vector3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
vector3 operator *(const vector3& v0, const float s) 
{
    return vector3(v0.x * s, v0.y * s, v0.z * s);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
vector3 operator -(const vector3& v) 
{
    return vector3(-v.x, -v.y, -v.z);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
vector3 operator /(const vector3& v0, const float s)
{
    float one_over_s = 1.0f/s;
    return vector3(v0.x*one_over_s, v0.y*one_over_s, v0.z*one_over_s);
}

//------------------------------------------------------------------------------
/**
    Dot product.
*/
static
inline
float operator %(const vector3& v0, const vector3& v1)
{
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

//------------------------------------------------------------------------------
/**
    Cross product.
*/
static 
inline 
vector3 operator *(const vector3& v0, const vector3& v1) 
{
    return vector3(v0.y * v1.z - v0.z * v1.y,
                    v0.z * v1.x - v0.x * v1.z,
                    v0.x * v1.y - v0.y * v1.x);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::lerp(const vector3& v0, float lerpVal)
{
    x = v0.x + ((x - v0.x) * lerpVal);
    y = v0.y + ((y - v0.y) * lerpVal);
    z = v0.z + ((z - v0.z) * lerpVal);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::saturate()
{
    x = n_saturate(x);
    y = n_saturate(y);
    z = n_saturate(z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector3::clamp(float min, float max)
{
    x = n_clamp(x, min, max);
    y = n_clamp(y, min, max);
    z = n_clamp(z, min, max);
}

//------------------------------------------------------------------------------
/**
    Find a vector that is orthogonal to self. Self should not be (0,0,0).
    Return value is not normalized.
*/
inline
vector3
vector3::findortho() const
{
    if (0.0 != x)
    {
        return vector3((-y - z) / x, 1.0, 1.0);
    } 
    else if (0.0 != y)
    {
        return vector3(1.0, (-x - z) / y, 1.0);
    } 
    else if (0.0 != z)
    {
        return vector3(1.0, 1.0, (-x - y) / z);
    } 
    else 
    {
        return vector3(0.0, 0.0, 0.0);
    }
}

//------------------------------------------------------------------------------
/**
    Dot product for vector3
*/
inline
float
vector3::dot(const vector3& v0) const
{
    return x * v0.x + y * v0.y + z * v0.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector3::operator ==(const vector3& rhs) const
{
    if ((this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector3::operator !=(const vector3& rhs) const
{
    if ((this->x != rhs.x) || (this->y != rhs.y) || (this->z != rhs.z))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector3::operator >(const vector3& rhs)
{
    if ((this->x > rhs.x) || (this->y > rhs.y) || (this->z > rhs.z))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector3::operator <(const vector3& rhs)
{
    if ((this->x < rhs.x) || (this->y < rhs.y) || (this->z < rhs.z))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
vector3::distance(const vector3& v0, const vector3& v1)
{
    vector3 v(v1 - v0);
    return (float) n_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
vector3::angle(const vector3& v0, const vector3& v1)
{
    vector3 v0n = v0;
    vector3 v1n = v1;
    v0n.norm();
    v1n.norm();
    float a = n_acos(v0n % v1n);
    return a;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3
vector3::lerp(const vector3& v0, const vector3& v1, float l)
{
    return v0 + ((v1 - v0) * l);
}

}; // namespace Math
//------------------------------------------------------------------------------
#endif
