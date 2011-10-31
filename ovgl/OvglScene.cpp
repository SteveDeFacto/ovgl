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
* @brief None.
*/

#include "OvglIncludes.h"
#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"

struct DisablePairCollision : public btCollisionWorld::ContactResultCallback
{
	virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1)
	{
		btTransform frame;
		frame.setIdentity();
		btGeneric6DofConstraint* Constraint;
		Constraint = new btGeneric6DofConstraint( *(btRigidBody*)colObj0, *(btRigidBody*)colObj1, frame, frame, true );
		Constraint->setLinearLowerLimit( btVector3(1, 1, 1 ) );
		Constraint->setLinearUpperLimit( btVector3(0, 0, 0 ) );
		Constraint->setAngularLowerLimit( btVector3(1, 1, 1 ) );
		Constraint->setAngularUpperLimit( btVector3(0, 0, 0 ) );
		DynamicsWorld->addConstraint(Constraint, true);
		return 0;
	}

	btDiscreteDynamicsWorld*	DynamicsWorld;
};

Ovgl::Camera* Ovgl::Scene::CreateCamera( Ovgl::Matrix44* matrix )
{
	Ovgl::Camera* camera = new Ovgl::Camera;
	camera->scene = this;
	camera->projMat = Ovgl::MatrixPerspectiveLH( (((float)OvglPi) / 2.0f), (640.0f / 480.0f) , 0.01f, 100000.0f );
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btSphereShape* Sphere = new btSphereShape(1.0f);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );
	Ovgl::CMesh* CollisionMesh = new Ovgl::CMesh;
	CollisionMesh->scene = this;
	CollisionMesh->actor = new btRigidBody(rbInfo);
	CollisionMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	camera->CollisionMesh = CollisionMesh;
	DynamicsWorld->addRigidBody(CollisionMesh->actor, btBroadphaseProxy::KinematicFilter, 0);
	this->cameras.push_back( camera );
	return camera;
};

Ovgl::Light* Ovgl::Scene::CreateLight( Ovgl::Matrix44* matrix, Ovgl::Vector4* color )
{
	Ovgl::Light* light = new Ovgl::Light;
	light->scene = this;
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btSphereShape* Sphere = new btSphereShape(1.0f);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );
	Ovgl::CMesh* CollisionMesh = new Ovgl::CMesh;
	CollisionMesh->scene = this;
	CollisionMesh->actor = new btRigidBody(rbInfo);
	CollisionMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	light->CollisionMesh = CollisionMesh;
	DynamicsWorld->addRigidBody(CollisionMesh->actor, btBroadphaseProxy::KinematicFilter, 0);
	light->color.x = color->x;
	light->color.y = color->y;
	light->color.z = color->z;
	this->lights.push_back( light );
	return light;
};

Ovgl::Emitter* Ovgl::Scene::CreateEmitter( Ovgl::Matrix44* matrix )
{
	Ovgl::Emitter* emitter = new Ovgl::Emitter;
	emitter->scene = this;
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btSphereShape* Sphere = new btSphereShape(1.0f);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, Sphere, btVector3(0,0,0) );
	Ovgl::CMesh* CollisionMesh = new Ovgl::CMesh;
	CollisionMesh->scene = this;
	CollisionMesh->actor = new btRigidBody(rbInfo);
	CollisionMesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	emitter->CollisionMesh = CollisionMesh;
	DynamicsWorld->addRigidBody(CollisionMesh->actor, btBroadphaseProxy::KinematicFilter, 0);
	this->emitters.push_back( emitter );
	return emitter;
};

