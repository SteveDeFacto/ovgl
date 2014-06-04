/**
 * @file OvglMesh.cpp
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
 * @brief This part of the library deals with the storage and manipulation of 3D geometry.
 */

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglMesh.h"
#include "OvglResource.h"
#include "OvglScene.h"
#include "OvglSkeleton.h"
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

namespace Ovgl
{
bool Vertex::operator == ( const Vertex& in ) const
{
	return memcmp(this, &in, sizeof(Vertex)) == 0;
}

bool Vertex::operator != ( const Vertex& in ) const
{
	return memcmp(this, &in, sizeof(Vertex)) != 0;
}

Face Face::flip()
{
	Face out = {0};
	out.indices[0] = indices[2];
	out.indices[1] = indices[1];
	out.indices[2] = indices[0];
	return out;
}

void Mesh::generateVertexNormals()
{
	for(uint32_t v = 0; v < vertices.size(); v++)
	{
		std::vector< Vector3 > adjFaceNormals;
		for( uint32_t f = 0; f < faces.size(); f++)
		{
			for(uint32_t i = 0; i < 3; i++)
			{
				if( faces[f].indices[i] == v )
				{
					adjFaceNormals.push_back( computeFaceNormal(f) );
				}
			}
		}
		vertices[v].normal = vector3Normalize( vector3Center( adjFaceNormals ) );
	}
}

void Mesh::update()
{
	SDL_GL_MakeCurrent(mediaLibrary->context->contextWindow, mediaLibrary->context->glContext);

	// Release buffers.
	if( vertexBuffer ) glDeleteBuffers(1, &vertexBuffer);
	if( indexBuffers )
	{
		for(uint32_t i = 0; i < subsetCount; i++)
			if( indexBuffers[i] )
				glDeleteBuffers(1, &indexBuffers[i]);
		delete [] indexBuffers;
	}
	for( uint32_t i = 0; i < skeleton->bones.size(); i++ )
	{
		if(skeleton->bones[i]->convex)
		{
			delete skeleton->bones[i]->convex;
		}
	}

	glGenBuffers( 1, &vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Create index buffers.
	std::set<uint32_t> usedAttributes(attributes.begin(), attributes.end());
	std::vector< std::vector< Face > > indexSubsets;
	indexSubsets.resize(usedAttributes.size());
	for( uint32_t i = 0; i < attributes.size(); i++ )
	{
		uint32_t s = 0;
		for( std::set<uint32_t>::iterator j = usedAttributes.begin(); j != usedAttributes.end(); ++j)
		{
			if( attributes[i] == *j )
			{
				indexSubsets[s].push_back(faces[i]);
			}
			s++;
		}
	}

	// Get subset count.
	subsetCount = indexSubsets.size();

	indexBuffers = new uint32_t[subsetCount];
	for( uint32_t i = 0; i < subsetCount; i++ )
	{
		glGenBuffers( 1, &indexBuffers[i] );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffers[i] );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSubsets[i].size()*sizeof(Face), &indexSubsets[i][0], GL_STATIC_DRAW );
	}
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// Create triangle mesh.
	btTriangleMesh* trimesh = new btTriangleMesh();
	for( uint32_t t = 0; t < faces.size(); t++ )
	{
		btVector3* v0 = (btVector3*)&vertices[faces[t].indices[0]].position;
		btVector3* v1 = (btVector3*)&vertices[faces[t].indices[1]].position;
		btVector3* v2 = (btVector3*)&vertices[faces[t].indices[2]].position;
		trimesh->addTriangle( *v0, *v1, *v2 );
	}
	if( trimesh->getNumTriangles() )
	{
		triangleMesh = new btBvhTriangleMeshShape(trimesh, false);
		triangleMesh->setMargin(0.1f);
	}
	else
	{
		delete trimesh;
	}

