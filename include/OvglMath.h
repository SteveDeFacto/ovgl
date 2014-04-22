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
		Vector2( float newX, float newY );
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
		Vector3( float newX, float newY, float newZ );
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
		Vector4( float newX, float newY, float newZ, float newW );
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
		Matrix44 rotation();
		Matrix44 translation();
		void toDoubles( double* data );
		void fromDoubles( double* data );
};

/**
 * Creates a default 4x4 identity matrix.
 */
DLLEXPORT Matrix44 matrixIdentity();

/**
 * Swaps the Y and Z axis of a matrix.
 * @param inMat
 */
DLLEXPORT Matrix44 matrixSwapYZ( const Matrix44& inMat );

/**
 * Swaps the X and Z axis of a matrix.
 * @param inMat
 */
DLLEXPORT Matrix44 matrixSwapXZ( const Matrix44& inMat );

/**
 * Inverses a matrix.
 * @param inVec
 * @param inMat
 */
DLLEXPORT Matrix44 matrixInverse( const Vector4& inVec, const Matrix44& inMat );

/**
 * Creates a 4x4 matrix scaled to the given parameters.
 * @param x Scale of X axis.
 * @param y Scale of Y axis.
 * @param z Scale of Z axis.
 */
DLLEXPORT Matrix44 matrixScaling( float x, float y, float z );

/**
 * Creates a 4x4 matrix with a position based on the given parameters.
 * @param x Translation of X axis.
 * @param y Translation of Y axis.
 * @param z Translation of Z axis.
 */
DLLEXPORT Matrix44 matrixTranslation( float x, float y, float z );

/**
 * Transposes a 4x4 matrix which means it exchanges the rows with the columns.
 * @param inMax Matrix to transpose.
 */
DLLEXPORT Matrix44 matrixTranspose( const Matrix44& inMat );

/**
 * Gives a 4x4 matrix rotated across the X axis. 
 * @param angle Angle of rotation along the X axis.
 */
DLLEXPORT Matrix44 matrixRotationX( float angle );

/**
 * Gives a 4x4 matrix rotated across the Y axis. 
 * @param angle Angle of rotation along the Y axis.
 */
DLLEXPORT Matrix44 matrixRotationY( float angle );

/**
 * Gives a 4x4 matrix rotated across the Z axis. 
 * @param angle Angle of rotation along the Z axis.
 */
DLLEXPORT Matrix44 matrixRotationZ( float angle );

/**
 * Create a 4x4 matrix with a rotation of the given euler angle.
 * @param yaw Euler yaw of the matrix.
 * @param pitch Euler pitch of the matrix.
 * @param roll Euler roll of the matrix.
 */
DLLEXPORT Matrix44 matrixRotationEuler( float yaw, float pitch, float roll );

/**
 * Create a 4x4 matrix with a rotation of the given axis and angle.
 * @param axis Axis to rotate the matrix.
 * @param angle Angle of the rotation.
 */
DLLEXPORT Matrix44 matrixRotationAxis( const Vector3& axis, float angle );

/**
 * Create a 4x4 matrix with a rotation of the given quaternion.
 * @param q Quaternion to convert to a matrix.
 */
DLLEXPORT Matrix44 matrixRotationQuaternion( const Vector4& q);

/**
 * Create a 4x4 left handed perspective matrix.
 * @param fov Field of view.
 * @param aspect Aspect ratio.
 * @param zn Nearest an object can be viewed.
 * @param zf Farthest an object can be viewed.
 */
DLLEXPORT Matrix44 matrixPerspectiveLH( float fov, float aspect, float zn, float zf);

/**
 * Creates a quaternion based on the rotation of the given matrix.
 * @param matrix Matrix to get quaternion from.
 */
DLLEXPORT Vector4 quaternionRotationMatrix( const Matrix44& matrix );

/**
 * Creates a quaternion based on the rotation of the given axis and angle.
 * @param axis Axis to rotate the quaternion.
 * @param angle Angle of the rotation.
 */
DLLEXPORT Vector4 quaternionRotationAxis( const Vector3& axis, float angle );

