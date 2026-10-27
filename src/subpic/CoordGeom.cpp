/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include <math.h>
#include "CoordGeom.h"

#define EPSILON (1e-7)
#define BIGNUMBER (1e+9)
#define IsZero(d) (std::fabs(d) < EPSILON)

//
// Vector
//

Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector::Set(float x, float y, float z)

{
	this->x = x;
	this->y = y;
	this->z = z;
}

[[nodiscard]] float Vector::Length()

{
	return(std::sqrt(x * x + y * y + z * z));
}

[[nodiscard]] float Vector::Sum()

{
	return(x + y + z);
}

[[nodiscard]] float Vector::CrossSum()

{
	return(x*y + x*z + y*z);
}

[[nodiscard]] Vector Vector::Cross()

{
	return(Vector(x*y, x*z, y*z));
}

[[nodiscard]] Vector Vector::Pow(const float exp)

{
	return(exp == 0 ? Vector(1, 1, 1) : exp == 1 ? *this : Vector(std::pow(x, exp), std::pow(y, exp), std::pow(z, exp)));
}

[[nodiscard]] Vector Vector::Unit()

{
	float l{Length()};
	if(!l || l == 1)
	{
	    return(*this);
	}
	return(*this * (1 / l));
}

[[nodiscard]] Vector& Vector::Unitalize()

{
	return(*this = Unit());
}

[[nodiscard]] Vector Vector::Normal(Vector& a, Vector& b)

{
	return((a - *this) % (b - a));
}

[[nodiscard]] float Vector::Angle(Vector& a, Vector& b)
{
	return(((a - *this).Unit()).Angle((b - *this).Unit()));
}

[[nodiscard]] float Vector::Angle(Vector& a)
{
	float angle{*this | a};
	return((angle > 1) ? 0 : (angle < -1) ? PI : std::acos(angle));
}

void Vector::Angle(float& u, float& v)
{
	Vector n{Unit()};

	u = std::asin(n.y);

	if(IsZero(n.z))
	{
	    v = PI/2 * Sgn(n.x);
	}
	else if(n.z > 0)
	{
	    v = std::atan(n.x / n.z);
	}
	else if(n.z < 0)
	{
	    v = IsZero(n.x) ? PI : (PI * Sgn(n.x) + std::atan(n.x / n.z));
	}
}

[[nodiscard]] Vector Vector::Angle()
{
	Vector ret;
	Angle(ret.x, ret.y);
	ret.z = 0;
	return(ret);
}

[[nodiscard]] Vector& Vector::Min(Vector& a)

{
	x = (x < a.x) ? x : a.x;
	y = (y < a.y) ? y : a.y;
	z = (z < a.z) ? z : a.z;
	return(*this);
}

[[nodiscard]] Vector& Vector::Max(Vector& a)

{
	x = (x > a.x) ? x : a.x;
	y = (y > a.y) ? y : a.y;
	z = (z > a.z) ? z : a.z;
	return(*this);
}

[[nodiscard]] Vector Vector::Abs()

{
	return(Vector(std::fabs(x), std::fabs(y), std::fabs(z)));
}

[[nodiscard]] Vector Vector::Reflect(Vector& n)

{
	return(n * ((-*this) | n) * 2 - (-*this));
}

[[nodiscard]] Vector Vector::Refract(Vector& N, const float nFront, const float nBack, float* nOut)

{
	Vector D{-*this};

	float N_dot_D{(N | D)};
	float n{N_dot_D >= 0 ? (nFront / nBack) : (nBack / nFront)};

	Vector cos_D{N * N_dot_D};
	Vector sin_T{(cos_D - D) * n};

	float len_sin_T{sin_T | sin_T};

	if(len_sin_T > 1) 
	{
		if(nOut) {*nOut = N_dot_D >= 0 ? nFront : nBack;}
		return((*this).Reflect(N));
	}

	float N_dot_T = std::sqrt(1.0 - len_sin_T);
	if(N_dot_D < 0)
	{
	    N_dot_T = -N_dot_T;
	}

	if(nOut) {*nOut = N_dot_D >= 0 ? nBack : nFront;}

	return(sin_T - (N * N_dot_T));
}

[[nodiscard]] Vector Vector::Refract2(Vector& N, const float nFrom, const float nTo, float* nOut)

