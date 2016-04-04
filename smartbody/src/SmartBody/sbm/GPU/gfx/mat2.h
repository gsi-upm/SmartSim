#ifndef GFXMAT2_INCLUDED // -*- C++ -*-
#define GFXMAT2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2x2 Matrix class
  
  $Id: mat2.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "vec2.h"

namespace gfx
{

class Mat2
{
private:
    Vec2 row[2];

public:
    // Standard constructors
    //
    Mat2() { *this = 0.0; }
    Mat2(double a, double b, double c, double d)
	{ row[0][0]=a; row[0][1]=b; row[1][0]=c; row[1][1]=d; }
    Mat2(const Vec2 &r0,const Vec2 &r1) { row[0]=r0; row[1]=r1; }
    Mat2(const Mat2 &m) { *this = m; }

    // Descriptive interface
    //
    typedef double value_type;
    typedef Vec2 vector_type;
    typedef Mat2 inverse_type;
    inline static int dim() { return 2; }

    // Access methods        note: A(i, j) == row i, col j
    //
    double& operator()(int i, int j)       { return row[i][j]; }
    double  operator()(int i, int j) const { return row[i][j]; }
    Vec2&       operator[](int i)       { return row[i]; }
    const Vec2& operator[](int i) const { return row[i]; }
    inline Vec2 col(int i) const {return Vec2(row[0][i],row[1][i]);}

    operator       double*()       { return row[0]; }
    operator const double*()       { return row[0]; }
    operator const double*() const { return row[0]; }


    // Assignment methods
    //
    inline Mat2& operator=(const Mat2& m);
    inline Mat2& operator=(double s);

    inline Mat2& operator+=(const Mat2& m);
    inline Mat2& operator-=(const Mat2& m);
    inline Mat2& operator*=(double s);
    inline Mat2& operator/=(double s);


    // Construction of standard matrices
    //
    inline static Mat2 I();
    inline static Mat2 outer_product(const Vec2 &u, const Vec2 &v)
    	{ return Mat2(u[0]*v[0], u[0]*v[1],   u[1]*v[0], u[1]*v[1]); }
    inline static Mat2 outer_product(const Vec2 &u) { return outer_product(u,u); }

    Mat2 &diag(double d);
    Mat2 &ident() { return diag(1.0); }
};

////////////////////////////////////////////////////////////////////////
//
// Method definitions
//

inline Mat2& Mat2::operator=(const Mat2& m)
	{ row[0]=m[0];  row[1]=m[1];  return *this; }

inline Mat2& Mat2::operator=(double s)
	{ row[0]=s;  row[1]=s;  return *this; }

inline Mat2& Mat2::operator+=(const Mat2& m)
	{ row[0] += m.row[0]; row[1] += m.row[1];  return *this;}

inline Mat2& Mat2::operator-=(const Mat2& m)
	{ row[0] -= m.row[0]; row[1] -= m.row[1];  return *this; }

inline Mat2& Mat2::operator*=(double s)
	{ row[0] *= s; row[1] *= s;  return *this; }

inline Mat2& Mat2::operator/=(double s)
	{ row[0] /= s; row[1] /= s;  return *this; }

////////////////////////////////////////////////////////////////////////
//
// Operator definitions
//

inline Mat2 operator+(const Mat2 &n, const Mat2 &m)
	{ return Mat2(n[0]+m[0], n[1]+m[1]); }

inline Mat2 operator-(const Mat2 &n, const Mat2 &m)
	{ return Mat2(n[0]-m[0], n[1]-m[1]); }

inline Mat2 operator-(const Mat2 &m)
	{ return Mat2(-m[0], -m[1]); }

inline Mat2 operator*(double s, const Mat2 &m)
	{ return Mat2(m[0]*s, m[1]*s); }
inline Mat2 operator*(const Mat2 &m, double s)
	{ return s*m; }

inline Mat2 operator/(const Mat2 &m, double s)
	{ return Mat2(m[0]/s, m[1]/s); }

inline Vec2 operator*(const Mat2 &m, const Vec2 &v)
	{ return Vec2(m[0]*v, m[1]*v); }

inline Mat2 operator*(const Mat2 &n, const Mat2 &m);

inline std::ostream &operator<<(std::ostream &out, const Mat2& M)
	{ return out << M[0] << std::endl  << M[1]; }

inline std::istream &operator>>(std::istream &in, Mat2& M)
	{ return in >> M[0] >> M[1]; }

////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline double det(const Mat2 &m)
	{ return m(0,0)*m(1,1) - m(0,1)*m(1,0); }

inline double trace(const Mat2 &m)
	{ return m(0,0) + m(1,1); }

inline Mat2 transpose(const Mat2 &m)
	{ return Mat2(m.col(0), m.col(1)); }

inline Mat2 adjoint(const Mat2 &m)
	{ return Mat2(perp(m[1]), -perp(m[0])); }

inline double invert(Mat2 &m_inv, const Mat2 &m);

inline bool eigenvalues(const Mat2&, Vec2& evals);
inline bool eigenvectors(const Mat2&, const Vec2& evals, Vec2 evecs[2]);
inline bool eigen(const Mat2&, Vec2& evals, Vec2 evecs[2]);

Mat2 Mat2::I() { return Mat2(1,0,  0,1); }

Mat2 &Mat2::diag(double d)
{
	row[0][0] = d;   row[0][1] = 0;
	row[1][0] = 0;   row[1][1] = d;

	return *this;
}

Mat2 operator*(const Mat2 &n, const Mat2& m)
{
	Mat2 A;
	int i,j;

	for(i=0;i<2;i++)
		for(j=0;j<2;j++)
			A(i,j) = n[i]*m.col(j);

	return A;
}

double invert(Mat2 &inv, const Mat2 &m)
{
	double d = det(m);

	if( d==0.0 )
		return 0.0;

	inv(0, 0) =  m(1,1)/d;
	inv(0, 1) = -m(0,1)/d;
	inv(1, 0) = -m(1,0)/d;
	inv(1, 1) =  m(0,0)/d;

	return d;
}

bool eigenvalues(const Mat2& M, Vec2& evals)
{
	double B = -M(0,0)-M(1,1);
	double C = det(M);

	double dis = B*B - 4.0*C;
	if( dis<FEQ_EPS )
		return false;
	else
	{
		double s = sqrt(dis);

		evals[0] = 0.5*(-B + s);
		evals[1] = 0.5*(-B - s);
		return true;
	}
}

bool eigenvectors(const Mat2& M, const Vec2& evals, Vec2 evecs[2])
{
	evecs[0] = Vec2(-M(0,1), M(0,0)-evals[0]);
	evecs[1] = Vec2(-M(0,1), M(0,0)-evals[1]);

	unitize(evecs[0]);
	unitize(evecs[1]);

	return true;
}

bool eigen(const Mat2& M, Vec2& evals, Vec2 evecs[2])
{
	bool result = eigenvalues(M, evals);
	if( result )
		eigenvectors(M, evals, evecs);
	return result;
}

} // namespace gfx

// GFXMAT2_INCLUDED
#endif