/**
 * Creates a quaternion based on the rotation of the given euler angle.
 * @param yaw Euler yaw of the quaternion.
 * @param pitch Euler pitch of the quaternion.
 * @param roll Euler roll of the quaternion.
 */
DLLEXPORT Vector4 quaternionRotationEuler( float yaw, float pitch, float roll );

/**
 * Creates a euler angle based on the rotation of the given matrix.
 * @param matrix Matrix to convert to euler.
 */
DLLEXPORT Vector3 eulerRotationMatrix( const Matrix44& matrix );

/**
 * Linear interpolation between the two four dimensional vectors.
 * @param vec1 Starting vector.
 * @param vec2 Ending vector.
 * @param u How much to interpolate it form 0 - 1.
 */
DLLEXPORT Vector4 vector4Lerp( const Vector4& vec1, const Vector4& vec2, float u);

/**
 * Spherical linear interpolation between the two vectors.
 * @param q1 Starting quaternion.
 * @param q2 Ending quaternion.
 * @param t How much to interpolate it form 0 - 1.
 */
DLLEXPORT Vector4 slerp( const Vector4& q1, const Vector4& q2, float t );

/**
 * The cross product of two three dimensional vectors. 
 * @param vec1 First vector.
 * @param vec2 Second vector.
 */
DLLEXPORT Vector3 vector3Cross( const Vector3& vec1, const Vector3& vec2 );

/**
 * Transforms the three dimensional vector based on the given matrix.
 * @param vector Vector to transform.
 * @param matrix Matrix to apply to vector.
 */
DLLEXPORT Vector3 vector3Transform( const Vector3& vector, const Matrix44& matrix );

/**
 * Normalizes the three dimensional vector.
 * @param vector Vector to normalize.
 */
DLLEXPORT Vector3 vector3Normalize( const Vector3& vector );

/**
 * Finds the center point of a list of vertices.
 * @param vertices List of vertices.
 */
DLLEXPORT Vector3 vector3Center( std::vector< Vector3 >& vertices );

/**
 * Gets the volume of a tetrahedron.
 * @param vector1 The first corner of the tetrahedron.
 * @param vector2 The second corner of the tetrahedron.
 * @param vector2 The third corner of the tetrahedron.
 * @param vector3 The forth corner of the tetrahedron.
 */
DLLEXPORT float volumeTetrahedron( const Ovgl::Vector3& vector1, const Ovgl::Vector3& vector2, const Ovgl::Vector3& vector3, const Ovgl::Vector3& vector4 );

/**
 * Rounds the float to the given number of decimal places.
 * @param expression The float to round.
 * @param numdecimalplaces The number of decimal places.
 */
DLLEXPORT float bound( float expression, int32_t decimalPlaces );

/**
 * Finds the distance between two three dimensional vectors.
 * @param vec1 The first vector.
 * @param vec2 The second vector.
 */
DLLEXPORT float distance( const Vector3& vector1, const Vector3& vector2);

/**
 * Gets the dot product of two three dimensional vectors.
 * @param vec1 The first vector.
 * @param vec2 The second vector.
 */
DLLEXPORT float vector3Dot( const Vector3& vec1, const Vector3& vec2 );

/**
 * Linear interpolation between two floats.
 * @param val1 The starting value.
 * @param val2 The ending value.
 * @param u The level of interpolation from 0.0 to 1.0.
 */
DLLEXPORT float lerp( float val1, float val2, float u);

/**
 * Converts degrees to radians.
 * @param degree A float containing the number of degrees to convert.
 */
DLLEXPORT float degToRad( float degree);

/**
 * Creates a bounding box based on a list of three dimensional vectors.
 * @param vectors The list of vectors.
 * @param out_min Pointer to a three dimensional vector to return the minimum bounds of the box.
 * @param out_min Pointer to a three dimensional vector to return the maximum bounds of the box.
 */
DLLEXPORT void vector3Box( std::vector< Vector3 >& vectors, Vector3& outMin, Vector3& outMax );
};