{
	Vector D{-*this};

	float N_dot_D{(N | D)};
	float n{nFrom / nTo};

	Vector cos_D{N * N_dot_D};
	Vector sin_T{(cos_D - D) * n};

	float len_sin_T{sin_T | sin_T};

	if(len_sin_T > 1) 
	{
		if(nOut) {*nOut = nFrom;}
		return((*this).Reflect(N));
	}

	float N_dot_T = std::sqrt(1.0 - len_sin_T);
	if(N_dot_D < 0)
	{
	    N_dot_T = -N_dot_T;
	}

	if(nOut) {*nOut = nTo;}

	return(sin_T - (N * N_dot_T));
}

[[nodiscard]] float Vector::operator | (Vector& v)
{
	return(x * v.x + y * v.y + z * v.z);
}

[[nodiscard]] Vector Vector::operator % (Vector& v)
{
	return(Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
}

float& Vector::operator [] (int i)
{
	return(!i ? x : (i == 1) ? y : z);
}

[[nodiscard]] Vector Vector::operator - ()
{
	return(Vector(-x, -y, -z));
}

[[nodiscard]] bool Vector::operator == (const Vector& v) const
{
	if(IsZero(x - v.x) && IsZero(y - v.y) && IsZero(z - v.z))
	{
	    return(true);
	}
	return(false);
}

[[nodiscard]] bool Vector::operator != (const Vector& v) const
{
	return((*this == v) ? false : true);
}

[[nodiscard]] Vector Vector::operator + (float d)
{
	return(Vector(x + d, y + d, z + d));
}

[[nodiscard]] Vector Vector::operator + (Vector& v)
{
	return(Vector(x + v.x, y + v.y, z + v.z));
}

[[nodiscard]] Vector Vector::operator - (float d)
{
	return(Vector(x - d, y - d, z - d));
}

[[nodiscard]] Vector Vector::operator - (Vector& v)
{
	return(Vector(x - v.x, y - v.y, z - v.z));
}

[[nodiscard]] Vector Vector::operator * (float d)
{
	return(Vector(x * d, y * d, z * d));
}

[[nodiscard]] Vector Vector::operator * (Vector& v)
{
	return(Vector(x * v.x, y * v.y, z * v.z));
}

[[nodiscard]] Vector Vector::operator / (float d)
{
	return(Vector(x / d, y / d, z / d));
}

[[nodiscard]] Vector Vector::operator / (Vector& v)
{
	return(Vector(x / v.x, y / v.y, z / v.z));
}

Vector& Vector::operator += (float d)
{
	x += d; y += d; z += d;
	return(*this);
}

Vector& Vector::operator += (Vector& v)
{
	x += v.x; y += v.y; z += v.z;
	return(*this);
}

Vector& Vector::operator -= (float d)
{
	x -= d; y -= d; z -= d;
	return(*this);
}

Vector& Vector::operator -= (Vector& v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return(*this);
}

Vector& Vector::operator *= (float d)
{
	x *= d; y *= d; z *= d;
	return(*this);
}

Vector& Vector::operator *= (Vector& v)
{
	x *= v.x; y *= v.y; z *= v.z;
	return(*this);
}

Vector& Vector::operator /= (float d)
{
	x /= d; y /= d; z /= d;
	return(*this);
}

Vector& Vector::operator /= (Vector& v)
{
	x /= v.x; y /= v.y; z /= v.z;
	return(*this);
}

//
// Ray
//

Ray::Ray(Vector& p, Vector& d)
{
	this->p = p;
	this->d = d;
}

void Ray::Set(Vector& p, Vector& d)

{
	this->p = p;
	this->d = d;
}

[[nodiscard]] float Ray::GetDistanceFrom(Ray& r)
{
	float t{(d | r.d)};
	if(IsZero(t))
	{
	    return(-BIGNUMBER);
	} // plane is paralell to the ray, return -infinite
	return(((r.p - p) | r.d) / t);
}

[[nodiscard]] float Ray::GetDistanceFrom(Vector& v)
{
	float t{((v - p) | d) / (d | d)};
	return(((p + d*t) - v).Length());
}

Vector Ray::operator [] (float t)
{
	return(p + d*t);
}

//
// XForm
//

XForm::XForm(Ray& r, Vector& s, bool isWorldToLocal)
{
	Initalize(r, s, isWorldToLocal);
}

void XForm::Initalize()
{
	m.Initalize();
}

void XForm::Initalize(Ray& r, Vector& s, const bool isWorldToLocal)
{
	Initalize();

	if(m_isWorldToLocal = isWorldToLocal)
	{
		*this -= r.p;
		*this >>= r.d;
		*this /= s;

	}
	else
	{
		*this *= s;
		*this <<= r.d;
		*this += r.p;
	}
}

void XForm::operator *= (Vector& v)
{
	Matrix s;
	s.mat[0U][0U] = v.x;
	s.mat[1U][1U] = v.y;
	s.mat[2U][2U] = v.z;
	m *= s;
}

void XForm::operator += (Vector& v)
{
	Matrix t;
	t.mat[3U][0U] = v.x;
	t.mat[3U][1U] = v.y;
	t.mat[3U][2U] = v.z;
	m *= t;
}

void XForm::operator <<= (Vector& v)
{
	Matrix x;
	x.mat[1U][1U] = std::cos(v.x); x.mat[1U][2U] = -std::sin(v.x);
	x.mat[2U][1U] = std::sin(v.x); x.mat[2U][2U] = std::cos(v.x);

	Matrix y;
	y.mat[0U][0U] = std::cos(v.y); y.mat[0U][2U] = -std::sin(v.y);
	y.mat[2U][0U] = std::sin(v.y); y.mat[2U][2U] = std::cos(v.y);

	Matrix z;
	z.mat[0U][0U] = std::cos(v.z); z.mat[0U][1U] = -std::sin(v.z);
	z.mat[1U][0U] = std::sin(v.z); z.mat[1U][1U] = std::cos(v.z);

	m = m_isWorldToLocal ? (m * y * x * z) : (m * z * x * y);
}

void XForm::operator /= (Vector& v)
{
	Vector s;
	s.x = IsZero(v.x) ? 0 : 1 / v.x;
	s.y = IsZero(v.y) ? 0 : 1 / v.y;
	s.z = IsZero(v.z) ? 0 : 1 / v.z;
	*this *= s;
}

void XForm::operator -= (Vector& v)
{
	*this += -v;
}

void XForm::operator >>= (Vector& v)
{
	*this <<= -v;
}

[[nodiscard]] Vector XForm::operator < (Vector& n)
{
	Vector ret;

	ret.x = n.x * m.mat[0U][0U] +
			n.y * m.mat[1U][0U] +
			n.z * m.mat[2U][0U];
	ret.y = n.x * m.mat[0U][1U] +
			n.y * m.mat[1U][1U] +
			n.z * m.mat[2U][1U];
	ret.z = n.x * m.mat[0U][2U] +
			n.y * m.mat[1U][2U] +
			n.z * m.mat[2U][2U];

	return(ret);
}

Vector XForm::operator << (Vector& v)
{
	Vector ret;

	ret.x = v.x * m.mat[0U][0U] +
			v.y * m.mat[1U][0U] +
			v.z * m.mat[2U][0U] +
			m.mat[3U][0U];
	ret.y = v.x * m.mat[0U][1U] +
			v.y * m.mat[1U][1U] +
			v.z * m.mat[2U][1U] +
			m.mat[3U][1U];
	ret.z = v.x * m.mat[0U][2U] +
			v.y * m.mat[1U][2U] +
			v.z * m.mat[2U][2U] +
			m.mat[3U][2U];

	return(ret);
}

Ray XForm::operator << (Ray& r)
{
	return(Ray(*this << r.p, *this < r.d));
}

//
// XForm::Matrix
//

XForm::Matrix::Matrix()
{
	Initalize();
}

void XForm::Matrix::Initalize()
{
	mat[0U][0U] = 1; mat[0U][1U] = 0; mat[0U][2U] = 0; mat[0U][3U] = 0;
	mat[1U][0U] = 0; mat[1U][1U] = 1; mat[1U][2U] = 0; mat[1U][3U] = 0;
	mat[2U][0U] = 0; mat[2U][1U] = 0; mat[2U][2U] = 1; mat[2U][3U] = 0;
	mat[3U][0U] = 0; mat[3U][1U] = 0; mat[3U][2U] = 0; mat[3U][3U] = 1;
}

[[nodiscard]] XForm::Matrix XForm::Matrix::operator * (Matrix& m)
{
	Matrix ret;

	for(int i{0}; i < 4; i++)
	{
		for(int j{0}; j < 4; j++)
		{
			ret.mat[i][j] = mat[i][0U] * m.mat[0U][j] +
							mat[i][1U] * m.mat[1U][j] +
							mat[i][2U] * m.mat[2U][j] +
							mat[i][3U] * m.mat[3U][j];

			if(IsZero(ret.mat[i][j]))
			{
			    ret.mat[i][j] = 0;
			}
		}
	}

	return(ret);
}

XForm::Matrix& XForm::Matrix::operator *= (XForm::Matrix& m)
{
	return(*this = *this * m);
}
