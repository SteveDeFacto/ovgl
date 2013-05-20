/**
* @file OvglScene.cpp
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
* @brief This file contains classes related to 3D scene objects.
*/

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglResource.h"
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"
#include "OvglWindow.h"
#include "OvglSkeleton.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

// This class is used with bullet as a callback to disable collisions between bones which are contacting each other during the creation of a skeleton.
// Doing this is necessary to prevent a bug where dynamic objects will flail wildly until they fling themselves into the unknown.
class DisablePairCollision : public btCollisionWorld::ContactResultCallback
{
public:
    virtual	btScalar addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0, int partId0, int index0,const btCollisionObjectWrapper* colObj1, int partId1, int index1);

    btDiscreteDynamicsWorld* DynamicsWorld;
};

btScalar DisablePairCollision::addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0,int partId0,int index0,const btCollisionObjectWrapper* colObj1,int partId1,int index1)
{

    // Create an identity matrix.
    btTransform frame;
    frame.setIdentity();

    // Create a constraint between the two bone shapes which are contacting each other.
    btGeneric6DofConstraint* constraint = NULL;

    constraint = new btGeneric6DofConstraint( *(btRigidBody*)colObj0, *(btRigidBody*)colObj1, frame, frame, true );

    // Set limits to be limitless.
    constraint->setLinearLowerLimit( btVector3(1, 1, 1 ) );
    constraint->setLinearUpperLimit( btVector3(0, 0, 0 ) );
    constraint->setAngularLowerLimit( btVector3(1, 1, 1 ) );
    constraint->setAngularUpperLimit( btVector3(0, 0, 0 ) );

    // Add constraint to scene.
	if(constraint)
		DynamicsWorld->addConstraint(constraint, true);

    return 0;
}

namespace Ovgl
{
Camera* Scene::CreateCamera( const Matrix44& matrix )
{
    // Create a new camera object.
    Camera* camera = new Camera;

    // Link this scene to camera.
    camera->scene = this;

    // Set default perspective for camera.
    camera->projMat = MatrixPerspectiveLH( (((float)OvglPi) / 2.0f), (640.0f / 480.0f) , 0.01f, 100000.0f );

    // Build a motion state object for bullet positioned where we want the camera.
    btTransform Transform;
    Transform.setFromOpenGLMatrix((float*)&matrix);
    btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);

    // Create a sphere shape.
    btSphereShape* Sphere = new btSphereShape(1.0f);
    btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );

    // Create a new collision mesh.
    CMesh* cmesh = new CMesh;

    // Link scene to collision mesh.
    cmesh->scene = this;

    // Create a dynamic object, set it to not respond to contact, and link it to the camera.
    cmesh->actor = new btRigidBody(rbInfo);
    cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    camera->cmesh = cmesh;

    // Add dynamic object to physics scene.
    DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);

    // Add camera to scene list of cameras.
    cameras.push_back( camera );

    // Return pointer to light.
    return camera;
};

Light* Scene::CreateLight( const Matrix44& matrix, const Vector4& color )
{
    // Create a new light object.
    Light* light = new Light;

    // Link this scene to light.
    light->scene = this;

    // Build a motion state object for bullet positioned where we want the light.
    btTransform Transform;
    Transform.setFromOpenGLMatrix((float*)&matrix);
    btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);

    // Create a sphere shape.
    btSphereShape* Sphere = new btSphereShape(1.0f);
    btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );

    // Create a new collision mesh.
    CMesh* cmesh = new CMesh;

    // Link scene to collision mesh.
    cmesh->scene = this;

    // Create a dynamic object, set it to not respond to contact, and link it to the light.
    cmesh->actor = new btRigidBody(rbInfo);
    cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    light->cmesh = cmesh;

    // Add dynamic object to physics scene.
    DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);

    // Set light colors.
    light->color.x = color.x;
    light->color.y = color.y;
    light->color.z = color.z;

    // Add light to scene list of lights.
    this->lights.push_back( light );

    // Return pointer to light.
    return light;
};

