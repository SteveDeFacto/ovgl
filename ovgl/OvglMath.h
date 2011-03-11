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
* @brief None.
*/

namespace Ovgl
{
	#define OvglPi (3.14159265358979323846)
	
	extern "C"
	{
		class Matrix33;
		class Matrix44;
		class Vector2;
		class Vector3;
		class Vector4;

		class __declspec(dllexport) Vector2
		{
		public:

			float x, y;
			float& operator[](size_t index);
		};

		class __declspec(dllexport) Vector3
		{
		public:
			float x, y, z;
			float& operator[](size_t index);
			Vector3 operator - ( const Vector3& ) const;
			Vector3 operator + ( const Vector3& ) const;
			Vector3 operator / ( const Vector3& ) const;
			Vector3 operator * ( const Vector3& ) const;
			Vector3 operator / ( const float& ) const;
			Vector3 operator * ( const float& ) const;
			void toDoubles( double* data );
			void fromDoubles( double* data );
		};

		class __declspec(dllexport) Vector4
		{
		public:
			float x, y, z, w;
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
	
		class __declspec(dllexport) Matrix33
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

		class __declspec(dllexport) Matrix44
		{
		public:
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
			Vector4& operator[](size_t index);
			Matrix44 operator * ( const Matrix44& ) const;
			Matrix33 to3x3();
			void toDoubles( double* data );
			void fromDoubles( double* data );
		};
	
		__declspec(dllexport) Matrix44 MatrixIdentity();
		__declspec(dllexport) Matrix44 MatrixInverse( Vector4* in_vec, Matrix44* in_mat );
		__declspec(dllexport) Matrix44 MatrixScaling( float x, float y, float z );
		__declspec(dllexport) Matrix44 MatrixTranslation( float x, float y, float z );
		__declspec(dllexport) Matrix44 MatrixRotationX( float angle );
		__declspec(dllexport) Matrix44 MatrixRotationY( float angle );
		__declspec(dllexport) Matrix44 MatrixRotationZ( float angle );
		__declspec(dllexport) Matrix44 MatrixRotationEuler( float yaw, float pitch, float roll );
		__declspec(dllexport) Matrix44 MatrixRotationAxis( Vector3* axis, float angle );
		__declspec(dllexport) Matrix44 MatrixRotationQuaternion(Vector4* q);
		__declspec(dllexport) Matrix44 MatrixPerspectiveLH( float ViewWidth, float ViewHeight, float NearZ, float FarZ );
		__declspec(dllexport) Vector4 QuaternionRotationMatrix( Matrix44* matrix );
		__declspec(dllexport) Vector4 QuaternionRotationAxis( Vector3* axis, float angle );
		__declspec(dllexport) Vector4 QuaternionRotationEuler( float yaw, float pitch, float roll );
		__declspec(dllexport) Vector4 Vector4Set( float w, float x, float y, float z );
		__declspec(dllexport) Vector4 Vector4Lerp( Vector4& vec1, Vector4& vec2, float u);
		__declspec(dllexport) Vector3 Vector3Set( float x, float y, float z );
		__declspec(dllexport) Vector3 Vector3Cross( Vector3* vec1, Vector3* vec2 );
		__declspec(dllexport) Vector3 Vector3Transform( Vector3* vector, Matrix44* matrix );
		__declspec(dllexport) Vector3 Vector3Normalize( Vector3* vector );
		__declspec(dllexport) Vector3 Vector3Center( std::vector<Vector3>& vertices );
		__declspec(dllexport) Vector2 Vector2Set( float x, float y );
		__declspec(dllexport) float Distance( Vector3* vector1, Vector3* vector2);
		__declspec(dllexport) float Vector3Dot( Vector3* vec1, Vector3* vec2 );
		__declspec(dllexport) float Lerp( float vec1, float vec2, float u);
		__declspec(dllexport) float DegToRad( float degree);
	};
};