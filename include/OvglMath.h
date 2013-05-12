/**
* @file OvglMath.h
* Copyright 2011 Steven Batchelor
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* @brief This header defines all math functions and classes used in Ovgl.
*/

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

namespace Ovgl
{
	// Define pi
	#define OvglPi	3.1415926535897932384626433832795

	// Pre-define classes.
	class Matrix33;
	class Matrix44;
	class Vector2;
	class Vector3;
	class Vector4;

	// A two dimensional vector using floating points.
    class DLLEXPORT Vector2
	{
	public:
		float x, y;
		Vector2();
		Vector2( float new_x, float new_y );
		Vector2 operator - ( const Vector2& ) const;
		Vector2 operator / ( const float& ) const;
		bool operator == (const Vector2&) const;
		bool operator != (const Vector2&) const;
		float& operator[](size_t index);
	};

	// A three dimensional vector using floating points.
    class DLLEXPORT Vector3
	{
	public:
		float x, y, z;
		Vector3();
		Vector3( float new_x, float new_y, float new_z );
		float& operator[](size_t index);
		Vector3 operator - ( const Vector3& ) const;
		Vector3 operator + ( const Vector3& ) const;
		Vector3 operator / ( const Vector3& ) const;
		Vector3 operator * ( const Vector3& ) const;
		Vector3 operator / ( const float& ) const;
		Vector3 operator * ( const float& ) const;
		bool operator == (const Vector3&) const;
		bool operator != (const Vector3&) const;
		void toDoubles( double* data );
		void fromDoubles( double* data );
	};

	// A four dimensional vector using floating points.
    class DLLEXPORT Vector4
	{
	public:
		float x, y, z, w;
		Vector4();
		Vector4( float new_x, float new_y, float new_z, float new_w );
		float& operator[](size_t index);
		Vector4 operator - ( const Vector4& ) const;
		Vector4 operator + ( const Vector4& ) const;
		Vector4 operator / ( const Vector4& ) const;
		Vector4 operator * ( const Vector4& ) const;
		Vector4 operator / ( const float& ) const;
		Vector4 operator * ( const float& ) const;
		void toDoubles( double* data );
		void fromDoubles( double* data );
	};
	
	// A three by three matrix using floating points.
    class DLLEXPORT Matrix33
	{
	public:
		float _11, _12, _13;
		float _21, _22, _23;
		float _31, _32, _33;
		Vector3& operator[](size_t index);
		Matrix33 operator * ( const Matrix33& ) const;
		Matrix44 to4x4();
		void toDoubles( double* data );
		void fromDoubles( double* data );
	};

	// A four by four matrix using floating points.
    class DLLEXPORT Matrix44
	{
	public:
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;
		Vector4& operator[](size_t index);
		Matrix44 operator * ( const Matrix44& ) const;
		Matrix33 to3x3();
		Matrix44 Rotation();
		Matrix44 Translation();
		void toDoubles( double* data );
		void fromDoubles( double* data );
	};
	
	// Creates a default 4x4 identity matrix.
    DLLEXPORT Matrix44 MatrixIdentity();

	// Swaps the Y and Z axis of a matrix.
    DLLEXPORT Matrix44 MatrixSwapYZ( const Matrix44& in_mat );

	// Swaps the X and Z axis of a matrix.
    DLLEXPORT Matrix44 MatrixSwapXZ( const Matrix44& in_mat );

	// Inverses a matrix.
    DLLEXPORT Matrix44 MatrixInverse( const Vector4& in_vec, const Matrix44& in_mat );

	// Creates a 4x4 matrix scaled to the given parameters.
    DLLEXPORT Matrix44 MatrixScaling( float x, float y, float z );

	// Creates a 4x4 matrix with a position based on the given parameters.
    DLLEXPORT Matrix44 MatrixTranslation( float x, float y, float z );

	// Transposes a 4x4 matrix which means it exchanges the rows with the columns.
    DLLEXPORT Matrix44 MatrixTranspose( const Matrix44& in_mat );

	// Gives a 4x4 matrix rotated across the X axis. 
    DLLEXPORT Matrix44 MatrixRotationX( float angle );

	// Gives a 4x4 matrix rotated across the Y axis. 
    DLLEXPORT Matrix44 MatrixRotationY( float angle );

	// Gives a 4x4 matrix rotated across the Z axis. 
    DLLEXPORT Matrix44 MatrixRotationZ( float angle );

