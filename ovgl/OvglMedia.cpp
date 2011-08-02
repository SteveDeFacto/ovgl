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

bool FrameCompare ( Ovgl::Frame* frame1, Ovgl::Frame* frame2 )
{
   return frame1->time < frame2->time;
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

			// Write animations.
			DWORD frame_count = Meshes[m]->keyframes.size();
			fwrite( &frame_count, 4, 1, output );
			for( DWORD f = 0; f < frame_count; f++ )
			{
				fwrite( &Meshes[m]->keyframes[f]->time, sizeof(DWORD), 1, output );
				DWORD key_count = Meshes[m]->keyframes[f]->keys.size();
				fwrite( &key_count, sizeof(DWORD), 1, output );
				if( key_count > 0)
				{
					fwrite( &Meshes[m]->keyframes[f]->keys[0], sizeof(Ovgl::Key), key_count, output );
				}
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
				DWORD bodyflags = Scenes[s]->objects[o]->cmesh->GetFlags();
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
			//	Ovgl::Vector3 anchor = Ovgl::Vector3Set(nxAnchor.x, nxAnchor.y, nxAnchor.z);
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

		// If file was unable to be opened present error message to debug output and 
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
			mesh->Inst = Inst;
			DWORD vertex_count;
			DWORD face_count;
			DWORD bone_count;
			DWORD child_count;
			DWORD frame_count;
			DWORD key_count;
			std::vector<Ovgl::Vector3> bone_vertices;
			std::vector<Ovgl::Face> bone_faces;
	
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
	
			// Get animations.
			fread( &frame_count, 4, 1, input ); 
			mesh->keyframes.resize( frame_count );
			for( DWORD f = 0; f < frame_count; f++ )
			{
				mesh->keyframes[f] = new Frame;
				fread( &mesh->keyframes[f]->time, sizeof(DWORD), 1, input );
				fread( &key_count, sizeof(DWORD), 1, input );
				if( key_count > 0)
				{
					mesh->keyframes[f]->keys.resize(key_count);
					fread( &mesh->keyframes[f]->keys[0], sizeof(Ovgl::Key), key_count, input );
				}
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
				object->cmesh->SetFlags(bone_flags);
			}

			DWORD light_count;
			fread( &light_count, 4, 1, input );
			for( DWORD l = 0; l < light_count; l++ )
			{
				// Get the pose of this light.
				Ovgl::Matrix44 matrix;
				fread( &matrix, sizeof(Ovgl::Matrix44), 1, input );
				scene->CreateLight( &matrix, &Ovgl::Vector4Set( 1.0f, 1.0f, 1.0f, 1.0f ) );
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

Ovgl::Mesh* Ovgl::MediaLibrary::ImportFBX( const std::string& file, const std::string& node )
{
	if(!file.empty())
	{
		KFbxImporter* FBXImporter;
		KFbxScene* FBXScene;
		FBXImporter = KFbxImporter::Create( (KFbxSdkManager*)Inst->FBXManager, "" );
		FBXScene = KFbxScene::Create( (KFbxSdkManager*)Inst->FBXManager, "" );
		FBXImporter->Initialize( file.c_str(), -1, ((KFbxSdkManager*)Inst->FBXManager)->GetIOSettings() );
		FBXImporter->Import( FBXScene );
		KFbxAnimLayer* lCurrentAnimationLayer = KFbxGetSrc<KFbxAnimStack>( FBXScene, 0 )->GetMember(FBX_TYPE(KFbxAnimLayer), 0);
		for(int n = 1; n < FBXScene->GetNodeCount(); n++)
		{
			KFbxNodeAttribute::EAttributeType AttributeType = FBXScene->GetNode(n)->GetNodeAttribute()->GetAttributeType();
			if ( AttributeType == KFbxNodeAttribute::eMESH )
			{
				KFbxNode* FBXNode = FBXScene->GetNode(n);
				if( node.compare(FBXNode->GetName()) == 0)
				{
					Ovgl::Matrix44 matrix;
					KFbxVector4 localT, localR, localS;
					KFbxMesh* FBXMesh = (KFbxMesh*) FBXNode->GetNodeAttribute();
					localT = FBXNode->GetParent()->LclTranslation.Get();
					localR = FBXNode->GetParent()->LclRotation.Get();
					localS = FBXNode->GetParent()->LclScaling.Get();
					KFbxSkin *FBXSkin = (KFbxSkin*)FBXMesh->GetDeformer(0);
					if(FBXSkin)
					matrix = Ovgl::MatrixScaling( (float)localS[0], (float)localS[1], (float)localS[2] ) * Ovgl::MatrixRotationEuler( (float)((localR[0] + 90)  * OvglPi / 180), -(float)(localR[2] * OvglPi / 180), (float)(( localR[1] + 90) * OvglPi / 180) )  * Ovgl::MatrixTranslation( (float)localT[0], (float)localT[1], (float)localT[2] );
					else
					matrix = Ovgl::MatrixScaling( (float)localS[0], (float)localS[1], (float)localS[2] ) * Ovgl::MatrixRotationEuler( (float)((localR[0])  * OvglPi / 180), -(float)(localR[2] * OvglPi / 180), (float)(( localR[1] + 90) * OvglPi / 180) )  * Ovgl::MatrixTranslation( (float)localT[0], (float)localT[1], (float)localT[2] );
					int ControlPointCount = FBXMesh->GetControlPointsCount();
					KFbxVector4* ControlPoints = FBXMesh->GetControlPoints();
					KFbxLayerElementUV* FBXLayerUVs = FBXMesh->GetLayer(0)->GetUVs();
					KFbxLayerElementMaterial* FBXLayerMats = FBXMesh->GetLayer(0)->GetMaterials();
					Ovgl::Vertex ZeroVertex = {0};
					std::vector<Ovgl::Vertex> vertices(ControlPointCount);
					std::vector<std::vector<float>> weights(ControlPointCount);
					std::vector<std::vector<float>> indices(ControlPointCount);
					std::vector<Ovgl::Face> faces;
					std::vector<DWORD> attributes;
					std::vector<KFbxNode*> BoneNodes;
					Ovgl::Mesh* mesh = new Ovgl::Mesh;
					mesh->Inst = Inst;
					mesh->vertices.resize(ControlPointCount);
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
							bone->matrix = Ovgl::MatrixRotationZ(1.57f) * bone->matrix * Ovgl::MatrixRotationX(1.57f) * Ovgl::MatrixRotationY(1.57f);
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
					else
					{
						Ovgl::Bone* bone = new Ovgl::Bone;
						bone->matrix = Ovgl::MatrixIdentity();
						bone->length = 1.0f;
						bone->mesh = new Ovgl::Mesh;
						bone->convex = NULL;
						mesh->bones.push_back(bone);
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
							Ovgl::Vertex vertex = {0};
							KFbxVector4 normal;
							KFbxVector2 uv;
							FBXMesh->GetPolygonVertexNormal( p, i, normal );
							vertex.position.x = (float)ControlPoints[vi][1];
							vertex.position.y = (float)ControlPoints[vi][2];
							vertex.position.z = (float)ControlPoints[vi][0];
							vertex.normal.x = (float)normal[1];
							vertex.normal.y = (float)normal[2];
							vertex.normal.z = (float)normal[0];
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
								mesh->attributes.push_back(FBXLayerMats->GetIndexArray().GetAt(p));
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
							mesh->attributes.push_back(FBXLayerMats->GetIndexArray().GetAt(p));
						}
						else
						{
							mesh->attributes.push_back(0);
						}
					}

					// Get animation frames for this mesh.
					for(DWORD bn = 0; bn < BoneNodes.size(); bn++ )
					{	
						KFbxAnimCurve* lAnimCurve[3];
						lAnimCurve[0] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_X);
						lAnimCurve[1] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_Y);
						lAnimCurve[2] = BoneNodes[bn]->LclRotation.GetCurve<KFbxAnimCurve>(lCurrentAnimationLayer, KFCURVENODE_R_Z);
						for( DWORD c = 0; c < 3; c++ )
						{
							for( DWORD k = 0; k < (DWORD)lAnimCurve[c]->KeyGetCount(); k++ )
							{
								KFbxAnimCurveKey lKey = lAnimCurve[c]->KeyGet(k);
								DWORD lKeyTime = (DWORD)lKey.GetTime().GetMilliSeconds();
								float lKeyValue = lKey.GetValue();
								bool found_frame = false;
								for( DWORD f = 0; f < mesh->keyframes.size(); f++ )
								{
									if( lKeyTime == mesh->keyframes[f]->time )
									{
										found_frame = true;
										bool found_key = false;
										for( DWORD k2 = 0; k2 < mesh->keyframes[f]->keys.size(); k2++ )
										{
											if( mesh->keyframes[f]->keys[k2].index == bn )
											{
												found_key = true;
												mesh->keyframes[f]->keys[k2].rotation[c] = lKeyValue;
											}
										}
										if( !found_key )
										{
											Ovgl::Key key;
											key.index = bn;
											ZeroMemory( &key.rotation, sizeof( Ovgl::Vector4 ) );
											key.rotation[c] = lKeyValue;
											mesh->keyframes[f]->keys.push_back(key);
										}
									}
								}
								if( !found_frame )
								{
									Ovgl::Frame* frame = new Ovgl::Frame;
									frame->time = lKeyTime;
									frame->keys.clear();
									mesh->keyframes.push_back(frame);
								}
							}
						}
					}
				
					// Convert euler angles to quaternions.
					for( DWORD f = 0; f < mesh->keyframes.size(); f++ )
					{
						for( DWORD k = 0; k < mesh->keyframes[f]->keys.size(); k++ )
						{
							Ovgl::Vector4 KeyEuler = mesh->keyframes[f]->keys[k].rotation;
							mesh->keyframes[f]->keys[k].rotation = Ovgl::QuaternionRotationEuler( (KeyEuler.x / 180.0f) * (float)OvglPi, (KeyEuler.y / 180.0f) * (float)OvglPi, (KeyEuler.z / 180.0f) * (float)OvglPi );
						}
					}
	
					// Sort frame times.
					std::sort(mesh->keyframes.begin(), mesh->keyframes.begin() + mesh->keyframes.size(), FrameCompare );
					mesh->VertexBuffer = 0;
					mesh->IndexBuffers = 0;
					mesh->GenerateBoneMeshes();
					mesh->Update();
					Meshes.push_back( mesh );
					return mesh;
				}
			}
		}
	}
	return NULL;
}

