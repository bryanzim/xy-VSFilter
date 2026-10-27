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

	[[nodiscard]] Vector Normal(Vector& a, Vector& b);
	[[nodiscard]] float Angle(Vector& a, Vector& b);
	[[nodiscard]] float Angle(Vector& a);
	void Angle(float& u, float& v); // returns spherical coords in radian, -PI/2 <= u <= PI/2, -PI <= v <= PI
	Vector Angle(); // does like prev., returns 'u' in 'ret.x', and 'v' in 'ret.y'

	Vector Unit();
	[[nodiscard]] Vector& Unitalize();
	[[nodiscard]] float Length();
	[[nodiscard]] float Sum(); // x + y + z
	[[nodiscard]] float CrossSum(); // xy + xz + yz
	Vector Cross(); // xy, xz, yz
	[[nodiscard]] Vector Pow(float exp);

	[[nodiscard]] Vector& Min(Vector& a);
	[[nodiscard]] Vector& Max(Vector& a);
	Vector Abs();

	[[nodiscard]] Vector Reflect(Vector& n);
	[[nodiscard]] Vector Refract(Vector& n, float nFront, float nBack, float* nOut = NULL);
	[[nodiscard]] Vector Refract2(Vector& n, float nFrom, float nTo, float* nOut = NULL);

	[[nodiscard]] Vector operator - ();
	float& operator [] (int i);

	[[nodiscard]] float operator | (Vector& v); // dot
	[[nodiscard]] Vector operator % (Vector& v); // cross

	[[nodiscard]] bool operator == (const Vector& v) const;
	[[nodiscard]] bool operator != (const Vector& v) const;

	[[nodiscard]] Vector operator + (float d);
	[[nodiscard]] Vector operator + (Vector& v);
	[[nodiscard]] Vector operator - (float d);
	[[nodiscard]] Vector operator - (Vector& v);
	[[nodiscard]] Vector operator * (float d);
	[[nodiscard]] Vector operator * (Vector& v);
	[[nodiscard]] Vector operator / (float d);
	[[nodiscard]] Vector operator / (Vector& v);
	Vector& operator += (float d);
	Vector& operator += (Vector& v);
	Vector& operator -= (float d);
	Vector& operator -= (Vector& v);
	Vector& operator *= (float d);
	Vector& operator *= (Vector& v);
	Vector& operator /= (float d);
	Vector& operator /= (Vector& v);
};

class Ray
{
public:
	Vector p, d;

	explicit Ray() {}
	Ray(Vector& p, Vector& d);
	void Set(Vector& p, Vector& d);

	[[nodiscard]] float GetDistanceFrom(Ray& r); // r = plane
	[[nodiscard]] float GetDistanceFrom(Vector& v); // v = point

	Vector operator [] (float t);
};

class XForm
{
	class Matrix
	{
	public:
		float mat[4U][4U];

		explicit Matrix();
		void Initalize();

		[[nodiscard]] Matrix operator * (Matrix& m);
		Matrix& operator *= (Matrix& m);
	} m;

	bool m_isWorldToLocal;

public:
	explicit XForm() {}
	XForm(Ray& r, Vector& s, bool isWorldToLocal = true);

	void Initalize();
	void Initalize(Ray& r, Vector& s, bool isWorldToLocal = true);

	void operator *= (Vector& s); // scale
	void operator += (Vector& t); // translate
	void operator <<= (Vector& r); // rotate

	void operator /= (Vector& s); // scale
	void operator -= (Vector& t); // translate
	void operator >>= (Vector& r); // rotate

//	transformations
	[[nodiscard]] Vector operator < (Vector& n); // normal
	Vector operator << (Vector& v); // vector
	Ray operator << (Ray& r); // ray
};
