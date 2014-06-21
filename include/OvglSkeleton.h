/**
 * @file OvglSkeleton.h
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

#include "OvglCommon.h"

// Forward declare external classes
class btRigidBody;
class btConvexHullShape;

namespace Ovgl
{
class Vertex;
class Face;
class Bone;
class Joint;
class Key;
class Animation;
class CMesh;
class Mesh;
class Context;
class Scene;
class Matrix44;
class Vector3;

extern "C"
{
	class DLLEXPORT VectorKey
	{
		public:
			double                                          time;
			Vector3                                         value;
	};

	class DLLEXPORT QuatKey
	{
		public:
			double                                          time;
			Vector4                                         value;
	};

	class DLLEXPORT Channel
	{
		public:
			uint32_t index;
			std::vector< VectorKey >						positionKeys;
			std::vector< QuatKey >							rotationKeys;
			std::vector< VectorKey	>						scalingKeys;
	};

	class DLLEXPORT Animation
	{
		public:
			std::vector< Channel >							channels;
			double											startTime;
			double											endTime;
	};

	class DLLEXPORT AnimationInstance
	{
		public:
			Animation*										animation;
			uint32_t										animationState;
			double											currentTime;
			double											startTime;
			double											endTime;
			double											stepTime;
			void play( float speed, bool repeat );
			void stop();
			void pause();
			void set( float time );
	};

	class DLLEXPORT Bone
	{
		public:
			uint32_t										index;
			btConvexHullShape*								convex;
			Mesh*											mesh;
			Matrix44										matrix;
			Matrix44										localTransform;
			Matrix44 										globalTransform;
			float											length;
			float											volume;
			Vector3											min;
			Vector3											max;
			Bone*											parent;
			std::vector< Bone* >							children;
	};

	class DLLEXPORT Joint
	{
		public:
			Joint*											parent;
			std::vector< Joint* >							children;
			Matrix44										offset;
			Matrix44										localTransform;
			Matrix44 										globalTransform;
	};	

	class DLLEXPORT Pose
	{
		public:
			std::vector< Matrix44 >							matrices;
			std::vector< Joint* >							joints;
			Joint*											rootJoint;
			void animate( Animation* anim, float time );
			void updateTransforms( Joint* pNode );
			void evaluate( Animation* anim, float pTime );
	};

	class DLLEXPORT Skeleton
	{
		public:
			Skeleton();
			~Skeleton();
			std::vector< Bone* >							bones;
			Bone*											rootBone;
			std::vector< Animation >						animations;
			void generateBoneShapes();
	};
}
}