	// Create bone shapes.
	for( uint32_t i = 0; i < skeleton->bones.size(); i++ )
	{
		if(skeleton->bones[i]->mesh->vertices.size() > 0)
		{
			skeleton->bones[i]->mesh->clean( 0.001f, CLEAN_CLOSE_VERTICES );
			skeleton->bones[i]->volume = skeleton->bones[i]->mesh->quickHull();
			skeleton->bones[i]->mesh->simplify(100, 0);
			skeleton->bones[i]->convex = new btConvexHullShape((float*)&skeleton->bones[i]->mesh->vertices[0], skeleton->bones[i]->mesh->vertices.size(), sizeof(Vertex));
		}
	}

	SDL_GL_MakeCurrent( 0, 0 );
}

Mesh::Mesh()
{
	triangleMesh = NULL;
}

Mesh::~Mesh()
{
	delete triangleMesh;
	for( uint32_t m = 0; m < mediaLibrary->meshes.size(); m++ )
	{
		if( mediaLibrary->meshes[m] == this )
		{
			mediaLibrary->meshes.erase( mediaLibrary->meshes.begin() + m );
		}
	}
	glDeleteBuffers( 1, &vertexBuffer );
	for( uint32_t i = 0; i < subsetCount; i++)
	{
		glDeleteBuffers( 1, &indexBuffers[i] );
	}
}

void CMesh::setPose( const Matrix44& matrix )
{
	actor->getWorldTransform().setFromOpenGLMatrix((float*)&matrix);
}

Matrix44 CMesh::getPose()
{
	Matrix44 matrix;
	actor->getWorldTransform().getOpenGLMatrix((float*)&matrix);
	return matrix;
}

void CMesh::setFlags( uint32_t flags )
{

}

uint32_t CMesh::getFlags()
{
	uint32_t flags = 0;
	return flags;
}

CMesh::CMesh()
{

}

CMesh::~CMesh()
{
	scene->dynamicsWorld->removeCollisionObject(actor);
	delete actor;
}

void Mesh::cubeCloud( float sx, float sy, float sz, int32_t count )
{
	for( int32_t i = 0; i < count; i++)
	{
		Vertex tempVertex;
		tempVertex.position.x = ((((float)rand() / RAND_MAX) * sx) * 2) - sx;
		tempVertex.position.y = ((((float)rand() / RAND_MAX) * sy) * 2) - sy;
		tempVertex.position.z = ((((float)rand() / RAND_MAX) * sz) * 2) - sz;
		tempVertex.weight[0] = 1.0f;
		vertices.push_back( tempVertex );
	}
}

float Mesh::quickHull()
{
	float volume = 0;

	// Generate a starting face that is at the farthest points. ( Right now it is just adding the first three points. )
	Face firstFace = {0};
	firstFace.indices[0] = 0;
	firstFace.indices[1] = 1;
	firstFace.indices[2] = 2;

	// Add both the front and back sides of the face to the mesh.
	faces.push_back( firstFace );
	faces.push_back( firstFace.flip() );

	// Start looping through the faces to create the convex mesh.
	uint32_t f = 0;
	while( (f < faces.size()) && (f < (vertices.size() * 2) - 4) )
	{
		bool foundVertex = false;
		uint32_t distantVertex = 0;
		Vector3 faceNormal = computeFaceNormal( f );
		Vector3 facePosition = vertices[faces[f].indices[0]].position;

		// Find the most distance vertex from this face.
		for( uint32_t v = 0; v < vertices.size(); v++ )
		{
			if( vector3Dot( faceNormal, (vertices[v].position - facePosition)) > vector3Dot( faceNormal, (vertices[distantVertex].position - facePosition)) && vector3Dot( faceNormal, (vertices[v].position - facePosition)) > 0.001f )
			{
				if( faces[f].indices[0] != v && faces[f].indices[1] != v && faces[f].indices[2] != v )
				{
					distantVertex = v;
					foundVertex = true;
				}
			}
		}
		if( foundVertex )
		{

			// If we do find a vertex then we need to find all faces that this vertex lands on.
			std::vector< uint32_t > TaggedFaces;
			for( uint32_t f2 = 0; f2 < faces.size(); f2++ )
			{
				Vector3 faceNormal2 = computeFaceNormal( f2 );
				if( vector3Dot( faceNormal2, (vertices[distantVertex].position - vertices[faces[f2].indices[0]].position)) > 0.0f )
				{
					TaggedFaces.push_back( f2 );
				}
			}

			for( uint32_t f2 = 0; f2 < TaggedFaces.size(); f2++ )
			{
				volume = volume + volumeTetrahedron(vertices[distantVertex].position, vertices[faces[TaggedFaces[f2]].indices[0]].position, vertices[faces[TaggedFaces[f2]].indices[1]].position, vertices[faces[TaggedFaces[f2]].indices[2]].position);
			}

			// Now that we know which faces this vertex lands on, we need to remap the mesh to include the new vertex.
			connectVertex( TaggedFaces, distantVertex);
		}
		else
		{
			// If we didn't find any vertices that land on this face then move to the next face.
			f++;
		}
	}

	// Clean the mesh from stray vertices.
	this->clean( 0.0f, CLEAN_ALL );
	return volume;
}

