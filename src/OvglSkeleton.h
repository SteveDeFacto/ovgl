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
	class Instance;
	class Scene;
	class Matrix44;
	class Vector3;

	extern "C"
	{
		class __declspec(dllexport) VectorKey
		{
		public:
			double time;
			Vector3 value;
		};

		class __declspec(dllexport) QuatKey
		{
		public:
			double time;
			Vector4 value;
		};

		class __declspec(dllexport) Channel
		{
		public:
			uint32_t index;
			std::vector< VectorKey >						position_keys;
			std::vector< QuatKey >							rotation_keys;
			std::vector< VectorKey	>						scaling_keys;
		};

		class __declspec(dllexport) Animation
		{
		public:
			std::vector< Channel >							channels;
			double											start_time;
			double											end_time;
		};

		class __declspec(dllexport) AnimationInstance
		{
		public:
			Animation*										animation;
			uint32_t										animation_state;
			double											current_time;
			double											start_time;
			double											end_time;
			double											step_time;
			void play( float speed, bool repeat );
			void stop();
			void pause();
			void set( float time );
		};

		class __declspec(dllexport) Bone
		{
		public:
			uint32_t										index;
			btConvexHullShape*								convex;
			Mesh*											mesh;
			Matrix44										matrix;
			Matrix44										local_transform;
			Matrix44 										global_transform;
			float											length;
			float											volume;
			Vector3											min;
			Vector3											max;
			Bone*											parent;
			std::vector< Bone* >							children;
		};

		class __declspec(dllexport) Joint
		{
		public:
			Joint*											parent;
			std::vector< Joint* >							children;
			Matrix44										offset;
			Matrix44										local_transform;
			Matrix44 										global_transform;
		};	
		
		class __declspec(dllexport) Pose
		{
		public:
			std::vector< Matrix44 >							matrices;
			std::vector< Joint* >							joints;
			Joint*											root_joint;
			void animate( Animation* anim, float time );
			void UpdateTransforms( Joint* pNode );
			void Evaluate( Animation* anim, float pTime );
		};

		class __declspec(dllexport) Skeleton
		{
		public:
			Skeleton();
			~Skeleton();
			std::vector< Bone* >							bones;
			Bone*											root_bone;
			std::vector< Animation >						animations;
			void generate_bone_shapes();
		};
	}
}