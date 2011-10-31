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
* @brief None.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"

void Ovgl::MediaLibrary::Release()
{
	for( DWORD i = 0; i < AudioBuffers.size(); i++ )
	{
		AudioBuffers[i]->Release();
	}
	for( DWORD i = 0; i < Materials.size(); i++ )
	{
		Materials[i]->Release();
	}
	for( DWORD i = 0; i < Meshes.size(); i++ )
	{
		Meshes[i]->Release();
	}
	for( DWORD i = 0; i < Scenes.size(); i++ )
	{
		Scenes[i]->Release();
	}
	for( DWORD i = 0; i < Shaders.size(); i++ )
	{
		Shaders[i]->Release();
	}
	for( DWORD i = 0; i < Textures.size(); i++ )
	{
		Textures[i]->Release();
	}
	delete this;
}

void Ovgl::MediaLibrary::Save( const std::string& file )
{
	if(!file.empty())
	{
		// Open file.
		FILE *output;
		fopen_s(&output, file.c_str(),"wb");
	
		// Write the number of meshes to the file.
		DWORD mesh_count = Meshes.size();
		fwrite( &mesh_count, 4, 1, output ); 
		for( DWORD m = 0; m < mesh_count; m++ )
		{
			// Get mesh variables.
			DWORD vertex_count = Meshes[m]->vertices.size();
			DWORD face_count = Meshes[m]->faces.size();
			DWORD bone_count = Meshes[m]->bones.size();
	
			// Write the number of vertices, faces, and bones.
			fwrite( &vertex_count, 4, 1, output );
			fwrite( &face_count, 4, 1, output );
			fwrite( &bone_count, 4, 1, output );
			
			// Write the vertices, faces, and bones.
			fwrite( &Meshes[m]->vertices[0], sizeof(Ovgl::Vertex), vertex_count, output);
			fwrite( &Meshes[m]->faces[0], sizeof(Ovgl::Face), face_count, output);
			fwrite( &Meshes[m]->attributes[0], sizeof(DWORD), face_count, output);

			// Write bones.
			for( DWORD i = 0; i < bone_count; i++ )
			{
				vertex_count = Meshes[m]->bones[i]->mesh->vertices.size();
				face_count = Meshes[m]->bones[i]->mesh->faces.size();
				fwrite( &vertex_count, 4, 1, output );
				fwrite( &face_count, 4, 1, output );
				if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it. 
				{
					fwrite( &Meshes[m]->bones[i]->mesh->vertices[0], sizeof(Ovgl::Vertex), vertex_count, output );
					fwrite( &Meshes[m]->bones[i]->mesh->faces[0], sizeof(Ovgl::Face), face_count, output );
				}
				fwrite( &Meshes[m]->bones[i]->matrix, sizeof(Ovgl::Matrix44), 1, output );
				fwrite( &Meshes[m]->bones[i]->length, sizeof(float), 1, output );
				fwrite( &Meshes[m]->bones[i]->min, sizeof(Ovgl::Vector3), 1, output );
				fwrite( &Meshes[m]->bones[i]->max, sizeof(Ovgl::Vector3), 1, output );
				fwrite( &Meshes[m]->bones[i]->parent, sizeof(DWORD), 1, output );
				DWORD child_count = Meshes[m]->bones[i]->childen.size();
				fwrite( &child_count, sizeof(DWORD), 1, output );
				if(child_count) fwrite( &Meshes[m]->bones[i]->childen[0], sizeof(DWORD), child_count, output );
			}
		}
		DWORD scene_count = Scenes.size();
		fwrite( &scene_count, 4, 1, output ); 
		for( DWORD s = 0; s < scene_count; s++ )
		{
			DWORD prop_count = Scenes[s]->props.size();
			fwrite( &prop_count, 4, 1, output );
			for( DWORD p = 0; p < Scenes[s]->props.size(); p++ )
			{
				Ovgl::Matrix44 matrix = Scenes[s]->props[p]->getPose();
				fwrite( &matrix, sizeof(Ovgl::Matrix44), 1, output );
				DWORD mesh_index = 0;
				for( DWORD sm = 0; sm < Meshes.size(); sm++ )
				{
					if( Scenes[s]->props[p]->mesh == Meshes[sm] )
					{
						mesh_index = sm;
					}
				}
				fwrite( &mesh_index, 4, 1, output );
				for( DWORD b = 0; b < Scenes[s]->props[p]->bones.size(); b++ )
				{
					DWORD bodyflags = Scenes[s]->props[p]->bones[b]->GetFlags();
					fwrite( &bodyflags, 4, 1, output );
				}
			}
			DWORD object_count = Scenes[s]->objects.size();
			fwrite( &object_count, 4, 1, output );
			for( DWORD o = 0; o < Scenes[s]->objects.size(); o++ )
			{
				Ovgl::Matrix44 matrix = Scenes[s]->objects[o]->getPose();
				fwrite( &matrix, sizeof(Ovgl::Matrix44), 1, output );
				DWORD mesh_index = 0;
				for( DWORD sm = 0; sm < Meshes.size(); sm++ )
				{
					if( Scenes[s]->objects[o]->mesh == Meshes[sm] )
					{
						mesh_index = sm;
					}
				}
				fwrite( &mesh_index, 4, 1, output );
				DWORD bodyflags = Scenes[s]->objects[o]->CollisionMesh->GetFlags();
				fwrite( &bodyflags, 4, 1, output );
			}
			DWORD light_count = Scenes[s]->lights.size();
			fwrite( &light_count, 4, 1, output );
			for( DWORD l = 0; l < light_count; l++ )
			{
				Ovgl::Matrix44 matrix = Scenes[s]->lights[l]->getPose();
				fwrite( &matrix, sizeof(Ovgl::Matrix44), 1, output );
			}
			DWORD camera_count = Scenes[s]->cameras.size();
			fwrite( &camera_count, 4, 1, output );
			for( DWORD c = 0; c < camera_count; c++ )
			{
				Ovgl::Matrix44 matrix = Scenes[s]->cameras[c]->getPose();
				fwrite( &matrix, sizeof(Ovgl::Matrix44), 1, output );
			}
			//DWORD joint_count = Scenes[s]->joints.size();
			//fwrite( &joint_count, 4, 1, output );
			//for( DWORD j = 0; j < joint_count; j++ )
			//{
			//	//Scenes[s]->joints[j]->obj[0];
			//	//Scenes[s]->joints[j]->obj[1];

			//	NxVec3 nxAnchor = Scenes[s]->joints[j]->joint->getGlobalAnchor();
			//	Ovgl::Vector3 anchor = Ovgl::Vector3(nxAnchor.x, nxAnchor.y, nxAnchor.z);
			//	fwrite( &anchor, sizeof(Ovgl::Vector3), 1, output );
			//}
		}
		DWORD texture_count = Textures.size();
		fwrite( &texture_count, 4, 1, output );


		// Close file.
		fclose(output);
	}
}

