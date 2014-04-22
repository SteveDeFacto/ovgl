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
 * @brief This part deals with 3D scenes and objects.
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
Camera* Scene::createCamera( const Matrix44& matrix )
{
	// Create a new camera object.
	Camera* camera = new Camera;

	// Link this scene to camera.
	camera->scene = this;

	// Set default perspective for camera.
	camera->projMat = matrixPerspectiveLH( (((float)OvglPi) / 2.0f), (640.0f / 480.0f) , 0.01f, 100000.0f );

	// Build a motion state object for bullet positioned where we want the camera.
	btTransform transform;
	transform.setFromOpenGLMatrix((float*)&matrix);
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);

	// Create a sphere shape.
	btSphereShape* sphere = new btSphereShape(1.0f);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, motionState, sphere, btVector3(0,0,0) );

	// Create a new collision mesh.
	CMesh* cMesh = new CMesh;

	// Link scene to collision mesh.
	cMesh->scene = this;

	// Create a dynamic object, set it to not respond to contact, and link it to the camera.
	cMesh->actor = new btRigidBody(rbInfo);
	cMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	camera->cMesh = cMesh;

	// Add dynamic object to physics scene.
	dynamicsWorld->addRigidBody(cMesh->actor, btBroadphaseProxy::KinematicFilter, 0);

	// Add camera to scene list of cameras.
	cameras.push_back( camera );

	// Return pointer to light.
	return camera;
};

Light* Scene::createLight( const Matrix44& matrix, const Vector4& color )
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
	CMesh* cMesh = new CMesh;

	// Link scene to collision mesh.
	cMesh->scene = this;

	// Create a dynamic object, set it to not respond to contact, and link it to the light.
	cMesh->actor = new btRigidBody(rbInfo);
	cMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	light->cMesh = cMesh;

	// Add dynamic object to physics scene.
	dynamicsWorld->addRigidBody(cMesh->actor, btBroadphaseProxy::KinematicFilter, 0);

	// Set light colors.
	light->color.x = color.x;
	light->color.y = color.y;
	light->color.z = color.z;

	// Add light to scene list of lights.
	this->lights.push_back( light );

	// Return pointer to light.
	return light;
};

Emitter* Scene::createEmitter( const Matrix44& matrix )
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
	CMesh* cMesh = new CMesh;

	// Link scene to collision mesh.
	cMesh->scene = this;

	// Create a dynamic object, set it to not respond to contact, and link it to the emitter.
	cMesh->actor = new btRigidBody(rbInfo);
	cMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	emitter->cMesh = cMesh;

	// Add dynamic object to physics scene.
	dynamicsWorld->addRigidBody(cMesh->actor, btBroadphaseProxy::KinematicFilter, 0);

	// Add emitter to scene list of emitters.
	this->emitters.push_back( emitter );

	// Return pointer to emitter.
	return emitter;
};