	// Create a 4x4 matrix with a rotation of the given euler angle.
    DLLEXPORT Matrix44 MatrixRotationEuler( float yaw, float pitch, float roll );

	// Create a 4x4 matrix with a rotation of the given axis and angle.
    DLLEXPORT Matrix44 MatrixRotationAxis( const Vector3& axis, float angle );

	// Create a 4x4 matrix with a rotation of the given quaternion.
    DLLEXPORT Matrix44 MatrixRotationQuaternion( const Vector4& q);

	// Create a 4x4 left handed perspective matrix.
    DLLEXPORT Matrix44 MatrixPerspectiveLH( float fov, float aspect, float zn, float zf);

	// Creates a quaternion based on the rotation of the given matrix.
    DLLEXPORT Vector4 QuaternionRotationMatrix( const Matrix44& matrix );

	// Creates a quaternion based on the rotation of the given axis and angle.
    DLLEXPORT Vector4 QuaternionRotationAxis( const Vector3& axis, float angle );

	// Creates a quaternion based on the rotation of the given euler angle.
    DLLEXPORT Vector4 QuaternionRotationEuler( float yaw, float pitch, float roll );

	// Creates a euler angle based on the rotation of the given matrix.
    DLLEXPORT Vector3 EulerRotationMatrix( const Matrix44& matrix );

	// Linear interpolation between the two four dimensional vectors.
    DLLEXPORT Vector4 Vector4Lerp( const Vector4& vec1, const Vector4& vec2, float u);

	// Spherical linear interpolation between the two vectors.
    DLLEXPORT Vector4 Slerp( const Vector4& q1, const Vector4& q2, float t );

	// The cross product of two three dimensional vectors. 
    DLLEXPORT Vector3 Vector3Cross( const Vector3& vec1, const Vector3& vec2 );

	// Transforms the three dimensional vector based on the given matrix.
    DLLEXPORT Vector3 Vector3Transform( const Vector3& vector, const Matrix44& matrix );

	// Normalizes the three dimensional vector.
    DLLEXPORT Vector3 Vector3Normalize( const Vector3& vector );

	// Finds the center point of a list of vertices.
    DLLEXPORT Vector3 Vector3Center( std::vector< Vector3 >& vertices );

	// Gets the volume of a tetrahedron.
	
	/**
	* Gets the volume of a tetrahedron.
	* @param vector1 The first corner of the tetrahedron.
	* @param vector2 The second corner of the tetrahedron.
	* @param vector2 The third corner of the tetrahedron.
	* @param vector3 The forth corner of the tetrahedron.
	*/
    DLLEXPORT float VolumeTetrahedron( const Ovgl::Vector3& vector1, const Ovgl::Vector3& vector2, const Ovgl::Vector3& vector3, const Ovgl::Vector3& vector4 );

	/**
	* Rounds the float to the given number of decimal places.
	* @param expression The float to round.
	* @param numdecimalplaces The number of decimal places.
	*/
    DLLEXPORT float Round( float expression, int32_t numdecimalplaces );

	/**
	* Finds the distance between two three dimensional vectors.
	* @param vec1 The first vector.
	* @param vec2 The second vector.
	*/
    DLLEXPORT float Distance( const Vector3& vector1, const Vector3& vector2);

	/**
	* Gets the dot product of two three dimensional vectors.
	* @param vec1 The first vector.
	* @param vec2 The second vector.
	*/
    DLLEXPORT float Vector3Dot( const Vector3& vec1, const Vector3& vec2 );

	/**
	* Linear interpolation between two floats.
	* @param val1 The starting value.
	* @param val2 The ending value.
	* @param u The level of interpolation from 0.0 to 1.0.
	*/
    DLLEXPORT float Lerp( float val1, float val2, float u);

	/**
	* Converts degrees to radians.
	* @param degree A float containing the number of degrees to convert.
	*/
    DLLEXPORT float DegToRad( float degree);

	/**
	* Creates a bounding box based on a list of three dimensional vectors.
	* @param vectors The list of vectors.
	* @param out_min Pointer to a three dimensional vector to return the minimum bounds of the box.
	* @param out_min Pointer to a three dimensional vector to return the maximum bounds of the box.
	*/
    DLLEXPORT void Vector3Box( std::vector< Vector3 >& vectors, Vector3& out_min, Vector3& out_max );
};
