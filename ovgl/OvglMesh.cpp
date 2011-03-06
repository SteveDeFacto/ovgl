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
* @brief None.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMesh.h"
#include "OvglScene.h"

bool Ovgl::Vertex::operator == ( const Ovgl::Vertex& in ) const
{
	if( memcmp(this, &in, sizeof(Ovgl::Vertex)) == 0 )
		return true;
	else
		return false;
}

bool Ovgl::Vertex::operator != ( const Ovgl::Vertex& in ) const
{
	if( memcmp(this, &in, sizeof(Ovgl::Vertex)) != 0 )
		return true;
	else
		return false;
}

Ovgl::Face Ovgl::Face::Flip()
{
	Ovgl::Face out = {0};
	out.indices[0] = indices[2];
	out.indices[1] = indices[1];
	out.indices[2] = indices[0];
	return out;
}

void Ovgl::Mesh::GenerateVertexNormals()
{
	for(DWORD v = 0; v < vertices.size(); v++)
	{
		std::vector<Ovgl::Vector3> adjFaceNormals;
		for( DWORD f = 0; f < faces.size(); f++)
		{
			for(DWORD i = 0; i < 3; i++)
			{
				if( faces[f].indices[i] == v )
				{
					adjFaceNormals.push_back( ComputeFaceNormal(f) );
				}
			}
		}
		vertices[v].normal = Vector3Normalize( &Vector3Center( adjFaceNormals ) );
	}
}

void Ovgl::Mesh::GenerateBoneMeshes()
{
	for( DWORD b = 0; b < bones.size(); b++ )
	{
		bones[b]->mesh->vertices.clear();
		for( DWORD v = 0; v < vertices.size(); v++ )
		{
			for( DWORD i = 0; i < 4; i++)
			{
				if(vertices[v].indices[i] == b && vertices[v].weight[i] > 0.1f)
				{
					Ovgl::Vertex Vertex = {0};
					Vertex.position = Ovgl::Vector3Transform( &vertices[v].position, &Ovgl::MatrixInverse( &Ovgl::Vector4Set( 0.0f, 0.0f, 0.0f, 0.0f ), &bones[b]->matrix));
					Vertex.weight[0] = 1.0f;
					bones[b]->mesh->vertices.push_back( Vertex );
				}
			}
		}
		bones[b]->mesh->Clean( 0.001f, CLOSE_VERTICES );
		bones[b]->mesh->QuickHull();
		bones[b]->mesh->Simplify( 256, 0 );
	}
}