void Mesh::connectVertex( std::vector< uint32_t >& faceList, uint32_t vertex)
{
	std::vector< Face > newFaces;

	// Find which faces share vertices with the faces we want to remap.
	for( uint32_t f2 = 0; f2 < faces.size(); f2++ )
	{
		for( uint32_t tf = 0; tf < faceList.size(); tf++ )
		{
			std::vector< uint32_t > SharedVertices;
			for( uint32_t i = 0; i < 3; i++ )
			{
				for( uint32_t i2 = 0; i2 < 3; i2++ )
				{
					if( faces[f2].indices[i2] == faces[faceList[tf]].indices[i] )
					{
						if( std::find(faceList.begin(), faceList.end(), f2) == faceList.end() )
						{
							SharedVertices.push_back(faces[f2].indices[i2]);
						}
					}
				}
			}

			//If the face shares more than 1 vertex then connect the edges to the vertex. We are using some tricks to reduce redundancy here.
			uint32_t sz = SharedVertices.size();
			if( sz > 1)
			{
				for(uint32_t sv = 0; sv < sz - (uint32_t)(sz == 2); sv++)
				{
					//Order the new face based on the face that shares vertices with the faceList.
					if( (faces[f2].indices[0] == SharedVertices[ sv % sz ] && faces[f2].indices[1] == SharedVertices[ (sv + 1) % sz ]) ||
							(faces[f2].indices[1] == SharedVertices[ sv % sz ] && faces[f2].indices[2] == SharedVertices[ (sv + 1) % sz ]) ||
							(faces[f2].indices[2] == SharedVertices[ sv % sz ] && faces[f2].indices[0] == SharedVertices[ (sv + 1) % sz ]) )
					{
						Face tempFace;
						tempFace.indices[0] = SharedVertices[(sv + 1) % sz];
						tempFace.indices[1] = SharedVertices[sv % sz];
						tempFace.indices[2] = vertex;
						newFaces.push_back( tempFace );
					}
					else if( ( faces[f2].indices[2] == SharedVertices[ sv % sz ] && faces[f2].indices[1] == SharedVertices[(sv + 1) % sz]) ||
							( faces[f2].indices[1] == SharedVertices[ sv % sz ] && faces[f2].indices[0] == SharedVertices[(sv + 1) % sz]) ||
							( faces[f2].indices[0] == SharedVertices[ sv % sz ] && faces[f2].indices[2] == SharedVertices[(sv + 1) % sz]) )
					{
						Face tempFace;
						tempFace.indices[0] = SharedVertices[sv % sz];
						tempFace.indices[1] = SharedVertices[(sv + 1) % sz];
						tempFace.indices[2] = vertex;
						newFaces.push_back( tempFace );
					}
				}
			}
		}
	}

	// Delete faces from faceList.
	for( uint32_t f = 0; f < faceList.size(); f++ )
	{
		faces.erase( faces.begin() + faceList[f] );
		for( uint32_t f2 = f; f2 < faceList.size(); f2++ )
		{
			if( faceList[f2] > faceList[f] )
			{
				faceList[f2] = faceList[f2] - 1;
			}
		}
	}

	// Add the new faces.
	faces.insert(faces.end(), newFaces.begin(), newFaces.end());
}