void Ovgl::MediaLibrary::Load( const std::string& file )
{
	if(!file.empty())
	{
		// Get the number of meshes currently in memory so we can offset the indices into the array.
		DWORD mesh_offset = Meshes.size();
	
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
		DWORD mesh_count;
		fread( &mesh_count, 4, 1, input );
		for( DWORD m = 0; m < mesh_count; m++ )
		{
			// Specify mesh variables.
			Ovgl::Mesh* mesh = new Ovgl::Mesh;
			mesh->ml = this;
			DWORD vertex_count;
			DWORD face_count;
			DWORD bone_count;
			DWORD child_count;
			std::vector< Ovgl::Vector3 > bone_vertices;
			std::vector< Ovgl::Face > bone_faces;
	
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
			fread(&mesh->vertices[0], sizeof(Ovgl::Vertex), vertex_count, input);
			fread(&mesh->faces[0], sizeof(Ovgl::Face), face_count, input);
			fread(&mesh->attributes[0], sizeof(DWORD), face_count, input);

			// Load bones.
			for( DWORD i = 0; i < bone_count; i++ )
			{
				mesh->bones[i] = new Ovgl::Bone;
				mesh->bones[i]->mesh = new Ovgl::Mesh;
				mesh->bones[i]->convex = NULL;
				fread( &vertex_count, 4, 1, input );
				fread( &face_count, 4, 1, input );
				if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it. 
				{
					mesh->bones[i]->mesh->vertices.resize(vertex_count);
					mesh->bones[i]->mesh->faces.resize(face_count);
					fread( &mesh->bones[i]->mesh->vertices[0], sizeof(Ovgl::Vertex), vertex_count, input );
					fread( &mesh->bones[i]->mesh->faces[0], sizeof(Ovgl::Face), face_count, input );
				}
				fread( &mesh->bones[i]->matrix, sizeof(Ovgl::Matrix44), 1, input );
				fread( &mesh->bones[i]->length, sizeof(float), 1, input );
				fread( &mesh->bones[i]->min, sizeof(Ovgl::Vector3), 1, input );
				fread( &mesh->bones[i]->max, sizeof(Ovgl::Vector3), 1, input );
				fread( &mesh->bones[i]->parent, sizeof(DWORD), 1, input );
				fread( &child_count, sizeof(DWORD), 1, input );
				mesh->bones[i]->childen.resize(child_count);
				if(child_count) fread( &mesh->bones[i]->childen[0], sizeof(DWORD), child_count, input );
			}
	
			// Nullify buffer addresses.
			mesh->VertexBuffer = 0;
			mesh->IndexBuffers = 0;

			// Update buffers.
			mesh->Update();

			Meshes.push_back(mesh);
		}

		//Get number of scenes.
		DWORD scene_count;
		fread( &scene_count, 4, 1, input );
		for( DWORD s = 0; s < scene_count; s++ )
		{
			Ovgl::Scene* scene = CreateScene();

			DWORD prop_count;
			fread( &prop_count, 4, 1, input );
			for( DWORD p = 0; p < prop_count; p++ )
			{
				// Get the pose of this prop.
				Ovgl::Matrix44 matrix;
				fread( &matrix, sizeof(Ovgl::Matrix44), 1, input );
				DWORD mesh_index;
				fread( &mesh_index, 4, 1, input );
				Ovgl::Prop* prop = scene->CreateProp( Meshes[mesh_index + mesh_offset], &matrix );
				for( DWORD b = 0; b < Meshes[mesh_index + mesh_offset]->bones.size(); b++ )
				{
					DWORD bone_flags;
					fread( &bone_flags, 4, 1, input );
					prop->bones[b]->SetFlags(bone_flags);
				}
			}

			DWORD object_count;
			fread( &object_count, 4, 1, input );
			for( DWORD o = 0; o < object_count; o++ )
			{
				// Get the pose of this object.
				Ovgl::Matrix44 matrix;
				fread( &matrix, sizeof(Ovgl::Matrix44), 1, input );
				DWORD mesh_index;
				fread( &mesh_index, 4, 1, input );
				Ovgl::Object* object = scene->CreateObject( Meshes[mesh_index + mesh_offset], &matrix );
				DWORD bone_flags;
				fread( &bone_flags, 4, 1, input );
				object->CollisionMesh->SetFlags(bone_flags);
			}

			DWORD light_count;
			fread( &light_count, 4, 1, input );
			for( DWORD l = 0; l < light_count; l++ )
			{
				// Get the pose of this light.
				Ovgl::Matrix44 matrix;
				fread( &matrix, sizeof(Ovgl::Matrix44), 1, input );
				scene->CreateLight( &matrix, &Ovgl::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
			}

			DWORD camera_count;
			fread( &camera_count, 4, 1, input );
			for( DWORD c = 0; c < camera_count; c++ )
			{
				// Get the pose of this camera.
				Ovgl::Matrix44 matrix;
				fread( &matrix, sizeof(Ovgl::Matrix44), 1, input );
				scene->CreateCamera( &matrix );
			}
		}
		// Close file.
		fclose(input);
	}
}

Ovgl::Mesh* Ovgl::MediaLibrary::ImportFBX( const std::string& file, bool GenerateBoneShapes, bool GeneratePVS )
{
	if(!file.empty())
	{
		Ovgl::Mesh* mesh = new Ovgl::Mesh;
		mesh->ml = this;
		KFbxImporter* FBXImporter;
		KFbxScene* FBXScene;
		FBXImporter = KFbxImporter::Create( (KFbxSdkManager*)Inst->FBXManager, "" );
		FBXScene = KFbxScene::Create( (KFbxSdkManager*)Inst->FBXManager, "" );
		FBXImporter->Initialize( file.c_str(), -1, ((KFbxSdkManager*)Inst->FBXManager)->GetIOSettings() );
		FBXImporter->Import( FBXScene );
		KFbxAnimStack* AnimationStack = KFbxGetSrc<KFbxAnimStack>( FBXScene, 0 );
		KFbxAnimLayer* lCurrentAnimationLayer;
		if(AnimationStack)
		{
			lCurrentAnimationLayer = KFbxGetSrc<KFbxAnimStack>( FBXScene, 0 )->GetMember(FBX_TYPE(KFbxAnimLayer), 0);
		}
		for(int n = 1; n < FBXScene->GetNodeCount(); n++)
		{
			KFbxNodeAttribute::EAttributeType AttributeType = FBXScene->GetNode(n)->GetNodeAttribute()->GetAttributeType();
			if ( AttributeType == KFbxNodeAttribute::eMESH )
			{
				DWORD VertexOffset = mesh->vertices.size();
				DWORD AttributeOffset = 0;
				for( DWORD i = 0; i < mesh->faces.size(); i++ )
				{
					if( mesh->attributes[i] > AttributeOffset )
					{
						AttributeOffset = mesh->attributes[i];
					}
				}
				KFbxNode* FBXNode = FBXScene->GetNode(n);
				Ovgl::Matrix44 matrix;
				KFbxVector4 localT, localR, localS;
				KFbxMesh* FBXMesh = (KFbxMesh*) FBXNode->GetNodeAttribute();
				localT = FBXNode->LclTranslation.Get();
				localR = FBXNode->LclRotation.Get();
				localS = FBXNode->LclScaling.Get();
				matrix = Ovgl::MatrixScaling( (float)localS[0], (float)localS[2], (float)localS[1] ) * Ovgl::MatrixRotationEuler( Ovgl::DegToRad((float)localR[2]), Ovgl::DegToRad((float)localR[0]), Ovgl::DegToRad((float)localR[1]) )  * Ovgl::MatrixTranslation( (float)localT[0], (float)localT[1], (float)localT[2] );
				KFbxSkin *FBXSkin = (KFbxSkin*)FBXMesh->GetDeformer(0);
				int ControlPointCount = FBXMesh->GetControlPointsCount();
				KFbxVector4* ControlPoints = FBXMesh->GetControlPoints();
				KFbxLayerElementUV* FBXLayerUVs = FBXMesh->GetLayer(0)->GetUVs();
				KFbxLayerElementMaterial* FBXLayerMats = FBXMesh->GetLayer(0)->GetMaterials();
				Ovgl::Vertex ZeroVertex;
				std::vector< std::vector< float > > weights(ControlPointCount);
				std::vector< std::vector< float > > indices(ControlPointCount);
				std::vector< Ovgl::Face > faces;
				std::vector< DWORD > attributes;
				std::vector< KFbxNode* > BoneNodes;
				mesh->vertices.resize( VertexOffset + ControlPointCount);

				// Get textures
				int lNbMat = FBXMesh->GetNode()->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
				for (int lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++)
				{
					KFbxSurfaceMaterial *lMaterial = (KFbxSurfaceMaterial *)FBXMesh->GetNode()->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex);
					Ovgl::Material* material = CreateMaterial();
					mesh->materials.push_back( material );
					if(lMaterial)
					{
						int lTextureIndex;
						FOR_EACH_TEXTURE(lTextureIndex)
						{
							KFbxProperty lProperty = lMaterial->FindProperty(KFbxLayerElement::TEXTURE_CHANNEL_NAMES[lTextureIndex]);
							if( lProperty.IsValid() )
							{
								KFbxTexture* lTexture = KFbxCast <KFbxTexture> (lProperty.GetSrcObject(KFbxTexture::ClassId, 0));
								if(lTexture)
								{
									KFbxFileTexture *lFileTexture = KFbxCast<KFbxFileTexture>(lTexture);
									std::string filename = lFileTexture->GetFileName();
									Ovgl::Texture* texFound = NULL;
									for ( unsigned int tex = 0; tex < Textures.size()-1; tex++)
									{
										if( filename.compare( Textures[tex]->File ) == 0 )
										{
											texFound = Textures[tex];
										}
									}
									if( texFound != NULL )
									{
										material->setFSTexture("txDiffuse", texFound);
									}
									else
									{
										texFound = ImportTexture(filename.c_str());
										material->setFSTexture("txDiffuse", texFound);
									}

									if(texFound->HasAlpha)
									{
										material->PostRender = true;
										material->NoZWrite = true;
									}
								}
							}
						}
					}
				}

				if(FBXSkin)
				{
					for (int c = 0; c < FBXSkin->GetClusterCount(); c++)
					{
						KFbxCluster* FBXCluster = FBXSkin->GetCluster(c);
						KFbxCluster::ELinkMode lClusterMode = FBXCluster->GetLinkMode();
						KFbxNode* FBXLinkNode = FBXCluster->GetLink();
						BoneNodes.push_back(FBXLinkNode);
						int CPIndexCount = FBXCluster->GetControlPointIndicesCount();
						int* CPIndices = FBXCluster->GetControlPointIndices();
						double* CPWeights = FBXCluster->GetControlPointWeights();
						Ovgl::Bone* bone = new Ovgl::Bone;
						bone->max.fromDoubles((double*)&FBXLinkNode->RotationMax.Get());
						bone->min.fromDoubles((double*)&FBXLinkNode->RotationMin.Get());
						KFbxXMatrix FBXLinkMatrix;
						FBXCluster->GetTransformLinkMatrix(FBXLinkMatrix);
						bone->matrix.fromDoubles( (double*)FBXLinkMatrix.Double44() );
						bone->matrix = (Ovgl::MatrixRotationZ(1.57f) * bone->matrix * Ovgl::MatrixRotationX(1.57f))* matrix;
						bone->length = 1.0f;
						bone->mesh = new Ovgl::Mesh;
						bone->convex = NULL;
						for(int i = 0; i < FBXLinkNode->GetChildCount(); i++)
						{
							for (int cc = 0; cc < FBXSkin->GetClusterCount(); cc++)
							{
								if(FBXSkin->GetCluster(cc)->GetLink() == FBXLinkNode->GetChild(i))
								{
									bone->childen.push_back(cc);
								}
							}
						}
						for (int cc = 0; cc < FBXSkin->GetClusterCount(); cc++)
						{
							if(FBXSkin->GetCluster(cc)->GetLink() == FBXLinkNode->GetParent())
							{
								bone->parent = cc;
							}
						}
						mesh->bones.push_back(bone);
						for (int v = 0 ; v < CPIndexCount ; v++)
						{
							weights[CPIndices[v]].push_back((float)CPWeights[v]);
							indices[CPIndices[v]].push_back((float)c);
						}
					}
				}

				for (int w = 0; w < ControlPointCount; w++)
				{
					weights[w].resize(4);
					indices[w].resize(4);
					if(!FBXSkin)
						weights[w][0] = 1.0f;
				}
				for( int p = 0; p < FBXMesh->GetPolygonCount(); p++ )
				{
					DWORD FaceIndices[4];
					for( int i = 0; i < FBXMesh->GetPolygonSize(p); i++ )
					{
						int vi = FBXMesh->GetPolygonVertex( p, i );
						Ovgl::Vertex vertex;
						KFbxVector4 normal;
						KFbxVector2 uv;
						FBXMesh->GetPolygonVertexNormal( p, i, normal );
						vertex.position.x = (float)ControlPoints[vi][0];
						vertex.position.y = (float)ControlPoints[vi][1];
						vertex.position.z = (float)ControlPoints[vi][2];
						vertex.position = Vector3Transform(&vertex.position, &(Ovgl::MatrixRotationX(1.57f) * matrix));
						vertex.normal.x = (float)normal[0];
						vertex.normal.y = (float)normal[1];
						vertex.normal.z = (float)normal[2];
						Ovgl::Matrix44 rotMat = matrix;
						rotMat._41 = 0; rotMat._42 = 0; rotMat._43 = 0;
						vertex.normal = Vector3Transform(&vertex.normal, &(Ovgl::MatrixRotationX(1.57f) * rotMat));
						if( FBXLayerUVs )
						{
							int MappingMode = FBXLayerUVs->GetMappingMode();
							int ReferenceMode = FBXLayerUVs->GetReferenceMode();
							if( MappingMode == KFbxLayerElement::eBY_CONTROL_POINT )
							{
								if( ReferenceMode == KFbxLayerElement::eDIRECT )
								{
									uv = FBXLayerUVs->GetDirectArray().GetAt(vi);
								}
								else if( ReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT )
								{
									int id = FBXLayerUVs->GetIndexArray().GetAt(vi);
									uv = FBXLayerUVs->GetDirectArray().GetAt(id);
								}
							}
							else if( MappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX )
							{
								int lTextureUVIndex = FBXMesh->GetTextureUVIndex(p, i);
								if( ReferenceMode == KFbxLayerElement::eDIRECT || ReferenceMode == KFbxLayerElement::eINDEX_TO_DIRECT )
								{
									uv = FBXLayerUVs->GetDirectArray().GetAt(lTextureUVIndex);
								}
							}
							vertex.texture.x = (float)uv[0];
							vertex.texture.y = (float)uv[1];
						}
						else
						{
							vertex.texture.x = 0.5f;
							vertex.texture.y = 0.5f;
						}
						vertex.weight[0] = weights[vi][0];
						vertex.weight[1] = weights[vi][1];
						vertex.weight[2] = weights[vi][2];
						vertex.weight[3] = weights[vi][3];
						vertex.indices[0] = indices[vi][0];
						vertex.indices[1] = indices[vi][1];
						vertex.indices[2] = indices[vi][2];
						vertex.indices[3] = indices[vi][3];
						if( vertex != mesh->vertices[vi] && mesh->vertices[vi] != ZeroVertex )
						{
							FaceIndices[i] = mesh->vertices.size();
							mesh->vertices.push_back( vertex );
						}
						else
						{
							FaceIndices[i] = (DWORD)vi;
							mesh->vertices[vi] = vertex;
						}
					}
					if(FBXMesh->GetPolygonSize(p) == 4)
					{
						Ovgl::Face face;
						face.indices[0] = FaceIndices[0];
						face.indices[1] = FaceIndices[2];
						face.indices[2] = FaceIndices[3];
						mesh->faces.push_back( face );
						if(FBXLayerMats)
						{
							mesh->attributes.push_back( FBXLayerMats->GetIndexArray().GetAt(p) + AttributeOffset + 1 );
						}
						else
						{
							mesh->attributes.push_back(0);
						}
					}
					Ovgl::Face face;
					face.indices[0] = FaceIndices[0];
					face.indices[1] = FaceIndices[1];
					face.indices[2] = FaceIndices[2];
					mesh->faces.push_back( face );
					if(FBXLayerMats)
					{
						mesh->attributes.push_back( FBXLayerMats->GetIndexArray().GetAt(p) + AttributeOffset + 1 );
					}
					else
					{
						mesh->attributes.push_back(0);
					}
				}
				
				// Get animation curves for this mesh.
				if(AnimationStack)
				{
					for(DWORD bn = 0; bn < BoneNodes.size(); bn++ )
					{	
						mesh->bones[bn]->Rot_Keys.resize(3);
						KFbxAnimCurve* lAnimCurve[3];
						lAnimCurve[0] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_X);
						lAnimCurve[1] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_Y);
						lAnimCurve[2] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_Z);
						for( DWORD c = 0; c < 3; c++ )
						{
							if(lAnimCurve[c])
							{

								for( int k = 0; k < lAnimCurve[c]->KeyGetCount(); k++ )
								{
									KFbxAnimCurveKey lKey = lAnimCurve[c]->KeyGet(k);
									Ovgl::Curve lCurve;
									lCurve.time = (DWORD)lKey.GetTime().GetMilliSeconds();
									lCurve.value = lKey.GetValue();
									mesh->bones[bn]->Rot_Keys[c].push_back(lCurve);
								}
							}
						}
					}
				}
				else
				{
					for(DWORD bn = 0; bn < BoneNodes.size(); bn++ )
					{	
						mesh->bones[bn]->Rot_Keys.resize(3);
						for( DWORD c = 0; c < 3; c++ )
						{
							Ovgl::Curve lCurve;
							lCurve.time = 0;
							lCurve.value = 0.0f;
							mesh->bones[bn]->Rot_Keys[c].push_back(lCurve);
						}
					}
				}
			}
		}
		if(mesh->bones.size() == 0 )
		{
			Ovgl::Bone* bone = new Ovgl::Bone;
			bone->matrix = Ovgl::MatrixIdentity();
			bone->length = 1.0f;
			bone->mesh = new Ovgl::Mesh;
			bone->convex = NULL;
			mesh->bones.push_back(bone);
		}
		if(GenerateBoneShapes)
		{
			for( DWORD i = 0; i < mesh->bones.size(); i++ )
			{
				for( DWORD v = 0; v < mesh->vertices.size(); v++ )
				{
					for( DWORD j = 0; j < 4; j++)
					{
						if(mesh->vertices[v].indices[j] == i && mesh->vertices[v].weight[j] > 0.1f)
						{
							Ovgl::Vertex Vertex;
							Vertex.position = Ovgl::Vector3Transform( &mesh->vertices[v].position, &Ovgl::MatrixInverse( &Ovgl::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), &mesh->bones[i]->matrix));
							Vertex.weight[0] = 1.0f;
							mesh->bones[i]->mesh->vertices.push_back( Vertex );
						}
					}
				}
			}
		}
		mesh->VertexBuffer = 0;
		mesh->IndexBuffers = 0;
		mesh->Update();
		Meshes.push_back( mesh );
		return mesh;
	}
	return NULL;
}

