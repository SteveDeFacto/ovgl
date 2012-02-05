/**
* @file OvglMedia.cpp
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
* @brief This file contains classes which import external media formats into an Ovgl media library.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"

namespace Ovgl
{
	MediaLibrary::MediaLibrary( Instance* instance, const std::string& file )
	{
		Inst = instance;
		instance->MediaLibraries.push_back(this);
	}

	void MediaLibrary::Release()
	{
		for( uint32_t i = 0; i < AudioBuffers.size(); i++ )
		{
			AudioBuffers[i]->Release();
		}
		for( uint32_t i = 0; i < Materials.size(); i++ )
		{
			Materials[i]->Release();
		}
		for( uint32_t i = 0; i < Meshes.size(); i++ )
		{
			Meshes[i]->Release();
		}
		for( uint32_t i = 0; i < Scenes.size(); i++ )
		{
			Scenes[i]->Release();
		}
		for( uint32_t i = 0; i < Shaders.size(); i++ )
		{
			Shaders[i]->Release();
		}
		for( uint32_t i = 0; i < Textures.size(); i++ )
		{
			Textures[i]->Release();
		}
		delete this;
	}

	void MediaLibrary::Save( const std::string& file )
	{
		if(!file.empty())
		{
			// Open file.
			FILE *output;
			fopen_s(&output, file.c_str(),"wb");
	
			// Write the number of meshes to the file.
			uint32_t mesh_count = Meshes.size();
			fwrite( &mesh_count, 4, 1, output ); 
			for( uint32_t m = 0; m < mesh_count; m++ )
			{
				// Get mesh variables.
				uint32_t vertex_count = Meshes[m]->vertices.size();
				uint32_t face_count = Meshes[m]->faces.size();
				uint32_t bone_count = Meshes[m]->bones.size();
		
				// Write the number of vertices, faces, and bones.
				fwrite( &vertex_count, 4, 1, output );
				fwrite( &face_count, 4, 1, output );
				fwrite( &bone_count, 4, 1, output );
				
				// Write the vertices, faces, and bones.
				fwrite( &Meshes[m]->vertices[0], sizeof(Vertex), vertex_count, output);
				fwrite( &Meshes[m]->faces[0], sizeof(Face), face_count, output);
				fwrite( &Meshes[m]->attributes[0], sizeof(uint32_t), face_count, output);
					
				// Write bones.
				for( uint32_t i = 0; i < bone_count; i++ )
				{
					vertex_count = Meshes[m]->bones[i]->mesh->vertices.size();
					face_count = Meshes[m]->bones[i]->mesh->faces.size();
					fwrite( &vertex_count, 4, 1, output );
					fwrite( &face_count, 4, 1, output );
					if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it. 
					{
						fwrite( &Meshes[m]->bones[i]->mesh->vertices[0], sizeof(Vertex), vertex_count, output );
						fwrite( &Meshes[m]->bones[i]->mesh->faces[0], sizeof(Face), face_count, output );
					}
					fwrite( &Meshes[m]->bones[i]->matrix, sizeof(Matrix44), 1, output );
					fwrite( &Meshes[m]->bones[i]->length, sizeof(float), 1, output );
					fwrite( &Meshes[m]->bones[i]->min, sizeof(Vector3), 1, output );
					fwrite( &Meshes[m]->bones[i]->max, sizeof(Vector3), 1, output );
					fwrite( &Meshes[m]->bones[i]->parent, sizeof(uint32_t), 1, output );
					uint32_t child_count = Meshes[m]->bones[i]->childen.size();
					fwrite( &child_count, sizeof(uint32_t), 1, output );
					if(child_count) fwrite( &Meshes[m]->bones[i]->childen[0], sizeof(uint32_t), child_count, output );
				}
			}
			uint32_t scene_count = Scenes.size();
			fwrite( &scene_count, 4, 1, output ); 
			for( uint32_t s = 0; s < scene_count; s++ )
			{
				uint32_t prop_count = Scenes[s]->props.size();
				fwrite( &prop_count, 4, 1, output );
				for( uint32_t p = 0; p < Scenes[s]->props.size(); p++ )
				{
					Matrix44 matrix = Scenes[s]->props[p]->getPose();
					fwrite( &matrix, sizeof(Matrix44), 1, output );
					uint32_t mesh_index = 0;
					for( uint32_t sm = 0; sm < Meshes.size(); sm++ )
					{
						if( Scenes[s]->props[p]->mesh == Meshes[sm] )
						{
							mesh_index = sm;
						}
					}
					fwrite( &mesh_index, 4, 1, output );
					for( uint32_t b = 0; b < Scenes[s]->props[p]->bones.size(); b++ )
					{
						uint32_t bodyflags = Scenes[s]->props[p]->bones[b]->GetFlags();
						fwrite( &bodyflags, 4, 1, output );
					}
				}
				uint32_t object_count = Scenes[s]->objects.size();
				fwrite( &object_count, 4, 1, output );
				for( uint32_t o = 0; o < Scenes[s]->objects.size(); o++ )
				{
					Matrix44 matrix = Scenes[s]->objects[o]->getPose();
					fwrite( &matrix, sizeof(Matrix44), 1, output );
					uint32_t mesh_index = 0;
					for( uint32_t sm = 0; sm < Meshes.size(); sm++ )
					{
						if( Scenes[s]->objects[o]->mesh == Meshes[sm] )
						{
							mesh_index = sm;
						}
					}
					fwrite( &mesh_index, 4, 1, output );
					uint32_t bodyflags = Scenes[s]->objects[o]->CollisionMesh->GetFlags();
					fwrite( &bodyflags, 4, 1, output );
				}
				uint32_t light_count = Scenes[s]->lights.size();
				fwrite( &light_count, 4, 1, output );
				for( uint32_t l = 0; l < light_count; l++ )
				{
					Matrix44 matrix = Scenes[s]->lights[l]->getPose();
					fwrite( &matrix, sizeof(Matrix44), 1, output );
				}
				uint32_t camera_count = Scenes[s]->cameras.size();
				fwrite( &camera_count, 4, 1, output );
				for( uint32_t c = 0; c < camera_count; c++ )
				{
					Matrix44 matrix = Scenes[s]->cameras[c]->getPose();
					fwrite( &matrix, sizeof(Matrix44), 1, output );
				}
				//uint32_t joint_count = Scenes[s]->joints.size();
				//fwrite( &joint_count, 4, 1, output );
				//for( uint32_t j = 0; j < joint_count; j++ )
				//{
				//	Scenes[s]->joints[j]->obj[0];
				//	Scenes[s]->joints[j]->obj[1];

				//	NxVec3 nxAnchor = Scenes[s]->joints[j]->joint->getGlobalAnchor();
				//	Vector3 anchor = Vector3(nxAnchor.x, nxAnchor.y, nxAnchor.z);
				//	fwrite( &anchor, sizeof(Vector3), 1, output );
				//}
			}
			uint32_t texture_count = Textures.size();
			fwrite( &texture_count, 4, 1, output );


			// Close file.
			fclose(output);
		}
	}

	void MediaLibrary::Load( const std::string& file )
	{
		if(!file.empty())
		{
			// Get the number of meshes currently in memory so we can offset the indices into the array.
			uint32_t mesh_offset = Meshes.size();
		
			// Open file.	
			FILE *input = NULL;
			fopen_s(&input, file.c_str(),"rb");
	
			// If file was unable to be opened present error message to debug output and end function
			if ( input == NULL )
			{
				std::wstring wfile;
				wfile = L"Unable to open the file ";
				wfile.append(file.begin(), file.end());
				OutputDebugString( wfile.c_str() );
				return;
			}

			//Get number of meshes.
			uint32_t mesh_count;
			fread( &mesh_count, 4, 1, input );
			for( uint32_t m = 0; m < mesh_count; m++ )
			{
				// Specify mesh variables.
				Mesh* mesh = new Mesh;
				mesh->ml = this;
				uint32_t vertex_count;
				uint32_t face_count;
				uint32_t bone_count;
				uint32_t child_count;
				std::vector< Vector3 > bone_vertices;
				std::vector< Face > bone_faces;
		
				// Get number of vertices, faces, and bones.
				fread( &vertex_count, 4, 1, input ); 
				fread( &face_count, 4, 1, input ); 
				fread( &bone_count, 4, 1, input ); 
		
				// Resize arrays.
				mesh->vertices.resize(vertex_count);
				mesh->faces.resize(face_count);
				mesh->attributes.resize(face_count);
				mesh->bones.resize(bone_count);
		
				// Load vertices, indices and attributes.
				fread(&mesh->vertices[0], sizeof(Vertex), vertex_count, input);
				fread(&mesh->faces[0], sizeof(Face), face_count, input);
				fread(&mesh->attributes[0], sizeof(uint32_t), face_count, input);

				// Load bones.
				for( uint32_t i = 0; i < bone_count; i++ )
				{
					mesh->bones[i] = new Bone;
					mesh->bones[i]->mesh = new Mesh;
					mesh->bones[i]->convex = NULL;
					fread( &vertex_count, 4, 1, input );
					fread( &face_count, 4, 1, input );
					if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it. 
					{
						mesh->bones[i]->mesh->vertices.resize(vertex_count);
						mesh->bones[i]->mesh->faces.resize(face_count);
						fread( &mesh->bones[i]->mesh->vertices[0], sizeof(Vertex), vertex_count, input );
						fread( &mesh->bones[i]->mesh->faces[0], sizeof(Face), face_count, input );
					}
					fread( &mesh->bones[i]->matrix, sizeof(Matrix44), 1, input );
					fread( &mesh->bones[i]->length, sizeof(float), 1, input );
					fread( &mesh->bones[i]->min, sizeof(Vector3), 1, input );
					fread( &mesh->bones[i]->max, sizeof(Vector3), 1, input );
					fread( &mesh->bones[i]->parent, sizeof(uint32_t), 1, input );
					fread( &child_count, sizeof(uint32_t), 1, input );
					mesh->bones[i]->childen.resize(child_count);
					if(child_count) fread( &mesh->bones[i]->childen[0], sizeof(uint32_t), child_count, input );
				}
		
				// Nullify buffer addresses.
				mesh->VertexBuffer = 0;
				mesh->IndexBuffers = 0;
					
				// Update buffers.
				mesh->Update();

				Meshes.push_back(mesh);
			}

			//Get number of scenes.
			uint32_t scene_count;
			fread( &scene_count, 4, 1, input );
			for( uint32_t s = 0; s < scene_count; s++ )
			{
				Scene* scene = CreateScene();
	
				uint32_t prop_count;
				fread( &prop_count, 4, 1, input );
				for( uint32_t p = 0; p < prop_count; p++ )
				{
					// Get the pose of this prop.
					Matrix44 matrix;
					fread( &matrix, sizeof(Matrix44), 1, input );
					uint32_t mesh_index;
					fread( &mesh_index, 4, 1, input );
					Prop* prop = scene->CreateProp( Meshes[mesh_index + mesh_offset], &matrix );
					for( uint32_t b = 0; b < Meshes[mesh_index + mesh_offset]->bones.size(); b++ )
					{
						uint32_t bone_flags;
						fread( &bone_flags, 4, 1, input );
						prop->bones[b]->SetFlags(bone_flags);
					}
				}

				uint32_t object_count;
				fread( &object_count, 4, 1, input );
				for( uint32_t o = 0; o < object_count; o++ )
				{
					// Get the pose of this object.
					Matrix44 matrix;
					fread( &matrix, sizeof(Matrix44), 1, input );
					uint32_t mesh_index;
					fread( &mesh_index, 4, 1, input );
					Object* object = scene->CreateObject( Meshes[mesh_index + mesh_offset], &matrix );
					uint32_t bone_flags;
					fread( &bone_flags, 4, 1, input );
					object->CollisionMesh->SetFlags(bone_flags);
				}

				uint32_t light_count;
				fread( &light_count, 4, 1, input );
				for( uint32_t l = 0; l < light_count; l++ )
				{
					// Get the pose of this light.
					Matrix44 matrix;
					fread( &matrix, sizeof(Matrix44), 1, input );
					scene->CreateLight( &matrix, &Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
				}

				uint32_t camera_count;
				fread( &camera_count, 4, 1, input );
				for( uint32_t c = 0; c < camera_count; c++ )
				{
					// Get the pose of this camera.
					Matrix44 matrix;
					fread( &matrix, sizeof(Matrix44), 1, input );
					scene->CreateCamera( &matrix );
				}
			}
			// Close file.
			fclose(input);
		}
	}

	Mesh* MediaLibrary::ImportModel( const std::string& file )
	{
		if(!file.empty())
		{
			Mesh* mesh = new Ovgl::Mesh;

			// Set media library to this library.
			mesh->ml = this;

			// Import scene from file.
			const aiScene* scene = aiImportFile(file.c_str(), 0);

			// Iterate through scene nodes.
			for( uint32_t n = 0; n < scene->mRootNode->mNumChildren; n++)
			{

				// Iterate through meshes of this node.
				for( uint32_t sm = 0; sm < scene->mRootNode->mChildren[n]->mNumMeshes; sm++)
				{
					uint32_t voffset = mesh->vertices.size();
					uint32_t foffset = mesh->faces.size();
					uint32_t aoffset = 0;
					for( uint32_t i = 0; i < mesh->faces.size(); i++ )
					{
						if( mesh->attributes[i] > aoffset )
						{
							aoffset = mesh->attributes[i];
						}
					}
					uint32_t m = scene->mRootNode->mChildren[n]->mMeshes[sm];
					Matrix44 matrix;
					if(scene->mMeshes[m]->HasBones())
						matrix = Ovgl::MatrixTranspose(*(Matrix44*)&scene->mRootNode->FindNode(scene->mMeshes[m]->mBones[0]->mName)->mTransformation).Translation();
					else
						matrix = *(Matrix44*)&scene->mRootNode->mChildren[n]->mTransformation.Transpose();
					matrix = matrix * MatrixRotationX(1.57f);
					if(scene->mMeshes[m]->HasBones())
					matrix = matrix * Ovgl::MatrixTranspose(*(Matrix44*)&scene->mMeshes[m]->mBones[0]->mOffsetMatrix).Translation();
					std::vector< std::vector< float > > weights(scene->mMeshes[m]->mNumVertices);
					std::vector< std::vector< float > > indices(scene->mMeshes[m]->mNumVertices);
					std::vector< Face > faces;
					std::vector< uint32_t > attributes;
					mesh->vertices.resize( voffset + scene->mMeshes[m]->mNumVertices);

					// Get skeleton.
					if(scene->mMeshes[m]->HasBones())
					{
						for( uint32_t b = 0; b < scene->mMeshes[m]->mNumBones; b++ )
						{
							Bone* bone = new Bone;
							mesh->bones.push_back(bone);
							bone->length = 1.0f;
							bone->mesh = new Mesh;
							bone->convex = NULL;
							aiNode* bnode = scene->mRootNode->FindNode(scene->mMeshes[m]->mBones[b]->mName);
							aiMatrix4x4 GlobalTransform = bnode->mTransformation;
							aiNode* parent = bnode->mParent;
							while( parent->mParent != scene->mRootNode )
							{
								GlobalTransform = parent->mTransformation * GlobalTransform;
								parent = parent->mParent;
							}
							bone->matrix = MatrixInverse( Vector4(), Ovgl::MatrixTranspose(*(Matrix44*)&scene->mMeshes[m]->mBones[b]->mOffsetMatrix)) * MatrixRotationX(1.57f);

							// Get bone hierarchy.
							for( uint32_t cb = 0; cb < bnode->mNumChildren; cb++ )
							{
								for( uint32_t mb = 0; mb < scene->mMeshes[m]->mNumBones; mb++ )
								{
									if( bnode->mParent->mName == scene->mMeshes[m]->mBones[mb]->mName )
									{
										bone->parent = mb;
									}
									if( bnode->mChildren[cb]->mName == scene->mMeshes[m]->mBones[mb]->mName )
									{
										bone->childen.push_back(mb);
									}
								}
							}
							for (uint32_t w = 0 ; w < scene->mMeshes[m]->mBones[b]->mNumWeights; w++)
							{
								weights[scene->mMeshes[m]->mBones[b]->mWeights[w].mVertexId].push_back((float)scene->mMeshes[m]->mBones[b]->mWeights[w].mWeight);
								indices[scene->mMeshes[m]->mBones[b]->mWeights[w].mVertexId].push_back((float)b);
							}
							
							// Get animation for this bone.
							for( uint32_t a = 0; a < scene->mNumAnimations; a++ )
							{
								for( uint32_t ac = 0; ac < scene->mAnimations[a]->mNumChannels; ac++ )
								{
									if(scene->mAnimations[a]->mChannels[ac]->mNodeName == scene->mMeshes[m]->mBones[b]->mName)
									{
										for( uint32_t rk = 0; rk < scene->mAnimations[a]->mChannels[ac]->mNumRotationKeys; rk++ )
										{
											scene->mAnimations[a]->mChannels[ac]->mPreState = aiAnimBehaviour_LINEAR;
											Ovgl::Curve curve;
											curve.time = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mTime;
											curve.value.w = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.w;
											curve.value.x = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.x;
											curve.value.y = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.y;
											curve.value.z = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.z;
											mesh->bones[b]->Rot_Keys.push_back( curve );
										}
									}
								}
							}
						}
					}
					
					// Cap off bone influences to no more than 4.
					for ( uint32_t w = 0; w < scene->mMeshes[m]->mNumVertices; w++)
					{
						weights[w].resize(4);
						indices[w].resize(4);
						if(!scene->mMeshes[m]->HasBones())
							weights[w][0] = 1.0f;
					}

					// Get geometry.
					for( uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++ )
					{
						uint32_t findices[4];
						for( uint32_t i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++ )
						{
							int vi = scene->mMeshes[m]->mFaces[f].mIndices[i];
							Vertex vertex;
							vertex.position.x = scene->mMeshes[m]->mVertices[vi].x;
							vertex.position.y = scene->mMeshes[m]->mVertices[vi].y;
							vertex.position.z = scene->mMeshes[m]->mVertices[vi].z;
							vertex.position = Vector3Transform(vertex.position, matrix);
							vertex.normal.x = scene->mMeshes[m]->mNormals[vi].x;
							vertex.normal.y = scene->mMeshes[m]->mNormals[vi].y;
							vertex.normal.z = scene->mMeshes[m]->mNormals[vi].z;
							vertex.normal = Vector3Transform(vertex.normal, matrix.Rotation());
							if(scene->mMeshes[m]->GetNumUVChannels() > 0)
							{
								vertex.texture.x = scene->mMeshes[m]->mTextureCoords[0][vi].x;
								vertex.texture.y = scene->mMeshes[m]->mTextureCoords[0][vi].y;
							}
							else
							{
								vertex.texture.x = 0.0f;
								vertex.texture.y = 0.0f;
							}
							vertex.weight[0] = weights[vi][0];
							vertex.weight[1] = weights[vi][1];
							vertex.weight[2] = weights[vi][2];
							vertex.weight[3] = weights[vi][3];
							vertex.indices[0] = indices[vi][0];
							vertex.indices[1] = indices[vi][1];
							vertex.indices[2] = indices[vi][2];
							vertex.indices[3] = indices[vi][3];
							findices[i] = (uint32_t)vi + voffset;
							mesh->vertices[vi+voffset] = vertex;
						}

						if(scene->mMeshes[m]->mFaces[f].mNumIndices == 4)
						{
							Face face;
							face.indices[0] = findices[0];
							face.indices[1] = findices[2];
							face.indices[2] = findices[3];
							mesh->faces.push_back( face );
							mesh->attributes.push_back(scene->mMeshes[m]->mMaterialIndex);
						}
						Face face;
						face.indices[0] = findices[0];
						face.indices[1] = findices[1];
						face.indices[2] = findices[2];
						mesh->faces.push_back( face );
						mesh->attributes.push_back(scene->mMeshes[m]->mMaterialIndex);
					}		
				}
			}

			// If no bones exist create one.
			if(mesh->bones.size() == 0 )
			{
				Bone* bone = new Bone;
				bone->matrix = MatrixIdentity();
				bone->length = 1.0f;
				bone->mesh = new Mesh;
				bone->convex = NULL;
				mesh->bones.push_back(bone);
			}

			// Save vertices which influence each bone for bone shape automatic generation.
			for( uint32_t i = 0; i < mesh->bones.size(); i++ )
			{
				for( uint32_t v = 0; v < mesh->vertices.size(); v++ )
				{
					for( uint32_t j = 0; j < 4; j++)
					{
						if(mesh->vertices[v].indices[j] == i && mesh->vertices[v].weight[j] > 0.1f)
						{
							Vertex Vertex;
							Vertex.position = Vector3Transform( mesh->vertices[v].position, MatrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), mesh->bones[i]->matrix));
							Vertex.weight[0] = 1.0f;
							mesh->bones[i]->mesh->vertices.push_back( Vertex );
						}
					}
				}
			}

			// Null index and vertex buffers.
			mesh->VertexBuffer = 0;
			mesh->IndexBuffers = 0;

			// Update video memory copies of index and vertex buffers.
			mesh->Update();
			Meshes.push_back( mesh );
			return mesh;
		}

		// Function failed so return NULL.
		return NULL;
	}

	Ovgl::Texture* Ovgl::MediaLibrary::ImportCubeMap( const std::string& front, const std::string& back, const std::string& top, const std::string& bottom, const std::string& left, const std::string& right )
	{
		// Create new texture
		Ovgl::Texture* texture = new Ovgl::Texture;
	
		// Set the texture's media library handle to this media library
		texture->MLibrary = this;

		// Create array of cube faces.
		std::string CubeFaces[6] = {front, back, top, bottom, left, right};

		glGenTextures(1, &texture->Image);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->Image);
		for (int i = 0; i < 6; i++)
		{
			// Image format
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			// Check the file signature and deduce its format
			fif = FreeImage_GetFileType( CubeFaces[i].c_str(), 0 );

			// If still unknown, try to guess the file format from the file extension
			if( fif == FIF_UNKNOWN )
			{
				fif = FreeImage_GetFIFFromFilename( CubeFaces[i].c_str() );
			}

			// If still unkown, return NULL
			if( fif == FIF_UNKNOWN )
			{
				return NULL;
			}

			// Load texture
			FIBITMAP* dib = FreeImage_Load( fif, CubeFaces[i].c_str() );
		
			// Convert to RGB format
			dib = FreeImage_ConvertTo32Bits( dib );

			// Get raw data and dimensions
			uint32_t w = FreeImage_GetWidth( dib );
			uint32_t h = FreeImage_GetHeight( dib );
			BYTE* pixeles = FreeImage_GetBits( dib );
			GLubyte* textura = new GLubyte[4*w*h];

			for( uint32_t j = 0; j < w * h; j++ )
			{
				textura[j*4+0] = pixeles[j*4+2];
				textura[j*4+1] = pixeles[j*4+1];
				textura[j*4+2] = pixeles[j*4+0];
				textura[j*4+3] = pixeles[j*4+3];
			}

			// Create texture.
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura);

			// Release FreeImage's copy of the image
			FreeImage_Unload( dib );

			// delete our converted copy of the texture pixels
			delete [] textura;
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);

		// Add texture to media library
		Textures.push_back( texture );

		// Return texture pointer
		return texture;
	}

	Ovgl::Texture* Ovgl::MediaLibrary::ImportTexture( const std::string& file )
	{
		struct stat stFileInfo; 
		int intStat = stat(file.c_str(), &stFileInfo); 
		if(intStat == 0)
		{ 
			// Image format
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			// Check the file signature and deduce its format
			fif = FreeImage_GetFileType( file.c_str(), 0 );

			// If still unknown, try to guess the file format from the file extension
			if( fif == FIF_UNKNOWN )
			{
				fif = FreeImage_GetFIFFromFilename( file.c_str() );
			}

			// If still unkown, return NULL
			if( fif == FIF_UNKNOWN )
			{
				return NULL;
			}

			// Load texture
			FIBITMAP* dib = FreeImage_Load( fif, file.c_str() );
		
			// Check if file was loaded
			if(dib == NULL)
			{
				return NULL;
			}

			// Get raw data and dimensions
			uint32_t w = FreeImage_GetWidth( dib );
			uint32_t h = FreeImage_GetHeight( dib );

			// Check if texture is valid.
			if( w == 0 && h == 0 )
			{
				return NULL;
			}

			// Create new texture
			Ovgl::Texture* texture = new Ovgl::Texture;

			// Set the texture's media library handle to this media library
			texture->MLibrary = this;

			// Set the texture's file name.
			texture->File = file;

			texture->HasAlpha = !!FreeImage_IsTransparent( dib );

			// Convert to RGB format
			dib = FreeImage_ConvertTo32Bits( dib );

			BYTE* pixeles = FreeImage_GetBits( dib );
			GLubyte* textura = new GLubyte[4*w*h];

			for( uint32_t j = 0; j < w * h; j++ )
			{
				textura[j*4+0] = pixeles[j*4+2];
				textura[j*4+1] = pixeles[j*4+1];
				textura[j*4+2] = pixeles[j*4+0];
				textura[j*4+3] = pixeles[j*4+3];
			}
		
			// Create OpenGL texture
			glGenTextures( 1, &texture->Image );
			glBindTexture( GL_TEXTURE_2D, texture->Image );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, NULL );

			// Release FreeImage's copy of the image
			FreeImage_Unload( dib );

			// delete our converted copy of the texture pixels
			delete [] textura;

			// Add texture to media library
			Textures.push_back( texture );

			// Return texture pointer
			return texture;
		}
		else
		{
			// File does not exist!
			return NULL;
		} 
	}

	Ovgl::Shader* Ovgl::MediaLibrary::ImportShader( const std::string& file )
	{
		Ovgl::Shader* shader = new Ovgl::Shader;
		shader->MLibrary = this;
		
		// Define debugging variables
		CGerror error;
		const char* string;
	
		// Create vertex program
		shader->VertexProgram = cgCreateProgramFromFile( Inst->CgContext, CG_SOURCE, file.c_str(), Inst->CgVertexProfile, "VS", NULL );
		string = cgGetLastErrorString(&error);
		if (error != CG_NO_ERROR)
		{
			OutputDebugStringA( string );
		}

		// Load vertex program
		cgGLLoadProgram( shader->VertexProgram );
		string = cgGetLastErrorString(&error);
		if (error != CG_NO_ERROR)
		{
			OutputDebugStringA( string );
		}

		// Create fragment program
		shader->FragmentProgram = cgCreateProgram( Inst->CgContext, CG_SOURCE, file.c_str(), Inst->CgFragmentProfile, "FS", NULL );
		string = cgGetLastErrorString(&error);
		if (error != CG_NO_ERROR)
		{
			OutputDebugStringA( string );
		}

		// Load vertex program
		cgGLLoadProgram( shader->FragmentProgram );
		string = cgGetLastErrorString(&error);
		if (error != CG_NO_ERROR)
		{
			OutputDebugStringA( string );
		}

		shader->GeometryProgram = NULL;

		//Add Effect to array
		Shaders.push_back( shader );

		//Return effect.
		return shader;
	}

	Ovgl::Scene* Ovgl::MediaLibrary::CreateScene()
	{
		Ovgl::Scene* scene = new Ovgl::Scene;
		scene->Inst = Inst;
		scene->SkyBox = NULL;
		scene->DynamicsWorld = new btDiscreteDynamicsWorld(Inst->PhysicsDispatcher,Inst->PhysicsBroadphase,Inst->PhysicsSolver,Inst->PhysicsConfiguration);
		scene->DynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.00001f;
		scene->DynamicsWorld->setGravity(btVector3( 0.0f, -9.8f, 0.0f ));
		GLDebugDrawer* Drawer = new GLDebugDrawer;
		Drawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe );
		scene->DynamicsWorld->setDebugDrawer( Drawer );
		btContactSolverInfo& info = scene->DynamicsWorld->getSolverInfo();
		info.m_numIterations = 20;
		Scenes.push_back(scene);
		return scene;
	};

	Ovgl::Material* Ovgl::MediaLibrary::CreateMaterial( )
	{
		Ovgl::Material* material = new Ovgl::Material;
		material->MLibrary = this;
		material->ShaderProgram = Inst->DefaultMedia->Shaders[0];
		material->NoZBuffer = false;
		material->NoZWrite = false;
		material->PostRender = false;
		material->setFSTexture("txDiffuse", Inst->DefaultMedia->Textures[0] );
		material->setFSTexture("txEnvironment", Inst->DefaultMedia->Textures[1] );
		Materials.push_back(material);
		return material;
	};

	Ovgl::Texture* Ovgl::MediaLibrary::CreateTexture( uint32_t width, uint32_t height )
	{
		// Create new texture
		Ovgl::Texture* texture = new Ovgl::Texture;

		// Set the texture's media library handle to this media library
		texture->MLibrary = this;

		GLubyte* textura = new GLubyte[4*width*height];

		for( uint32_t j = 0; j < width * height; j++ )
		{
			textura[j*4+0] = 255;
			textura[j*4+1] = 255;
			textura[j*4+2] = 255;
			textura[j*4+3] = 255;
		}

		// Create OpenGL texture
		glGenTextures( 1, &texture->Image );
		glBindTexture( GL_TEXTURE_2D, texture->Image );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, NULL );

		// Add texture to media library
		Textures.push_back( texture );

		return texture;
	};

	Ovgl::Texture* Ovgl::MediaLibrary::CreateCubemap( uint32_t width, uint32_t height )
	{
		// Create new texture
		Ovgl::Texture* texture = new Ovgl::Texture;

		// Set the texture's media library handle to this media library
		texture->MLibrary = this;

		GLubyte* textura = new GLubyte[4*width*height];

		for( uint32_t j = 0; j < width * height; j++ )
		{
			textura[j*4+0] = 255;
			textura[j*4+1] = 255;
			textura[j*4+2] = 255;
			textura[j*4+3] = 255;
		}

		// Create OpenGL texture
		glGenTextures( 1, &texture->Image );
		glBindTexture( GL_TEXTURE_CUBE_MAP, texture->Image );

		for (int i = 0; i < 6; i++)
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);

		// Add texture to media library
		Textures.push_back( texture );

		return texture;
	};

	Ovgl::AudioBuffer* Ovgl::MediaLibrary::ImportOGG( const std::string& file )
	{
		Ovgl::AudioBuffer* buffer = new Ovgl::AudioBuffer;
		buffer->Inst = Inst;
		OggVorbis_File info;
		FILE* f = NULL;
		fopen_s( &f, file.c_str(), "rb" );
		if ( f == NULL )
		{
			std::wstring wfile;
			wfile = L"Ovgl::MediaLibrary::ImportOGG was unable to open the file ";
			wfile.append( file.begin(), file.end() );
			OutputDebugString( wfile.c_str() );
			return NULL;
		}
		ov_open_callbacks(f, &info, NULL, 0, OV_CALLBACKS_DEFAULT);
		vorbis_info *vi = ov_info(&info, -1);
	
		if (vi->channels == 1)
		{
			buffer->format = AL_FORMAT_MONO16;
		}
		else
		{
			buffer->format = AL_FORMAT_STEREO16;
		}

		buffer->frequency = vi->rate;

		buffer->data.resize( vi->channels * (uint32_t)ov_pcm_total( &info, -1 ));
		uint32_t pos = 0;
		int sec = 0;
		int ret = 1;
		while( ret && pos < (buffer->data.size()*2) )
		{
			ret = ov_read( &info, (char*)&buffer->data[0] + pos, (buffer->data.size()*2) - pos, 0, 2, 1, &sec );
			pos += ret;
		}
		fclose( f );

		if( buffer->format == AL_FORMAT_MONO16 )
		{
			alGenBuffers( 1, &buffer->mono );
			alBufferData( buffer->mono, AL_FORMAT_MONO16, (ALvoid*)&buffer->data[0], buffer->data.size()*2, buffer->frequency );
		}
		else
		{
			alGenBuffers( 1, &buffer->stereo );
			alBufferData( buffer->stereo, AL_FORMAT_STEREO16, (ALvoid*)&buffer->data[0], buffer->data.size()*2, buffer->frequency );
			std::vector< signed short > mono(buffer->data.size() / 2);
			for (uint32_t i = 0; i < mono.size(); i++)
			{
				mono[i] = (buffer->data[2*i] + buffer->data[2*i+1]) / 2;
			}
			alGenBuffers( 1, &buffer->mono );
			alBufferData( buffer->mono, AL_FORMAT_MONO16, (ALvoid*)&mono[0], mono.size()*2, buffer->frequency );
		}
		AudioBuffers.push_back(buffer);
		return buffer;
	};
}