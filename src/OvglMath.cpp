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
* @brief This part of the library deals with mathmatical storage and computation.
*/

#include "OvglCommon.h"
#include "OvglContext.h"
#include "OvglMath.h"

namespace Ovgl
{
Vector2::Vector2()
{
    x = 0.0f;
    y = 0.0f;
}

Vector2::Vector2( float new_x, float new_y )
{
    x = new_x;
    y = new_y;
}

float& Vector2::operator [] ( size_t index )
{
    return (&x)[index];
}

Vector2 Vector2::operator - ( const Vector2& in ) const
{
    Vector2 out;
    out.x = x - in.x;
    out.y = y - in.y;
    return out;
}

Vector2 Vector2::operator / ( const float& in ) const
{
    Vector2 out;
    out.x = x / in;
    out.y = y / in;
    return out;
}

bool Vector2::operator == ( const Vector2& in ) const
{
    if( in.x == x && in.y == y )
        return true;
    else
        return false;
}

bool Vector2::operator != ( const Vector2& in ) const
{
    if( in.x ==x && in.y == y )
        return true;
    else
        return false;
}

Vector3::Vector3()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector3::Vector3( float new_x, float new_y, float new_z )
{
    x = new_x;
    y = new_y;
    z = new_z;
}

float& Vector3::operator [] ( size_t index )
{
    return (&x)[index];
}

Vector3 Vector3::operator - ( const Vector3& in ) const
{
    Vector3 out;
    out.x = x - in.x;
    out.y = y - in.y;
    out.z = z - in.z;
    return out;
}

Vector3 Vector3::operator + ( const Vector3& in ) const
{
    Vector3 out;
    out.x = x + in.x;
    out.y = y + in.y;
    out.z = z + in.z;
    return out;
}

Vector3 Vector3::operator / ( const Vector3& in ) const
{
    Vector3 out;
    out.x = x / in.x;
    out.y = y / in.y;
    out.z = z / in.z;
    return out;
}

Vector3 Vector3::operator * ( const Vector3& in ) const
{
    Vector3 out;
    out.x = x * in.x;
    out.y = y * in.y;
    out.z = z * in.z;
    return out;
}

Vector3 Vector3::operator / ( const float& in ) const
{
    Vector3 out;
    out.x = x / in;
    out.y = y / in;
    out.z = z / in;
    return out;
}

Vector3 Vector3::operator * ( const float& in ) const
{
    Vector3 out;
    out.x = x * in;
    out.y = y * in;
    out.z = z * in;
    return out;
}

bool Vector3::operator == ( const Vector3& in ) const
{
    if( x == in.x && y == in.y && z == in.z )
        return true;
    else
        return false;
}

bool Vector3::operator != ( const Vector3& in ) const
{
    if( x == in.x && y == in.y && z == in.z )
        return true;
    else
        return false;
}

Vector4::Vector4()
{
    w = 0.0f;
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector4::Vector4( float new_x, float new_y, float new_z, float new_w )
{
    w = new_w;
    x = new_x;
    y = new_y;
    z = new_z;
}

Vector4 Vector4::operator - ( const Vector4& in ) const
{
    Vector4 out;
    out.w = w - in.w;
    out.x = x - in.x;
    out.y = y - in.y;
    out.z = z - in.z;
    return out;
}

Vector4 Vector4::operator + ( const Vector4& in ) const
{
    Vector4 out;
    out.w = w + in.w;
    out.x = x + in.x;
    out.y = y + in.y;
    out.z = z + in.z;
    return out;
}

Vector4 Vector4::operator / ( const Vector4& in ) const
{
    Vector4 out;
    out.w = w / in.w;
    out.x = x / in.x;
    out.y = y / in.y;
    out.z = z / in.z;
    return out;
}

Vector4 Vector4::operator * ( const Vector4& in ) const
{
    Vector4 out;
    out.w = w * in.w;
    out.x = x * in.x;
    out.y = y * in.y;
    out.z = z * in.z;
    return out;
}

Vector4 Vector4::operator / ( const float& in ) const
{
    Vector4 out;
    out.w = w / in;
    out.x = x / in;
    out.y = y / in;
    out.z = z / in;
    return out;
}

Vector4 Vector4::operator * ( const float& in ) const
{
    Vector4 out;
    out.w = w * in;
    out.x = x * in;
    out.y = y * in;
    out.z = z * in;
    return out;
}

Matrix33::Matrix33()
{
    _11 = 1;
    _12 = 0;
    _13 = 0;

    _21 = 0;
	_22 = 1;
    _23 = 0;

    _31 = 0;
    _32 = 0;
    _33 = 1;
}

Matrix44::Matrix44()
{
    _11 = 1;
    _12 = 0;
    _13 = 0;
    _14 = 0;

    _21 = 0;
	_22 = 1;
    _23 = 0;
    _24 = 0;

    _31 = 0;
    _32 = 0;
    _33 = 1;
    _34 = 0;

    _41 = 0;
    _42 = 0;
    _43 = 0;
    _44 = 1;
}

Matrix44 Matrix44::operator * ( const Matrix44& in ) const
{
    Matrix44 out;
	Matrix44 mine = *this;
	for( int row = 0; row < 4; row++)
	{
		for( int col = 0; col < 4; col++)
		{
    		out[row][col] = mine[row][0] * in[0][col] + mine[row][1] * in[1][col] + mine[row][2] * in[2][col] + mine[row][3] * in[3][col];
		}
	}
	return out;
}

Matrix44 Matrix33::to4x4()
{
    Matrix44 out;
    out._11 = _11;
	out._12 = _12;
	out._13 = _13;
	out._21 = _21;
	out._22 = _22;
	out._23 = _23;
	out._31 = _31;
	out._32 = _32;
	out._33 = _33;
    return out;
}


void Matrix33::toDoubles( double* data )
{
    for( int32_t r = 0; r < 3; r++ )
    {
        for( int32_t c = 0; c < 3; c++ )
        {
            data[(4 * r) + c] = (double)(*this)[r][c];
        }
    }
}

void Matrix33::fromDoubles( double* data )
{
    for( int32_t r = 0; r < 3; r++ )
    {
        for( int32_t c = 0; c < 3; c++ )
        {
            (*this)[r][c] = (float)data[(4 * r) + c];
        }
    }
}

Matrix33 Matrix33::operator * ( const Matrix33& in ) const
{
    Matrix33 out;
	Matrix33 mine = *this;
	for( int row = 0; row < 3; row++)
	{
		for( int col = 0; col < 3; col++)
		{
    		out[row][col] = mine[row][0] * in[0][col] + mine[row][1] * in[1][col] + mine[row][2] * in[2][col];
		}
	}
	return out;
}

Matrix44 Matrix44::rotation()
{
    Matrix44 out;
    out = *this;
    out._41 = 0.0f;
    out._42 = 0.0f;
    out._43 = 0.0f;
    return out;
}

Matrix44 Matrix44::translation()
{
    Matrix44 out;
    out = matrixTranslation(this->_41, this->_42, this->_43);
    return out;
}

Matrix33 Matrix44::to3x3()
{
    Matrix33 out;
	out._11 = _11;
	out._12 = _12;
	out._13 = _13;
	out._21 = _21;
	out._22 = _22;
	out._23 = _23;
	out._31 = _31;
	out._32 = _32;
	out._33 = _33;
    return out;
}

void Matrix44::toDoubles( double* data )
{
    for( int32_t r = 0; r < 4; r++ )
    {
        for( int32_t c = 0; c < 4; c++ )
        {
            data[(4 * r) + c] = (double)(*this)[r][c];
        }
    }
}

void Matrix44::fromDoubles( double* data )
{
    for( int32_t r = 0; r < 4; r++ )
    {
        for( int32_t c = 0; c < 4; c++ )
        {
            (*this)[r][c] = (float)data[(4 * r) + c];
        }
    }
}

Matrix44 matrixIdentity()
{
    Matrix44 out;
    out._11 = 1;
    out._22 = 1;
    out._33 = 1;
    out._44 = 1;
    return out;
}

Matrix44 matrixInverse ( const Vector4& in_vec, const Matrix44& in_mat)
{
    Matrix44 out;
    float inv[16], det;
    inv[0] =   in_mat._22*in_mat._33*in_mat._44 - in_mat._22*in_mat._34*in_mat._43 - in_mat._32*in_mat._23*in_mat._44
            + in_mat._32*in_mat._24*in_mat._43 + in_mat._42*in_mat._23*in_mat._34 - in_mat._42*in_mat._24*in_mat._33;
    inv[4] =  -in_mat._21*in_mat._33*in_mat._44 + in_mat._21*in_mat._34*in_mat._43 + in_mat._31*in_mat._23*in_mat._44
            - in_mat._31*in_mat._24*in_mat._43 - in_mat._41*in_mat._23*in_mat._34 + in_mat._41*in_mat._24*in_mat._33;
    inv[8] =   in_mat._21*in_mat._32*in_mat._44 - in_mat._21*in_mat._34*in_mat._42 - in_mat._31*in_mat._22*in_mat._44
            + in_mat._31*in_mat._24*in_mat._42 + in_mat._41*in_mat._22*in_mat._34 - in_mat._41*in_mat._24*in_mat._32;
    inv[12] = -in_mat._21*in_mat._32*in_mat._43 + in_mat._21*in_mat._33*in_mat._42 + in_mat._31*in_mat._22*in_mat._43
            - in_mat._31*in_mat._23*in_mat._42 - in_mat._41*in_mat._22*in_mat._33 + in_mat._41*in_mat._23*in_mat._32;
    inv[1] =  -in_mat._12*in_mat._33*in_mat._44 + in_mat._12*in_mat._34*in_mat._43 + in_mat._32*in_mat._13*in_mat._44
            - in_mat._32*in_mat._14*in_mat._43 - in_mat._42*in_mat._13*in_mat._34 + in_mat._42*in_mat._14*in_mat._33;
    inv[5] =   in_mat._11*in_mat._33*in_mat._44 - in_mat._11*in_mat._34*in_mat._43 - in_mat._31*in_mat._13*in_mat._44
            + in_mat._31*in_mat._14*in_mat._43 + in_mat._41*in_mat._13*in_mat._34 - in_mat._41*in_mat._14*in_mat._33;
    inv[9] =  -in_mat._11*in_mat._32*in_mat._44 + in_mat._11*in_mat._34*in_mat._42 + in_mat._31*in_mat._12*in_mat._44
            - in_mat._31*in_mat._14*in_mat._42 - in_mat._41*in_mat._12*in_mat._34 + in_mat._41*in_mat._14*in_mat._32;
    inv[13] =  in_mat._11*in_mat._32*in_mat._43 - in_mat._11*in_mat._33*in_mat._42 - in_mat._31*in_mat._12*in_mat._43
            + in_mat._31*in_mat._13*in_mat._42 + in_mat._41*in_mat._12*in_mat._33 - in_mat._41*in_mat._13*in_mat._32;
    inv[2] =   in_mat._12*in_mat._23*in_mat._44 - in_mat._12*in_mat._24*in_mat._43 - in_mat._22*in_mat._13*in_mat._44
            + in_mat._22*in_mat._14*in_mat._43 + in_mat._42*in_mat._13*in_mat._24 - in_mat._42*in_mat._14*in_mat._23;
    inv[6] =  -in_mat._11*in_mat._23*in_mat._44 + in_mat._11*in_mat._24*in_mat._43 + in_mat._21*in_mat._13*in_mat._44
            - in_mat._21*in_mat._14*in_mat._43 - in_mat._41*in_mat._13*in_mat._24 + in_mat._41*in_mat._14*in_mat._23;
    inv[10] =  in_mat._11*in_mat._22*in_mat._44 - in_mat._11*in_mat._24*in_mat._42 - in_mat._21*in_mat._12*in_mat._44
            + in_mat._21*in_mat._14*in_mat._42 + in_mat._41*in_mat._12*in_mat._24 - in_mat._41*in_mat._14*in_mat._22;
    inv[14] = -in_mat._11*in_mat._22*in_mat._43 + in_mat._11*in_mat._23*in_mat._42 + in_mat._21*in_mat._12*in_mat._43
            - in_mat._21*in_mat._13*in_mat._42 - in_mat._41*in_mat._12*in_mat._23 + in_mat._41*in_mat._13*in_mat._22;
    inv[3] =  -in_mat._12*in_mat._23*in_mat._34 + in_mat._12*in_mat._24*in_mat._33 + in_mat._22*in_mat._13*in_mat._34
            - in_mat._22*in_mat._14*in_mat._33 - in_mat._32*in_mat._13*in_mat._24 + in_mat._32*in_mat._14*in_mat._23;
    inv[7] =   in_mat._11*in_mat._23*in_mat._34 - in_mat._11*in_mat._24*in_mat._33 - in_mat._21*in_mat._13*in_mat._34
            + in_mat._21*in_mat._14*in_mat._33 + in_mat._31*in_mat._13*in_mat._24 - in_mat._31*in_mat._14*in_mat._23;
    inv[11] = -in_mat._11*in_mat._22*in_mat._34 + in_mat._11*in_mat._24*in_mat._32 + in_mat._21*in_mat._12*in_mat._34
            - in_mat._21*in_mat._14*in_mat._32 - in_mat._31*in_mat._12*in_mat._24 + in_mat._31*in_mat._14*in_mat._22;
    inv[15] =  in_mat._11*in_mat._22*in_mat._33 - in_mat._11*in_mat._23*in_mat._32 - in_mat._21*in_mat._12*in_mat._33
            + in_mat._21*in_mat._13*in_mat._32 + in_mat._31*in_mat._12*in_mat._23 - in_mat._31*in_mat._13*in_mat._22;
    det = in_mat._11*inv[0] + in_mat._12*inv[4] + in_mat._13*inv[8] + in_mat._14*inv[12];
    if (det == 0.0f)
        return out;

    det = 1.0f / det;

    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            out[c][r] = inv[(c*4) + r] * det;
    return out;
}


Matrix44 matrixScaling( float x, float y, float z )
{
    Matrix44 out;
    out._11 = x;
    out._22 = y;
    out._33 = z;
    out._44 = 1;
    return out;
}

Matrix44 matrixTranslation( float x, float y, float z )
{
    Matrix44 out;
    out._11 = 1;
    out._22 = 1;
    out._33 = 1;
    out._41 = x;
    out._42 = y;
    out._43 = z;
    out._44 = 1;
    return out;
}

Matrix44 matrixTranspose( const Matrix44& in_mat )
{
    Matrix44 out;
    out._11 = in_mat._11;
    out._21 = in_mat._12;
    out._31 = in_mat._13;
    out._41 = in_mat._14;
    out._12 = in_mat._21;
    out._22 = in_mat._22;
    out._32 = in_mat._23;
    out._42 = in_mat._24;
    out._13 = in_mat._31;
    out._23 = in_mat._32;
    out._33 = in_mat._33;
    out._43 = in_mat._34;
    out._14 = in_mat._41;
    out._24 = in_mat._42;
    out._34 = in_mat._43;
    out._44 = in_mat._44;
    return out;
}

Matrix44 matrixRotationX( float angle )
{
    Matrix44 out;
    out._11 = 1;
    out._22 = cos( angle );
    out._23 = -sin( angle );
    out._32 = sin( angle );
    out._33 = cos( angle );
    out._44 = 1;
    return out;
}

Matrix44 matrixRotationY( float angle )
{
    Matrix44 out;
    out._11 = cos( angle );
    out._13 = sin( angle );
    out._22 = 1;
    out._31 = -sin( angle );
    out._33 = cos( angle );
    out._44 = 1;
    return out;
}

Matrix44 matrixRotationZ( float angle )
{
    Matrix44 out;
    out._11 = cos( angle );
    out._12 = -sin( angle );
    out._21 = sin( angle );
    out._22 = cos( angle );
    out._33 = 1;
    out._44 = 1;
    return out;
}

Matrix44 matrixRotationEuler( float roll, float pitch, float yaw )
{
    float rollSin = sin(roll), rollCos = cos(roll);
	float pitchSin = sin(pitch), pitchCos = cos(pitch);
	float yawSin = sin(yaw), yawCos = cos(yaw);
	
	Matrix44 out;
    out._11 = ( rollCos * yawCos ) + ( rollSin * pitchSin * yawSin );
    out._12 = ( rollSin * pitchCos );
    out._13 = ( rollCos * -yawSin ) + ( rollSin * pitchSin * yawCos );
    out._21 = (-rollSin * yawCos ) + ( rollCos * pitchSin * yawSin );
    out._22 = ( rollCos * pitchCos );
    out._23 = ( rollSin * yawSin ) + ( rollCos * pitchSin * yawCos );
    out._31 = ( pitchCos * yawSin );
    out._32 = -pitchSin;
    out._33 = ( pitchCos * yawCos );
    out._44 = 1;
    return out;
}

Matrix44 matrixRotationAxis( const Vector3& axis, float angle )
{
    Matrix44 out;
    float s = sinf(angle);
    float c = cosf(angle);
    float t = 1.0F - c;
    out._11 = t * axis.x * axis.x + c;
    out._21 = t * axis.x * axis.y + s * axis.z;
    out._31 = t * axis.x * axis.z - s * axis.y;
    out._41 = 0.0F;
    out._12 = t * axis.y * axis.x - s * axis.z;
    out._22 = t * axis.y * axis.y + c;
    out._32 = t * axis.y * axis.z + s * axis.x;
    out._42 = 0.0F;
    out._13 = t * axis.z * axis.x + s * axis.y;
    out._23 = t * axis.z * axis.y - s * axis.x;
    out._33 = t * axis.z * axis.z + c;
    out._43 = 0.0F;
    out._14 = 0.0F;
    out._24 = 0.0F;
    out._34 = 0.0F;
    out._44 = 1.0F;
    return out;
}

Matrix44 matrixPerspectiveLH( float fov, float aspect, float zn, float zf)
{
    Matrix44 out;
    float yScale = cos(fov/2) / sin(fov/2);
    float xScale = yScale / aspect;
    out._11 = -xScale;
    out._12 = 0;
    out._13 = 0;
    out._14 = 0;
    out._21 = 0;
    out._22 = yScale;
    out._23 = 0;
    out._24 = 0;
    out._31 = 0;
    out._32 = 0;
    out._33 = zf/(zf-zn);
    out._34 = 1;
    out._41 = 0;
    out._42 = 0;
    out._43 = -zn*zf/(zf-zn);
    out._44 = 0;
    return out;
}

Matrix44 matrixRotationQuaternion( const Vector4& q )
{
    Matrix44 out;
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;
    float xx2 = q.x * x2;
    float yy2 = q.y * y2;
    float zz2 = q.z * z2;
    float xy2 = q.x * y2;
    float yz2 = q.y * z2;
    float zx2 = q.z * x2;
    float xw2 = q.w * x2;
    float yw2 = q.w * y2;
    float zw2 = q.w * z2;
    out._11 = float(1) - yy2 - zz2;
    out._12 = xy2 + zw2;
    out._13 = zx2 - yw2;
    out._21 = xy2 - zw2;
    out._22 = float(1) - zz2 - xx2;
    out._23 = yz2 + xw2;
    out._31 = zx2 + yw2;
    out._32 = yz2 - xw2;
    out._33 = float(1) - xx2 - yy2;
    out._14 = out._24 = out._34 = out._41 = out._42 = out._43 = 0;
    out._44 = 1;
    return out;
}

Matrix44 matrixSwapYZ( const Matrix44& in_mat )
{
    Matrix44 out;
    out._11 = in_mat._11;
    out._12 = in_mat._13;
    out._13 = in_mat._12;
    out._14 = in_mat._14;
    out._21 = in_mat._21;
    out._22 = in_mat._23;
    out._23 = in_mat._22;
    out._24 = in_mat._24;
    out._31 = in_mat._31;
    out._32 = in_mat._33;
    out._33 = in_mat._32;
    out._34 = in_mat._34;
    out._41 = in_mat._41;
    out._42 = in_mat._43;
    out._43 = in_mat._42;
    out._44 = in_mat._44;
    return out;
}

Matrix44 matrixSwapXZ( const Matrix44& in_mat )
{
    Matrix44 out;
    out._11 = in_mat._13;
    out._12 = in_mat._12;
    out._13 = in_mat._11;
    out._14 = in_mat._14;
    out._21 = in_mat._23;
    out._22 = in_mat._22;
    out._23 = in_mat._21;
    out._24 = in_mat._24;
    out._31 = in_mat._33;
    out._32 = in_mat._32;
    out._33 = in_mat._31;
    out._34 = in_mat._34;
    out._41 = in_mat._43;
    out._42 = in_mat._42;
    out._43 = in_mat._41;
    out._44 = in_mat._44;
    return out;
}

Vector4 quaternionRotationMatrix( const Matrix44& matrix )
{
    Vector4 out;
    out.w = (float)sqrt(1.0f + matrix._11 + matrix._22 + matrix._33) / 2.0f;
    float w4 = (4.0f * out.w);
    out.x = (matrix._32 - matrix._23) / w4 ;
    out.y = (matrix._13 - matrix._31) / w4 ;
    out.z = (matrix._21 - matrix._12) / w4 ;
    return out;
}

Vector4 quaternionRotationAxis( Vector3& axis, float angle )
{
    Vector4 out;
    out.w  = cosf( angle / 2 );
    out.x = axis.x * sinf( angle / 2 );
    out.y = axis.y * sinf( angle / 2 );
    out.z = axis.z * sinf( angle / 2 );
    return out;
}

Vector4 quaternionRotationEuler( float yaw, float pitch, float roll )
{
    Vector4 out;
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

Vector3 eulerRotationMatrix( Matrix44& matrix )
{
    Vector3 out;
    if (matrix._21 > 0.998)
    {
        out.x = atan2(matrix._13, matrix._33);
        out.z = (float)(OvglPi/2);
        out.y = 0;
        return out;
    }
    if (matrix._21 < -0.998)
    {
        out.x = atan2(matrix._13, matrix._33);
        out.z = (float)(-OvglPi/2);
        out.y = 0;
        return out;
    }
    out.x = atan2( -matrix._31, matrix._11 );
    out.y = atan2( -matrix._23, matrix._22 );
    out.z = asin( matrix._21 );
    return out;
}

float& Vector4::operator [] ( size_t index )
{
    return (&x)[index];
}

Vector3& Matrix33::operator [] ( size_t index ) const
{
    return (Vector3&)*((Vector3*)((&_11) + (index * 3)));
}

Vector4& Matrix44::operator [] ( size_t index ) const
{
    return (Vector4&)*((Vector4*)((&_11) + (index * 4)));
}

void Vector3::toDoubles( double* data )
{
    data[0] = (double)x;
    data[1] = (double)y;
    data[2] = (double)z;
}

void Vector3::fromDoubles( double* data )
{
    x = (float)data[0];
    y = (float)data[1];
    z = (float)data[2];
}

float length( const Vector3& vector )
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
}

float distance( const Vector3& vector1, const Vector3& vector2 )
{
    Vector3 vector = vector1 - vector2;
    float out = length( vector );
	return out;
}

float vector3Dot( const Vector3& vec1, const Vector3& vec2 )
{
    return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

Vector3 vector3Cross( const Vector3& vec1, const Vector3& vec2 )
{
    Vector3 out;
    out.x = vec1.y * vec2.z - vec2.y * vec1.z;
    out.y = vec1.z * vec2.x - vec2.z * vec1.x;
    out.z = vec1.x * vec2.y - vec2.x * vec1.y;
    return out;
}

Vector3 vector3Transform( const Vector3& vector, const Matrix44& matrix )
{
    Vector3 out;
    out.x = vector.x * matrix._11 + vector.y * matrix._21 + vector.z * matrix._31 + matrix._41;
    out.y = vector.x * matrix._12 + vector.y * matrix._22 + vector.z * matrix._32 + matrix._42;
    out.z = vector.x * matrix._13 + vector.y * matrix._23 + vector.z * matrix._33 + matrix._43;
    return out;
}

Vector3 vector3Normalize( const Vector3& vector )
{
    Vector3 out;
    float l = length( vector );
    out.x = vector.x / l;
    out.y = vector.y / l;
    out.z = vector.z / l;
    return out;
}

Vector3 vector3Center( std::vector< Vector3 >& vertices )
{
    Vector3 out;
    for( uint32_t v = 0; v < vertices.size(); v++ )
    {
        out = out + vertices[v];
    }
    out.x = out.x / vertices.size();
    out.y = out.y / vertices.size();
    out.z = out.z / vertices.size();
    return out;
}

Vector4 vector4Lerp( Vector4& vec1, Vector4& vec2, float u )
{
    return (vec1 * (1 - u)) + (vec2 * u);
}

float round( float expression, int32_t numdecimalplaces )
{
    return floorf( expression * pow(10.0f, numdecimalplaces ) ) / pow( 10.0f, numdecimalplaces );
}

float lerp( float val1, float val2, float u )
{
    return ( val1 * (1 - u) ) + ( val2 * u );
}

Vector4 slerp( const Vector4& q1, const Vector4& q2, float t )
{

    Vector4 qm;

    Vector4 qt = q2;

    float cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

    if( cosHalfTheta < 0 )
    {
        qt.w = -qt.w; qt.x = -qt.x; qt.y = -qt.y; qt.z = qt.z;
        cosHalfTheta = -cosHalfTheta;
    }


    if( abs(cosHalfTheta) >= 1.0f )
	{
        qm.w = q1.w;qm.x = q1.x;qm.y = q1.y;qm.z = q1.z;
        return qm;
    }

    float halfTheta = acos(cosHalfTheta);
    float sinHalfTheta = sqrt(1.0f - cosHalfTheta*cosHalfTheta);

    if( fabs(sinHalfTheta) < 0.001 )
    {
        qm.w = (q1.w * 0.5f + qt.w * 0.5f);
        qm.x = (q1.x * 0.5f + qt.x * 0.5f);
        qm.y = (q1.y * 0.5f + qt.y * 0.5f);
        qm.z = (q1.z * 0.5f + qt.z * 0.5f);
        return qm;
    }
    float ratioA = sinf((1.0f - t) * halfTheta) / sinHalfTheta;
    float ratioB = sinf(t * halfTheta) / sinHalfTheta;

    qm.w = ( q1.w * ratioA + qt.w * ratioB );
    qm.x = ( q1.x * ratioA + qt.x * ratioB );
    qm.y = ( q1.y * ratioA + qt.y * ratioB );
    qm.z = ( q1.z * ratioA + qt.z * ratioB );

    return qm;
}

float degToRad( float degree )
{
    return degree * ( (float)OvglPi ) / 180.0f;
}

float volumeTetrahedron( const Vector3& vector1, const Vector3& vector2, const Vector3& vector3, const Vector3& vector4 )
{
    float width = distance( vector1, vector2 );
    float height = distance( vector1, vector3 );
    float depth = distance( vector1, vector4 );
    return ( width * height * depth ) / 3.0f;
}

void vector3Box( std::vector< Ovgl::Vector3 >& vectors, Ovgl::Vector3* min, Ovgl::Vector3* max )
{
    for( uint32_t i = 0; i < vectors.size(); i++ )
    {
        if( vectors[i].x < min->x )
        {
            min->x = vectors[i].x;
        }
        if( vectors[i].y < min->y )
        {
            min->y = vectors[i].y;
        }
        if( vectors[i].z < min->z )
        {
            min->z = vectors[i].z;
        }
        if( vectors[i].x > max->x )
        {
            max->x = vectors[i].x;
        }
        if( vectors[i].y > max->y )
        {
            max->y = vectors[i].y;
        }
        if( vectors[i].z > max->z )
        {
            max->z = vectors[i].z;
        }
    }
}
}