Ovgl::Prop* Ovgl::Scene::CreateProp( Ovgl::Mesh* mesh, Ovgl::Matrix44* matrix )
{
	Ovgl::Prop* prop = new Ovgl::Prop;
	prop->scene = this;
	prop->mesh = mesh;
	prop->materials.resize(mesh->subset_count);
	for( DWORD s = 0; s < prop->materials.size(); s++)
	{
		prop->materials[s] = Inst->DefaultMedia->Materials[0];
	}
	for( DWORD i = 0; i < mesh->bones.size(); i++ )
	{
		btTransform Transform;
		Transform.setFromOpenGLMatrix((float*)&(mesh->bones[i]->matrix * (*matrix) ) );
		btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
		btVector3 localInertia(0,0,0);
		mesh->bones[i]->convex->calculateLocalInertia(1, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rbInfo( mesh->bones[i]->volume, MotionState, mesh->bones[i]->convex, localInertia );
		Ovgl::CMesh* CollisionMesh = new Ovgl::CMesh;
		CollisionMesh->scene = this;
		CollisionMesh->actor = new btRigidBody(rbInfo);
		DynamicsWorld->addRigidBody(CollisionMesh->actor, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
		prop->bones.push_back(CollisionMesh);
	}
	prop->joints.resize(prop->bones.size());
	prop->CreateJoints( prop->mesh->root_bone );
	for(UINT np = 0; np < prop->bones.size(); np++)
	{
		for(UINT nq = 0; nq < prop->bones.size(); nq++)
		{
			if( np!=nq )
			{
				DisablePairCollision Callback;
				Callback.DynamicsWorld = DynamicsWorld;
				DynamicsWorld->contactPairTest(prop->bones[np]->actor, prop->bones[nq]->actor, Callback);
			}
		}
	}
	this->props.push_back( prop );
	prop->matrices.resize( prop->bones.size() );
	return prop;
};

Ovgl::Object* Ovgl::Scene::CreateObject( Ovgl::Mesh* mesh, Ovgl::Matrix44* matrix )
{
	Ovgl::Object* object = new Ovgl::Object;
	object->Scene = this;
	object->mesh = mesh;
	object->materials.resize(mesh->subset_count);
	bool zeroFound = false;
	for( DWORD i = 0; i < mesh->attributes.size(); i++ )
	{
		if( mesh->attributes[i] == 0)
		{
			zeroFound = true;
		}
	}
	if(zeroFound)
	{
		object->materials[0] = Inst->DefaultMedia->Materials[0];
	}
	for( DWORD s = 0; s < object->materials.size() - zeroFound; s++)
	{
		object->materials[s + zeroFound] = mesh->materials[s];
	}
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, mesh->TriangleMesh, btVector3(0,0,0) );
	Ovgl::CMesh* CollisionMesh = new Ovgl::CMesh;
	CollisionMesh->scene = this;
	CollisionMesh->actor = new btRigidBody(rbInfo);
	CollisionMesh->actor->setCollisionFlags( CollisionMesh->actor->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	object->CollisionMesh = CollisionMesh;
	DynamicsWorld->addRigidBody(CollisionMesh->actor, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
	this->objects.push_back( object );
	return object;
};

Ovgl::Actor* Ovgl::Scene::CreateActor( Ovgl::Mesh* mesh, float radius, float height, Ovgl::Matrix44* matrix, Ovgl::Matrix44* offset )
{
	Ovgl::Actor* actor = new Ovgl::Actor;
	actor->mesh = mesh;
	if( mesh )
	{
		actor->matrices.resize( mesh->bones.size() );
		for( DWORD i = 0; i < actor->matrices.size(); i++ )
		{
			actor->matrices[i] = MatrixIdentity();
		}
		actor->materials.resize( mesh->subset_count );
		bool zeroFound = false;
		for( DWORD i = 0; i < mesh->attributes.size(); i++ )
		{
			if( mesh->attributes[i] == 0)
			{
				zeroFound = true;
			}
		}
		if(zeroFound)
		{
			actor->materials[0] = Inst->DefaultMedia->Materials[0];
		}
		for( DWORD s = 0; s < actor->materials.size() - zeroFound; s++)
		{
			actor->materials[s + zeroFound] = mesh->materials[s];
		}
	}
	actor->crouch = false;
	actor->onGround = false;
	actor->offset = *offset;
	actor->CameraOffset = MatrixIdentity();
	actor->lookDirection = Ovgl::Vector3( 0.0f, 0.0f, 0.0f );
	actor->velocity = Ovgl::Vector3( 0.0f, 0.0f, 0.0f );
	actor->walkDirection = Ovgl::Vector3( 0.0f, 0.0f, 0.0f );
	actor->height = height;
	actor->radius = radius;
	actor->maxSlope = (float)(OvglPi / 4);
	actor->scene = this;
	btTransform startTransform;
	startTransform.setIdentity ();
	startTransform.setFromOpenGLMatrix((float*)matrix);
	actor->ghostObject = new btPairCachingGhostObject();
	actor->ghostObject->setWorldTransform(startTransform);
	Inst->PhysicsBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btConvexShape* capsule = new btCapsuleShape(radius, height);
	actor->ghostObject->setCollisionShape (capsule);
	actor->ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	actor->controller = new btKinematicCharacterController(actor->ghostObject, capsule, height/3.0f);
	actor->controller->setMaxSlope(actor->maxSlope);
	actor->controller->setGravity(9.8f);
	DynamicsWorld->addCollisionObject(actor->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	DynamicsWorld->addAction(actor->controller);
	Ovgl::Matrix44 camera_matrix;
	camera_matrix = *matrix * Ovgl::MatrixTranslation( 0.0f, height, 0.0f );
	actor->camera = CreateCamera( &camera_matrix );
	actors.push_back(actor);
	return actor;
};

Ovgl::Joint* Ovgl::Scene::CreateJoint( Ovgl::CMesh* obj1, Ovgl::CMesh* obj2)
{
	Ovgl::Joint* joint = new Ovgl::Joint;
	joint->scene = this;
	joint->obj[0] = obj1;
	joint->obj[1] = obj2;
	Ovgl::Matrix44 bodyMatA, bodyMatB;
	bodyMatA = joint->obj[0]->getPose();
	bodyMatB = joint->obj[1]->getPose();
	btTransform frameInA, frameInB;
	frameInA.setFromOpenGLMatrix((float*)&(bodyMatB * Ovgl::MatrixInverse(&Ovgl::Vector4( 0.0f, 0.0f, 0.0f, 0.0f), &bodyMatA)));
	frameInB.setIdentity();
	joint->joint = new btGeneric6DofConstraint( *obj1->actor, *obj2->actor, frameInA, frameInB, true );
	this->DynamicsWorld->addConstraint(joint->joint, true);
	joints.push_back(joint);
	return joint;
}

void Ovgl::Actor::Jump( float force )
{
	controller->setJumpSpeed( force );
	controller->jump();
}

void Ovgl::Prop::setPose( Ovgl::Matrix44* matrix )
{
	bones[mesh->root_bone]->setPose(matrix);
}

void Ovgl::Object::setPose( Ovgl::Matrix44* matrix )
{
	btTransform Transform;
	Transform.setFromOpenGLMatrix( (float*)matrix );
	CollisionMesh->actor->setWorldTransform(Transform);
}

void Ovgl::Camera::setPose( Ovgl::Matrix44* matrix )
{
	btTransform Transform;
	Transform.setFromOpenGLMatrix( (float*)matrix );
	CollisionMesh->actor->setWorldTransform(Transform);
}

Ovgl::Matrix44 Ovgl::Prop::getPose()
{
	Ovgl::Matrix44 matrix;
	bones[mesh->root_bone]->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Object::getPose()
{
	Ovgl::Matrix44 matrix;
	CollisionMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Emitter::getPose()
{
	Ovgl::Matrix44 matrix;
	CollisionMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Camera::getPose()
{
	Ovgl::Matrix44 matrix;
	CollisionMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

Ovgl::Matrix44 Ovgl::Light::getPose()
{
	Ovgl::Matrix44 matrix;
	CollisionMesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

Ovgl::Matrix44 Ovgl::Actor::getPose()
{
	Ovgl::Matrix44 matrix;
	ghostObject->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

void Ovgl::Prop::Update( int bone, Ovgl::Matrix44* matrix )
{
	Ovgl::Matrix44 inv_matrix, inv_mesh_bone, tmatrix;
	matrices[bone] = bones[bone]->getPose();
	if( this->mesh->bones.size() > 0 )
	{
		inv_mesh_bone = Ovgl::MatrixInverse( &Ovgl::Vector4(0,0,0,0), &this->mesh->bones[bone]->matrix );
		matrices[bone] = ( inv_mesh_bone * matrices[bone]  );
		inv_matrix = Ovgl::MatrixInverse( &Ovgl::Vector4(0,0,0,0), matrix );
		tmatrix = matrices[bone] * inv_matrix;
		for (UINT i = 0; i < this->mesh->bones[bone]->childen.size(); i++)
		{
			Ovgl::Matrix44 child;
			child = mesh->bones[mesh->bones[bone]->childen[i]]->matrix.Translation();
			child = child * tmatrix * (*matrix);
			Ovgl::Prop::Update( mesh->bones[bone]->childen[i], &child );	
		}
	}
}

void Ovgl::Actor::UpdateAnimation( int bone, Ovgl::Matrix44* matrix, DWORD time )
{
	// Initialize animation rotation matrix.
	Ovgl::Matrix44 animRot;
	Ovgl::Matrix44 animRot2;
	animRot = Ovgl::MatrixIdentity();
	animRot2 = Ovgl::MatrixIdentity();

	// Get animation rotation.
	float currentRot[3] = {0};
	for( unsigned int a = 0; a < 3; a++)
	{
		Ovgl::Curve uCurve;
		Ovgl::Curve lCurve;
		uCurve.time = ULONG_MAX;
		uCurve.value = 0;
		lCurve.time = 0;
		lCurve.value = 0;

		// Find two frames that are directly before and after the current time.
		for( unsigned int i = 0; i < mesh->bones[bone]->Rot_Keys[a].size(); i++)
		{
			Ovgl::Curve nCurve = mesh->bones[bone]->Rot_Keys[a][i];
			if(nCurve.time > lCurve.time && nCurve.time < time )
			{
				lCurve = nCurve;
			}
			if(nCurve.time < uCurve.time && nCurve.time > time )
			{
				uCurve = nCurve;
			}
		}

		// If we can't find an upper curve then just set it to the lower curve.
		if(uCurve.time == ULONG_MAX)
		{
			uCurve = lCurve;
		}

		// Check if we found any frames then interpolate between the two curves.
		if(uCurve.time > 0)
		{
			currentRot[a] = Ovgl::DegToRad(Ovgl::Lerp(lCurve.value, uCurve.value, (float)(time - lCurve.time) / (float)(uCurve.time - lCurve.time) ));
		}
	}
	animRot = Ovgl::MatrixRotationEuler(currentRot[0], currentRot[1], currentRot[2]);

	// Offset the center of rotation.
	animRot2 = Ovgl::MatrixInverse( &Ovgl::Vector4(), &mesh->bones[bone]->matrix.Translation()) * animRot * mesh->bones[bone]->matrix.Translation(); 

	// Get difference from original pose to the animated pose.
	matrices[bone] = animRot2 * (*matrix) * Ovgl::MatrixInverse( &Ovgl::Vector4(), &mesh->bones[bone]->matrix);

	// Loop through all child bones and update their animations.
	for( unsigned int i = 0; i < mesh->bones[bone]->childen.size(); i++)
	{
		Ovgl::Matrix44 accumulate;
		Ovgl::Matrix44 Bone2Parent;
		Bone2Parent = Ovgl::MatrixInverse( &Ovgl::Vector4(), &mesh->bones[bone]->matrix ) * mesh->bones[mesh->bones[bone]->childen[i]]->matrix;
		accumulate = animRot2 * (*matrix) * Bone2Parent;
		Ovgl::Actor::UpdateAnimation( mesh->bones[bone]->childen[i], &accumulate, time );
	}
}

void Ovgl::Prop::CreateJoints( DWORD bone )
{
	if( mesh->bones[bone]->childen.size() > 0 )
	{
		Ovgl::Joint* joint = new Ovgl::Joint;
		joint->scene = this->scene;
		joint->obj[0] = bones[bone];
		for(UINT i = 0; i < mesh->bones[bone]->childen.size(); i++)
		{
			joint->obj[1] = bones[mesh->bones[bone]->childen[i]];
			Ovgl::Bone* childBone = this->mesh->bones[mesh->bones[bone]->childen[i]];
			Ovgl::Matrix44 bodyMatA, bodyMatB;
			bodyMatA = joint->obj[0]->getPose();
			bodyMatB = joint->obj[1]->getPose();
			btTransform frameInA, frameInB;
			frameInA.setFromOpenGLMatrix((float*)&(bodyMatB * Ovgl::MatrixInverse(&Ovgl::Vector4( 0.0f, 0.0f, 0.0f, 0.0f), &bodyMatA)));
			frameInB.setIdentity();
			joint->joint = new btGeneric6DofConstraint( *joint->obj[0]->actor, *joint->obj[1]->actor, frameInA, frameInB, true );
			joint->joint->setAngularLowerLimit( btVector3(Ovgl::DegToRad(-childBone->max.x), Ovgl::DegToRad(childBone->min.y), Ovgl::DegToRad(-childBone->max.z)) );
			joint->joint->setAngularUpperLimit( btVector3(Ovgl::DegToRad(-childBone->min.x), Ovgl::DegToRad(childBone->max.y), Ovgl::DegToRad(-childBone->min.z)) );
			scene->DynamicsWorld->addConstraint(joint->joint, true);
			joints[mesh->bones[bone]->childen[i]] = joint;
			Ovgl::Prop::CreateJoints(mesh->bones[bone]->childen[i]);
		}
	}
}

Ovgl::Animation* Ovgl::Prop::CreateAnimation( float current, float start, float end )
{
	Animation* animation = new Ovgl::Animation;
	animation->animationState = 0;
	animation->currentTime = current;
	animation->startTime = start;
	animation->endTime = end;
	animation->stepTime = 1;
	return animation;
}

void Ovgl::Scene::Update( DWORD UpdateTime )
{
	// Update actor positions.
	for(UINT a = 0; a < actors.size(); a++)
	{
		Ovgl::Vector3 corrected_trajectory;
		corrected_trajectory = Ovgl::Vector3Transform( &(actors[a]->walkDirection / (1.0f + (float)actors[a]->crouch) ), &Ovgl::MatrixRotationY( -actors[a]->lookDirection.z) );
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

		Ovgl::Matrix44 matrix;
		matrix = actors[a]->getPose();
		Ovgl::Matrix44 cam_mat;
		cam_mat = Ovgl::MatrixTranslation( 0.0f, 0.0f, actors[a]->radius / 2 ) * Ovgl::MatrixRotationEuler( actors[a]->lookDirection.x, actors[a]->lookDirection.y, actors[a]->lookDirection.z) * Ovgl::MatrixTranslation(matrix._41, matrix._42, matrix._43) * Ovgl::MatrixTranslation( 0.0f, (actors[a]->height * shape->getLocalScaling().getY()) / 2, 0.0f );
		actors[a]->camera->setPose(&(actors[a]->CameraOffset * cam_mat));

		Ovgl::Matrix44 new_matrix = Ovgl::MatrixRotationY( -actors[a]->lookDirection.z) * Ovgl::MatrixTranslation(matrix._41, matrix._42, matrix._43);
		actors[a]->ghostObject->getWorldTransform().setFromOpenGLMatrix((float*)&new_matrix);

		// Update animations.
		static float aTime = 0;
		if(actors[a]->mesh != NULL)
		{
			actors[a]->UpdateAnimation( actors[a]->mesh->root_bone, &Ovgl::MatrixIdentity(), aTime /15);
			aTime = aTime + UpdateTime;
		}
		for(unsigned int i = 0; i < actors[a]->matrices.size(); i++)
		{
			actors[a]->matrices[i] = actors[a]->matrices[i]* (actors[a]->offset * new_matrix);
		}
	}

	// Update camera positions.
	for( UINT c = 0; c < cameras.size(); c++ )
	{
		for( UINT r = 0; r < Inst->RenderTargets.size(); r++ )
		{
			if( Inst->RenderTargets[r]->view == cameras[c] )
			{
				Ovgl::Matrix44* cmatrix = &cameras[c]->getPose();
				ALfloat ListenerOri[] = { -cmatrix->_21, cmatrix->_22, -cmatrix->_23, -cmatrix->_31, cmatrix->_32, -cmatrix->_33 };
				ALfloat ListenerPos[] = { cmatrix->_41, cmatrix->_42, cmatrix->_43 };
				ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
				alListenerfv(AL_POSITION,	ListenerPos);
				alListenerfv(AL_VELOCITY,	ListenerVel);
				alListenerfv(AL_ORIENTATION, ListenerOri);
		
				for( UINT i = 0; i < cameras[c]->voices.size(); i++ )
				{
					if(cameras[c]->voices[i] != NULL)
					{
						if(cameras[c]->voices[i]->instance->emitter != NULL)
						{
							ALint state;
							alGetSourcei(cameras[c]->voices[i]->source, AL_SOURCE_STATE, &state);
							if(state == AL_PLAYING)
							{
								Ovgl::Matrix44* ematrix = &cameras[c]->voices[i]->instance->emitter->getPose();
								ALfloat SourceOri[] = { ematrix->_21, ematrix->_22, ematrix->_23, ematrix->_31, ematrix->_32, ematrix->_33 };
								ALfloat SourcePos[] = { ematrix->_41, ematrix->_42, ematrix->_43 };
								ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
								alSourcefv( cameras[c]->voices[i]->source, AL_POSITION, SourcePos );
								alSourcefv( cameras[c]->voices[i]->source, AL_VELOCITY, SourceVel );
								alSourcefv( cameras[c]->voices[i]->source, AL_ORIENTATION, SourceOri );
							}
						}
					}
				}
			}
		}
	}

	// Update prop bones.
	for( UINT p = 0; p < props.size(); p++ )
	{
		props[p]->Update(props[p]->mesh->root_bone, &Ovgl::MatrixIdentity() );
	}

	// Update physics scene.
	DynamicsWorld->stepSimulation( ((float)(UpdateTime)) / 1000.0f, UpdateTime / 5 );
};

void Ovgl::Actor::Release()
{
	for( unsigned int i = 0; i < scene->actors.size(); i++)
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

void Ovgl::Emitter::Release()
{
	for( unsigned int l = 0; l < scene->emitters.size(); l++)
	{
		if( scene->emitters[l] == this)
		{
			scene->emitters.erase( scene->emitters.begin() + l );
		}
	}
	delete CollisionMesh->actor->getCollisionShape();
	CollisionMesh->Release();
	delete this;
}

void Ovgl::Light::Release()
{
	for( unsigned int l = 0; l < scene->lights.size(); l++)
	{
		if( scene->lights[l] == this)
		{
			scene->lights.erase( scene->lights.begin() + l );
		}
	}
	delete CollisionMesh->actor->getCollisionShape();
	CollisionMesh->Release();
	delete this;
}

void Ovgl::Camera::Release()
{
	for( unsigned int c = 0; c < scene->cameras.size(); c++)
	{
		if( scene->cameras[c] == this)
		{
			scene->cameras.erase( scene->cameras.begin() + c );
		}
	}
	delete CollisionMesh->actor->getCollisionShape();
	CollisionMesh->Release();
	delete this;
}

void Ovgl::Prop::Release()
{
	for( unsigned int i = 0; i < scene->props.size(); i++)
	{
		if( scene->props[i] == this)
		{
			scene->props.erase( scene->props.begin() + i );
		}
	}
	for( unsigned int i = 0; i < bones.size(); i++)
	{
		bones[i]->Release();
	}
	delete this;
}

void Ovgl::Object::Release()
{
	for( unsigned int i = 0; i < Scene->objects.size(); i++)
	{
		if( Scene->objects[i] == this)
		{
			Scene->objects.erase( Scene->objects.begin() + i );
		}
	}
	CollisionMesh->Release();
	delete this;
}

void Ovgl::Scene::Release()
{

	for( int i = DynamicsWorld->getNumConstraints() - 1; i >= 0 ; i--)
	{
		btTypedConstraint* constraint = DynamicsWorld->getConstraint(i);
		DynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	for( unsigned int i = 0; i < actors.size(); i++ )
	{
		actors[i]->Release();
	}
	for( unsigned int i = 0; i < props.size(); i++ )
	{
		props[i]->Release();
	}
	for( unsigned int i = 0; i < objects.size(); i++ )
	{
		objects[i]->Release();
	}
	for( unsigned int i = 0; i < cameras.size(); i++ )
	{
		cameras[i]->Release();
	}
	for( unsigned int i = 0; i < lights.size(); i++ )
	{
		lights[i]->Release();
	}
	for( unsigned int i = 0; i < emitters.size(); i++ )
	{
		emitters[i]->Release();
	}

	delete DynamicsWorld;
	delete this;
}

void Ovgl::Joint::Release()
{
	delete joint;
	delete this;
}

Ovgl::Prop* Ovgl::Actor::Kill()
{
	Ovgl::Prop* body = scene->CreateProp(mesh, &(offset * getPose()) );
	Release();
	return body;
}