void Ovgl::Mesh::Update()
{
	// Release buffers.
	if( VertexBuffer ) VertexBuffer->Release();
	if( IndexBuffers )
	{
		for(UINT i = 0; i < subset_count; i++)
			if( IndexBuffers[i] ) 
				IndexBuffers[i]->Release();
		delete [] IndexBuffers;
	}
	for( DWORD i = 0; i < bones.size(); i++ )
	{
		if(bones[i]->convex)
		{
			Inst->PhysX->releaseConvexMesh( *bones[i]->convex );
		}
	}

	// Create Vertex buffer.
	D3D10_BUFFER_DESC Buffer_Desc;
	D3D10_SUBRESOURCE_DATA InitData;
	Buffer_Desc.Usage = D3D10_USAGE_DEFAULT;
	Buffer_Desc.ByteWidth = sizeof( Ovgl::Vertex ) * vertices.size();
	Buffer_Desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;
	InitData.pSysMem = &vertices[0];
	Inst->D3DDevice->CreateBuffer( &Buffer_Desc, &InitData, &VertexBuffer );

	// Create Index buffers.
	std::vector<std::vector<Ovgl::Face>> index_subsets;
	for( DWORD i = 0; i < this->faces.size(); i++ )
	{
		if( (attributes[i]+1) > index_subsets.size() )
			index_subsets.resize(attributes[i]+1);
		index_subsets[attributes[i]].push_back(faces[i]);
	}
	subset_count = index_subsets.size();
	IndexBuffers = new ID3D10Buffer*[subset_count];
	for( DWORD i = 0; i < subset_count; i++ )
	{
		Buffer_Desc.Usage = D3D10_USAGE_DEFAULT;
		Buffer_Desc.ByteWidth = sizeof( Ovgl::Face ) * index_subsets[i].size();
		Buffer_Desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		Buffer_Desc.CPUAccessFlags = 0;
		Buffer_Desc.MiscFlags = 0;
		InitData.pSysMem = &index_subsets[i][0];
		Inst->D3DDevice->CreateBuffer( &Buffer_Desc, &InitData, &IndexBuffers[i] );
	}

	// Find the root bone.
	root_bone = 0;
	for( DWORD b1 = 0; b1 < bones.size(); b1++ )
	{
		bool is_root = true;
		for( DWORD b2 = 0; b2 < bones.size(); b2++ )
		{
			for( DWORD c = 0; c < bones[b2]->childen.size(); c++ )
				if( bones[b2]->childen[c] == b1 )
				{
					is_root = false;
					break;
				}
		}
		if( is_root )
		{
			root_bone = b1;
		}
	}

	// Create Bone Meshes.
	for( DWORD i = 0; i < bones.size(); i++ )
	{
		if( (bones[i]->mesh->vertices.size() > 0) && (bones[i]->mesh->faces.size() > 0) )
		{
			NxConvexMeshDesc ConvexMeshDesc;
			ConvexMeshDesc.numVertices = bones[i]->mesh->vertices.size();
			ConvexMeshDesc.pointStrideBytes = sizeof(Ovgl::Vertex);
			ConvexMeshDesc.points = &bones[i]->mesh->vertices[0];
			ConvexMeshDesc.numTriangles = bones[i]->mesh->faces.size();
			ConvexMeshDesc.triangleStrideBytes = sizeof(Ovgl::Face);
			ConvexMeshDesc.triangles = &bones[i]->mesh->faces[0];
			ConvexMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
			NxStreamDefault buf;
			Inst->Cooking->NxCookConvexMesh(ConvexMeshDesc, buf);
			bones[i]->convex = Inst->PhysX->createConvexMesh(buf);
		}
		else
		{
			std::vector<Ovgl::Vertex> vertices;
			std::vector<Ovgl::Face> faces;
			vertices.resize(8);
			faces.resize(12);
			vertices[0].position.x = -0.125f * bones[i]->length;
			vertices[0].position.y = 0.0f;
			vertices[0].position.z = -0.125f * bones[i]->length;
			vertices[1].position.x = 0.125f * bones[i]->length;
			vertices[1].position.y = 0.0f;
			vertices[1].position.z = -0.125f * bones[i]->length;
			vertices[2].position.x = -0.125f * bones[i]->length;
			vertices[2].position.y = 0.5f * bones[i]->length;
			vertices[2].position.z = -0.125f * bones[i]->length;
			vertices[3].position.x = 0.125f * bones[i]->length;
			vertices[3].position.y = 1.0f * bones[i]->length;
			vertices[3].position.z = -0.125f * bones[i]->length;
			vertices[4].position.x = -0.125f * bones[i]->length;
			vertices[4].position.y = 0.0f;
			vertices[4].position.z = 0.125f * bones[i]->length;
			vertices[5].position.x = 0.125f * bones[i]->length;
			vertices[5].position.y = 0.0f;
			vertices[5].position.z = 0.125f * bones[i]->length;
			vertices[6].position.x = -0.125f * bones[i]->length;
			vertices[6].position.y = 1.0f * bones[i]->length;
			vertices[6].position.z = 0.125f * bones[i]->length;
			vertices[7].position.x = 0.125f * bones[i]->length;
			vertices[7].position.y = 1.0f * bones[i]->length;
			vertices[7].position.z = 0.125f * bones[i]->length;
			faces[0].indices[0] = 2;
			faces[0].indices[1] = 1;
			faces[0].indices[2] = 0;
			faces[1].indices[0] = 3;
			faces[1].indices[1] = 1;
			faces[1].indices[2] = 2;
			faces[2].indices[0] = 4;
			faces[2].indices[1] = 5;
			faces[2].indices[2] = 6;
			faces[3].indices[0] = 6;
			faces[3].indices[1] = 5;
			faces[3].indices[2] = 7;
			faces[4].indices[0] = 0;
			faces[4].indices[1] = 1;
			faces[4].indices[2] = 4;
			faces[5].indices[0] = 4;
			faces[5].indices[1] = 1;
			faces[5].indices[2] = 5;
			faces[6].indices[0] = 6;
			faces[6].indices[1] = 3;
			faces[6].indices[2] = 2;
			faces[7].indices[0] = 7;
			faces[7].indices[1] = 3;
			faces[7].indices[2] = 6;
			faces[8].indices[0] = 2;
			faces[8].indices[1] = 0;
			faces[8].indices[2] = 4;
			faces[9].indices[0] = 2;
			faces[9].indices[1] = 4;
			faces[9].indices[2] = 6;
			faces[10].indices[0] = 1;
			faces[10].indices[1] = 3;
			faces[10].indices[2] = 5;
			faces[11].indices[0] = 5;
			faces[11].indices[1] = 3;
			faces[11].indices[2] = 7;
			NxConvexMeshDesc ConvexMeshDesc;    
			ConvexMeshDesc.numVertices = vertices.size();
			ConvexMeshDesc.numTriangles = faces.size();
			ConvexMeshDesc.pointStrideBytes = sizeof(Ovgl::Vertex);
			ConvexMeshDesc.triangleStrideBytes = sizeof(Ovgl::Face);
			ConvexMeshDesc.points = &vertices[0];
			ConvexMeshDesc.triangles = &faces[0];
			ConvexMeshDesc.flags = 0;
			NxStreamDefault buf;
			Inst->Cooking->NxCookConvexMesh(ConvexMeshDesc, buf);
			bones[i]->convex = Inst->PhysX->createConvexMesh( buf );
		}
	}
}

