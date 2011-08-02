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
	virtual	btScalar	addSingleResult(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
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
	camera->projMat = Ovgl::MatrixPerspectiveLH( (((float)OvglPi) / 2.0f), (640.0f / 480.0f) , 0.01f, 1000.0f );
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, this->Inst->Shapes[0], btVector3(0,0,0) );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = new btRigidBody(rbInfo);
	cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	camera->cmesh = cmesh;
	DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);
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
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, this->Inst->Shapes[0], btVector3(0,0,0) );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = new btRigidBody(rbInfo);
	cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	light->cmesh = cmesh;
	DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);
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
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, this->Inst->Shapes[0], btVector3(0,0,0) );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = new btRigidBody(rbInfo);
	cmesh->actor->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	emitter->cmesh = cmesh;
	DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::KinematicFilter, 0);
	this->emitters.push_back( emitter );
	return emitter;
};

Ovgl::Prop* Ovgl::Scene::CreateProp( Ovgl::Mesh* mesh, Ovgl::Matrix44* matrix )
{
	Ovgl::Prop* prop = new Ovgl::Prop;
	prop->scene = this;
	prop->mesh = mesh;
	prop->subsets.resize(mesh->subset_count);
	for( DWORD s = 0; s < prop->subsets.size(); s++)
	{
		prop->subsets[s] = Inst->DefaultMedia->Materials[0];
	}
	for( DWORD i = 0; i < mesh->bones.size(); i++ )
	{
		btTransform Transform;
		Transform.setFromOpenGLMatrix((float*)&(mesh->bones[i]->matrix * (*matrix) ) );
		btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
		btVector3 localInertia(0,0,0);
		mesh->bones[i]->convex->calculateLocalInertia(1, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rbInfo( 1, MotionState, mesh->bones[i]->convex, localInertia );
		Ovgl::CMesh* cmesh = new Ovgl::CMesh;
		cmesh->actor = new btRigidBody(rbInfo);
		DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::CharacterFilter);
		prop->bones.push_back(cmesh);
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
	object->scene = this;
	object->mesh = mesh;
	object->subsets.resize(mesh->subset_count);
	for( DWORD s = 0; s < object->subsets.size(); s++)
	{
		object->subsets[s] = Inst->DefaultMedia->Materials[0];
	}
	btTriangleMesh* trimesh = new btTriangleMesh();
    for( UINT t = 0; t < mesh->faces.size(); t++ )
    {
        btVector3* v0 = (btVector3*)&mesh->vertices[mesh->faces[t].indices[0]].position;
        btVector3* v1 = (btVector3*)&mesh->vertices[mesh->faces[t].indices[1]].position;
        btVector3* v2 = (btVector3*)&mesh->vertices[mesh->faces[t].indices[2]].position;
        trimesh->addTriangle( *v0, *v1, *v2 );
    }
	btBvhTriangleMeshShape* TriangleMesh = new btBvhTriangleMeshShape(trimesh, true);
	TriangleMesh->setMargin(0.1f);
	btTransform Transform;
	Transform.setFromOpenGLMatrix((float*)matrix);
	btDefaultMotionState* MotionState = new btDefaultMotionState(Transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0, MotionState, TriangleMesh, btVector3(0,0,0) );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->scene = this;
	cmesh->actor = new btRigidBody(rbInfo);
	object->cmesh = cmesh;
	DynamicsWorld->addRigidBody(cmesh->actor, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
	this->objects.push_back( object );
	return object;
};

Ovgl::Actor* Ovgl::Scene::CreateActor( Ovgl::Mesh* mesh, float radius, float height, Ovgl::Matrix44* matirx )
{
	Ovgl::Actor* actor = new Ovgl::Actor;
	actor->crouch = false;
	actor->grounded = false;
	actor->direction = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->velocity = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->trajectory = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->height = height;
	actor->radius = radius;
	actor->scene = this;
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(matirx->_41, matirx->_42, matirx->_43));
	btPairCachingGhostObject* ghost_object;
	ghost_object = new btPairCachingGhostObject();
	ghost_object->setWorldTransform(startTransform);
	btConvexShape* capsule = new btCapsuleShape(radius, height);
	ghost_object->setCollisionShape(capsule);
	ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	actor->controller = new btKinematicCharacterController( ghost_object, capsule, height * 0.25f );
	actor->controller->setUseGhostSweepTest(false);
//	actor->controller->setMaxSlope(0);

	DynamicsWorld->addCollisionObject( ghost_object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	DynamicsWorld->addAction( actor->controller );
	Ovgl::Matrix44 camera_matrix;
	camera_matrix = Ovgl::MatrixTranslation( matirx->_41, matirx->_42, matirx->_43 ) * Ovgl::MatrixTranslation( 0.0f, height, 0.0f );
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
	frameInA.setFromOpenGLMatrix((float*)&(bodyMatB * Ovgl::MatrixInverse(&Ovgl::Vector4Set( 0.0f, 0.0f, 0.0f, 0.0f), &bodyMatA)));
	frameInB.setIdentity();
	joint->joint = new btGeneric6DofConstraint( *obj1->actor, *obj2->actor, frameInA, frameInB, true );
	this->DynamicsWorld->addConstraint(joint->joint, true);
	joints.push_back(joint);
	return joint;
}

void Ovgl::Actor::Jump( float force )
{
	if( grounded )
	{
		grounded = false;
		velocity.y = force;
	}
}

void Ovgl::Prop::setPose( Ovgl::Matrix44* matrix )
{
	bones[mesh->root_bone]->setPose(matrix);
}

void Ovgl::Object::setPose( Ovgl::Matrix44* matrix )
{
	btTransform Transform;
	Transform.setFromOpenGLMatrix( (float*)matrix );
	cmesh->actor->setWorldTransform(Transform);
}

void Ovgl::Camera::setPose( Ovgl::Matrix44* matrix )
{
	btTransform Transform;
	Transform.setFromOpenGLMatrix( (float*)matrix );
	cmesh->actor->setWorldTransform(Transform);
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
	cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Emitter::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Camera::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

Ovgl::Matrix44 Ovgl::Light::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
	return matrix;
}

void Ovgl::Prop::Update( int bone, Ovgl::Matrix44* matrix )
{
	Ovgl::Matrix44 inv_matrix, inv_mesh_bone, tmatrix;
	matrices[bone] = bones[bone]->getPose();
	if( this->mesh->bones.size() > 0 )
	{
		inv_mesh_bone = Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &this->mesh->bones[bone]->matrix );
		matrices[bone] = ( inv_mesh_bone * matrices[bone]  );
		inv_matrix = Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), matrix );
		tmatrix = matrices[bone] * inv_matrix;
		for (UINT i = 0; i < this->mesh->bones[bone]->childen.size(); i++)
		{
			Ovgl::Matrix44 child;
			child = Ovgl::MatrixTranslation( this->mesh->bones[this->mesh->bones[bone]->childen[i]]->matrix._41, this->mesh->bones[this->mesh->bones[bone]->childen[i]]->matrix._42, mesh->bones[mesh->bones[bone]->childen[i]]->matrix._43 );
			child = child * tmatrix * (*matrix);
			Ovgl::Prop::Update( mesh->bones[bone]->childen[i], &child );	
		}
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
			frameInA.setFromOpenGLMatrix((float*)&(bodyMatB * Ovgl::MatrixInverse(&Ovgl::Vector4Set( 0.0f, 0.0f, 0.0f, 0.0f), &bodyMatA)));
			frameInB.setIdentity();
			joint->joint = new btGeneric6DofConstraint( *joint->obj[0]->actor, *joint->obj[1]->actor, frameInA, frameInB, true );
			joint->joint->setAngularLowerLimit( btVector3(-childBone->max.x * (float)(OvglPi / 180.0), childBone->min.y * (float)(OvglPi / 180.0), -childBone->max.z * (float)(OvglPi / 180.0)) );
			joint->joint->setAngularUpperLimit( btVector3(-childBone->min.x * (float)(OvglPi / 180.0), childBone->max.y * (float)(OvglPi / 180.0), -childBone->min.z * (float)(OvglPi / 180.0)) );
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
		corrected_trajectory = Ovgl::Vector3Transform( &actors[a]->trajectory, &Ovgl::MatrixRotationY( -actors[a]->direction.z) );
		actors[a]->controller->setWalkDirection(  btVector3(corrected_trajectory.x, corrected_trajectory.y, corrected_trajectory.z ));
		Ovgl::Matrix44 matrix;
		btPairCachingGhostObject* ghost;
		ghost = actors[a]->controller->getGhostObject();
		ghost->getWorldTransform().getOpenGLMatrix( (float*)&matrix );
		btVector3 actor_scale = ghost->getCollisionShape()->getLocalScaling();
		Ovgl::Matrix44 cam_mat;
		cam_mat = Ovgl::MatrixTranslation( 0.0f, 0.0f, actors[a]->radius / 2 ) * Ovgl::MatrixRotationEuler( actors[a]->direction.x, actors[a]->direction.y, actors[a]->direction.z) * Ovgl::MatrixTranslation(matrix._41, matrix._42, matrix._43) * Ovgl::MatrixTranslation( 0.0f, actors[a]->height / 2, 0.0f );
		actors[a]->camera->setPose(&cam_mat);

		if( (actors[a]->crouch) && (actor_scale.getY() > 0.5f ))
		{
			ghost->getCollisionShape()->setLocalScaling( btVector3(1, actor_scale.getY() - ((float)UpdateTime * 0.01f), 1 ) );

		}
		else if( (!actors[a]->crouch) & (actor_scale.getY() < 1.0f ) )
		{
			ghost->getCollisionShape()->setLocalScaling( btVector3(1, actor_scale.getY() + ((float)UpdateTime * 0.01f), 1 ) );
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
				ALfloat ListenerOri[] = { cmatrix->_21, cmatrix->_22, cmatrix->_23, cmatrix->_31, cmatrix->_32, cmatrix->_33 };
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
	DynamicsWorld->stepSimulation(((float)(UpdateTime) * 0.001f));
};



void Ovgl::Light::Release()
{
	for( DWORD l = 0; l < scene->lights.size(); l++)
	{
		if( scene->lights[l] == this)
		{
			scene->lights.erase( scene->lights.begin() + l );
		}
	}
	delete this;
}

void Ovgl::Camera::Release()
{
	for( DWORD c = 0; c < scene->cameras.size(); c++)
	{
		if( scene->cameras[c] == this)
		{
			scene->cameras.erase( scene->cameras.begin() + c );
		}
	}
	delete this;
}

void Ovgl::Prop::Release()
{
	for( DWORD p = 0; p < scene->props.size(); p++)
	{
		if( scene->props[p] == this)
		{
			scene->props.erase( scene->props.begin() + p );
		}
	}
	delete this;
}

void Ovgl::Scene::Release()
{
	for( DWORD p = 0; p < props.size(); p++ )
	{
		props[p]->Release();
	}

	for( DWORD c = 0; c < cameras.size(); c++ )
	{
		cameras[c]->Release();
	}

	for( DWORD l = 0; l < lights.size(); l++ )
	{
		lights[l]->Release();
	}
	delete this;
}

void Ovgl::Joint::Release()
{
	delete joint;
	delete this;
}