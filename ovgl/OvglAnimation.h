/**
* @file OvglAnimation.h
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
	class Matrix44;

	extern "C"
	{
		class cBone {
		public:

			std::string Name;
			Matrix44 Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
			cBone* Parent;
			std::vector<cBone*> Children;

			cBone() :Parent(0){}
			~cBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
		};


 
		class SceneAnimator
		{
		public:

			SceneAnimator(): Skeleton(0), CurrentAnimIndex(-1) {}
			~SceneAnimator(){ Release(); }

			void Init(const aiScene* pScene, Mesh* mesh1);
			void Release();

			Ovgl::Mesh* mesh;

			int32_t CurrentAnimIndex;

			cBone* Skeleton;
			std::map<std::string, cBone*> BonesByName;
			std::map<std::string, uint32_t> AnimationNameToId;
			std::vector<cBone*> Bones;
			std::vector<Matrix44> Transforms;

			void UpdateTransforms(cBone* pNode);
			void Calculate( float pTime);

			void CalcBoneMatrices();

			void ExtractAnimations(const aiScene* pScene);
			cBone* CreateBoneTree( aiNode* pNode, cBone* pParent);

			void Evaluate( float pTime, std::vector<cBone*>& bones);
	
		};
	}
}