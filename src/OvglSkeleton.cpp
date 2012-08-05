/**
* @file OvglSkeleton.cpp
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

#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglMesh.h"
#include "OvglScene.h"
#include "OvglWindow.h"
#include "OvglSkeleton.h"

namespace Ovgl
{
Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
}

void Pose::Evaluate( Animation* anim, float pTime )
{
    float time = 0.0f;
    time = pTime;

    for( unsigned int a = 0; a < anim->channels.size(); a++)
    {
        Ovgl::Channel* channel = &anim->channels[a];

        Vector3 presentPosition( 0, 0, 0);
        if( channel->position_keys.size() > 0)
        {
            unsigned int frame = 0;
            while( frame < channel->position_keys.size() - 1)
            {
                if( time < channel->position_keys[frame+1].time)
                {
                    break;
                }
                frame++;
            }

            unsigned int nextFrame = (frame + 1) % channel->position_keys.size();

            VectorKey key = channel->position_keys[frame];
            VectorKey nextKey = channel->position_keys[nextFrame];
            double diffTime = nextKey.time - key.time;
            if( diffTime > 0)
            {
                float factor = float( (time - key.time) / diffTime);
                presentPosition = key.value + (nextKey.value - key.value) * factor;
            }
            else
            {
                presentPosition = key.value;
            }
        }

        Vector4 presentRotation( 0, 0, 0, 1 );
        if( channel->rotation_keys.size() > 0)
        {
            unsigned int frame = 0;
            while( frame < channel->rotation_keys.size()  - 1)
            {
                if( time < channel->rotation_keys[frame+1].time)
                    break;
                frame++;
            }

            unsigned int nextFrame = (frame + 1) % channel->rotation_keys.size();

            QuatKey key = channel->rotation_keys[frame];
            QuatKey nextKey = channel->rotation_keys[nextFrame];
            double diffTime = nextKey.time - key.time;
            if( diffTime > 0)
            {
                float factor = float( (time - key.time) / diffTime);
                presentRotation = Slerp( key.value, nextKey.value, factor);
            }
            else
            {
                presentRotation = key.value;
            }
        }

        Vector3 presentScaling( 1, 1, 1);
        if( channel->scaling_keys.size() > 0)
        {
            unsigned int frame = 0;
            while( frame < channel->scaling_keys.size() - 1)
            {
                if( time < channel->scaling_keys[frame+1].time)
                    break;
                frame++;
            }

            presentScaling = channel->scaling_keys[frame].value;
        }

        Matrix44 mat = Ovgl::MatrixRotationQuaternion( presentRotation );

        mat._11 *= presentScaling.x; mat._12 *= presentScaling.x; mat._13 *= presentScaling.x;
        mat._21 *= presentScaling.y; mat._22 *= presentScaling.y; mat._23 *= presentScaling.y;
        mat._31 *= presentScaling.z; mat._32 *= presentScaling.z; mat._33 *= presentScaling.z;
        mat._41 = presentPosition.x; mat._42 = presentPosition.y; mat._43 = presentPosition.z;

        joints[channel->index]->local_transform = mat;
    }
}

void Pose::animate( Animation* anim, float time )
{
    Evaluate( anim, time );
    UpdateTransforms( root_joint );
    for(uint32_t b = 0; b < matrices.size(); b++)
    {
        matrices[b] = joints[b]->global_transform;
        matrices[b] = MatrixInverse( Vector4(), joints[b]->offset) * matrices[b];
    }
}

void Pose::UpdateTransforms(Joint* pNode)
{
    pNode->global_transform = pNode->local_transform;
    Joint* parent = pNode->parent;
    while( parent )
    {
        pNode->global_transform = pNode->global_transform * parent->local_transform;
        parent  = parent->parent;
    }

    for( uint32_t i = 0; i < pNode->children.size(); i++)
    {
        UpdateTransforms( pNode->children[i]);
    }
}
}
