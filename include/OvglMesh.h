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
 * @brief This header defines all mesh functions and classes used in Ovgl.
 */

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

// Forward declare external classes
class btRigidBody;
class btBvhTriangleMeshShape;

namespace Ovgl
{
enum Clean { CLEAN_ALL, CLEAN_STRAY_VERTICES, CLEAN_CLOSE_VERTICES, CLEAN_BROKEN_FACES };

class Vertex;
class Face;
class Bone;
class Joint;
class Key;
class Animation;
class CMesh;
class Mesh;
class Matrix44;
class Skeleton;

extern "C"
{
	/**
	 * Vertices represent points in a mesh that faces connect to form polygons.
	 * @brief Mesh vertex class.
	 */
	class DLLEXPORT Vertex
	{
		public:
			Ovgl::Vector3                       position;
			Ovgl::Vector3                       normal;
			Ovgl::Vector2                       texture;
			Ovgl::Vector4                       weight;
			Ovgl::Vector4                       indices;
			bool operator == (const Vertex&) const;
			bool operator != (const Vertex&) const;
	};
	/**
	 * Faces are a list of 3 vertex indices that make up a triangle.
	 * @brief Mesh face class.
	 */
	class DLLEXPORT Face
	{
		public:
			uint32_t indices[3];
			Face flip();
	};

	class DLLEXPORT CMesh
	{
		public:
			CMesh();
			~CMesh();
			Scene*                              scene;
			btRigidBody*                        actor;
			void setPose( const Matrix44& matrix );
			Matrix44 getPose();
			void setFlags( uint32_t flags );
			uint32_t getFlags();
	};

	class DLLEXPORT Mesh
	{
		public:
			Mesh();
			~Mesh();
			ResourceManager*                    mediaLibrary;
			std::vector< Vertex >               vertices;
			std::vector< Face >                 faces;
			std::vector< uint32_t >             attributes;
			uint32_t                            subsetCount;
			uint32_t                            vertexBuffer;
			uint32_t*                           indexBuffers;
			btBvhTriangleMeshShape*             triangleMesh;
			Skeleton*                           skeleton;
			void generateVertexNormals();
			void cubeCloud( float sx, float sy, float sz, int32_t count );
			float quickHull();
			void simplify( uint32_t maxFaces, uint32_t maxVertices );
			void clean( float min, uint32_t flags  );
			void connectVertex( std::vector< uint32_t >& faceList, uint32_t vertex );
			void mergeVerices( std::vector< uint32_t >& vertexList, uint32_t flag );
			Ovgl::Vector3 computeFaceNormal( uint32_t face );
			void update();
	};
}
}
