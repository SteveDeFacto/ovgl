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
				fwrite( &Meshes[m]->bones[i]->min, sizeof(D3DXVECTOR3), 1, output );
				fwrite( &Meshes[m]->bones[i]->max, sizeof(D3DXVECTOR3), 1, output );
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
				fread( &mesh->bones[i]->min, sizeof(D3DXVECTOR3), 1, input );
				fread( &mesh->bones[i]->max, sizeof(D3DXVECTOR3), 1, input );
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
			mesh->VertexBuffer = NULL;
			mesh->IndexBuffers = NULL;

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
		for(int n = 1; n < KFbxGetSrcCount<KFbxNode>(FBXScene); n++)
		{
			KFbxNodeAttribute::EAttributeType AttributeType = KFbxGetSrc<KFbxNode>(FBXScene, n)->GetNodeAttribute()->GetAttributeType();
			if ( AttributeType == KFbxNodeAttribute::eMESH )
			{
				KFbxNode* FBXNode = KFbxGetSrc<KFbxNode>(FBXScene, n);
				if( node.compare(FBXNode->GetName()) == 0)
				{
					Ovgl::Matrix44 matrix;
					KFbxVector4 localT, localR, localS;
					KFbxMesh* FBXMesh = (KFbxMesh*) FBXNode->GetNodeAttribute();
					localT = FBXNode->GetParent()->LclTranslation.Get();
					localR = FBXNode->GetParent()->LclRotation.Get();
					localS = FBXNode->GetParent()->LclScaling.Get();
					KFbxDeformer *FBXDeformer = FBXMesh->GetDeformer(0);
					if(FBXDeformer)
					matrix = Ovgl::MatrixScaling( (float)localS[0], (float)localS[1], (float)localS[2] ) * Ovgl::MatrixRotationEuler( (float)((localR[0] + 90)  * OvglPi / 180), -(float)(localR[2] * OvglPi / 180), (float)(( localR[1] + 90) * OvglPi / 180) )  * Ovgl::MatrixTranslation( (float)localT[0], (float)localT[1], (float)localT[2] );
					else
					matrix = Ovgl::MatrixScaling( (float)localS[0], (float)localS[1], (float)localS[2] ) * Ovgl::MatrixRotationEuler( (float)((localR[0])  * OvglPi / 180), -(float)(localR[2] * OvglPi / 180), (float)(( localR[1] + 90) * OvglPi / 180) )  * Ovgl::MatrixTranslation( (float)localT[0], (float)localT[1], (float)localT[2] );
					int ControlPointCount = FBXMesh->GetControlPointsCount();
					KFbxVector4* ControlPoints = FBXMesh->GetControlPoints();
					KFbxLayerElementUV* FBXLayerUVs = FBXMesh->GetLayer(0)->GetUVs();
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
					if(FBXDeformer)
					{
						KFbxSkin *FBXSkin = KFbxCast<KFbxSkin>(FBXDeformer);
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
						if(!FBXDeformer)
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
							mesh->attributes.push_back(0);
						}
						Ovgl::Face face;
						face.indices[0] = FaceIndices[0];
						face.indices[1] = FaceIndices[1];
						face.indices[2] = FaceIndices[2];
						mesh->faces.push_back( face );
						mesh->attributes.push_back(0);
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
					mesh->VertexBuffer = NULL;
					mesh->IndexBuffers = NULL;
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

Ovgl::Texture* Ovgl::MediaLibrary::ImportDDS( const std::string& file )
{
	Ovgl::Texture* texture = new Ovgl::Texture; 
	texture->Inst = Inst;
	D3DX10CreateShaderResourceViewFromFileA( Inst->D3DDevice, file.c_str(), NULL, NULL, &texture->SRV, NULL);
	Textures.push_back(texture);
	return texture;
}

Ovgl::Effect* Ovgl::MediaLibrary::ImportFX( const std::string& file )
{
	Ovgl::Effect* effect = new Ovgl::Effect;
	effect->Inst = Inst;
	
	//  Create effect.
	std::wstring wfilename;
	wfilename.assign( file.begin(), file.end() );
	D3DX10CreateEffectFromFile( wfilename.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, Inst->D3DDevice, NULL, NULL, &effect->SFX, NULL, NULL );
	
	// Get shader variables.
	effect->Technique = effect->SFX->GetTechniqueByName( "Render" );
	effect->Shadow_Maps = effect->SFX->GetVariableBySemantic( "SHADOWMAPS" )->AsShaderResource();
	effect->Cube_Views = effect->SFX->GetVariableBySemantic( "CUBEVIEWS" )->AsMatrix();
	effect->Light_Count = effect->SFX->GetVariableBySemantic( "LIGHTCOUNT" )->AsScalar();
	effect->Lights = effect->SFX->GetVariableBySemantic( "LIGHTARRAY" )->AsVector();
	effect->Light_Colors = effect->SFX->GetVariableBySemantic( "LIGHTCOLORARRAY" )->AsVector();
    effect->Bones = effect->SFX->GetVariableBySemantic( "BONEARRAY" )->AsMatrix();
    effect->View = effect->SFX->GetVariableBySemantic( "WORLDVIEW" )->AsMatrix();
	effect->Projection = effect->SFX->GetVariableBySemantic( "PROJECTION" )->AsMatrix();

	// Get technique.
	D3D10_PASS_DESC PassDesc;
	effect->Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	// Create vertex layout desc.
	D3D10_INPUT_ELEMENT_DESC vertexlayout[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,	24,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	32,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	48,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create vertex layout.
	Inst->D3DDevice->CreateInputLayout( vertexlayout, sizeof(vertexlayout)/sizeof(vertexlayout[0]), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &effect->Layout );

	//Add Effect to array
	Effects.push_back( effect );

	//Return effect.
	return effect;
}

Ovgl::Scene* Ovgl::MediaLibrary::CreateScene( )
{
	Ovgl::Scene* scene = new Ovgl::Scene;
	scene->Inst = Inst;
	NxSceneDesc sceneDesc;
	sceneDesc.gravity.set( 0.0f, -0.2f, 0.0f );
	if( Inst->PhysX->getHWVersion() != NX_HW_VERSION_NONE)
		#ifdef _DEBUG
			sceneDesc.simType = NX_SIMULATION_SW;
		#else
			sceneDesc.simType = NX_SIMULATION_HW;
		#endif
	scene->physics_scene = Inst->PhysX->createScene(sceneDesc);
	NxMaterial* defaultMaterial = scene->physics_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
	Scenes.push_back(scene);
	return scene;
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
		wfile = L"Ovgl::Instance::CreateAudioBuffer was unable to open the file ";
		wfile.append( file.begin(), file.end() );
		OutputDebugString( wfile.c_str() );
		return NULL;
	}
	ov_open_callbacks(f, &info, NULL, 0, OV_CALLBACKS_DEFAULT);
	vorbis_info *vi = ov_info(&info, -1);
	buffer->format = new WAVEFORMATEX;
	memset( buffer->format, 0, sizeof(WAVEFORMATEX) );
	buffer->format->cbSize          = sizeof(WAVEFORMATEX);
	buffer->format->nChannels       = vi->channels;
	buffer->format->wBitsPerSample  = 16;
	buffer->format->nSamplesPerSec  = vi->rate;
	buffer->format->nAvgBytesPerSec = buffer->format->nSamplesPerSec * buffer->format->nChannels * 2;
	buffer->format->nBlockAlign     = 2 * buffer->format->nChannels;
	buffer->format->wFormatTag      = 1;
	buffer->data.resize( vi->channels * 2 * (UINT)ov_pcm_total( &info, -1 ));
	DWORD pos = 0;
	int sec = 0;
	int ret = 1;
	while( ret && pos < buffer->data.size() )
	{
		ret = ov_read( &info, &buffer->data[0] + pos, buffer->data.size() - pos, 0, 2, 1, &sec );
		pos += ret;
	}
	AudioBuffers.push_back(buffer);
	return buffer;
};