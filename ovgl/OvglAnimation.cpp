/**
* @file OvglAnimation.cpp
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
* @brief This file contains classes which handle animation.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMesh.h"
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglWindow.h"
#include "OvglAnimation.h"
#include "OvglSkeleton.h"

namespace Ovgl
{

	void SceneAnimator::Evaluate( float pTime, std::vector<cBone*>& bones)
	{
		float time = 0.0f;
		time = pTime;

		for( unsigned int a = 0; a < mesh->skeleton->animations[0].channels.size(); a++)
		{
			Ovgl::Channel* channel = &mesh->skeleton->animations[0].channels[a];

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
	
				PositionKey key = channel->position_keys[frame];
				PositionKey nextKey = channel->position_keys[nextFrame];
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

				RotationKey key = channel->rotation_keys[frame];
				RotationKey nextKey = channel->rotation_keys[nextFrame];
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
		
			bones[channel->index]->LocalTransform = mat;
			
		}

	}
	
	void SceneAnimator::Release()
	{
		CurrentAnimIndex = -1;
		delete Skeleton;
		Skeleton = NULL;
	}
	void SceneAnimator::Init(const aiScene* pScene, Mesh* mesh1)
	{
		this->mesh = mesh1;
		if(!pScene->HasAnimations())
		{
			return;
		}
		Release();
		Skeleton = CreateBoneTree( pScene->mRootNode, NULL);
		CurrentAnimIndex=0;
	
		for (unsigned int i = 0; i < pScene->mNumMeshes;++i)
		{
			const aiMesh* mesh = pScene->mMeshes[i];
		
			for (unsigned int n = 0; n < mesh->mNumBones;++n)
			{
				const aiBone* bone = mesh->mBones[n];
				std::map<std::string, cBone*>::iterator found = BonesByName.find(bone->mName.data);
				if(found != BonesByName.end())
				{
					bool skip = false;
					for(size_t j(0); j< Bones.size(); j++)
					{
						std::string bname = bone->mName.data;
						if(Bones[j]->Name == bname)
						{
							skip = true;
							break;
						}
					}
					if(!skip)
					{
						std::string tes = found->second->Name;
						found->second->Offset = *(Matrix44*)&bone->mOffsetMatrix;
						found->second->Offset = Ovgl::MatrixTranspose(found->second->Offset);
						Bones.push_back(found->second);
					}
				} 
			}
		}
		Transforms.resize( Bones.size());
	}

	void SceneAnimator::Calculate(float pTime)
	{
		Evaluate( pTime, Bones);
		UpdateTransforms(Skeleton);
	}

	cBone* SceneAnimator::CreateBoneTree( aiNode* pNode, cBone* pParent)
	{
		cBone* internalNode = new cBone();
		internalNode->Name = pNode->mName.data;
		internalNode->Parent = pParent;
		BonesByName[internalNode->Name] = internalNode;
		internalNode->LocalTransform = *(Matrix44*)&pNode->mTransformation;
		internalNode->LocalTransform = Ovgl::MatrixTranspose(internalNode->LocalTransform);
		internalNode->OriginalLocalTransform = internalNode->LocalTransform;
		internalNode->GlobalTransform = internalNode->LocalTransform;
		cBone* parent = internalNode->Parent;
		while( parent )
		{
			internalNode->GlobalTransform = internalNode->GlobalTransform * parent->LocalTransform;
			parent  = parent->Parent;
		}

		for( unsigned int a = 0; a < pNode->mNumChildren; a++)
		{
			internalNode->Children.push_back(CreateBoneTree( pNode->mChildren[a], internalNode));
		}
		return internalNode;
	}

	void SceneAnimator::UpdateTransforms(cBone* pNode)
	{
		pNode->GlobalTransform = pNode->LocalTransform;
		cBone* parent = pNode->Parent;
		while( parent )
		{
			pNode->GlobalTransform = pNode->GlobalTransform * parent->LocalTransform;
			parent  = parent->Parent;
		}

		for( std::vector<cBone*>::iterator it = pNode->Children.begin(); it != pNode->Children.end(); ++it)
			UpdateTransforms( *it);
	}
}