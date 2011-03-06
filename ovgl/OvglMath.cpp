/**
* @file OvglMath.cpp
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

//#include "OvglIncludes.h"
//#include "OvglMath.h"
//
//Ovgl::Matrix44 Ovgl::Matrix44::operator * ( const Ovgl::Matrix44& in ) const
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = _11 * in._11 + _12 * in._21 + _13 * in._31 + _14 * in._41;
//	out._12 = _11 * in._12 + _12 * in._22 + _13 * in._32 + _14 * in._42;
//	out._13 = _11 * in._13 + _12 * in._23 + _13 * in._33 + _14 * in._43;
//	out._14 = _11 * in._14 + _12 * in._24 + _13 * in._34 + _14 * in._44;
//	out._21 = _21 * in._11 + _22 * in._21 + _23 * in._31 + _24 * in._41;
//	out._22 = _21 * in._12 + _22 * in._22 + _23 * in._32 + _24 * in._42;
//	out._23 = _21 * in._13 + _22 * in._23 + _23 * in._33 + _24 * in._43;
//	out._24 = _21 * in._14 + _22 * in._24 + _23 * in._34 + _24 * in._44;
//	out._31 = _31 * in._11 + _32 * in._21 + _33 * in._31 + _34 * in._41;
//	out._32 = _31 * in._12 + _32 * in._22 + _33 * in._32 + _34 * in._42;
//	out._33 = _31 * in._13 + _32 * in._23 + _33 * in._33 + _34 * in._43;
//	out._34 = _31 * in._14 + _32 * in._24 + _33 * in._34 + _34 * in._44;
//	out._41 = _41 * in._11 + _42 * in._21 + _43 * in._31 + _44 * in._41;
//	out._42 = _41 * in._12 + _42 * in._22 + _43 * in._32 + _44 * in._42;
//	out._43 = _41 * in._13 + _42 * in._23 + _43 * in._33 + _44 * in._43;
//	out._44 = _41 * in._14 + _42 * in._24 + _43 * in._34 + _44 * in._44;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::Matrix33::to4x4()
//{
//	Ovgl::Matrix44 out = {0};
//	CopyMemory( &out, this, sizeof(Ovgl::Matrix33) );
//	out._44 = 1;
//	return out;
//}
//
//void Ovgl::Matrix33::toDoubles( double* data )
//{
//	for( int r = 0; r < 3; r++ )
//	{
//		for( int c = 0; c < 3; c++ )
//		{
//			data[(4 * r) + c] = (double)(*this)[r][c];
//		}
//	}
//}
//
//void Ovgl::Matrix33::fromDoubles( double* data )
//{
//	for( int r = 0; r < 3; r++ )
//	{
//		for( int c = 0; c < 3; c++ )
//		{
//			(*this)[r][c] = (float)data[(4 * r) + c];
//		}
//	}
//}
//
//Ovgl::Matrix33 Ovgl::Matrix33::operator * ( const Ovgl::Matrix33& in ) const
//{
//	Ovgl::Matrix33 out = {0};
//	out._11 = _11 * in._11 + _12 * in._21 + _13 * in._31;
//	out._12 = _11 * in._12 + _12 * in._22 + _13 * in._32;
//	out._13 = _11 * in._13 + _12 * in._23 + _13 * in._33;
//	out._21 = _21 * in._11 + _22 * in._21 + _23 * in._31;
//	out._22 = _21 * in._12 + _22 * in._22 + _23 * in._32;
//	out._23 = _21 * in._13 + _22 * in._23 + _23 * in._33;
//	out._31 = _31 * in._11 + _32 * in._21 + _33 * in._31;
//	out._32 = _31 * in._12 + _32 * in._22 + _33 * in._32;
//	out._33 = _31 * in._13 + _32 * in._23 + _33 * in._33;
//	return out;
//}
//
//Ovgl::Matrix33 Ovgl::Matrix44::to3x3()
//{
//	Ovgl::Matrix33 out = {0};
//	CopyMemory( &out, this, sizeof(Ovgl::Matrix33) );
//	return out;
//}
//
//void Ovgl::Matrix44::toDoubles( double* data )
//{
//	for( int r = 0; r < 4; r++ )
//	{
//		for( int c = 0; c < 4; c++ )
//		{
//			data[(4 * r) + c] = (double)(*this)[r][c];
//		}
//	}
//}
//
//void Ovgl::Matrix44::fromDoubles( double* data )
//{
//	for( int r = 0; r < 4; r++ )
//	{
//		for( int c = 0; c < 4; c++ )
//		{
//			(*this)[r][c] = (float)data[(4 * r) + c];
//		}
//	}
//}
//
//Ovgl::Matrix44 Ovgl::MatrixIdentity()
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = 1;
//	out._22 = 1;
//	out._33 = 1;
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixInverse ( Ovgl::Vector4* in_vec, Ovgl::Matrix44* in_mat)
//{
//	Ovgl::Matrix44 out = {0};
//	D3DXMATRIX temp;
//	CopyMemory(&temp, in_mat, sizeof(Ovgl::Matrix44));
//	D3DXMatrixInverse( &temp, NULL, &temp );
//	CopyMemory(&out, &temp, sizeof(Ovgl::Matrix44));
//    return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixScaling( float x, float y, float z )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = x;
//	out._22 = y;
//	out._33 = z;
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixTranslation( float x, float y, float z )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = 1;
//	out._22 = 1;
//	out._33 = 1;
//	out._41 = x;
//	out._42 = y;
//	out._43 = z;
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixRotationX( float angle )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = 1;
//	out._22 = cos( angle );
//	out._23 = -sin( angle );
//	out._32 = sin( angle );
//	out._33 = cos( angle );
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixRotationY( float angle )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = cos( angle );
//	out._13 = sin( angle );
//	out._22 = 1;
//	out._31 = -sin( angle );
//	out._33 = cos( angle );
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixRotationZ( float angle )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = cos( angle );
//	out._12 = -sin( angle );
//	out._21 = sin( angle );
//	out._22 = cos( angle );
//	out._33 = 1;
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixRotationYawPitchRoll( float roll, float pitch, float yaw )
//{
//	Ovgl::Matrix44 out = {0};
//	out._11 = ( cos(roll) * cos(yaw) ) + ( sin(roll) * sin(pitch) * sin(yaw) );
//	out._12 = ( sin(roll) * cos(pitch) );
//	out._13 = ( cos(roll) * -sin(yaw) ) + ( sin(roll) * sin(pitch) * cos(yaw) );
//	out._21 = (-sin(roll) * cos(yaw) ) + ( cos(roll) * sin(pitch) * sin(yaw) );
//	out._22 = ( cos(roll) * cos(pitch) );
//	out._23 = ( sin(roll) * sin(yaw) ) + ( cos(roll) * sin(pitch) * cos(yaw) );
//	out._31 = ( cos(pitch) * sin(yaw) );
//	out._32 = -sin(pitch);
//	out._33 = ( cos(pitch) * cos(yaw) );
//	out._44 = 1;
//	return out;
//}
//
//Ovgl::Matrix44 Ovgl::MatrixRotationAxis( Ovgl::Vector3* axis, float angle )
//{
//	Ovgl::Matrix44 out = {0};
//	float s = sinf(angle);
//	float c = cosf(angle);
//	float t = 1.0F - c;
//	out._11 = t * axis->x * axis->x + c;
//	out._21 = t * axis->x * axis->y + s * axis->z;
//	out._31 = t * axis->x * axis->z - s * axis->y;
//	out._41 = 0.0F;
//	out._12 = t * axis->y * axis->x - s * axis->z;
//	out._22 = t * axis->y * axis->y + c;
//	out._32 = t * axis->y * axis->z + s * axis->x;
//	out._42 = 0.0F;
//	out._13 = t * axis->z * axis->x + s * axis->y;
//	out._23 = t * axis->z * axis->y - s * axis->x;
//	out._33 = t * axis->z * axis->z + c;
//	out._43 = 0.0F;
//	out._14 = 0.0F;
//	out._24 = 0.0F;
//	out._34 = 0.0F;
//	out._44 = 1.0F;
//	return out;
//}
//
//
//Ovgl::Matrix44 Ovgl::MatrixPerspectiveLH( float ViewWidth, float ViewHeight, float NearZ, float FarZ )
//{
//	Ovgl::Matrix44 out = {0};
//	D3DXMATRIX temp;
//	D3DXMatrixPerspectiveFovLH( &temp, ViewWidth, ViewHeight, NearZ, FarZ );
//	CopyMemory(&out, &temp, sizeof(Ovgl::Matrix44));
//	return out;
//}
//
//
//Ovgl::Matrix44 Ovgl::MatrixRotationQuaternion( Ovgl::Vector4* q )
//{
//	Ovgl::Matrix44 out = {0};
//    float sqw = q->w * q->w;
//    float sqx = q->x * q->x;
//    float sqy = q->y * q->y;
//    float sqz = q->z * q->z;
//    float invs = 1 / (sqx + sqy + sqz + sqw);
//    out._11 = ( sqx - sqy - sqz + sqw) * invs;
//    out._22 = (-sqx + sqy - sqz + sqw) * invs;
//    out._33 = (-sqx - sqy + sqz + sqw) * invs;
//    float tmp1 = q->x * q->y;
//    float tmp2 = q->z * q->w;
//    out._21 = 2.0f * (tmp1 + tmp2) * invs;
//    out._12 = 2.0f * (tmp1 - tmp2) * invs;
//    tmp1 = q->x * q->z;
//    tmp2 = q->y * q->w;
//    out._31 = 2.0f * (tmp1 - tmp2) * invs;
//    out._13 = 2.0f * (tmp1 + tmp2) * invs;
//    tmp1 = q->y * q->z;
//    tmp2 = q->x * q->w;
//    out._32 = 2.0f * (tmp1 + tmp2) * invs;
//    out._23 = 2.0f * (tmp1 - tmp2) * invs;
//	out._14 = 0.0f;
//	out._24 = 0.0f;
//	out._34 = 0.0f;
//	out._41 = 0.0f;
//	out._42 = 0.0f;
//	out._43 = 0.0f;
//	out._44 = 1.0f;
//	return out;
//}
//
//float& Ovgl::Vector2::operator [] (size_t index)
//{
//	return (&x)[index];
//}
//
//float& Ovgl::Vector3::operator [] (size_t index)
//{
//	return (&x)[index];
//}
//
//float& Ovgl::Vector4::operator [] (size_t index)
//{
//	return (&x)[index];
//}
//
//Ovgl::Vector3& Ovgl::Matrix33::operator [] (size_t index)
//{
//	return (Ovgl::Vector3&)*((Ovgl::Vector3*)((&_11) + (index*3)));
//}
//
//Ovgl::Vector4& Ovgl::Matrix44::operator [] (size_t index)
//{
//	return (Ovgl::Vector4&)*((Ovgl::Vector4*)((&_11) + (index*4)));
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator - ( const Ovgl::Vector4& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w - in.w;
//	out.x = x - in.x;
//	out.y = y - in.y;
//	out.z = z - in.z;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator + ( const Ovgl::Vector4& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w + in.w;
//	out.x = x + in.x;
//	out.y = y + in.y;
//	out.z = z + in.z;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator / ( const Ovgl::Vector4& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w / in.w;
//	out.x = x / in.x;
//	out.y = y / in.y;
//	out.z = z / in.z;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator * ( const Ovgl::Vector4& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w * in.w;
//	out.x = x * in.x;
//	out.y = y * in.y;
//	out.z = z * in.z;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator / ( const float& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w / in;
//	out.x = x / in;
//	out.y = y / in;
//	out.z = z / in;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4::operator * ( const float& in ) const
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w * in;
//	out.x = x * in;
//	out.y = y * in;
//	out.z = z * in;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4Set( float w, float x, float y, float z )
//{
//	Ovgl::Vector4 out = {0};
//	out.w = w;
//	out.x = x;
//	out.y = y;
//	out.z = z;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::QuaternionRotationMatrix( Ovgl::Matrix44* matrix )
//{
//	Ovgl::Vector4 out = {0};
//	out.w = (float)sqrt(1.0f + matrix->_11 + matrix->_22 + matrix->_33) / 2.0f;
//	float w4 = (4.0f * out.w);
//	out.x = (matrix->_32 - matrix->_23) / w4 ;
//	out.y = (matrix->_13 - matrix->_31) / w4 ;
//	out.z = (matrix->_21 - matrix->_12) / w4 ;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::QuaternionRotationAxis( Ovgl::Vector3* axis, float angle )
//{
//	Ovgl::Vector4 out = {0};
//	out.w  = cosf( angle / 2 );
//	out.x = axis->x * sinf( angle / 2 );
//	out.y = axis->y * sinf( angle / 2 );
//	out.z = axis->z * sinf( angle / 2 );
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator - ( const Ovgl::Vector3& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x - in.x;
//	out.y = y - in.y;
//	out.z = z - in.z;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator + ( const Ovgl::Vector3& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x + in.x;
//	out.y = y + in.y;
//	out.z = z + in.z;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator / ( const Ovgl::Vector3& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x / in.x;
//	out.y = y / in.y;
//	out.z = z / in.z;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator * ( const Ovgl::Vector3& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x * in.x;
//	out.y = y * in.y;
//	out.z = z * in.z;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator / ( const float& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x / in;
//	out.y = y / in;
//	out.z = z / in;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3::operator * ( const float& in ) const
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x * in;
//	out.y = y * in;
//	out.z = z * in;
//	return out;
//}
//
//void Ovgl::Vector3::toDoubles( double* data )
//{
//	data[0] = (double)x;
//	data[1] = (double)y;
//	data[2] = (double)z;
//}
//
//void Ovgl::Vector3::fromDoubles( double* data )
//{
//	x = (float)data[0];
//	y = (float)data[1];
//	z = (float)data[2];
//}
//
//Ovgl::Vector3 Ovgl::Vector3Set( float x, float y, float z )
//{
//	Ovgl::Vector3 out = {0};
//	out.x = x;
//	out.y = y;
//	out.z = z;
//	return out;
//}
//
//float Ovgl::Distance( Ovgl::Vector3* vector1, Ovgl::Vector3* vector2 )
//{
//	Ovgl::Vector3 vector = (*vector1) - (*vector2);
//	float out = sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
//	return out;
//}
//
//float Ovgl::Vector3Dot( Ovgl::Vector3* vec1, Ovgl::Vector3* vec2 )
//{
//	return vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
//}
//
//Ovgl::Vector3 Ovgl::Vector3Cross( Ovgl::Vector3* vec1, Ovgl::Vector3* vec2 )
//{
//	Ovgl::Vector3 out = {0};
//	out.x = vec1->y * vec2->z - vec2->y * vec1->z;
//	out.y = vec1->z * vec2->x - vec2->z * vec1->x;
//	out.z = vec1->x * vec2->y - vec2->x * vec1->y;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3Transform( Ovgl::Vector3* vector, Ovgl::Matrix44* matrix )
//{
//	Ovgl::Vector3 out = {0};
//	out.x = vector->x * matrix->_11 + vector->y * matrix->_21 + vector->z * matrix->_31 + matrix->_41;
//	out.y = vector->x * matrix->_12 + vector->y * matrix->_22 + vector->z * matrix->_32 + matrix->_42;
//	out.z = vector->x * matrix->_13 + vector->y * matrix->_23 + vector->z * matrix->_33 + matrix->_43;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3Normalize( Ovgl::Vector3* vector )
//{
//	Ovgl::Vector3 out = {0};
//	float l = sqrt((vector->x * vector->x) + (vector->y * vector->y) + (vector->z * vector->z));
//	out.x = vector->x / l;
//	out.y = vector->y / l;
//	out.z = vector->z / l;
//	return out;
//}
//
//Ovgl::Vector3 Ovgl::Vector3Center( std::vector<Ovgl::Vector3>& vertices )
//{
//	Ovgl::Vector3 out = {0};
//	for( DWORD v = 0; v < vertices.size(); v++ )
//	{
//		out = out + vertices[v];
//	}
//	out.x = out.x / vertices.size();
//	out.y = out.y / vertices.size();
//	out.z = out.z / vertices.size();
//	return out;
//}
//
//Ovgl::Vector2 Ovgl::Vector2Set( float x, float y )
//{
//	Ovgl::Vector2 out = {0};
//	out.x = x;
//	out.y = y;
//	return out;
//}
//
//Ovgl::Vector4 Ovgl::Vector4Lerp( Ovgl::Vector4& vec1, Ovgl::Vector4& vec2, float u)
//{
//	return (vec1 * (1 - u)) + (vec2 * u);
//}
//
//float Ovgl::Lerp( float val1, float val2, float u)
//{
//	return (val1 * (1 - u)) + (val2 * u);
//}

#include "OvglIncludes.h"
#include "OvglMath.h"

Ovgl::Matrix44 Ovgl::Matrix44::operator * ( const Ovgl::Matrix44& in ) const
{
	Ovgl::Matrix44 out = {0};
	out._11 = _11 * in._11 + _12 * in._21 + _13 * in._31 + _14 * in._41;
	out._12 = _11 * in._12 + _12 * in._22 + _13 * in._32 + _14 * in._42;
	out._13 = _11 * in._13 + _12 * in._23 + _13 * in._33 + _14 * in._43;
	out._14 = _11 * in._14 + _12 * in._24 + _13 * in._34 + _14 * in._44;
	out._21 = _21 * in._11 + _22 * in._21 + _23 * in._31 + _24 * in._41;
	out._22 = _21 * in._12 + _22 * in._22 + _23 * in._32 + _24 * in._42;
	out._23 = _21 * in._13 + _22 * in._23 + _23 * in._33 + _24 * in._43;
	out._24 = _21 * in._14 + _22 * in._24 + _23 * in._34 + _24 * in._44;
	out._31 = _31 * in._11 + _32 * in._21 + _33 * in._31 + _34 * in._41;
	out._32 = _31 * in._12 + _32 * in._22 + _33 * in._32 + _34 * in._42;
	out._33 = _31 * in._13 + _32 * in._23 + _33 * in._33 + _34 * in._43;
	out._34 = _31 * in._14 + _32 * in._24 + _33 * in._34 + _34 * in._44;
	out._41 = _41 * in._11 + _42 * in._21 + _43 * in._31 + _44 * in._41;
	out._42 = _41 * in._12 + _42 * in._22 + _43 * in._32 + _44 * in._42;
	out._43 = _41 * in._13 + _42 * in._23 + _43 * in._33 + _44 * in._43;
	out._44 = _41 * in._14 + _42 * in._24 + _43 * in._34 + _44 * in._44;
	return out;
}

Ovgl::Matrix44 Ovgl::Matrix33::to4x4()
{
	Ovgl::Matrix44 out = {0};
	CopyMemory( &out, this, sizeof(Ovgl::Matrix33) );
	out._44 = 1;
	return out;
}


void Ovgl::Matrix33::toDoubles( double* data )
{
	for( int r = 0; r < 3; r++ )
	{
		for( int c = 0; c < 3; c++ )
		{
			data[(4 * r) + c] = (double)(*this)[r][c];
		}
	}
}

void Ovgl::Matrix33::fromDoubles( double* data )
{
	for( int r = 0; r < 3; r++ )
	{
		for( int c = 0; c < 3; c++ )
		{
			(*this)[r][c] = (float)data[(4 * r) + c];
		}
	}
}

Ovgl::Matrix33 Ovgl::Matrix33::operator * ( const Ovgl::Matrix33& in ) const
{
	Ovgl::Matrix33 out = {0};
	out._11 = _11 * in._11 + _12 * in._21 + _13 * in._31;
	out._12 = _11 * in._12 + _12 * in._22 + _13 * in._32;
	out._13 = _11 * in._13 + _12 * in._23 + _13 * in._33;
	out._21 = _21 * in._11 + _22 * in._21 + _23 * in._31;
	out._22 = _21 * in._12 + _22 * in._22 + _23 * in._32;
	out._23 = _21 * in._13 + _22 * in._23 + _23 * in._33;
	out._31 = _31 * in._11 + _32 * in._21 + _33 * in._31;
	out._32 = _31 * in._12 + _32 * in._22 + _33 * in._32;
	out._33 = _31 * in._13 + _32 * in._23 + _33 * in._33;
	return out;
}

Ovgl::Matrix33 Ovgl::Matrix44::to3x3()
{
	Ovgl::Matrix33 out = {0};
	CopyMemory( &out, this, sizeof(Ovgl::Matrix33) );
	return out;
}

void Ovgl::Matrix44::toDoubles( double* data )
{
	for( int r = 0; r < 4; r++ )
	{
		for( int c = 0; c < 4; c++ )
		{
			data[(4 * r) + c] = (double)(*this)[r][c];
		}
	}
}

void Ovgl::Matrix44::fromDoubles( double* data )
{
	for( int r = 0; r < 4; r++ )
	{
		for( int c = 0; c < 4; c++ )
		{
			(*this)[r][c] = (float)data[(4 * r) + c];
		}
	}
}

Ovgl::Matrix44 Ovgl::MatrixIdentity()
{
	Ovgl::Matrix44 out = {0};
	out._11 = 1;
	out._22 = 1;
	out._33 = 1;
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixInverse ( Ovgl::Vector4* in_vec, Ovgl::Matrix44* in_mat)
{
	Ovgl::Matrix44 out = {0};
	D3DXMATRIX temp;
	CopyMemory(&temp, in_mat, sizeof(Ovgl::Matrix44));
	D3DXMatrixInverse( &temp, NULL, &temp );
	CopyMemory(&out, &temp, sizeof(Ovgl::Matrix44));
    return out;
}

Ovgl::Matrix44 Ovgl::MatrixScaling( float x, float y, float z )
{
	Ovgl::Matrix44 out = {0};
	out._11 = x;
	out._22 = y;
	out._33 = z;
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixTranslation( float x, float y, float z )
{
	Ovgl::Matrix44 out = {0};
	out._11 = 1;
	out._22 = 1;
	out._33 = 1;
	out._41 = x;
	out._42 = y;
	out._43 = z;
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixRotationX( float angle )
{
	Ovgl::Matrix44 out = {0};
	out._11 = 1;
	out._22 = cos( angle );
	out._23 = -sin( angle );
	out._32 = sin( angle );
	out._33 = cos( angle );
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixRotationY( float angle )
{
	Ovgl::Matrix44 out = {0};
	out._11 = cos( angle );
	out._13 = sin( angle );
	out._22 = 1;
	out._31 = -sin( angle );
	out._33 = cos( angle );
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixRotationZ( float angle )
{
	Ovgl::Matrix44 out = {0};
	out._11 = cos( angle );
	out._12 = -sin( angle );
	out._21 = sin( angle );
	out._22 = cos( angle );
	out._33 = 1;
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixRotationEuler( float roll, float pitch, float yaw )
{
	Ovgl::Matrix44 out = {0};
	out._11 = ( cos(roll) * cos(yaw) ) + ( sin(roll) * sin(pitch) * sin(yaw) );
	out._12 = ( sin(roll) * cos(pitch) );
	out._13 = ( cos(roll) * -sin(yaw) ) + ( sin(roll) * sin(pitch) * cos(yaw) );
	out._21 = (-sin(roll) * cos(yaw) ) + ( cos(roll) * sin(pitch) * sin(yaw) );
	out._22 = ( cos(roll) * cos(pitch) );
	out._23 = ( sin(roll) * sin(yaw) ) + ( cos(roll) * sin(pitch) * cos(yaw) );
	out._31 = ( cos(pitch) * sin(yaw) );
	out._32 = -sin(pitch);
	out._33 = ( cos(pitch) * cos(yaw) );
	out._44 = 1;
	return out;
}

Ovgl::Matrix44 Ovgl::MatrixRotationAxis( Ovgl::Vector3* axis, float angle )
{
	Ovgl::Matrix44 out = {0};
	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1.0F - c;
	out._11 = t * axis->x * axis->x + c;
	out._21 = t * axis->x * axis->y + s * axis->z;
	out._31 = t * axis->x * axis->z - s * axis->y;
	out._41 = 0.0F;
	out._12 = t * axis->y * axis->x - s * axis->z;
	out._22 = t * axis->y * axis->y + c;
	out._32 = t * axis->y * axis->z + s * axis->x;
	out._42 = 0.0F;
	out._13 = t * axis->z * axis->x + s * axis->y;
	out._23 = t * axis->z * axis->y - s * axis->x;
	out._33 = t * axis->z * axis->z + c;
	out._43 = 0.0F;
	out._14 = 0.0F;
	out._24 = 0.0F;
	out._34 = 0.0F;
	out._44 = 1.0F;
	return out;
}


Ovgl::Matrix44 Ovgl::MatrixPerspectiveLH( float ViewWidth, float ViewHeight, float NearZ, float FarZ )
{
	Ovgl::Matrix44 out = {0};
	D3DXMATRIX temp;
	D3DXMatrixPerspectiveFovLH( &temp, ViewWidth, ViewHeight, NearZ, FarZ );
	CopyMemory(&out, &temp, sizeof(Ovgl::Matrix44));
	return out;
}


Ovgl::Matrix44 Ovgl::MatrixRotationQuaternion( Ovgl::Vector4* q )
{
	Ovgl::Matrix44 out = {0};
    float sqw = q->w * q->w;
    float sqx = q->x * q->x;
    float sqy = q->y * q->y;
    float sqz = q->z * q->z;
    float invs = 1 / (sqx + sqy + sqz + sqw);
    out._11 = ( sqx - sqy - sqz + sqw) * invs;
    out._22 = (-sqx + sqy - sqz + sqw) * invs;
    out._33 = (-sqx - sqy + sqz + sqw) * invs;
    float tmp1 = q->x * q->y;
    float tmp2 = q->z * q->w;
    out._21 = 2.0f * (tmp1 + tmp2) * invs;
    out._12 = 2.0f * (tmp1 - tmp2) * invs;
    tmp1 = q->x * q->z;
    tmp2 = q->y * q->w;
    out._31 = 2.0f * (tmp1 - tmp2) * invs;
    out._13 = 2.0f * (tmp1 + tmp2) * invs;
    tmp1 = q->y * q->z;
    tmp2 = q->x * q->w;
    out._32 = 2.0f * (tmp1 + tmp2) * invs;
    out._23 = 2.0f * (tmp1 - tmp2) * invs;
	out._14 = 0.0f;
	out._24 = 0.0f;
	out._34 = 0.0f;
	out._41 = 0.0f;
	out._42 = 0.0f;
	out._43 = 0.0f;
	out._44 = 1.0f;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator - ( const Ovgl::Vector4& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w - in.w;
	out.x = x - in.x;
	out.y = y - in.y;
	out.z = z - in.z;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator + ( const Ovgl::Vector4& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w + in.w;
	out.x = x + in.x;
	out.y = y + in.y;
	out.z = z + in.z;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator / ( const Ovgl::Vector4& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w / in.w;
	out.x = x / in.x;
	out.y = y / in.y;
	out.z = z / in.z;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator * ( const Ovgl::Vector4& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w * in.w;
	out.x = x * in.x;
	out.y = y * in.y;
	out.z = z * in.z;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator / ( const float& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w / in;
	out.x = x / in;
	out.y = y / in;
	out.z = z / in;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4::operator * ( const float& in ) const
{
	Ovgl::Vector4 out = {0};
	out.w = w * in;
	out.x = x * in;
	out.y = y * in;
	out.z = z * in;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4Set( float w, float x, float y, float z )
{
	Ovgl::Vector4 out = {0};
	out.w = w;
	out.x = x;
	out.y = y;
	out.z = z;
	return out;
}

Ovgl::Vector4 Ovgl::QuaternionRotationMatrix( Ovgl::Matrix44* matrix )
{
	Ovgl::Vector4 out = {0};
	out.w = (float)sqrt(1.0f + matrix->_11 + matrix->_22 + matrix->_33) / 2.0f;
	float w4 = (4.0f * out.w);
	out.x = (matrix->_32 - matrix->_23) / w4 ;
	out.y = (matrix->_13 - matrix->_31) / w4 ;
	out.z = (matrix->_21 - matrix->_12) / w4 ;
	return out;
}

Ovgl::Vector4 Ovgl::QuaternionRotationAxis( Ovgl::Vector3* axis, float angle )
{
	Ovgl::Vector4 out = {0};
	out.w  = cosf( angle / 2 );
	out.x = axis->x * sinf( angle / 2 );
	out.y = axis->y * sinf( angle / 2 );
	out.z = axis->z * sinf( angle / 2 );
	return out;
}

Ovgl::Vector4 Ovgl::QuaternionRotationEuler( float yaw, float pitch, float roll )
{
	Ovgl::Vector4 out = {0};	
	float c1 = cos( yaw / 2 );
	float s1 = sin( yaw / 2 );
	float c2 = cos( pitch / 2 );
	float s2 = sin( pitch / 2 );
	float c3 = cos( roll / 2 );
	float s3 = sin( roll / 2 );
	float c1c2 = c1 * c2;
	float s1s2 = s1 * s2;
	out.w = c1c2 * c3 - s1s2 * s3;
	out.x = c1c2 * s3 + s1s2 * c3;
	out.y = s1 * c2 * c3 + c1 * s2 * s3;
	out.z = c1 * s2 * c3 - s1 * c2 * s3;
	return out;
}

float& Ovgl::Vector2::operator [] (size_t index)
{
	return (&x)[index];
}

float& Ovgl::Vector3::operator [] (size_t index)
{
	return (&x)[index];
}

float& Ovgl::Vector4::operator [] (size_t index)
{
	return (&x)[index];
}

Ovgl::Vector3& Ovgl::Matrix33::operator [] (size_t index)
{
	return (Ovgl::Vector3&)*((Ovgl::Vector3*)((&_11) + (index*3)));
}

Ovgl::Vector4& Ovgl::Matrix44::operator [] (size_t index)
{
	return (Ovgl::Vector4&)*((Ovgl::Vector4*)((&_11) + (index*4)));
}

Ovgl::Vector3 Ovgl::Vector3::operator - ( const Ovgl::Vector3& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x - in.x;
	out.y = y - in.y;
	out.z = z - in.z;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3::operator + ( const Ovgl::Vector3& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x + in.x;
	out.y = y + in.y;
	out.z = z + in.z;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3::operator / ( const Ovgl::Vector3& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x / in.x;
	out.y = y / in.y;
	out.z = z / in.z;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3::operator * ( const Ovgl::Vector3& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x * in.x;
	out.y = y * in.y;
	out.z = z * in.z;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3::operator / ( const float& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x / in;
	out.y = y / in;
	out.z = z / in;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3::operator * ( const float& in ) const
{
	Ovgl::Vector3 out = {0};
	out.x = x * in;
	out.y = y * in;
	out.z = z * in;
	return out;
}

void Ovgl::Vector3::toDoubles( double* data )
{
	data[0] = (double)x;
	data[1] = (double)y;
	data[2] = (double)z;
}

void Ovgl::Vector3::fromDoubles( double* data )
{
	x = (float)data[0];
	y = (float)data[1];
	z = (float)data[2];
}

Ovgl::Vector3 Ovgl::Vector3Set( float x, float y, float z )
{
	Ovgl::Vector3 out = {0};
	out.x = x;
	out.y = y;
	out.z = z;
	return out;
}

float Ovgl::Distance( Ovgl::Vector3* vector1, Ovgl::Vector3* vector2 )
{
	Ovgl::Vector3 vector = (*vector1) - (*vector2);
	float out = sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
	return out;
}

float Ovgl::Vector3Dot( Ovgl::Vector3* vec1, Ovgl::Vector3* vec2 )
{
	return vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
}

Ovgl::Vector3 Ovgl::Vector3Cross( Ovgl::Vector3* vec1, Ovgl::Vector3* vec2 )
{
	Ovgl::Vector3 out = {0};
	out.x = vec1->y * vec2->z - vec2->y * vec1->z;
	out.y = vec1->z * vec2->x - vec2->z * vec1->x;
	out.z = vec1->x * vec2->y - vec2->x * vec1->y;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3Transform( Ovgl::Vector3* vector, Ovgl::Matrix44* matrix )
{
	Ovgl::Vector3 out = {0};
	out.x = vector->x * matrix->_11 + vector->y * matrix->_21 + vector->z * matrix->_31 + matrix->_41;
	out.y = vector->x * matrix->_12 + vector->y * matrix->_22 + vector->z * matrix->_32 + matrix->_42;
	out.z = vector->x * matrix->_13 + vector->y * matrix->_23 + vector->z * matrix->_33 + matrix->_43;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3Normalize( Ovgl::Vector3* vector )
{
	Ovgl::Vector3 out = {0};
	float l = sqrt((vector->x * vector->x) + (vector->y * vector->y) + (vector->z * vector->z));
	out.x = vector->x / l;
	out.y = vector->y / l;
	out.z = vector->z / l;
	return out;
}

Ovgl::Vector3 Ovgl::Vector3Center( std::vector<Ovgl::Vector3>& vertices )
{
	Ovgl::Vector3 out = {0};
	for( DWORD v = 0; v < vertices.size(); v++ )
	{
		out = out + vertices[v];
	}
	out.x = out.x / vertices.size();
	out.y = out.y / vertices.size();
	out.z = out.z / vertices.size();
	return out;
}

Ovgl::Vector2 Ovgl::Vector2Set( float x, float y )
{
	Ovgl::Vector2 out = {0};
	out.x = x;
	out.y = y;
	return out;
}

Ovgl::Vector4 Ovgl::Vector4Lerp( Ovgl::Vector4& vec1, Ovgl::Vector4& vec2, float u)
{
	return (vec1 * (1 - u)) + (vec2 * u);
}

float Ovgl::Lerp( float val1, float val2, float u)
{
	return (val1 * (1 - u)) + (val2 * u);
}