void Ovgl::Mesh::Save( const std::string& file  )
{
}

void Ovgl::Mesh::Release()
{
	for( DWORD m = 0; m < Inst->Meshes.size(); m++ )
	{
		if( Inst->Meshes[m] == this )
		{
			Inst->Meshes.erase( Inst->Meshes.begin() + m );
		}
	}
	VertexBuffer->Release();
	for( DWORD i = 0; i < subset_count; i++)
	{
		IndexBuffers[i]->Release();
	}
	delete this;
}

Ovgl::Matrix44 Ovgl::CMesh::getPose()
{
	Ovgl::Matrix44 matrix;
	actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
	return matrix;
}

void Ovgl::CMesh::setPose( Ovgl::Matrix44* matrix )
{
	NxMat34 mat;
	mat.setColumnMajor44( (float*)matrix );
	actor->setGlobalPose( mat );
}

void Ovgl::CMesh::Release()
{
	scene->physics_scene->releaseActor( *actor );
}

void Ovgl::Mesh::CubeCloud( float sx, float sy, float sz, int count )
{
	for( int i = 0; i < count; i++)
	{
		Ovgl::Vertex TempVertex = {0};
		TempVertex.position.x = ((((float)rand() / RAND_MAX) * sx) * 2) - sx;
		TempVertex.position.y = ((((float)rand() / RAND_MAX) * sy) * 2) - sy;
		TempVertex.position.z = ((((float)rand() / RAND_MAX) * sz) * 2) - sz;
		TempVertex.weight[0] = 1.0f;
		vertices.push_back( TempVertex );
	}
}

void Ovgl::Mesh::QuickHull()
{
	// Generate a starting face that is at the farthest points. ( Right now it is just adding the first three points. )

	Ovgl::Face FirstFace = {0};
	FirstFace.indices[0] = 0;
	FirstFace.indices[1] = 1;
	FirstFace.indices[2] = 2;

	// Add both the front and back sides of the face to the mesh.
	faces.push_back( FirstFace );
	faces.push_back( FirstFace.Flip() );

	// Start looping through the faces to create the convex mesh.
	DWORD f = 0;
	while( (f < faces.size()) && (f < (vertices.size() * 2) - 4) )
	{
		bool foundVertex = false;
		DWORD distantVertex = 0;
		Ovgl::Vector3 faceNormal = ComputeFaceNormal( f );
		Ovgl::Vector3 facePosition = vertices[faces[f].indices[0]].position;

		// Find the most distance vertex from this face.
		for( DWORD v = 0; v < vertices.size(); v++ )
		{
			if( Vector3Dot( &faceNormal, &(vertices[v].position - facePosition)) > Vector3Dot( &faceNormal, &(vertices[distantVertex].position - facePosition)) && Vector3Dot( &faceNormal, &(vertices[v].position - facePosition)) > 0.001f )
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
			std::vector<DWORD> TaggedFaces;
			for( DWORD f2 = 0; f2 < faces.size(); f2++ )
			{
				Ovgl::Vector3 faceNormal2 = ComputeFaceNormal( f2 );
				if( Vector3Dot( &faceNormal2, &(vertices[distantVertex].position - vertices[faces[f2].indices[0]].position)) > 0.0f )
				{
					TaggedFaces.push_back( f2 );
				}
			}

            // Now that we know which faces this vertex lands on we need to remap the mesh to include the new vertex.
			ConnectVertex( TaggedFaces, distantVertex);
		}
		else
		{
			// If we didn't find any vertices that land on this face then move to the next face.
			f++;
		}
	}

	// Clean the mesh from stray vertices.
	this->Clean( 0.0f, CLEAN_ALL );
}

