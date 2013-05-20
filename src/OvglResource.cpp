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

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglResource.h"
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"
#include "OvglSkeleton.h"
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <FreeImage.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/ftglyph.h>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

namespace Ovgl
{
ResourceManager::ResourceManager( Context* pcontext, const std::string& file )
{
    context = pcontext;
    context->media_libraries.push_back(this);
}

ResourceManager::~ResourceManager()
{
    for( uint32_t i = 0; i < sounds.size(); i++ )
    {
        sounds[i]->release();
    }
    for( uint32_t i = 0; i < materials.size(); i++ )
    {
        materials[i]->Release();
    }
    for( uint32_t i = 0; i < meshes.size(); i++ )
    {
        delete meshes[i];
    }
    for( uint32_t i = 0; i < scenes.size(); i++ )
    {
        scenes[i]->Release();
    }
    for( uint32_t i = 0; i < shaders.size(); i++ )
    {
        shaders[i]->Release();
    }
    for( uint32_t i = 0; i < textures.size(); i++ )
    {
        textures[i]->Release();
    }
}

void ResourceManager::save_resources( const std::string& file )
{
    if(!file.empty())
    {
        // Open file.
        FILE *output;
        output = fopen(file.c_str(),"wb");

        // Write the number of meshes to the file.
        uint32_t mesh_count = meshes.size();
        fwrite( &mesh_count, 4, 1, output );
        for( uint32_t m = 0; m < mesh_count; m++ )
        {
            // Get mesh variables.
            uint32_t vertex_count = meshes[m]->vertices.size();
            uint32_t face_count = meshes[m]->faces.size();
            uint32_t bone_count = meshes[m]->skeleton->bones.size();

            // Write the number of vertices, faces, and bones.
            fwrite( &vertex_count, 4, 1, output );
            fwrite( &face_count, 4, 1, output );
            fwrite( &bone_count, 4, 1, output );

            // Write the vertices, faces, and bones.
            fwrite( &meshes[m]->vertices[0], sizeof(Vertex), vertex_count, output);
            fwrite( &meshes[m]->faces[0], sizeof(Face), face_count, output);
            fwrite( &meshes[m]->attributes[0], sizeof(uint32_t), face_count, output);

            // Write bones.
            for( uint32_t i = 0; i < bone_count; i++ )
            {
                vertex_count = meshes[m]->skeleton->bones[i]->mesh->vertices.size();
                face_count = meshes[m]->skeleton->bones[i]->mesh->faces.size();
                fwrite( &vertex_count, 4, 1, output );
                fwrite( &face_count, 4, 1, output );
                if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it.
                {
                    fwrite( &meshes[m]->skeleton->bones[i]->mesh->vertices[0], sizeof(Vertex), vertex_count, output );
                    fwrite( &meshes[m]->skeleton->bones[i]->mesh->faces[0], sizeof(Face), face_count, output );
                }
                fwrite( &meshes[m]->skeleton->bones[i]->matrix, sizeof(Matrix44), 1, output );
                fwrite( &meshes[m]->skeleton->bones[i]->length, sizeof(float), 1, output );
                fwrite( &meshes[m]->skeleton->bones[i]->min, sizeof(Vector3), 1, output );
                fwrite( &meshes[m]->skeleton->bones[i]->max, sizeof(Vector3), 1, output );
                fwrite( &meshes[m]->skeleton->bones[i]->parent, sizeof(uint32_t), 1, output );
                uint32_t child_count = meshes[m]->skeleton->bones[i]->children.size();
                fwrite( &child_count, sizeof(uint32_t), 1, output );
                if(child_count) fwrite( &meshes[m]->skeleton->bones[i]->children[0], sizeof(uint32_t), child_count, output );
            }
        }
        uint32_t scene_count = scenes.size();
        fwrite( &scene_count, 4, 1, output );
        for( uint32_t s = 0; s < scene_count; s++ )
        {
            uint32_t prop_count = scenes[s]->props.size();
            fwrite( &prop_count, 4, 1, output );
            for( uint32_t p = 0; p < scenes[s]->props.size(); p++ )
            {
                Matrix44 matrix = scenes[s]->props[p]->getPose();
                fwrite( &matrix, sizeof(Matrix44), 1, output );
                uint32_t mesh_index = 0;
                for( uint32_t sm = 0; sm < meshes.size(); sm++ )
                {
                    if( scenes[s]->props[p]->mesh == meshes[sm] )
                    {
                        mesh_index = sm;
                    }
                }
                fwrite( &mesh_index, 4, 1, output );
                for( uint32_t b = 0; b < scenes[s]->props[p]->bones.size(); b++ )
                {
                    uint32_t bodyflags = scenes[s]->props[p]->bones[b]->get_flags();
                    fwrite( &bodyflags, 4, 1, output );
                }
            }
            uint32_t object_count = scenes[s]->objects.size();
            fwrite( &object_count, 4, 1, output );
            for( uint32_t o = 0; o < scenes[s]->objects.size(); o++ )
            {
                Matrix44 matrix = scenes[s]->objects[o]->getPose();
                fwrite( &matrix, sizeof(Matrix44), 1, output );
                uint32_t mesh_index = 0;
                for( uint32_t sm = 0; sm < meshes.size(); sm++ )
                {
                    if( scenes[s]->objects[o]->mesh == meshes[sm] )
                    {
                        mesh_index = sm;
                    }
                }
                fwrite( &mesh_index, 4, 1, output );
                uint32_t bodyflags = scenes[s]->objects[o]->cmesh->get_flags();
                fwrite( &bodyflags, 4, 1, output );
            }
            uint32_t light_count = scenes[s]->lights.size();
            fwrite( &light_count, 4, 1, output );
            for( uint32_t l = 0; l < light_count; l++ )
            {
                Matrix44 matrix = scenes[s]->lights[l]->getPose();
                fwrite( &matrix, sizeof(Matrix44), 1, output );
            }
            uint32_t camera_count = scenes[s]->cameras.size();
            fwrite( &camera_count, 4, 1, output );
            for( uint32_t c = 0; c < camera_count; c++ )
            {
                Matrix44 matrix = scenes[s]->cameras[c]->getPose();
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
        uint32_t texture_count = textures.size();
        fwrite( &texture_count, 4, 1, output );


        // Close file.
        fclose(output);
    }
}

void ResourceManager::load_resources( const std::string& file )
{
    if(!file.empty())
    {
        // Get the number of meshes currently in memory so we can offset the indices into the array.
        uint32_t mesh_offset = meshes.size();

        // Open file.
        FILE *input = NULL;
        input = fopen( file.c_str(),"rb" );

        //Get number of meshes.
        uint32_t mesh_count;
        fread( &mesh_count, 4, 1, input );
        for( uint32_t m = 0; m < mesh_count; m++ )
        {
            // Specify mesh variables.
            Mesh* mesh = new Mesh;
            mesh->media_library = this;
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
            mesh->skeleton->bones.resize(bone_count);

            // Load vertices, indices and attributes.
            fread(&mesh->vertices[0], sizeof(Vertex), vertex_count, input);
            fread(&mesh->faces[0], sizeof(Face), face_count, input);
            fread(&mesh->attributes[0], sizeof(uint32_t), face_count, input);

            // Load bones.
            for( uint32_t i = 0; i < bone_count; i++ )
            {
                mesh->skeleton->bones[i] = new Bone;
                mesh->skeleton->bones[i]->mesh = new Mesh;
                mesh->skeleton->bones[i]->convex = NULL;
                fread( &vertex_count, 4, 1, input );
                fread( &face_count, 4, 1, input );
                if( (vertex_count > 0) & (face_count > 0) )	// If this bone has a collision mesh then load it.
                {
                    mesh->skeleton->bones[i]->mesh->vertices.resize(vertex_count);
                    mesh->skeleton->bones[i]->mesh->faces.resize(face_count);
                    fread( &mesh->skeleton->bones[i]->mesh->vertices[0], sizeof(Vertex), vertex_count, input );
                    fread( &mesh->skeleton->bones[i]->mesh->faces[0], sizeof(Face), face_count, input );
                }
                fread( &mesh->skeleton->bones[i]->matrix, sizeof(Matrix44), 1, input );
                fread( &mesh->skeleton->bones[i]->length, sizeof(float), 1, input );
                fread( &mesh->skeleton->bones[i]->min, sizeof(Vector3), 1, input );
                fread( &mesh->skeleton->bones[i]->max, sizeof(Vector3), 1, input );
                fread( &mesh->skeleton->bones[i]->parent, sizeof(uint32_t), 1, input );
                fread( &child_count, sizeof(uint32_t), 1, input );
                mesh->skeleton->bones[i]->children.resize(child_count);
                if(child_count) fread( &mesh->skeleton->bones[i]->children[0], sizeof(uint32_t), child_count, input );
            }

            // Nullify buffer addresses.
            mesh->vertex_buffer = 0;
            mesh->index_buffers = 0;

            // Update buffers.
            mesh->update();

            meshes.push_back(mesh);
        }

        //Get number of scenes.
        uint32_t scene_count;
        fread( &scene_count, 4, 1, input );
        for( uint32_t s = 0; s < scene_count; s++ )
        {
            Scene* scene = create_scene();

            uint32_t prop_count;
            fread( &prop_count, 4, 1, input );
            for( uint32_t p = 0; p < prop_count; p++ )
            {
                // Get the pose of this prop.
                Matrix44 matrix;
                fread( &matrix, sizeof(Matrix44), 1, input );
                uint32_t mesh_index;
                fread( &mesh_index, 4, 1, input );
                Prop* prop = scene->CreateProp( meshes[mesh_index + mesh_offset], matrix, true );
                for( uint32_t b = 0; b < meshes[mesh_index + mesh_offset]->skeleton->bones.size(); b++ )
                {
                    uint32_t bone_flags;
                    fread( &bone_flags, 4, 1, input );
                    prop->bones[b]->set_flags(bone_flags);
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
                Object* object = scene->CreateObject( meshes[mesh_index + mesh_offset], matrix );
                uint32_t bone_flags;
                fread( &bone_flags, 4, 1, input );
                object->cmesh->set_flags(bone_flags);
            }

            uint32_t light_count;
            fread( &light_count, 4, 1, input );
            for( uint32_t l = 0; l < light_count; l++ )
            {
                // Get the pose of this light.
                Matrix44 matrix;
                fread( &matrix, sizeof(Matrix44), 1, input );
                scene->CreateLight( matrix, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
            }

            uint32_t camera_count;
            fread( &camera_count, 4, 1, input );
            for( uint32_t c = 0; c < camera_count; c++ )
            {
                // Get the pose of this camera.
                Matrix44 matrix;
                fread( &matrix, sizeof(Matrix44), 1, input );
                scene->CreateCamera( matrix );
            }
        }
        // Close file.
        fclose(input);
    }
}

Mesh* ResourceManager::import_model( const std::string& file, bool z_up )
{
    if(!file.empty())
    {
        Mesh* mesh = new Mesh;

        // Create a new skeleton.
        mesh->skeleton = new Skeleton;

        // Set media library to this library.
        mesh->media_library = this;

		mesh->subset_count = 0;

        // Import scene from file.
        const aiScene* scene = aiImportFile(file.c_str(), 0);

        // Iterate through scene nodes.
        for( uint32_t n = 0; n < scene->mRootNode->mNumChildren; n++)
        {

            // Iterate through meshes of this node.
            for( uint32_t sm = 0; sm < scene->mRootNode->mChildren[n]->mNumMeshes; sm++)
            {
                uint32_t voffset = mesh->vertices.size();
                uint32_t aoffset = 0;
                uint32_t boffset = mesh->skeleton->bones.size();
                for( uint32_t i = 0; i < mesh->faces.size(); i++ )
                {
                    if( mesh->attributes[i] > aoffset )
                    {
                        aoffset = mesh->attributes[i];
                    }
                }
                uint32_t m = scene->mRootNode->mChildren[n]->mMeshes[sm];
                Matrix44 matrix;
                if(!scene->mMeshes[m]->HasBones())
                {
                    matrix = *(Matrix44*)&scene->mRootNode->mChildren[n]->mTransformation.Transpose();
                }
                if(z_up)
                {
                    matrix = matrix * MatrixRotationX(1.57f);
                }

                std::vector< std::vector< float > > weights(scene->mMeshes[m]->mNumVertices);
                std::vector< std::vector< float > > indices(scene->mMeshes[m]->mNumVertices);
                mesh->vertices.resize( voffset + scene->mMeshes[m]->mNumVertices);

                // Get skeleton.
                if(scene->mMeshes[m]->HasBones())
                {
					int mybones = scene->mMeshes[m]->mNumBones;
                    mesh->skeleton->bones.resize(scene->mMeshes[m]->mNumBones + boffset);

                    for( uint32_t b = 0; b < scene->mMeshes[m]->mNumBones; b++ )
                    {
                        mesh->skeleton->bones[b + boffset] = new Bone;
						mesh->skeleton->bones[b + boffset]->convex = NULL;
                    }

                    for( uint32_t b = 0; b < scene->mMeshes[m]->mNumBones; b++ )
                    {

                        Bone* bone = mesh->skeleton->bones[b+ boffset];
                        bone->length = 1.0f;
                        bone->mesh = new Mesh;
                        bone->convex = NULL;
                        bone->parent = NULL;
                        aiNode* bnode = scene->mRootNode->FindNode(scene->mMeshes[m]->mBones[b]->mName);
                        bone->local_transform = Ovgl::MatrixTranspose(*(Matrix44*)&bnode->mTransformation);
                        bone->matrix = MatrixInverse( Vector4(), Ovgl::MatrixTranspose(*(Matrix44*)&scene->mMeshes[m]->mBones[b]->mOffsetMatrix));

                        if(!z_up)
                        {
                            bone->matrix = MatrixRotationZ(1.57f) * bone->matrix * MatrixRotationX(1.57f);
                        }

                        for( uint32_t i = 0; i < scene->mMeshes[m]->mNumBones; i++ )
                        {
                            if( bnode->mName == scene->mMeshes[m]->mBones[i]->mName )
                            {
                                bone->index = i;
                            }
                        }

                        // Get bone hierarchy.
                        for( uint32_t mb = 0; mb < scene->mMeshes[m]->mNumBones; mb++ )
                        {
                            if( bnode->mParent->mName == scene->mMeshes[m]->mBones[mb]->mName )
                            {
                                bone->parent = mesh->skeleton->bones[mb];
                            }
                            for( uint32_t cb = 0; cb < bnode->mNumChildren; cb++ )
                            {
                                if( bnode->mChildren[cb]->mName == scene->mMeshes[m]->mBones[mb]->mName )
                                {
                                    bone->children.push_back(mesh->skeleton->bones[mb]);
                                }
                            }
                        }

                        // Get bone skinning information.
                        for (uint32_t w = 0 ; w < scene->mMeshes[m]->mBones[b]->mNumWeights; w++)
                        {
                            weights[scene->mMeshes[m]->mBones[b]->mWeights[w].mVertexId].push_back((float)scene->mMeshes[m]->mBones[b]->mWeights[w].mWeight);
                            indices[scene->mMeshes[m]->mBones[b]->mWeights[w].mVertexId].push_back((float)b);
                        }


                        // Get bone animation.
                        mesh->skeleton->animations.resize(scene->mNumAnimations);
                        for( uint32_t a = 0; a < scene->mNumAnimations; a++ )
                        {
                            mesh->skeleton->animations[a].channels.resize(scene->mAnimations[a]->mNumChannels);
                            for( uint32_t ac = 0; ac < scene->mAnimations[a]->mNumChannels; ac++ )
                            {
                                if(scene->mAnimations[a]->mChannels[ac]->mNodeName == scene->mMeshes[m]->mBones[b]->mName)
                                {
                                    mesh->skeleton->animations[a].channels[ac].index = b;
                                    for( uint32_t pk = 0; pk < scene->mAnimations[a]->mChannels[ac]->mNumPositionKeys; pk++ )
                                    {
                                        VectorKey position_key;
                                        position_key.time = scene->mAnimations[a]->mChannels[ac]->mPositionKeys[pk].mTime;
                                        position_key.value.x = scene->mAnimations[a]->mChannels[ac]->mPositionKeys[pk].mValue.x;
                                        position_key.value.y = scene->mAnimations[a]->mChannels[ac]->mPositionKeys[pk].mValue.y;
                                        position_key.value.z = scene->mAnimations[a]->mChannels[ac]->mPositionKeys[pk].mValue.z;
                                        mesh->skeleton->animations[a].channels[ac].position_keys.push_back( position_key );
                                        if(position_key.time > mesh->skeleton->animations[a].end_time)
                                        {
                                            mesh->skeleton->animations[a].end_time = position_key.time;
                                        }
                                        if(position_key.time < mesh->skeleton->animations[a].start_time)
                                        {
                                            mesh->skeleton->animations[a].start_time = position_key.time;
                                        }
                                    }
                                    for( uint32_t rk = 0; rk < scene->mAnimations[a]->mChannels[ac]->mNumRotationKeys; rk++ )
                                    {
                                        QuatKey rotation_key;
                                        rotation_key.time = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mTime;
                                        rotation_key.value.w = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.w;
                                        rotation_key.value.x = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.x;
                                        rotation_key.value.y = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.y;
                                        rotation_key.value.z = scene->mAnimations[a]->mChannels[ac]->mRotationKeys[rk].mValue.z;
                                        mesh->skeleton->animations[a].channels[ac].rotation_keys.push_back( rotation_key );
                                        if(rotation_key.time > mesh->skeleton->animations[a].end_time)
                                        {
                                            mesh->skeleton->animations[a].end_time = rotation_key.time;
                                        }
                                        if(rotation_key.time < mesh->skeleton->animations[a].start_time)
                                        {
                                            mesh->skeleton->animations[a].start_time = rotation_key.time;
                                        }
                                    }
                                    for( uint32_t sk = 0; sk < scene->mAnimations[a]->mChannels[ac]->mNumScalingKeys; sk++ )
                                    {
                                        VectorKey scaling_key;
                                        scaling_key.time = scene->mAnimations[a]->mChannels[ac]->mScalingKeys[sk].mTime;
                                        scaling_key.value.x = scene->mAnimations[a]->mChannels[ac]->mScalingKeys[sk].mValue.x;
                                        scaling_key.value.y = scene->mAnimations[a]->mChannels[ac]->mScalingKeys[sk].mValue.y;
                                        scaling_key.value.z = scene->mAnimations[a]->mChannels[ac]->mScalingKeys[sk].mValue.z;
                                        mesh->skeleton->animations[a].channels[ac].scaling_keys.push_back( scaling_key );
                                        if(scaling_key.time > mesh->skeleton->animations[a].end_time)
                                        {
                                            mesh->skeleton->animations[a].end_time = scaling_key.time;
                                        }
                                        if(scaling_key.time < mesh->skeleton->animations[a].start_time)
                                        {
                                            mesh->skeleton->animations[a].start_time = scaling_key.time;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

				// Cap off bone influences to no more than 4.
                for ( uint32_t w = 0; w < scene->mMeshes[m]->mNumVertices; w++)
                {
					// Bubble sort the bone influences
					bool swapped = true;
					while(swapped)
					{
						swapped = false;
						for( uint32_t i = 1; i< weights[w].size();i++)
						{
							if(weights[w][i-1] < weights[w][i])
							{
								std::swap(weights[w][i-1], weights[w][i]);
								std::swap(indices[w][i-1], indices[w][i]);
								swapped = true;
							}
						}
					}
                    weights[w].resize(4);
                    indices[w].resize(4);
                    if(!scene->mMeshes[m]->HasBones())
                        weights[w][0] = 1.0f;
                }

                // Get geometry.
                for( uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++ )
                {
                    uint32_t f_indices[4];
                    for( uint32_t i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++ )
                    {
                        int vi = scene->mMeshes[m]->mFaces[f].mIndices[i];
                        Vertex vertex;
                        vertex.position.x = scene->mMeshes[m]->mVertices[vi].x;
                        vertex.position.y = scene->mMeshes[m]->mVertices[vi].y;
                        vertex.position.z = scene->mMeshes[m]->mVertices[vi].z;
                        vertex.normal.x = scene->mMeshes[m]->mNormals[vi].x;
                        vertex.normal.y = scene->mMeshes[m]->mNormals[vi].y;
                        vertex.normal.z = scene->mMeshes[m]->mNormals[vi].z;

                        if( scene->mMeshes[m]->HasBones() && !z_up)
                        {
                            vertex.position = Vector3Transform(vertex.position, MatrixRotationX(1.57f));
                            vertex.normal = Vector3Transform(vertex.normal, MatrixRotationX(1.57f));
                        }
                        else if(!scene->mMeshes[m]->HasBones())
                        {
                            vertex.position = Vector3Transform(vertex.position, matrix);
                            vertex.normal = Vector3Transform(vertex.normal, matrix.Rotation());
                        }
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
                        f_indices[i] = (uint32_t)vi + voffset;
                        mesh->vertices[vi+voffset] = vertex;
                    }

                    if(scene->mMeshes[m]->mFaces[f].mNumIndices == 4)
                    {
                        Face face;
                        face.indices[0] = f_indices[0];
                        face.indices[1] = f_indices[2];
                        face.indices[2] = f_indices[3];
                        mesh->faces.push_back( face );
                        mesh->attributes.push_back(scene->mMeshes[m]->mMaterialIndex);
                    }
                    Face face;
                    face.indices[0] = f_indices[0];
                    face.indices[1] = f_indices[1];
                    face.indices[2] = f_indices[2];
                    mesh->faces.push_back( face );
                    mesh->attributes.push_back(scene->mMeshes[m]->mMaterialIndex);
                }
            }
        }

        // If no bones exist create one.
        if(mesh->skeleton->bones.size() == 0 )
        {
            Bone* bone = new Bone;
            bone->matrix = MatrixIdentity();
            bone->length = 1.0f;
            bone->mesh = new Mesh;
            bone->convex = NULL;
			bone->index = 0;
            mesh->skeleton->bones.push_back(bone);
			mesh->skeleton->root_bone = bone;
        }

        // Save vertices which influence each bone for bone shape automatic generation.
        for( uint32_t i = 0; i < mesh->skeleton->bones.size(); i++ )
        {
            for( uint32_t v = 0; v < mesh->vertices.size(); v++ )
            {
                for( uint32_t j = 0; j < 4; j++)
                {
                    if(mesh->vertices[v].indices[j] == i && mesh->vertices[v].weight[j] > 0.1f)
                    {
                        Vertex vertex;
                        vertex.position = Vector3Transform( mesh->vertices[v].position, MatrixInverse( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ), mesh->skeleton->bones[i]->matrix));
                        vertex.weight[0] = 1.0f;
                        mesh->skeleton->bones[i]->mesh->vertices.push_back( vertex );
                    }
                }
            }
        }

        // Find the root bone.
        mesh->skeleton->root_bone = NULL;
        for( uint32_t b1 = 0; b1 < mesh->skeleton->bones.size(); b1++ )
        {
            bool is_root = true;
            for( uint32_t b2 = 0; b2 < mesh->skeleton->bones.size(); b2++ )
            {
                for( uint32_t c = 0; c < mesh->skeleton->bones[b2]->children.size(); c++ )
                {
                    if( mesh->skeleton->bones[b2]->children[c]->index == b1 )
                    {
                        is_root = false;
                        break;
                    }
                }
            }
            if( is_root )
            {
                mesh->skeleton->root_bone = mesh->skeleton->bones[b1];
                mesh->skeleton->root_bone->parent = NULL;
                mesh->skeleton->root_bone->local_transform = mesh->skeleton->root_bone->local_transform * MatrixRotationX(1.57f);
            }
        }

        // Null index and vertex buffers.
        mesh->vertex_buffer = 0;
        mesh->index_buffers = 0;

        // Update video memory copies of index and vertex buffers.
		if(mesh->vertices.size() > 0)
		{
			mesh->update();
		}
        meshes.push_back( mesh );
        return mesh;
    }

    // Function failed so return NULL.
    return NULL;
}

Texture* ResourceManager::import_cubemap( const std::string& front, const std::string& back, const std::string& top, const std::string& bottom, const std::string& left, const std::string& right )
{
    // Create new texture
    Texture* texture = new Texture;

    // Set the texture's media library handle to this media library
    texture->MLibrary = this;

    // Create array of cube faces.
    std::string CubeFaces[6] = {front, back, top, bottom, left, right};

    SDL_GL_MakeCurrent(NULL, context->gl_context);

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

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    SDL_GL_MakeCurrent(NULL, NULL);

    // Add texture to media library
    textures.push_back( texture );

    // Return texture pointer
    return texture;
}

Texture* ResourceManager::import_texture( const std::string& file )
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

        SDL_GL_MakeCurrent(NULL, context->gl_context);

        // Create OpenGL texture
        glGenTextures( 1, &texture->Image );
        glBindTexture( GL_TEXTURE_2D, texture->Image );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura );
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, 0 );

        SDL_GL_MakeCurrent(NULL, NULL);

        // Release FreeImage's copy of the image
        FreeImage_Unload( dib );

        // delete our converted copy of the texture pixels
        delete [] textura;

        // Add texture to media library
        textures.push_back( texture );

        // Return texture pointer
        return texture;
    }
    else
    {
        // File does not exist!
        return NULL;
    }
}

Shader* ResourceManager::import_shader( const std::string& file )
{
    Ovgl::Shader* shader = new Ovgl::Shader;
    shader->MLibrary = this;

    // Define debugging variables
    CGerror error;
    const char* string;

    // Create effect
    shader->effect = cgCreateEffectFromFile( context->cg_context, file.c_str(), NULL );

    // Check for errors
    string = cgGetLastErrorString(&error);
    if(error)
    {
        fprintf(stderr, "Error: %s\n", string);
        string = cgGetLastListing(context->cg_context);
        fprintf(stderr, "Compiler: %s\n", string);
    }

    //Add Effect to array
    shaders.push_back( shader );

    //Return effect.
    return shader;
}

Scene* ResourceManager::create_scene()
{
    Ovgl::Scene* scene = new Ovgl::Scene;
    scene->context = context;
    scene->sky_box = NULL;
    scene->DynamicsWorld = new btDiscreteDynamicsWorld( context->physics_dispatcher, context->physics_broadphase, context->physics_solver, context->physics_configuration );
    scene->DynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.00001f;
    scene->DynamicsWorld->setGravity(btVector3( 0.0f, -9.8f, 0.0f ));
    btContactSolverInfo& info = scene->DynamicsWorld->getSolverInfo();
    info.m_numIterations = 20;
    scenes.push_back(scene);
    return scene;
}

Material* ResourceManager::create_material( )
{
    Material* material = new Material;
    material->MLibrary = this;
    material->ShaderProgram = context->default_media->shaders[0];
    material->NoZBuffer = false;
    material->NoZWrite = false;
    material->PostRender = false;
    material->setEffectTexture("txDiffuse", context->default_media->textures[0] );
    material->setEffectTexture("txEnvironment", context->default_media->textures[1] );
    materials.push_back(material);
    return material;
}

Texture* ResourceManager::create_texture( uint32_t width, uint32_t height )
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
    //glGenerateMipmap( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 0 );

    // Add texture to media library
    textures.push_back( texture );

    return texture;
}

Texture* ResourceManager::create_cubemap( uint32_t width, uint32_t height )
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

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Add texture to media library
    textures.push_back( texture );

    return texture;
}

AudioBuffer* ResourceManager::import_audio( const std::string& file )
{
    Ovgl::AudioBuffer* buffer = new Ovgl::AudioBuffer;
    buffer->context = context;
    AVFrame* frame = avcodec_alloc_frame();
    AVFormatContext* formatContext = NULL;
    avformat_open_input(&formatContext, file.c_str(), NULL, NULL);
    avformat_find_stream_info(formatContext, NULL);
    AVStream* audioStream = NULL;
    for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
    {
        if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStream = formatContext->streams[i];
            break;
        }
    }
    AVCodecContext* codecContext = audioStream->codec;
    codecContext->codec = avcodec_find_decoder(codecContext->codec_id);
    avcodec_open2(codecContext, codecContext->codec, NULL);

    if (codecContext->channels == 1)
    {
        buffer->format = AL_FORMAT_MONO16;
    }
    else
    {
        buffer->format = AL_FORMAT_STEREO16;
    }

    buffer->frequency = codecContext->sample_rate;

    AVPacket packet;
    av_init_packet(&packet);
    while (av_read_frame(formatContext, &packet) == 0)
    {
        if (packet.stream_index == audioStream->index)
        {
            int frameFinished = 0;
            avcodec_decode_audio4(codecContext, frame, &frameFinished, &packet);
            if (frameFinished)
            {
                int data_size = av_samples_get_buffer_size(NULL, codecContext->channels, frame->nb_samples, codecContext->sample_fmt, 1);
                buffer->data.resize( buffer->data.size() + data_size );
                memcpy( &buffer->data[buffer->data.size() - data_size], frame->data[0], data_size);
            }
        }
        av_free_packet(&packet);
    }

    av_free(frame);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    if( buffer->format == AL_FORMAT_MONO16 )
    {
        alGenBuffers( 1, &buffer->mono );
        alBufferData( buffer->mono, AL_FORMAT_MONO16, (ALvoid*)&buffer->data[0], buffer->data.size(), buffer->frequency );
    }
    else
    {
        alGenBuffers( 1, &buffer->stereo );
        alBufferData( buffer->stereo, AL_FORMAT_STEREO16, (ALvoid*)&buffer->data[0], buffer->data.size(), buffer->frequency );
        std::vector< int8_t > mono(buffer->data.size() / 2);
        for (uint32_t i = 0; i < mono.size(); i++)
        {
            mono[i] = (buffer->data[2*i] + buffer->data[2*i+1]) / 2;
        }
        alGenBuffers( 1, &buffer->mono );
        alBufferData( buffer->mono, AL_FORMAT_MONO16, (ALvoid*)&mono[0], mono.size(), buffer->frequency );
    }

    sounds.push_back(buffer);
    return buffer;
}

Font::Font( ResourceManager* resource_manager, const std::string& file, uint32_t size )
{
    this->size = size;
    FT_Face ftface;
    FT_New_Face( resource_manager->context->ftlibrary, file.c_str(), 0, &ftface );

    for(int i = 0; i < 256; i++)
    {
        FT_Set_Pixel_Sizes( ftface, 0, size);
        FT_Load_Char(ftface, i, FT_LOAD_RENDER);
        FT_Glyph ftglyph;
        FT_Get_Glyph( ftface->glyph, &ftglyph );
        FT_BitmapGlyph bmglyph = (FT_BitmapGlyph)ftglyph;
        charoffsets[i] = bmglyph->top;
        glGenTextures( 1, &charset[i] );
        glBindTexture( GL_TEXTURE_2D, charset[i] );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_ALPHA};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ftface->glyph->bitmap.width, ftface->glyph->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, ftface->glyph->bitmap.buffer );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
	resource_manager->fonts.push_back(this);
}
}