Emitter* Scene::CreateEmitter( const Matrix44& matrix )
{
    // Create a new emitter object.
    Emitter* emitter = new Emitter;

    // Link this scene to emitter.
    emitter->scene = this;

    // Build a motion state object for bullet positioned where we want the emitter.
    btTransform Transform;
    Transform.setFromOpenGLMatrix((float*)&matrix);
    btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);

    // Create a sphere shape.
    btSphereShape* Sphere = new btSphereShape(1.0f);
    btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );

    // Create a new collision mesh.
    CMesh* cmesh = new CMesh;

    // Link scene to collision mesh.
    cmesh->scene = this;

    // Create a dynamic object, set it to not respond to contact, and link it to the emitter.
    cmesh->actor = new btRigidBody(rbInfo);
    cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    emitter->cmesh = cmesh;

    // Add dynamic object to physics scene.
    DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);

    // Add emitter to scene list of emitters.
    this->emitters.push_back( emitter );

    // Return pointer to emitter.
    return emitter;
};

Prop* Scene::CreateProp( Mesh* mesh, const Matrix44& matrix, bool disable_pair_collision )
{
    // Create a new prop object.
    Prop* prop = new Prop;

    // Link this scene to prop.
    prop->scene = this;
    prop->mesh = mesh;
    prop->materials.resize(mesh->subset_count);
    for( uint32_t s = 0; s < prop->materials.size(); s++)
    {
        prop->materials[s] = context->default_media->materials[0];
    }
    for( uint32_t i = 0; i < mesh->skeleton->bones.size(); i++ )
    {
		if(mesh->skeleton->bones[i]->convex != NULL)
		{
			btTransform Transform;
			Matrix44 mat = (mesh->skeleton->bones[i]->matrix * matrix );
			Transform.setFromOpenGLMatrix((float*)&mat );
			btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
			btVector3 localInertia(0,0,0);
			mesh->skeleton->bones[i]->convex->calculateLocalInertia(1, localInertia);
			btRigidBody::btRigidBodyConstructionInfo rbInfo( mesh->skeleton->bones[i]->volume, MotionState, mesh->skeleton->bones[i]->convex, localInertia );
			CMesh* cmesh = new CMesh;
			cmesh->scene = this;
			cmesh->actor = new btRigidBody(rbInfo);
			DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
			prop->bones.push_back(cmesh);
		}
		else
		{
			prop->bones.push_back(NULL);
		}
    }
    prop->constraints.resize(prop->bones.size());
    prop->CreateJoints( prop->mesh->skeleton->root_bone );
	if(disable_pair_collision)
	{
		for(uint32_t np = 0; np < prop->bones.size(); np++)
		{
			for(uint32_t nq = 0; nq < prop->bones.size(); nq++)
			{
				if( np!=nq && prop->bones[np] != NULL && prop->bones[nq] != NULL )
				{
					DisablePairCollision Callback;
					Callback.DynamicsWorld = DynamicsWorld;
					DynamicsWorld->contactPairTest(prop->bones[np]->actor, prop->bones[nq]->actor, Callback);
				}
			}
		}
	}
    props.push_back( prop );
    prop->matrices.resize( prop->bones.size() );
    return prop;
};

Object* Scene::CreateObject( Mesh* mesh, const Matrix44& matrix )
{
    Object* object = new Object;
    object->scene = this;
    object->mesh = mesh;
    object->materials.resize(mesh->subset_count);
    for( uint32_t s = 0; s < object->materials.size(); s++)
    {
        object->materials[s] = context->default_media->materials[0];
    }
    btTransform Transform;
    Transform.setFromOpenGLMatrix((float*)&matrix);
    btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, mesh->triangle_mesh, btVector3(0,0,0) );
    CMesh* cmesh = new CMesh;
    cmesh->scene = this;
    cmesh->actor = new btRigidBody(rbInfo);
    cmesh->actor->setCollisionFlags( cmesh->actor->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    object->cmesh = cmesh;
    DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
    this->objects.push_back( object );
    return object;
};