void Ovgl::Mesh::ConnectVertex( std::vector<DWORD>& faceList, DWORD vertex)
{
	std::vector<Ovgl::Face> newFaces;

	// Find which faces share vertices with the faces we want to remap.
	for( DWORD f2 = 0; f2 < faces.size(); f2++ )
	{
		for( DWORD tf = 0; tf < faceList.size(); tf++ )
		{
			std::vector<DWORD> SharedVertices;
			for( DWORD i = 0; i < 3; i++ )
			{
				for( DWORD i2 = 0; i2 < 3; i2++ )
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
			DWORD sz = SharedVertices.size();
			if( sz > 1)
			{
				for(DWORD sv = 0; sv < sz - (DWORD)(sz == 2); sv++)
				{
					//Order the new face based on the face that shares vertices with the faceList.
					if( (faces[f2].indices[0] == SharedVertices[ sv % sz ] && faces[f2].indices[1] == SharedVertices[ (sv + 1) % sz ]) ||
						(faces[f2].indices[1] == SharedVertices[ sv % sz ] && faces[f2].indices[2] == SharedVertices[ (sv + 1) % sz ]) ||
						(faces[f2].indices[2] == SharedVertices[ sv % sz ] && faces[f2].indices[0] == SharedVertices[ (sv + 1) % sz ]) )
					{
						Ovgl::Face tempFace;
						tempFace.indices[0] = SharedVertices[(sv + 1) % sz];
						tempFace.indices[1] = SharedVertices[sv % sz];
						tempFace.indices[2] = vertex;
						newFaces.push_back( tempFace );
					}
					else if( ( faces[f2].indices[2] == SharedVertices[ sv % sz ] && faces[f2].indices[1] == SharedVertices[(sv + 1) % sz]) ||
						( faces[f2].indices[1] == SharedVertices[ sv % sz ] && faces[f2].indices[0] == SharedVertices[(sv + 1) % sz]) ||
						( faces[f2].indices[0] == SharedVertices[ sv % sz ] && faces[f2].indices[2] == SharedVertices[(sv + 1) % sz]) )
					{
						Ovgl::Face tempFace;
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
	for( DWORD f = 0; f < faceList.size(); f++ )
	{
		faces.erase( faces.begin() + faceList[f] );
		for( DWORD f2 = f; f2 < faceList.size(); f2++ )
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


void Ovgl::Mesh::MergeVerices( std::vector<DWORD>& vertexList, DWORD flag )
{
	// Move all vertices to last vertex;
	if( flag == 0 )
	{
		for( DWORD v = 0; v < vertexList.size() - 1; v++)
		{
			vertices[vertexList[v]].position = vertices[vertexList[vertexList.size() - 1]].position;
		}
	}
	// Clean the mesh.
	this->Clean( 0.001f, CLEAN_ALL );
}

void Ovgl::Mesh::Simplify( DWORD max_faces, DWORD max_vertices )
{
//	this->Clean( 0.0f, CLEAN_ALL );
	while(  (faces.size() > max_faces && max_faces != 0) || (vertices.size() > max_vertices && max_vertices != 0) )
	{
		// Compute vertex weights.
		DWORD LeastAngleVertexIndex = 0;
		float LeastAngleVertexWeight = FLT_MAX;
		std::vector<std::vector<DWORD>> adjVertexFaces(vertices.size());
		for( DWORD v = 0; v < vertices.size(); v++)
		{
			// Get list of adjacent faces
			std::vector<Ovgl::Vector3> adjFaceNormals;
			for( DWORD f = 0; f < faces.size(); f++)
			{
				for(DWORD i = 0; i < 3; i++)
				{
					if( faces[f].indices[i] == v )
					{
						adjFaceNormals.push_back( ComputeFaceNormal(f) );
						adjVertexFaces[v].push_back( f );
					}
				}
			}
			float farthestDistance = 0;
			for( DWORD fn1 = 0; fn1 < adjFaceNormals.size(); fn1++)
			{
				for( DWORD fn2 = 0; fn2 < adjFaceNormals.size(); fn2++)
				{
					float checkDistance = Ovgl::Distance( &adjFaceNormals[fn1], &adjFaceNormals[fn2] );
					if( checkDistance > farthestDistance )
					{
						farthestDistance = checkDistance;
					}
				}
			}
			if(farthestDistance < LeastAngleVertexWeight)
			{
				LeastAngleVertexWeight = farthestDistance;
				LeastAngleVertexIndex = v;
			}
		}
		// Find the closest vertex to the vertex with the least weight.
		DWORD LeastDistantVertex = 0;
		float LeastDistance = FLT_MAX;
		for( DWORD af = 0; af < adjVertexFaces[LeastAngleVertexIndex].size(); af++)
		{
			for( DWORD i = 0; i < 3; i++ )
			{
				float CheckDistance = Ovgl::Distance( &vertices[LeastAngleVertexIndex].position, &vertices[faces[adjVertexFaces[LeastAngleVertexIndex][af]].indices[i]].position);
				if( CheckDistance < LeastDistance && faces[adjVertexFaces[LeastAngleVertexIndex][af]].indices[i] != LeastAngleVertexIndex )
				{
					LeastDistantVertex = faces[adjVertexFaces[LeastAngleVertexIndex][af]].indices[i];
					LeastDistance = Ovgl::Distance( &vertices[LeastAngleVertexIndex].position, &vertices[LeastDistantVertex].position);
				}
			}
		}
		std::vector<DWORD> VerticesToMerge;
		VerticesToMerge.push_back( LeastAngleVertexIndex );
		VerticesToMerge.push_back( LeastDistantVertex );
		this->MergeVerices( VerticesToMerge, 0 );
	}

}

void Ovgl::Mesh::Clean( float min, DWORD flags )
{
	switch( flags )
	{
		case STRAY_VERTICES:
		{
			DWORD v = 0;
			while( v < vertices.size() )
			{
				bool is_ref = false;
				for( DWORD f = 0; f < faces.size(); f++ )
				{
					for( DWORD i = 0; i < 3; i++ )
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
					for( DWORD f = 0; f < faces.size(); f++ )
					{
						for( DWORD i = 0; i < 3; i++ )
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
		case CLOSE_VERTICES:
		{
			for( DWORD v1 = 0; v1 < vertices.size(); v1++ )
			{
				for( DWORD v2 = 0; v2 < vertices.size(); v2++ )
				{
					if( Distance(&vertices[v1].position, &vertices[v2].position) < min && v1 != v2 )
					{
						vertices.erase( vertices.begin() + v2);
						if(v1 > v2)
						{
							v1 = v1 - 1;
						}
						for( DWORD f = 0; f < faces.size(); f++ )
						{
							for( DWORD i = 0; i < 3; i++ )
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
		case BROKEN_FACES:
		{
			for( DWORD f = 0; f < faces.size(); f++ )
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
			DWORD v = 0;
			while( v < vertices.size() )
			{
				bool is_ref = false;
				for( DWORD f = 0; f < faces.size(); f++ )
				{
					for( DWORD i = 0; i < 3; i++ )
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
					for( DWORD f = 0; f < faces.size(); f++ )
					{
						for( DWORD i = 0; i < 3; i++ )
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
			for( DWORD v1 = 0; v1 < vertices.size(); v1++ )
			{
				for( DWORD v2 = 0; v2 < vertices.size(); v2++ )
				{
					if( Distance(&vertices[v1].position, &vertices[v2].position) < min && v1 != v2 )
					{
						vertices.erase( vertices.begin() + v2);
						if(v1 > v2)
						{
							v1 = v1 - 1;
						}
						for( DWORD f = 0; f < faces.size(); f++ )
						{
							for( DWORD i = 0; i < 3; i++ )
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
			for( DWORD f = 0; f < faces.size(); f++ )
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
	
Ovgl::Vector3 Ovgl::Mesh::ComputeFaceNormal( DWORD face )
{
	Ovgl::Vector3 out = {0};
	Ovgl::Vector3 v1 = vertices[faces[face].indices[2]].position - vertices[faces[face].indices[1]].position;
	Ovgl::Vector3 v2 = vertices[faces[face].indices[0]].position - vertices[faces[face].indices[1]].position;
	out = Vector3Normalize( &Ovgl::Vector3Cross( &v1, &v2 ) );
	return out;
}