Ovgl::Texture* Ovgl::MediaLibrary::ImportCubeMap( const std::string& POS_X, const std::string& NEG_X, const std::string& POS_Y, const std::string& NEG_Y, const std::string& POS_Z, const std::string& NEG_Z )
{
	// Create new texture
	Ovgl::Texture* texture = new Ovgl::Texture;

	// Set the texture's media library handle to this media library
	texture->MLibrary = this;

	// Create array of cube faces.
	std::string CubeFaces[6] = {POS_X, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z};

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

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura);
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
	// Create new texture
	Ovgl::Texture* texture = new Ovgl::Texture;

	// Set the texture's media library handle to this media library
	texture->MLibrary = this;

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

	// Create OpenGL texture
	glGenTextures( 1, &texture->Image );
	glBindTexture( GL_TEXTURE_2D, texture->Image );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
	glBindTexture( GL_TEXTURE_2D, NULL );

	// Release FreeImage's copy of the image
	FreeImage_Unload( dib );

	// Add texture to media library
	Textures.push_back( texture );

	// Return texture pointer
	return texture;
}

Ovgl::Shader* Ovgl::MediaLibrary::ImportCG( const std::string& file )
{
	Ovgl::Shader* shader = new Ovgl::Shader;
	shader->MLibrary = this;
	
	// Define debugging variables
	CGerror error;
	const char* string;

	// Create vertex program
	shader->VertexProgram = cgCreateProgramFromFile( Inst->CgContext, CG_SOURCE, file.c_str(), CG_PROFILE_GPU_VP, "VS", NULL );
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
	shader->FragmentProgram = cgCreateProgram( Inst->CgContext, CG_SOURCE, file.c_str(), CG_PROFILE_GPU_FP, "FS", NULL );
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

Ovgl::Scene* Ovgl::MediaLibrary::CreateScene( )
{
	Ovgl::Scene* scene = new Ovgl::Scene;
	scene->Inst = Inst;
	scene->DynamicsWorld = new btDiscreteDynamicsWorld(Inst->PhysicsDispatcher,Inst->PhysicsBroadphase,Inst->PhysicsSolver,Inst->PhysicsConfiguration);
	scene->DynamicsWorld->setGravity(btVector3( 0.0f,-9.8f, 0.0f ));
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
	Materials.push_back(material);
	return material;
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
		std::vector<signed short> mono(buffer->data.size() / 2);
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