Actor* Scene::CreateActor( Mesh* mesh, float radius, float height, const Matrix44& matrix, const Matrix44& offset )
{
    Actor* actor = new Actor;
    actor->mesh = mesh;
    if( mesh )
    {
        actor->pose = new Pose;
        actor->pose->matrices.resize( mesh->skeleton->bones.size() );
        actor->pose->joints.resize( mesh->skeleton->bones.size() );
        for( uint32_t i = 0; i < mesh->skeleton->bones.size(); i++ )
        {
            actor->pose->joints[i] = new Joint;
        }
        for( uint32_t i = 0; i < mesh->skeleton->bones.size(); i++ )
        {
            actor->pose->matrices[i] = MatrixIdentity();
            actor->pose->joints[i]->offset = mesh->skeleton->bones[i]->matrix;
            actor->pose->joints[i]->local_transform = mesh->skeleton->bones[i]->local_transform;
            if(mesh->skeleton->bones[i]->parent)
            {
                actor->pose->joints[i]->parent = actor->pose->joints[mesh->skeleton->bones[i]->parent->index];
            }
            else
            {
                actor->pose->joints[i]->parent = NULL;
            }
            for( uint32_t c = 0; c < mesh->skeleton->bones[i]->children.size(); c++ )
            {
                actor->pose->joints[i]->children.push_back( actor->pose->joints[mesh->skeleton->bones[i]->children[c]->index] );
            }
        }
        actor->pose->root_joint = actor->pose->joints[mesh->skeleton->root_bone->index ];
        actor->materials.resize( mesh->subset_count );
        for( uint32_t i = 0; i < mesh->subset_count; i++ )
        {
            actor->materials[i] = context->default_media->materials[0];
        }
    }
    actor->crouch = false;
    actor->onGround = false;
    actor->offset = offset;
    actor->CameraOffset = MatrixIdentity();
    actor->lookDirection = Vector3( 0.0f, 0.0f, 0.0f );
    actor->velocity = Vector3( 0.0f, 0.0f, 0.0f );
    actor->walkDirection = Vector3( 0.0f, 0.0f, 0.0f );
    actor->height = height;
    actor->radius = radius;
    actor->maxSlope = (float)(OvglPi / 4);
    actor->scene = this;
    btTransform startTransform;
    startTransform.setIdentity ();
    startTransform.setFromOpenGLMatrix((float*)&matrix);
    actor->ghostObject = new btPairCachingGhostObject();
    actor->ghostObject->setWorldTransform(startTransform);
    context->physics_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    btConvexShape* capsule = new btCapsuleShape(radius, height);
    actor->ghostObject->setCollisionShape (capsule);
    actor->ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
    actor->controller = new btKinematicCharacterController(actor->ghostObject, capsule, height/3.0f);
    actor->controller->setMaxSlope(actor->maxSlope);
    actor->controller->setGravity(9.8f);
    DynamicsWorld->addCollisionObject(actor->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    DynamicsWorld->addAction(actor->controller);
    Matrix44 camera_matrix;
    camera_matrix = matrix * MatrixTranslation( 0.0f, height, 0.0f );
    actor->camera = CreateCamera( camera_matrix );
    actors.push_back(actor);
    return actor;
};

Constraint* Scene::CreateConstraint( CMesh* obj1, CMesh* obj2)
{
    Constraint* joint = new Constraint;
    joint->scene = this;
    joint->obj[0] = obj1;
    joint->obj[1] = obj2;
    Matrix44 bodyMatA, bodyMatB;
    bodyMatA = joint->obj[0]->get_pose();
    bodyMatB = joint->obj[1]->get_pose();
    btTransform frameInA, frameInB;
    Matrix44 diff = (bodyMatB * MatrixInverse(Vector4( 0.0f, 0.0f, 0.0f, 0.0f), bodyMatA));
    frameInA.setFromOpenGLMatrix((float*)&diff);
    frameInB.setIdentity();
    joint->joint = new btGeneric6DofConstraint( *obj1->actor, *obj2->actor, frameInA, frameInB, true );
    this->DynamicsWorld->addConstraint(joint->joint, true);
    constraints.push_back(joint);
    return joint;
}

void Actor::Jump( float force )
{
    controller->setJumpSpeed( force );
    controller->jump();
}

void Prop::setPose( const Matrix44& matrix )
{
    bones[mesh->skeleton->root_bone->index]->set_pose(matrix);
}

void Object::setPose( const Matrix44& matrix )
{
    btTransform Transform;
    Transform.setFromOpenGLMatrix( (float*)&matrix );
    cmesh->actor->setWorldTransform(Transform);
}

void Camera::setPose( const Matrix44& matrix )
{
    btTransform Transform;
    Transform.setFromOpenGLMatrix( (float*)&matrix );
    cmesh->actor->setWorldTransform(Transform);
}

Matrix44 Prop::getPose()
{
    Matrix44 matrix;
    bones[mesh->skeleton->root_bone->index]->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
};

Matrix44 Object::getPose()
{
    Matrix44 matrix;
    cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
};

Matrix44 Emitter::getPose()
{
    Matrix44 matrix;
    cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
};

Matrix44 Camera::getPose()
{
    Matrix44 matrix;
    cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
}

Matrix44 Light::getPose()
{
    Matrix44 matrix;
    cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
}

Matrix44 Actor::getPose()
{
    Matrix44 matrix;
    ghostObject->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
    return matrix;
}

void Prop::update( Bone* bone, Matrix44* matrix )
{
    Matrix44 inv_matrix, inv_mesh_bone, tmatrix;
    matrices[bone->index] = bones[bone->index]->get_pose();
    if( this->mesh->skeleton->bones.size() > 0 )
    {
        inv_mesh_bone = MatrixInverse( Vector4(0,0,0,0), bone->matrix );
        matrices[bone->index] = ( inv_mesh_bone * matrices[bone->index]  );
        inv_matrix = MatrixInverse( Vector4(0,0,0,0), *matrix );
        tmatrix = matrices[bone->index] * inv_matrix;
        for (uint32_t i = 0; i < bone->children.size(); i++)
        {
            Matrix44 child;
            child = bone->children[i]->matrix.Translation();
            child = child * tmatrix * (*matrix);
            Prop::update( bone->children[i], &child );
        }
    }
}

void Prop::CreateJoints( Bone* bone )
{
    if( bone->children.size() > 0 )
    {
        Constraint* joint = new Constraint;
        joint->scene = this->scene;
        joint->obj[0] = bones[bone->index];
        for(uint32_t i = 0; i < bone->children.size(); i++)
        {
            joint->obj[1] = bones[bone->children[i]->index];
            Bone* childBone = bone->children[i];
            Matrix44 bodyMatA, bodyMatB;
            bodyMatA = joint->obj[0]->get_pose();
            bodyMatB = joint->obj[1]->get_pose();
            btTransform frameInA, frameInB;
            Matrix44 diff = (bodyMatB * MatrixInverse(Vector4( 0.0f, 0.0f, 0.0f, 0.0f), bodyMatA));
            frameInA.setFromOpenGLMatrix((float*)&diff);
            frameInB.setIdentity();
            joint->joint = new btGeneric6DofConstraint( *joint->obj[0]->actor, *joint->obj[1]->actor, frameInA, frameInB, true );
            joint->joint->setAngularLowerLimit( btVector3(DegToRad(-childBone->max.x), DegToRad(childBone->min.y), DegToRad(-childBone->max.z)) );
            joint->joint->setAngularUpperLimit( btVector3(DegToRad(-childBone->min.x), DegToRad(childBone->max.y), DegToRad(-childBone->min.z)) );
            scene->DynamicsWorld->addConstraint(joint->joint, true);
            constraints[bone->children[i]->index] = joint;
            Prop::CreateJoints(bone->children[i]);
        }
    }
}

AnimationInstance* Prop::PlayAnimation( double start, double end, bool repeat )
{
    AnimationInstance* animation = new AnimationInstance;
    animation->animation_state = 0;
    animation->current_time = 0;
    animation->start_time = start;
    animation->end_time = end;
    animation->step_time = 1;
    return animation;
}

AnimationInstance* Actor::PlayAnimation( Animation* anim, double start, double end, bool repeat )
{
    AnimationInstance* animation = new AnimationInstance;
    animation->animation = anim;
    animation->animation_state = 1 + repeat;
    animation->current_time = start;
    animation->start_time = start;
    animation->end_time = end;
    animation->step_time = 1;
    animations.push_back(animation);
    return animation;
}

void Scene::Update( uint32_t UpdateTime )
{
    // Update actor positions.
    for(uint32_t a = 0; a < actors.size(); a++)
    {
        Vector3 corrected_trajectory;
        corrected_trajectory = Vector3Transform( (actors[a]->walkDirection / (1.0f + (float)actors[a]->crouch) ), MatrixRotationY( -actors[a]->lookDirection.z) );
        actors[a]->controller->setWalkDirection(btVector3(corrected_trajectory.x, corrected_trajectory.y, corrected_trajectory.z));

        btCollisionShape* shape = actors[a]->ghostObject->getCollisionShape();
        if( (actors[a]->crouch) && shape->getLocalScaling().getY() > 0.5f )
        {
            shape->setLocalScaling( btVector3(1, shape->getLocalScaling().getY() - ((float)UpdateTime * 0.005f), 1 ) );
        }
        else if( (!actors[a]->crouch) && (shape->getLocalScaling().getY() < 1.0f ) )
        {
            shape->setLocalScaling( btVector3(1, shape->getLocalScaling().getY() + ((float)UpdateTime * 0.005f), 1 ) );
            if( actors[a]->controller->onGround() )
            {
                btTransform transform = actors[a]->ghostObject->getWorldTransform();
                transform.setOrigin( transform.getOrigin() + btVector3( 0.0f, ((float)UpdateTime * 0.005f), 0.0f ) );
                actors[a]->ghostObject->setWorldTransform( transform );
            }
        }
        else if(shape->getLocalScaling().getY() <= 0.5f)
        {
            shape->setLocalScaling( btVector3( 1.0f, 0.5f, 1.0f ) );
        }
        else if(shape->getLocalScaling().getY() >= 1.0f)
        {
            shape->setLocalScaling( btVector3( 1.0f, 1.0f, 1.0f ) );
        }

        Matrix44 matrix;
        matrix = actors[a]->getPose();
        Matrix44 cam_mat;
        cam_mat = MatrixTranslation( 0.0f, 0.0f, actors[a]->radius / 2 ) * MatrixRotationEuler( actors[a]->lookDirection.x, actors[a]->lookDirection.y, actors[a]->lookDirection.z) * MatrixTranslation(matrix._41, matrix._42, matrix._43) * MatrixTranslation( 0.0f, (actors[a]->height * shape->getLocalScaling().getY()) / 2, 0.0f );
        Matrix44 offsetedcam = (actors[a]->CameraOffset * cam_mat);
        actors[a]->camera->setPose(offsetedcam);

        Matrix44 new_matrix = MatrixRotationY( -actors[a]->lookDirection.z) * MatrixTranslation(matrix._41, matrix._42, matrix._43);
        actors[a]->ghostObject->getWorldTransform().setFromOpenGLMatrix((float*)&new_matrix);

        // Update animations.
        if(actors[a]->animations.size() > 0 && actors[a]->mesh->skeleton->animations.size())
        {
            for(uint32_t i = 0; i < actors[a]->animations.size(); i++)
            {
                if(actors[a]->animations[i]->current_time > actors[a]->animations[i]->end_time)
                {
                    if(actors[a]->animations[i]->animation_state == 2)
                    {
                        actors[a]->animations[i]->current_time = actors[a]->animations[i]->start_time;
                    }
                    else
                    {
                        actors[a]->animations[i]->current_time = actors[a]->animations[i]->end_time;
                    }
                }
                actors[a]->pose->animate( actors[a]->animations[i]->animation, (float)actors[a]->animations[i]->current_time);

                if(actors[a]->animations[i]->animation_state > 0)
                {
                    actors[a]->animations[i]->current_time = actors[a]->animations[i]->current_time + (((double)UpdateTime)/100);
                }
            }
        }
        else
        {
            for(uint32_t i = 0; i < actors[a]->pose->matrices.size(); i++)
            {
                actors[a]->pose->matrices[i] = MatrixIdentity();
            }
        }

        for(uint32_t i = 0; i < actors[a]->pose->matrices.size(); i++)
        {
            actors[a]->pose->matrices[i] = actors[a]->pose->matrices[i] * (actors[a]->offset * new_matrix);
        }
    }

    // Update camera positions.
    for( uint32_t c = 0; c < cameras.size(); c++ )
    {
        for( uint32_t w = 0; w < context->windows.size(); w++ )
            for( uint32_t r = 0; r < context->windows[w]->render_targets.size(); r++ )
            {
                if( context->windows[w]->render_targets[r]->view == cameras[c] )
                {
                    Matrix44 cmatrix = cameras[c]->getPose();
                    ALfloat listener_ori[] = { -cmatrix._21, cmatrix._22, -cmatrix._23, -cmatrix._31, cmatrix._32, -cmatrix._33 };
                    ALfloat listener_pos[] = { cmatrix._41, cmatrix._42, cmatrix._43 };
                    ALfloat listener_vel[] = { 0.0, 0.0, 0.0 };
                    alListenerfv(AL_POSITION,	listener_pos);
                    alListenerfv(AL_VELOCITY,	listener_vel);
                    alListenerfv(AL_ORIENTATION, listener_ori);

                    for( uint32_t i = 0; i < cameras[c]->voices.size(); i++ )
                    {
                        if(cameras[c]->voices[i] != NULL)
                        {
                            if(cameras[c]->voices[i]->instance->emitter != NULL)
                            {
                                ALint state;
                                alGetSourcei(cameras[c]->voices[i]->source, AL_SOURCE_STATE, &state);
                                if(state == AL_PLAYING)
                                {
                                    Matrix44 ematrix = cameras[c]->voices[i]->instance->emitter->getPose();
                                    ALfloat source_ori[] = { ematrix._21, ematrix._22, ematrix._23, ematrix._31, ematrix._32, ematrix._33 };
                                    ALfloat source_pos[] = { ematrix._41, ematrix._42, ematrix._43 };
                                    ALfloat source_vel[] = { 0.0, 0.0, 0.0 };
                                    alSourcefv( cameras[c]->voices[i]->source, AL_POSITION, source_pos );
                                    alSourcefv( cameras[c]->voices[i]->source, AL_VELOCITY, source_vel );
                                    alSourcefv( cameras[c]->voices[i]->source, AL_ORIENTATION, source_ori );
                                }
                            }
                        }
                    }
                }
            }
    }

    // Update prop bones.
    for( uint32_t p = 0; p < props.size(); p++ )
    {
        Matrix44 mat = MatrixIdentity();
        props[p]->update(props[p]->mesh->skeleton->root_bone, &mat );
    }

    // Update physics scene.
    DynamicsWorld->stepSimulation( ((float)(UpdateTime)) / 1000.0f, UpdateTime / 5 );
};

void Actor::Release()
{
    for( uint32_t i = 0; i < scene->actors.size(); i++)
    {
        if( scene->actors[i] == this)
        {
            scene->actors.erase( scene->actors.begin() + i );
        }
    }
    scene->DynamicsWorld->removeCollisionObject( ghostObject );
    delete ghostObject;
    scene->DynamicsWorld->removeAction(controller);
    delete controller;
    delete this;
}

void Emitter::Release()
{
    for( uint32_t l = 0; l < scene->emitters.size(); l++)
    {
        if( scene->emitters[l] == this)
        {
            scene->emitters.erase( scene->emitters.begin() + l );
        }
    }
    delete cmesh->actor->getCollisionShape();
    delete cmesh;
    delete this;
}

void Light::Release()
{
    for( uint32_t l = 0; l < scene->lights.size(); l++)
    {
        if( scene->lights[l] == this)
        {
            scene->lights.erase( scene->lights.begin() + l );
        }
    }
    delete cmesh->actor->getCollisionShape();
    delete cmesh;
    delete this;
}

void Camera::Release()
{
    for( uint32_t c = 0; c < scene->cameras.size(); c++)
    {
        if( scene->cameras[c] == this)
        {
            scene->cameras.erase( scene->cameras.begin() + c );
        }
    }
    delete cmesh->actor->getCollisionShape();
    delete cmesh;
    delete this;
}

void Prop::Release()
{
    for( uint32_t i = 0; i < scene->props.size(); i++)
    {
        if( scene->props[i] == this)
        {
            scene->props.erase( scene->props.begin() + i );
        }
    }
    for( uint32_t i = 0; i < bones.size(); i++)
    {
        delete bones[i];
    }
    delete this;
}

void Object::Release()
{
    for( uint32_t i = 0; i < scene->objects.size(); i++)
    {
        if( scene->objects[i] == this)
        {
            scene->objects.erase( scene->objects.begin() + i );
        }
    }
    delete cmesh;
    delete this;
}

void Scene::Release()
{

    for( int32_t i = DynamicsWorld->getNumConstraints() - 1; i >= 0 ; i--)
    {
        btTypedConstraint* constraint = DynamicsWorld->getConstraint(i);
        DynamicsWorld->removeConstraint(constraint);
        delete constraint;
    }

    for( uint32_t i = 0; i < actors.size(); i++ )
    {
        actors[i]->Release();
    }
    for( uint32_t i = 0; i < props.size(); i++ )
    {
        props[i]->Release();
    }
    for( uint32_t i = 0; i < objects.size(); i++ )
    {
        objects[i]->Release();
    }
    for( uint32_t i = 0; i < cameras.size(); i++ )
    {
        cameras[i]->Release();
    }
    for( uint32_t i = 0; i < lights.size(); i++ )
    {
        lights[i]->Release();
    }
    for( uint32_t i = 0; i < emitters.size(); i++ )
    {
        emitters[i]->Release();
    }

    delete DynamicsWorld;
    delete this;
}

void Constraint::Release()
{
    delete joint;
    delete this;
}

Prop* Actor::Kill()
{
    Matrix44 mat = (offset * getPose());
    Prop* body = scene->CreateProp(mesh, mat, true );
    Release();
    return body;
}
}
