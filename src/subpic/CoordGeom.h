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

#pragma once 

#ifndef PI
#define PI (3.141592654f)
#endif

#define DegToRad(d) ((d)*PI/180.0)
#define RadToDeg(r) ((r)*180.0/PI)
#define Sgn(d) (IsZero(d) ? 0 : (d) > 0 ? 1 : -1)
#define SgnPow(d, p) (IsZero(d) ? 0 : (std::pow(std::fabs(d), p) * Sgn(d)))

class Vector
{
public:
	float x, y, z;

	explicit Vector() {x = y = z = 0;}
	Vector(float x, float y, float z);
	void Set(float x, float y, float z);

	[[nodiscard]] Vector Normal(const Vector& a, const Vector& b) const;
	[[nodiscard]] float Angle(const Vector& a, const Vector& b) const;
	[[nodiscard]] float Angle(const Vector& a) const;
	void Angle(float& u, float& v) const; // returns spherical coords in radian, -PI/2 <= u <= PI/2, -PI <= v <= PI
	Vector Angle() const; // does like prev., returns 'u' in 'ret.x', and 'v' in 'ret.y'

	Vector Unit() const;
	[[nodiscard]] Vector& Unitalize();
	[[nodiscard]] float Length() const;
	[[nodiscard]] float Sum() const; // x + y + z
	[[nodiscard]] float CrossSum() const; // xy + xz + yz
	Vector Cross() const; // xy, xz, yz
	[[nodiscard]] Vector Pow(float exp) const;

	[[nodiscard]] Vector& Min(const Vector& a);
	[[nodiscard]] Vector& Max(const Vector& a);
	Vector Abs() const;

	[[nodiscard]] Vector Reflect(const Vector& n) const;
	[[nodiscard]] Vector Refract(const Vector& n, float nFront, float nBack, float* nOut = nullptr) const;
	[[nodiscard]] Vector Refract2(const Vector& n, float nFrom, float nTo, float* nOut = nullptr) const;

	[[nodiscard]] Vector operator - () const;
	float& operator [] (int i);

	[[nodiscard]] float operator | (const Vector& v) const; // dot
	[[nodiscard]] Vector operator % (const Vector& v) const; // cross

	[[nodiscard]] bool operator == (const Vector& v) const;
	[[nodiscard]] bool operator != (const Vector& v) const;

	[[nodiscard]] Vector operator + (float d) const;
	[[nodiscard]] Vector operator + (const Vector& v) const;
	[[nodiscard]] Vector operator - (float d) const;
	[[nodiscard]] Vector operator - (const Vector& v) const;
	[[nodiscard]] Vector operator * (float d) const;
	[[nodiscard]] Vector operator * (const Vector& v) const;
	[[nodiscard]] Vector operator / (float d) const;
	[[nodiscard]] Vector operator / (const Vector& v) const;
	Vector& operator += (float d);
	Vector& operator += (const Vector& v);
	Vector& operator -= (float d);
	Vector& operator -= (const Vector& v);
	Vector& operator *= (float d);
	Vector& operator *= (const Vector& v);
	Vector& operator /= (float d);
	Vector& operator /= (const Vector& v);
};

class Ray
{
public:
	Vector p, d;

	explicit Ray() {}
	Ray(const Vector& p, const Vector& d);
	void Set(const Vector& p, const Vector& d);

	[[nodiscard]] float GetDistanceFrom(const Ray& r) const; // r = plane
	[[nodiscard]] float GetDistanceFrom(const Vector& v) const; // v = point

	Vector operator [] (float t) const;
};

class XForm
{
	class Matrix
	{
	public:
		float mat[4U][4U];

		explicit Matrix();
		void Initalize();

		[[nodiscard]] Matrix operator * (const Matrix& m) const;
		Matrix& operator *= (const Matrix& m);
	} m;

	bool m_isWorldToLocal;

public:
	explicit XForm() {}
	XForm(const Ray& r, const Vector& s, bool isWorldToLocal = true);

	void Initalize();
	void Initalize(const Ray& r, const Vector& s, bool isWorldToLocal = true);

	void operator *= (const Vector& s); // scale
	void operator += (const Vector& t); // translate
	void operator <<= (const Vector& r); // rotate

	void operator /= (const Vector& s); // scale
	void operator -= (const Vector& t); // translate
	void operator >>= (const Vector& r); // rotate

//	transformations
	[[nodiscard]] Vector operator < (const Vector& n) const; // normal
	Vector operator << (const Vector& v) const; // vector
	Ray operator << (const Ray& r) const; // ray
};