void Mesh::mergeVerices( std::vector< uint32_t >& vertexList, uint32_t flag )
{
	// Move all vertices to last vertex;
	if( flag == 0 )
	{
		for( uint32_t v = 0; v < vertexList.size() - 1; v++)
		{
			vertices[vertexList[v]].position = vertices[vertexList[vertexList.size() - 1]].position;
		}
	}
	// Clean the mesh.
	clean( 0.001f, CLEAN_ALL );
}

void Mesh::simplify( uint32_t maxFaces, uint32_t maxVertices )
{
	while(  (faces.size() > maxFaces && maxFaces != 0) || (vertices.size() > maxVertices && maxVertices != 0) )
	{
		// Compute vertex weights.
		uint32_t leastAngleVertexIndex = 0;
		float leastAngleVertexWeight = FLT_MAX;
		std::vector< std::vector< uint32_t > > adjVertexFaces(vertices.size());
		for( uint32_t v = 0; v < vertices.size(); v++)
		{
			// Get list of adjacent faces
			std::vector< Vector3 > adjFaceNormals;
			for( uint32_t f = 0; f < faces.size(); f++)
			{
				for(uint32_t i = 0; i < 3; i++)
				{
					if( faces[f].indices[i] == v )
					{
						adjFaceNormals.push_back( computeFaceNormal(f) );
						adjVertexFaces[v].push_back( f );
					}
				}
			}
			float farthestDistance = 0;
			for( uint32_t fn1 = 0; fn1 < adjFaceNormals.size(); fn1++)
			{
				for( uint32_t fn2 = 0; fn2 < adjFaceNormals.size(); fn2++)
				{
					float checkDistance = distance( adjFaceNormals[fn1], adjFaceNormals[fn2] );
					if( checkDistance > farthestDistance )
					{
						farthestDistance = checkDistance;
					}
				}
			}
			if(farthestDistance < leastAngleVertexWeight)
			{
				leastAngleVertexWeight = farthestDistance;
				leastAngleVertexIndex = v;
			}
		}
		// Find the closest vertex to the vertex with the least weight.
		uint32_t leastDistantVertex = 0;
		float leastDistance = FLT_MAX;
		for( uint32_t af = 0; af < adjVertexFaces[leastAngleVertexIndex].size(); af++)
		{
			for( uint32_t i = 0; i < 3; i++ )
			{
				float CheckDistance = distance( vertices[leastAngleVertexIndex].position, vertices[faces[adjVertexFaces[leastAngleVertexIndex][af]].indices[i]].position);
				if( CheckDistance < leastDistance && faces[adjVertexFaces[leastAngleVertexIndex][af]].indices[i] != leastAngleVertexIndex )
				{
					leastDistantVertex = faces[adjVertexFaces[leastAngleVertexIndex][af]].indices[i];
					leastDistance = distance( vertices[leastAngleVertexIndex].position, vertices[leastDistantVertex].position);
				}
			}
		}
		std::vector< uint32_t > verticesToMerge;
		verticesToMerge.push_back( leastAngleVertexIndex );
		verticesToMerge.push_back( leastDistantVertex );
		this->mergeVerices( verticesToMerge, 0 );
	}
}

