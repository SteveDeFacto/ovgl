/**
* @file OvglMesh.h
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

namespace Ovgl
{
	enum Clean { CLEAN_ALL, CLEAN_STRAY_VERTICES, CLEAN_CLOSE_VERTICES, CLEAN_BROKEN_FACES };

	class Vertex;
	class Face;
	class Bone;
	class Joint;
	class Curve;
	class Animation;
	class CMesh;
	class Mesh;
	class Instance;
	class Scene;
	class Matrix44;
	class PVS;

	extern "C"
	{
		/**
		* Vertices represent points in a mesh that faces connect to form polygons.
		* @brief Mesh vertex class.
		*/
		class __declspec(dllexport) Vertex
		{
		public:
			Ovgl::Vector3 position;
			Ovgl::Vector3 normal;
			Ovgl::Vector2 texture;
			Ovgl::Vector4 weight;
			Ovgl::Vector4 indices;
			bool operator == (const Vertex&) const;
			bool operator != (const Vertex&) const;
		};
		/**
		* Faces are a list of 3 vertex indices that make up a triangle.
		* @brief Mesh face class.
		*/
		class __declspec(dllexport) Face
		{
		public:
			DWORD indices[3];
			Face Flip();
		};

		class __declspec(dllexport) Curve
		{
		public:
			DWORD time;
			float value;
		};

		class __declspec(dllexport) Animation
		{
		public:
			DWORD								animationState;
			float								currentTime;
			float								startTime;
			float								endTime;
			float								stepTime;
			void play( float speed, bool repeat );
			void stop();
			void pause();
			void set( float time );
		};

		class __declspec(dllexport) Bone
		{
		public:
			btConvexHullShape*					convex;
			Mesh*								mesh;
			Matrix44							matrix;
			float								length;
			float								volume;
			Ovgl::Vector3						min;
			Ovgl::Vector3						max;
			DWORD								parent;
			std::vector< std::vector< Curve > >	Rot_Keys;
			std::vector< DWORD >				childen;
		};

		class __declspec(dllexport) PVS
		{
		public:
			Mesh*								Mesh;
			std::vector< PVS* >					PVSets;
		};

		class __declspec(dllexport) CMesh
		{
		public:
			Scene* scene;
			btRigidBody* actor;
			void setPose( Matrix44* matrix );
			Matrix44 getPose();
			void SetFlags( DWORD flags );
			DWORD GetFlags();
			void Release();
		};

		class __declspec(dllexport) Mesh
		{
		public:
			MediaLibrary*						ml;
			std::vector< Vertex >				vertices;
			std::vector< Face >					faces;
			std::vector< DWORD >				attributes;
			std::vector< Bone* >				bones;
			std::vector< Material* >			materials;
			DWORD								subset_count;
			DWORD								root_bone;
			unsigned int						VertexBuffer;
			unsigned int*						IndexBuffers;
			btBvhTriangleMeshShape*				TriangleMesh;
			std::vector< PVS* >					PVSCache;
			void Save( const std::string& file );
			void GenerateVertexNormals();
			void CubeCloud( float sx, float sy, float sz, int count );
			float QuickHull();
			void Simplify( DWORD max_faces, DWORD max_vertices );
			void Clean( float min, DWORD flags  );
			void ConnectVertex( std::vector< DWORD >& faceList, DWORD vertex );
			void MergeVerices( std::vector< DWORD >& vertexList, DWORD flag );
			Ovgl::Vector3 ComputeFaceNormal( DWORD face );
			void Update();
			void Release();
		};
	}
}