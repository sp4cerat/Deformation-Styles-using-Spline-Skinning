#ifndef _VecMath_class
#define _VecMath_class

#include <sstream>
#include <math.h>
#include "mathlib/Vector.h"
#include "mathlib/Matrix.h"

struct vec3f
{
    float x, y, z;
 
    inline vec3f( void ) {}

    inline vec3f operator =( vector3 a )
	 { vec3f b ; b.x = a.x; b.y = a.y; b.z = a.z; return b;}

    inline vec3f( vector3 a )
	 { x = a.x; y = a.y; z = a.z; }

    inline vec3f( const float X, const float Y, const float Z )
    { x = X; y = Y; z = Z; }

    inline vec3f operator + ( const vec3f& a ) const
    { return vec3f( x + a.x, y + a.y, z + a.z ); }

	inline vec3f operator += ( const vec3f& a ) const
    { return vec3f( x + a.x, y + a.y, z + a.z ); }

    inline vec3f operator * ( const float a ) const
    { return vec3f( x * a, y * a, z * a ); }

    inline vec3f operator * ( const vec3f a ) const
    { return vec3f( x * a.x, y * a.y, z * a.z ); }

    inline vec3f operator / ( const vec3f a ) const
    { return vec3f( x / a.x, y / a.y, z / a.z ); }

    inline vec3f operator - ( const vec3f& a ) const
    { return vec3f( x - a.x, y - a.y, z - a.z ); }

    inline vec3f operator / ( const float a ) const
    { return vec3f( x / a, y / a, z / a ); }

    inline float dot( const vec3f& a ) const
    { return a.x*x + a.y*y + a.z*z; }

    inline vec3f cross( const vec3f& a , const vec3f& b ) 
    { 
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z;
		z = a.x * b.y - a.y * b.x;
		return *this;
	}

    inline float angle( const vec3f& v ) 
    { 
		vec3f a = v , b = *this;
		float dot = v.x*x + v.y*y + v.z*z;
		float len = a.length() * b.length();
		if(len==0)len=1;

		return (float) acos ( dot  / len );
	}

    inline float angle2( const vec3f& v , const vec3f& w ) 
    { 
		vec3f a = v , b= *this;
		float dot = a.x*b.x + a.y*b.y + a.z*b.z;
		float len = a.length() * b.length();
		if(len==0)len=1;

		vec3f plane; plane.cross( b,w ); 

		if ( plane.x * a.x + plane.y * a.y + plane.z * a.z > 0 )
			return (float) -acos ( dot  / len );

		return (float) acos ( dot  / len );
	}

    inline vec3f rot_x( float a ) 
    { 
		float yy = cos ( a ) * y + sin ( a ) * z;
		float zz = cos ( a ) * z - sin ( a ) * y;
		y = yy; z = zz;
		return *this;
	}
    inline vec3f rot_y( float a ) 
    { 
		float xx = cos ( -a ) * x + sin ( -a ) * z;
		float zz = cos ( -a ) * z - sin ( -a ) * x;
		x = xx; z = zz;
		return *this;
	}
    inline void clamp( float min, float max ) 
    {
		if (x<min) x=min;
		if (y<min) y=min;
		if (z<min) z=min;
		if (x>max) x=max;
		if (y>max) y=max;
		if (z>max) z=max;
	}
    inline vec3f rot_z( float a ) 
    { 
		float yy = cos ( a ) * y + sin ( a ) * x;
		float xx = cos ( a ) * x - sin ( a ) * y;
		y = yy; x = xx;
		return *this;
	}
    inline vec3f invert() 
	{
		x=-x;y=-y;z=-z;return *this;
	}

    inline float length() const
    { return (float)sqrt(x*x + y*y + z*z); }

    inline vec3f normalize( float desired_length = 1 )
    { 
		float square = x*x + y*y + z*z;
		if (square <= 0.00001f ) 
		{
			x=1;y=0;z=0;
			return *this; 
		}
		float len = desired_length / (float)sqrt(square); 
		x*=len;y*=len;z*=len;
		return *this; 
	}
    static vec3f normalize( vec3f a );

	static void random_init();
	static float random_float();
	static vec3f random();

	static int random_number;
};


class Matrix
{
public:

    float m[16];

	void identity()
	{
		static float identity[16]={
				1,0,0,0,			
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			};
		memcpy( m,identity,16 );
	}
 
    Matrix( vec3f a,vec3f b,vec3f c,vec3f d = vec3f(0,0,0)) { 

		identity() ;

		m[ 0] = a.x;m[ 1] = b.x;m[ 2] = c.x;m[ 3] = d.x;
		m[ 4] = a.y;m[ 5] = b.y;m[ 6] = c.y;m[ 7] = d.y;
		m[ 8] = a.z;m[ 9] = b.z;m[10] = c.z;m[11] = d.z;
		m[15]=1;
	}

    Matrix( void ) { 
	
		identity() ;
	}

	inline vec3f operator * ( const vec3f& a ) const
	{ 
		vec3f out;
		out.x = a.x*m[ 0] + a.y*m[ 4] + a.z*m[ 8] + m[12];
		out.y = a.x*m[ 1] + a.y*m[ 5] + a.z*m[ 9] + m[13];
		out.z = a.x*m[ 2] + a.y*m[ 6] + a.z*m[10] + m[14];
//		out.x = a.x*m[ 0] + a.y*m[ 1] + a.z*m[ 2] + m[ 3];
//		out.y = a.x*m[ 4] + a.y*m[ 5] + a.z*m[ 6] + m[ 7];
//		out.z = a.x*m[ 8] + a.y*m[ 9] + a.z*m[10] + m[11];
//		out.x = a.x*m[ 0] + a.x*m[ 1] + a.x*m[ 2] + m[ 3];
//		out.y = a.y*m[ 4] + a.y*m[ 5] + a.y*m[ 6] + m[ 7];
//		out.z = a.z*m[ 8] + a.z*m[ 9] + a.z*m[10] + m[11];
		return out;
	}

	Matrix operator * ( const Matrix& a ) const
    { 
		int i,j,k; 
		Matrix result;

		for (i=0;i<4;i++)
			for (j=0;j<4;j++)
			{
				result.m[i+j*4]=0;
					
				for (k=0;k<4;k++)
					result.m[i+j*4] += a.m[i+k*4] *	m[j*4+k] ;
			}
		return result;
	}
};

#endif