void Mesh::clean( float min, uint32_t flags )
{
	switch( flags )
	{
		case CLEAN_STRAY_VERTICES:
			{
				uint32_t v = 0;
				while( v < vertices.size() )
				{
					bool is_ref = false;
					for( uint32_t f = 0; f < faces.size(); f++ )
					{
						for( uint32_t i = 0; i < 3; i++ )
						{
							if(faces[f].indices[i] == v)
							{
								is_ref = true;
							}
						}
					}
					if(!is_ref)
					{
						vertices.erase( vertices.begin() + v);
						for( uint32_t f = 0; f < faces.size(); f++ )
						{
							for( uint32_t i = 0; i < 3; i++ )
							{
								if(faces[f].indices[i] > v)
								{
									faces[f].indices[i] = faces[f].indices[i] - 1;
								}
							}
						}
					}
					else
					{
						v++;
					}
				}
			}
			break;
		case CLEAN_CLOSE_VERTICES:
			{
				for( uint32_t v1 = 0; v1 < vertices.size(); v1++ )
				{
					for( uint32_t v2 = 0; v2 < vertices.size(); v2++ )
					{
						if( distance(vertices[v1].position, vertices[v2].position) < min && v1 != v2 )
						{
							vertices.erase( vertices.begin() + v2);
							if(v1 > v2)
							{
								v1 = v1 - 1;
							}
							for( uint32_t f = 0; f < faces.size(); f++ )
							{
								for( uint32_t i = 0; i < 3; i++ )
								{
									if(faces[f].indices[i] == v2)
									{
										faces[f].indices[i] = v1;
									}
									else if((faces[f].indices[i] > v2))
									{
										faces[f].indices[i] = faces[f].indices[i] - 1;
									}
								}
							}
						}
					}
				}
			}
			break;
		case CLEAN_BROKEN_FACES:
			{
				for( uint32_t f = 0; f < faces.size(); f++ )
				{
					if(faces[f].indices[0] == faces[f].indices[1] || faces[f].indices[1] == faces[f].indices[2] || faces[f].indices[2] == faces[f].indices[0])
					{
						faces.erase( faces.begin() + f );
					}
				}
			}
			break;
		case CLEAN_ALL:
			{
				uint32_t v = 0;
				while( v < vertices.size() )
				{
					bool is_ref = false;
					for( uint32_t f = 0; f < faces.size(); f++ )
					{
						for( uint32_t i = 0; i < 3; i++ )
						{
							if(faces[f].indices[i] == v)
							{
								is_ref = true;
							}
						}
					}
					if(!is_ref)
					{
						vertices.erase( vertices.begin() + v);
						for( uint32_t f = 0; f < faces.size(); f++ )
						{
							for( uint32_t i = 0; i < 3; i++ )
							{
								if(faces[f].indices[i] > v)
								{
									faces[f].indices[i] = faces[f].indices[i] - 1;
								}
							}
						}
					}
					else
					{
						v++;
					}
				}
				for( uint32_t v1 = 0; v1 < vertices.size(); v1++ )
				{
					for( uint32_t v2 = 0; v2 < vertices.size(); v2++ )
					{
						if( distance(vertices[v1].position, vertices[v2].position) < min && v1 != v2 )
						{
							vertices.erase( vertices.begin() + v2);
							if(v1 > v2)
							{
								v1 = v1 - 1;
							}
							for( uint32_t f = 0; f < faces.size(); f++ )
							{
								for( uint32_t i = 0; i < 3; i++ )
								{
									if(faces[f].indices[i] == v2)
									{
										faces[f].indices[i] = v1;
									}
									else if((faces[f].indices[i] > v2))
									{
										faces[f].indices[i] = faces[f].indices[i] - 1;
									}
								}
							}
						}
					}
				}
				for( uint32_t f = 0; f < faces.size(); f++ )
				{
					if(faces[f].indices[0] == faces[f].indices[1] || faces[f].indices[1] == faces[f].indices[2] || faces[f].indices[2] == faces[f].indices[0])
					{
						faces.erase( faces.begin() + f );
					}
				}
			}
			break;
	}
}

Vector3 Mesh::computeFaceNormal( uint32_t face )
{
	Vector3 out;
	uint32_t test = faces[face].indices[2];
	Vector3 v1 = vertices[faces[face].indices[2]].position - vertices[faces[face].indices[1]].position;
	Vector3 v2 = vertices[faces[face].indices[0]].position - vertices[faces[face].indices[1]].position;
	out = vector3Normalize( vector3Cross( v1, v2 ) );
	return out;
}
}