Ovgl::Mesh* Ovgl::MediaLibrary::ImportBSP( const std::string& file )
{
	Ovgl::Mesh* mesh = new Ovgl::Mesh;
	if(!file.empty())
	{
		FILE* f = NULL;
		fopen_s( &f, file.c_str(), "rb" );
		if ( f == NULL )
		{
			std::wstring wfile;
			wfile = L"Ovgl::MediaLibrary::ImportBSP was unable to open the file ";
			wfile.append( file.begin(), file.end() );
			OutputDebugString( wfile.c_str() );
			return NULL;
		}
		fseek(f, 0, SEEK_END);
		DWORD filesize = ftell(f);
		std::vector< char > buffer(filesize);
		fread(&buffer[0], 1, filesize, f);

		return mesh;
	}
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
		DWORD w = FreeImage_GetWidth( dib );
		DWORD h = FreeImage_GetHeight( dib );
		BYTE* pixeles = FreeImage_GetBits( dib );
		GLubyte* textura = new GLubyte[4*w*h];

		for( DWORD j = 0; j < w * h; j++ )
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
		DWORD w = FreeImage_GetWidth( dib );
		DWORD h = FreeImage_GetHeight( dib );

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

		for( DWORD j = 0; j < w * h; j++ )
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
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
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

Ovgl::Shader* Ovgl::MediaLibrary::ImportCG( const std::string& file )
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

Ovgl::Texture* Ovgl::MediaLibrary::CreateTexture( unsigned int width, unsigned int height )
{
	// Create new texture
	Ovgl::Texture* texture = new Ovgl::Texture;

	// Set the texture's media library handle to this media library
	texture->MLibrary = this;

	GLubyte* textura = new GLubyte[4*width*height];

	for( DWORD j = 0; j < width * height; j++ )
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

Ovgl::Texture* Ovgl::MediaLibrary::CreateCubemap( unsigned int width, unsigned int height )
{
	// Create new texture
	Ovgl::Texture* texture = new Ovgl::Texture;

	// Set the texture's media library handle to this media library
	texture->MLibrary = this;

	GLubyte* textura = new GLubyte[4*width*height];

	for( DWORD j = 0; j < width * height; j++ )
	{
		textura[j*4+0] = 255;
		textura[j*4+1] = 255;
		textura[j*4+2] = 255;
		textura[j*4+3] = 255;
	}

	// Create OpenGL texture
	glGenTextures( 1, &texture->Image );
	glBindTexture( GL_TEXTURE_CUBE_MAP, texture->Image );
;
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

	buffer->data.resize( vi->channels * (UINT)ov_pcm_total( &info, -1 ));
	DWORD pos = 0;
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
		for (UINT i = 0; i < mono.size(); i++)
		{
			mono[i] = (buffer->data[2*i] + buffer->data[2*i+1]) / 2;
		}
		alGenBuffers( 1, &buffer->mono );
		alBufferData( buffer->mono, AL_FORMAT_MONO16, (ALvoid*)&mono[0], mono.size()*2, buffer->frequency );
	}
	AudioBuffers.push_back(buffer);
	return buffer;
};