Prop* Scene::createProp( Mesh* mesh, const Matrix44& matrix, bool disable_pair_collision )
{
	// Create a new prop object.
	Prop* prop = new Prop;

	// Link this scene to prop.
	prop->scene = this;
	prop->mesh = mesh;
	prop->materials.resize(mesh->subsetCount);
	for( uint32_t s = 0; s < prop->materials.size(); s++)
	{
		prop->materials[s] = context->defaultMedia->materials[0];
	}
	for( uint32_t i = 0; i < mesh->skeleton->bones.size(); i++ )
	{
		if(mesh->skeleton->bones[i]->convex != NULL)
		{
			btTransform transform;
			Matrix44 mat = (mesh->skeleton->bones[i]->matrix * matrix );
			transform.setFromOpenGLMatrix((float*)&mat );
			btDefaultMotionState* motionState = new btDefaultMotionState(transform);
			btVector3 localInertia(0,0,0);
			mesh->skeleton->bones[i]->convex->calculateLocalInertia(1, localInertia);
			btRigidBody::btRigidBodyConstructionInfo rbInfo( mesh->skeleton->bones[i]->volume, motionState, mesh->skeleton->bones[i]->convex, localInertia );
			CMesh* cMesh = new CMesh;
			cMesh->scene = this;
			cMesh->actor = new btRigidBody(rbInfo);
			dynamicsWorld->addRigidBody(cMesh->actor, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
			prop->bones.push_back(cMesh);
		}
		else
		{
			prop->bones.push_back(NULL);
		}
	}
	prop->constraints.resize(prop->bones.size());
	prop->createJoints( prop->mesh->skeleton->rootBone );
	if(disable_pair_collision)
	{
		for(uint32_t np = 0; np < prop->bones.size(); np++)
		{
			for(uint32_t nq = 0; nq < prop->bones.size(); nq++)
			{
				if( np!=nq && prop->bones[np] != NULL && prop->bones[nq] != NULL )
				{
					DisablePairCollision callback;
					callback.DynamicsWorld = dynamicsWorld;
					dynamicsWorld->contactPairTest(prop->bones[np]->actor, prop->bones[nq]->actor, callback);
				}
			}
		}
	}
	props.push_back( prop );
	prop->matrices.resize( prop->bones.size() );
	return prop;
};

Object* Scene::createObject( Mesh* mesh, const Matrix44& matrix )
{
	if( mesh != NULL)
	{
		Object* object = new Object;
		object->scene = this;
		object->mesh = mesh;
		object->materials.resize(mesh->subsetCount);
		for( uint32_t s = 0; s < object->materials.size(); s++)
		{
			object->materials[s] = context->defaultMedia->materials[0];
		}
		btTransform Transform;
		Transform.setFromOpenGLMatrix((float*)&matrix);
		btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, mesh->triangleMesh, btVector3(0,0,0) );
		CMesh* cMesh = new CMesh;
		cMesh->scene = this;
		cMesh->actor = new btRigidBody(rbInfo);
		cMesh->actor->setCollisionFlags( cMesh->actor->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		object->cMesh = cMesh;
		dynamicsWorld->addRigidBody(cMesh->actor, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
		this->objects.push_back( object );
		return object;
	}
	else
	{
		// File does not exist!
		fprintf(stderr, "Error: Unable to create object. Invalid mesh.");
		return NULL;
	}
};

Actor* Scene::createActor( Mesh* mesh, float radius, float height, const Matrix44& matrix, const Matrix44& offset )
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
			actor->pose->matrices[i] = matrixIdentity();
			actor->pose->joints[i]->offset = mesh->skeleton->bones[i]->matrix;
			actor->pose->joints[i]->localTransform = mesh->skeleton->bones[i]->localTransform;
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
		actor->pose->rootJoint = actor->pose->joints[mesh->skeleton->rootBone->index ];
		actor->materials.resize( mesh->subsetCount );
		for( uint32_t i = 0; i < mesh->subsetCount; i++ )
		{
			actor->materials[i] = context->defaultMedia->materials[0];
		}
	}
	actor->crouch = false;
	actor->onGround = false;
	actor->offset = offset;
	actor->cameraOffset = matrixIdentity();
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
	context->physicsBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btConvexShape* capsule = new btCapsuleShape(radius, height);
	actor->ghostObject->setCollisionShape (capsule);
	actor->ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	actor->controller = new btKinematicCharacterController(actor->ghostObject, capsule, height/3.0f);
	actor->controller->setMaxSlope(actor->maxSlope);
	actor->controller->setGravity(9.8f);
	dynamicsWorld->addCollisionObject(actor->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	dynamicsWorld->addAction(actor->controller);
	Matrix44 cameraMatrix;
	cameraMatrix = matrix * matrixTranslation( 0.0f, height, 0.0f );
	actor->camera = createCamera( cameraMatrix );
	actors.push_back(actor);
	return actor;
};

Constraint* Scene::createConstraint( CMesh* obj1, CMesh* obj2)
{
	Constraint* joint = new Constraint;
	joint->scene = this;
	joint->obj[0] = obj1;
	joint->obj[1] = obj2;
	Matrix44 bodyMatA, bodyMatB;
	bodyMatA = joint->obj[0]->getPose();
	bodyMatB = joint->obj[1]->getPose();
	btTransform frameInA, frameInB;
	Matrix44 diff = (bodyMatB * matrixInverse(Vector4( 0.0f, 0.0f, 0.0f, 0.0f), bodyMatA));
	frameInA.setFromOpenGLMatrix((float*)&diff);
	frameInB.setIdentity();
	joint->joint = new btGeneric6DofConstraint( *obj1->actor, *obj2->actor, frameInA, frameInB, true );
	this->dynamicsWorld->addConstraint(joint->joint, true);
	constraints.push_back(joint);
	return joint;
}

void Actor::jump( float force )
{
	controller->setJumpSpeed( force );
	controller->jump();
}

void Prop::setPose( const Matrix44& matrix )
{
	bones[mesh->skeleton->rootBone->index]->setPose(matrix);
}

void Object::setPose( const Matrix44& matrix )
{
	btTransform transform;
	transform.setFromOpenGLMatrix( (float*)&matrix );
	cMesh->actor->setWorldTransform(transform);
}

void Camera::setPose( const Matrix44& matrix )
{
	btTransform transform;
	transform.setFromOpenGLMatrix( (float*)&matrix );
	cMesh->actor->setWorldTransform(transform);
}

Matrix44 Prop::getPose()
{
	Matrix44 matrix;
	bones[mesh->skeleton->rootBone->index]->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Matrix44 Object::getPose()
{
	Matrix44 matrix;
	cMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Matrix44 Emitter::getPose()
{
	Matrix44 matrix;
	cMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Matrix44 Camera::getPose()
{
	Matrix44 matrix;
	cMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

Matrix44 Light::getPose()
{
	Matrix44 matrix;
	cMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
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
	Matrix44 invMatrix, invMeshBone, tMatrix;
	matrices[bone->index] = bones[bone->index]->getPose();
	if( this->mesh->skeleton->bones.size() > 0 )
	{
		invMeshBone = matrixInverse( Vector4(0,0,0,0), bone->matrix );
		matrices[bone->index] = ( invMeshBone * matrices[bone->index]  );
		invMatrix = matrixInverse( Vector4(0,0,0,0), *matrix );
		tMatrix = matrices[bone->index] * invMatrix;
		for (uint32_t i = 0; i < bone->children.size(); i++)
		{
			Matrix44 child;
			child = bone->children[i]->matrix.translation();
			child = child * tMatrix * (*matrix);
			Prop::update( bone->children[i], &child );
		}
	}
}

void Prop::createJoints( Bone* bone )
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
			bodyMatA = joint->obj[0]->getPose();
			bodyMatB = joint->obj[1]->getPose();
			btTransform frameInA, frameInB;
			Matrix44 diff = (bodyMatB * matrixInverse(Vector4( 0.0f, 0.0f, 0.0f, 0.0f), bodyMatA));
			frameInA.setFromOpenGLMatrix((float*)&diff);
			frameInB.setIdentity();
			joint->joint = new btGeneric6DofConstraint( *joint->obj[0]->actor, *joint->obj[1]->actor, frameInA, frameInB, true );
			joint->joint->setAngularLowerLimit( btVector3(degToRad(-childBone->max.x), degToRad(childBone->min.y), degToRad(-childBone->max.z)) );
			joint->joint->setAngularUpperLimit( btVector3(degToRad(-childBone->min.x), degToRad(childBone->max.y), degToRad(-childBone->min.z)) );
			scene->dynamicsWorld->addConstraint(joint->joint, true);
			constraints[bone->children[i]->index] = joint;
			Prop::createJoints(bone->children[i]);
		}
	}
}

AnimationInstance* Prop::playAnimation( double start, double end, bool repeat )
{
	AnimationInstance* animation = new AnimationInstance;
	animation->animationState = 0;
	animation->currentTime = 0;
	animation->startTime = start;
	animation->endTime = end;
	animation->stepTime = 1;
	return animation;
}

AnimationInstance* Actor::playAnimation( Animation* anim, double start, double end, bool repeat )
{
	AnimationInstance* animation = new AnimationInstance;
	animation->animation = anim;
	animation->animationState = 1 + repeat;
	animation->currentTime = start;
	animation->startTime = start;
	animation->endTime = end;
	animation->stepTime = 1;
	animations.push_back(animation);
	return animation;
}

void Scene::update( uint32_t UpdateTime )
{
	// Update actor positions.
	for(uint32_t a = 0; a < actors.size(); a++)
	{
		Vector3 correctedTrajectory;
		correctedTrajectory = vector3Transform( (actors[a]->walkDirection / (1.0f + (float)actors[a]->crouch) ), matrixRotationY( -actors[a]->lookDirection.z) );
		actors[a]->controller->setWalkDirection(btVector3(correctedTrajectory.x, correctedTrajectory.y, correctedTrajectory.z));

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
		cam_mat = matrixTranslation( 0.0f, 0.0f, actors[a]->radius / 2 ) * matrixRotationEuler( actors[a]->lookDirection.x, actors[a]->lookDirection.y, actors[a]->lookDirection.z) * matrixTranslation(matrix._41, matrix._42, matrix._43) * matrixTranslation( 0.0f, (actors[a]->height * shape->getLocalScaling().getY()) / 2, 0.0f );
		Matrix44 offsetedcam = (actors[a]->cameraOffset * cam_mat);
		actors[a]->camera->setPose(offsetedcam);

		Matrix44 new_matrix = matrixRotationY( -actors[a]->lookDirection.z) * matrixTranslation(matrix._41, matrix._42, matrix._43);
		actors[a]->ghostObject->getWorldTransform().setFromOpenGLMatrix((float*)&new_matrix);

		// Update animations.
		if(actors[a]->animations.size() > 0 && actors[a]->mesh->skeleton->animations.size())
		{
			for(uint32_t i = 0; i < actors[a]->animations.size(); i++)
			{
				if(actors[a]->animations[i]->currentTime > actors[a]->animations[i]->endTime)
				{
					if(actors[a]->animations[i]->animationState == 2)
					{
						actors[a]->animations[i]->currentTime = actors[a]->animations[i]->startTime;
					}
					else
					{
						actors[a]->animations[i]->currentTime = actors[a]->animations[i]->endTime;
					}
				}
				actors[a]->pose->animate( actors[a]->animations[i]->animation, (float)actors[a]->animations[i]->currentTime);

				if(actors[a]->animations[i]->animationState > 0)
				{
					actors[a]->animations[i]->currentTime = actors[a]->animations[i]->currentTime + (((double)UpdateTime)/100);
				}
			}
		}
		else
		{
			for(uint32_t i = 0; i < actors[a]->pose->matrices.size(); i++)
			{
				actors[a]->pose->matrices[i] = matrixIdentity();
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
			for( uint32_t r = 0; r < context->windows[w]->renderTargets.size(); r++ )
			{
				if( context->windows[w]->renderTargets[r]->view == cameras[c] )
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
		Matrix44 mat = matrixIdentity();
		props[p]->update(props[p]->mesh->skeleton->rootBone, &mat );
	}

	// Update physics scene.
	dynamicsWorld->stepSimulation( ((float)(UpdateTime)) / 1000.0f, UpdateTime / 5 );
};

void Actor::release()
{
	for( uint32_t i = 0; i < scene->actors.size(); i++)
	{
		if( scene->actors[i] == this)
		{
			scene->actors.erase( scene->actors.begin() + i );
		}
	}
	scene->dynamicsWorld->removeCollisionObject( ghostObject );
	delete ghostObject;
	scene->dynamicsWorld->removeAction(controller);
	delete controller;
	delete this;
}

void Emitter::release()
{
	for( uint32_t l = 0; l < scene->emitters.size(); l++)
	{
		if( scene->emitters[l] == this)
		{
			scene->emitters.erase( scene->emitters.begin() + l );
		}
	}
	delete cMesh->actor->getCollisionShape();
	delete cMesh;
	delete this;
}

void Light::release()
{
	for( uint32_t l = 0; l < scene->lights.size(); l++)
	{
		if( scene->lights[l] == this)
		{
			scene->lights.erase( scene->lights.begin() + l );
		}
	}
	delete cMesh->actor->getCollisionShape();
	delete cMesh;
	delete this;
}

void Camera::release()
{
	for( uint32_t c = 0; c < scene->cameras.size(); c++)
	{
		if( scene->cameras[c] == this)
		{
			scene->cameras.erase( scene->cameras.begin() + c );
		}
	}
	delete cMesh->actor->getCollisionShape();
	delete cMesh;
	delete this;
}

void Prop::release()
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

void Object::release()
{
	for( uint32_t i = 0; i < scene->objects.size(); i++)
	{
		if( scene->objects[i] == this)
		{
			scene->objects.erase( scene->objects.begin() + i );
		}
	}
	delete cMesh;
	delete this;
}

void Scene::release()
{

	for( int32_t i = dynamicsWorld->getNumConstraints() - 1; i >= 0 ; i--)
	{
		btTypedConstraint* constraint = dynamicsWorld->getConstraint(i);
		dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	for( uint32_t i = 0; i < actors.size(); i++ )
	{
		actors[i]->release();
	}
	for( uint32_t i = 0; i < props.size(); i++ )
	{
		props[i]->release();
	}
	for( uint32_t i = 0; i < objects.size(); i++ )
	{
		objects[i]->release();
	}
	for( uint32_t i = 0; i < cameras.size(); i++ )
	{
		cameras[i]->release();
	}
	for( uint32_t i = 0; i < lights.size(); i++ )
	{
		lights[i]->release();
	}
	for( uint32_t i = 0; i < emitters.size(); i++ )
	{
		emitters[i]->release();
	}

	delete dynamicsWorld;
	delete this;
}

void Constraint::Release()
{
	delete joint;
	delete this;
}

Prop* Actor::kill()
{
	Matrix44 mat = (offset * getPose());
	Prop* body = scene->createProp(mesh, mat, true );
	release();
	return body;
}
}
