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
#include "OvglAudio.h"
#include "OvglScene.h"
#include "OvglMesh.h"

class ControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		NxActor& shape = hit.shape->getActor();
		NxF32 coeff = shape.getMass() * hit.length * 30.0f;
		shape.addForceAtLocalPos(hit.dir*coeff, NxVec3(0,0,0), NX_IMPULSE);
		Ovgl::Actor* actor = (Ovgl::Actor*)hit.controller->getUserData();
		if( acos(hit.worldNormal.y) > Ovgl::DegToRad(45.0f) )
		{
			Ovgl::Vector3 normal = Ovgl::Vector3Set(hit.worldNormal.x, 0.0f, hit.worldNormal.z);
			actor->velocity = actor->velocity + (normal / 100);
			actor->grounded = false;
		}
		else
		{
			actor->grounded = true;
		}
		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

} g_ControllerHitReport;

class g_BoneContactReport : public NxUserContactReport
{
	void onContactNotify(NxContactPair& pair, NxU32 events)
	{
		pair.actors[0]->getScene().setActorPairFlags( *pair.actors[0], *pair.actors[1], NX_IGNORE_PAIR );
	}

} g_BoneContactReport;

Ovgl::Camera* Ovgl::Scene::CreateCamera( Ovgl::Matrix44* matrix )
{
	Ovgl::Camera* camera = new Ovgl::Camera;
	camera->scene = this;
	NxActorDesc actorDesc;
	NxConvexShapeDesc ConvexShapeDesc;
	ConvexShapeDesc.meshData = this->Inst->Shapes[0];
	actorDesc.shapes.pushBack(&ConvexShapeDesc);
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 8;
	bodyDesc.flags = NX_BF_DISABLE_GRAVITY;
	bodyDesc.mass = 0;
	actorDesc.body = &bodyDesc;
	actorDesc.globalPose.setColumnMajor44( (float*)matrix );
	actorDesc.density = 0.00001f;
	camera->projMat = Ovgl::MatrixPerspectiveLH( (((float)OvglPi) / 2.0f), (640.0f / 480.0f) , 0.01f, 1000.0f );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = physics_scene->createActor(actorDesc);
	camera->cmesh = cmesh;
	camera->cmesh->actor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
	this->cameras.push_back( camera );
	return camera;
};

Ovgl::Light* Ovgl::Scene::CreateLight( Ovgl::Matrix44* matrix, Ovgl::Vector4* color )
{
	Ovgl::Light* light = new Ovgl::Light;
	light->scene = this;
	NxActorDesc actorDesc;
	NxConvexShapeDesc ConvexShapeDesc;
	ConvexShapeDesc.meshData = this->Inst->Shapes[0];
	actorDesc.shapes.pushBack(&ConvexShapeDesc);
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 8;
	bodyDesc.flags = NX_BF_DISABLE_GRAVITY;
	bodyDesc.mass = 0;
	actorDesc.body = &bodyDesc;
	actorDesc.globalPose.setColumnMajor44((float*)matrix);
	actorDesc.density = 0.00001f;
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = physics_scene->createActor(actorDesc);
	light->cmesh = cmesh;
	light->cmesh->actor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
	light->color.x = color->x;
	light->color.y = color->y;
	light->color.z = color->z;
	this->lights.push_back( light );
	return light;
};

Ovgl::Prop* Ovgl::Scene::CreateProp( Ovgl::Mesh* mesh, Ovgl::Matrix44* matrix )
{
	Ovgl::Prop* prop = new Ovgl::Prop;
	prop->scene = this;
	prop->mesh = mesh;
	prop->subsets.resize(mesh->subset_count);
	for( DWORD s = 0; s < prop->subsets.size(); s++)
	{
		prop->subsets[s] = Inst->DefaultEffect;
	}
	for( DWORD i = 0; i < mesh->bones.size(); i++ )
	{
		NxActorDesc actorDesc;
		NxConvexShapeDesc ConvexShapeDesc;
		ConvexShapeDesc.meshData = mesh->bones[i]->convex;
//		ConvexShapeDesc.skinWidth = 0.0f;
		actorDesc.shapes.pushBack( &ConvexShapeDesc );
		NxBodyDesc bodyDesc;
		bodyDesc.solverIterationCount = 8;
		bodyDesc.contactReportThreshold = 10.0f;
		actorDesc.body = &bodyDesc;
		Ovgl::Matrix44 tmatrix;
		tmatrix = mesh->bones[i]->matrix * (*matrix);
		actorDesc.globalPose.setColumnMajor44( (float*)&tmatrix );
		actorDesc.density = 12;
		Ovgl::CMesh* cmesh = new Ovgl::CMesh;
		cmesh->scene = this;
		cmesh->actor = physics_scene->createActor( actorDesc );
		cmesh->actor->getShapes()[0]->setGroup(1);
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
				physics_scene->setActorPairFlags( *prop->bones[np]->actor, *prop->bones[nq]->actor, NX_IGNORE_PAIR);
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
		object->subsets[s] = Inst->DefaultEffect;
	}
	NxActorDesc actorDesc;
	NxTriangleMeshShapeDesc TriangleMeshShapeDesc;
	NxTriangleMeshDesc TriangleMeshDesc;    
	TriangleMeshDesc.numVertices = mesh->vertices.size();
	TriangleMeshDesc.numTriangles = mesh->faces.size();
	TriangleMeshDesc.pointStrideBytes = sizeof(Ovgl::Vertex);
	TriangleMeshDesc.triangleStrideBytes = sizeof(Ovgl::Face);
	TriangleMeshDesc.points = &mesh->vertices[0];
	TriangleMeshDesc.triangles = &mesh->faces[0];
	TriangleMeshDesc.flags = 0;
	NxStreamDefault buf;
	Inst->Cooking->NxCookTriangleMesh( TriangleMeshDesc, buf );
	NxTriangleMesh* TriangleMesh;
	TriangleMesh = Inst->PhysX->createTriangleMesh( buf );
	TriangleMeshShapeDesc.meshData = TriangleMesh;
	actorDesc.shapes.pushBack( &TriangleMeshShapeDesc );
	actorDesc.globalPose.setColumnMajor44( (float*)matrix );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->scene = this;
	cmesh->actor = physics_scene->createActor( actorDesc );
	cmesh->actor->getShapes()[0]->setGroup(1);
	object->cmesh = cmesh;
	this->objects.push_back( object );
	return object;
};

Ovgl::Emitter* Ovgl::Scene::CreateEmitter( Ovgl::Matrix44* matrix )
{
	Ovgl::Emitter* emitter = new Ovgl::Emitter;
	emitter->scene = this;
	NxActorDesc actorDesc;
	NxConvexShapeDesc ConvexShapeDesc;
	ConvexShapeDesc.meshData = this->Inst->Shapes[0];
	actorDesc.shapes.pushBack(&ConvexShapeDesc);
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 8;
	bodyDesc.flags = NX_BF_DISABLE_GRAVITY;
	bodyDesc.mass = 0;
	actorDesc.body = &bodyDesc;
	actorDesc.globalPose.setColumnMajor44((float*)matrix);
	actorDesc.density = 0.00001f;
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = physics_scene->createActor(actorDesc);
	emitter->cmesh = cmesh;
	emitter->cmesh->actor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
	this->emitters.push_back( emitter );
	return emitter;
};

Ovgl::Actor* Ovgl::Scene::CreateActor( Ovgl::Mesh* mesh, float radius, float height, Ovgl::Matrix44* matirx )
{
	Ovgl::Actor* actor = new Ovgl::Actor;
	actor->desc = new NxCapsuleControllerDesc;
	actor->desc->position = NxExtendedVec3( matirx->_41, matirx->_42, matirx->_43 );
	actor->desc->radius = radius;
	actor->desc->height = height;
	actor->desc->upDirection = NX_Y;
	actor->desc->slopeLimit = cosf(Ovgl::DegToRad(45.0f));
	actor->desc->skinWidth = height / 50.0f;
	actor->desc->stepOffset = height / 4.0f;
	actor->desc->callback = &g_ControllerHitReport;
	actor->desc->climbingMode = CLIMB_CONSTRAINED;
	actor->desc->userData = actor;
	actor->controller = (NxCapsuleController*)Inst->Manager->createController( physics_scene, *actor->desc );
	actor->crouch = false;
	actor->grounded = false;
	actor->direction = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->velocity = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->trajectory = Ovgl::Vector3Set( 0.0f, 0.0f, 0.0f );
	actor->scene = this;
	Ovgl::Matrix44 matrix;
	matrix = Ovgl::MatrixTranslation( (float)actor->desc->position.x, (float)actor->desc->position.y, (float)actor->desc->position.z );
	Ovgl::CMesh* cmesh = new Ovgl::CMesh;
	cmesh->actor = actor->controller->getActor();
	actor->cmesh = cmesh;
	Ovgl::Matrix44 camera_offset;
	camera_offset = Ovgl::MatrixTranslation( 0.0f, (actor->desc->height / 10), (actor->desc->radius / 5) );
	Ovgl::Matrix44 camera_matrix;
	camera_matrix = Ovgl::MatrixTranslation( (float)actor->desc->position.x, (float)(actor->desc->position.y + (actor->desc->height / 2.2)), (float)actor->desc->position.z );
	camera_matrix = camera_offset * camera_matrix;
	actor->camera = CreateCamera( &camera_matrix );
	actor->camera->cmesh->actor->raiseBodyFlag( NX_BF_KINEMATIC );
	actor->hit.cmesh = NULL;
	actor->hit.actor = NULL;
	actor->hit.prop = NULL;
	actors.push_back(actor);
	return actor;
};

Ovgl::Joint* Ovgl::Scene::CreateJoint( Ovgl::CMesh* obj1, Ovgl::CMesh* obj2, Ovgl::Vector3* anchor )
{
	Ovgl::Joint* joint = new Ovgl::Joint;
	joint->scene = this;
	joint->obj[0] = obj1;
	joint->obj[1] = obj2;
	NxD6JointDesc d6Desc;
	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.actor[0] = obj1->actor;
	d6Desc.actor[1] = obj2->actor;
	if(anchor)
	{
		d6Desc.setGlobalAnchor( NxVec3(anchor->x, anchor->y, anchor->z) );
	}
	else
	{
		d6Desc.setGlobalAnchor( obj1->actor->getGlobalPosition() );
	}
	joint->joint = (NxD6Joint*)physics_scene->createJoint( d6Desc )->is(NX_JOINT_D6);
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

Ovgl::Matrix44 Ovgl::Prop::getPose()
{
	Ovgl::Matrix44 matrix;
	bones[mesh->root_bone]->actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Object::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
	return matrix;
};

void Ovgl::Object::setPose( Ovgl::Matrix44* matrix )
{
	NxMat34 mat;
	mat.setColumnMajor44( (float*)matrix );
	cmesh->actor->setGlobalPose( mat );
}

Ovgl::Matrix44 Ovgl::Emitter::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
	return matrix;
};

Ovgl::Matrix44 Ovgl::Camera::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
	return matrix;
}

void Ovgl::Camera::setPose( Ovgl::Matrix44* matrix )
{
	NxMat34 mat;
	mat.setColumnMajor44( (float*)matrix );
	cmesh->actor->setGlobalPose( mat );
}

Ovgl::Matrix44 Ovgl::Light::getPose()
{
	Ovgl::Matrix44 matrix;
	cmesh->actor->getGlobalPose().getColumnMajor44( (float*)&matrix );
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
		Ovgl::Matrix44 tempPose, inv_parentPose, parentPose, bonePose, localPose, offset;
		NxD6JointDesc d6Desc;
		d6Desc.actor[0] = bones[bone]->actor;
		parentPose = bones[bone]->getPose();
		for(UINT i = 0; i < mesh->bones[bone]->childen.size(); i++)
		{
			d6Desc.actor[1] = bones[mesh->bones[bone]->childen[i]]->actor;
			bonePose = bones[mesh->bones[bone]->childen[i]]->getPose();
			Ovgl::Bone* childBone = this->mesh->bones[mesh->bones[bone]->childen[i]];
			float Pitch = (childBone->min.x + ((childBone->max.x - childBone->min.x) / 2.0f)) * 3.14158f / 180.0f;
			float Roll = (childBone->min.z + ((childBone->max.z - childBone->min.z) / 2.0f)) * 3.14158f / 180.0f;
			offset = Ovgl::MatrixRotationEuler( 0, Pitch, Roll );
			tempPose =  offset * bonePose;
			bones[mesh->bones[bone]->childen[i]]->setPose( &tempPose );
			inv_parentPose = Ovgl::MatrixInverse( &Ovgl::Vector4Set(0,0,0,0), &parentPose );
			localPose = tempPose * inv_parentPose;
			NxMat34 mat;
			mat.setColumnMajor44( (float*)&localPose );
			d6Desc.localAxis[0]= mat.M * NxVec3(0.0f,1.0f,0.0f);
			d6Desc.localAxis[1]= NxVec3(0.0f,1.0f,0.0f);
			d6Desc.localNormal[0]= mat.M * NxVec3(1.0f,0.0f,0.0f);
			d6Desc.localNormal[1]= NxVec3(1.0f,0.0f,0.0f);
			d6Desc.setGlobalAnchor( NxVec3( bonePose._41, bonePose._42, bonePose._43 ) );
			d6Desc.twistLimit.low.value = (NxReal)(mesh->bones[mesh->bones[bone]->childen[i]]->min.y * 3.14158f / 180.0f);
			d6Desc.twistLimit.high.value = (NxReal)(mesh->bones[mesh->bones[bone]->childen[i]]->max.y * 3.14158f / 180.0f);
			d6Desc.swing1Limit.value = (NxReal)(((mesh->bones[mesh->bones[bone]->childen[i]]->max.x - mesh->bones[mesh->bones[bone]->childen[i]]->min.x) / 2.0f) * 3.14158f / 180.0f);
			d6Desc.swing2Limit.value = (NxReal)(((mesh->bones[mesh->bones[bone]->childen[i]]->max.z - mesh->bones[mesh->bones[bone]->childen[i]]->min.z) / 2.0f) * 3.14158f / 180.0f);
			d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.twistMotion = NX_D6JOINT_MOTION_LIMITED;
			d6Desc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;
			d6Desc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;
			joints[mesh->bones[bone]->childen[i]] = new Ovgl::Joint;
			joints[mesh->bones[bone]->childen[i]]->joint = (NxD6Joint*)bones[bone]->actor->getScene().createJoint( d6Desc )->is(NX_JOINT_D6);
			bones[mesh->bones[bone]->childen[i]]->setPose( &bonePose );
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
	for( UINT j = 0; j < joints.size(); j++ )
	{
		if(j != mesh->root_bone)
		{
			NxD6JointDesc d6Desc;
			joints[j]->joint->saveToDesc( d6Desc );
			d6Desc.twistLimit.low.value = 0;
			d6Desc.twistLimit.high.value = 0;
			d6Desc.swing1Limit.value = 0;
			d6Desc.swing2Limit.value = 0;
			d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
			d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
			d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
			d6Desc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
			d6Desc.slerpDrive.forceLimit = FLT_MAX;
			d6Desc.slerpDrive.spring = 0.0f;
			d6Desc.slerpDrive.damping = 0.0f;
			d6Desc.flags |= NX_D6JOINT_SLERP_DRIVE;
			Joint* joint = new Joint;
			joint->scene = this->scene;
			joint->joint = (NxD6Joint*)bones[0]->actor->getScene().createJoint( d6Desc )->is(NX_JOINT_D6);
			animation->joints.push_back( joint );
		}
		else
		{
			animation->joints.push_back( NULL );
		}
	}
	animations.push_back( animation );
	return animation;
}

void Ovgl::Scene::Update( DWORD UpdateTime )
{
	//for( UINT p = 0; p < props.size(); p++ )
	//{
	//	for( UINT a = 0; a < props[p]->animations.size(); a++ )
	//	{
	//		if( (props[p]->animations[a]->animationState == 1) | (props[p]->animations[a]->animationState == 2) ) props[p]->animations[a]->currentTime = props[p]->animations[a]->currentTime + ((((float)(UpdateTime) * 0.001f) * 15) * props[p]->animations[a]->stepTime);
	//		if((props[p]->animations[a]->currentTime > props[p]->animations[a]->endTime) & (props[p]->animations[a]->animationState == 2) )
	//		{
	//			props[p]->animations[a]->currentTime = props[p]->animations[a]->startTime;
	//		}
	//		else if( (props[p]->animations[a]->currentTime > props[p]->animations[a]->endTime) & (props[p]->animations[a]->animationState == 1) )
	//		{
	//			props[p]->animations[a]->currentTime = props[p]->animations[a]->startTime;
	//			props[p]->animations[a]->animationState = 0;
	//			for( UINT j = 0; j < props[p]->animations[a]->joints.size(); j++ )
	//			{
	//				if(j != props[p]->mesh->root_bone)
	//				{
	//					NxD6JointDesc d6Desc;
	//					props[p]->animations[a]->joints[j]->joint->saveToDesc( d6Desc );
	//					d6Desc.twistLimit.low.value = 0;
	//					d6Desc.twistLimit.high.value = 0;
	//					d6Desc.swing1Limit.value = 0;
	//					d6Desc.swing2Limit.value = 0;
	//					d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	//					d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	//					d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
	//					d6Desc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	//					d6Desc.slerpDrive.forceLimit = FLT_MAX;
	//					d6Desc.slerpDrive.spring = 0.0f;
	//					d6Desc.slerpDrive.damping = 0.0f;
	//					d6Desc.flags |= NX_D6JOINT_SLERP_DRIVE;
	//					props[p]->animations[a]->joints[j]->joint->loadFromDesc( d6Desc );
	//				}
	//			}
	//		}
	//		if( props[p]->animations[a]->animationState != 0 )
	//		{
	//			for(UINT f = 0; f < props[a]->mesh->keyframes.size() - 1; f++ )
	//			{
	//				DWORD t1 = 0;
	//				DWORD t2 = 0;
	//				Ovgl::Vector4 v1 = Ovgl::Vector4Set(0.0f, 0.0f, 0.0f, 0.0f);
	//				Ovgl::Vector4 v2 = Ovgl::Vector4Set(0.0f, 0.0f, 0.0f, 0.0f);
	//				for(UINT k = 0; k < props[p]->mesh->keyframes[f]->keys.size(); k++ )
	//				{
	//					if((props[p]->mesh->keyframes[f]->time <= props[p]->animations[a]->currentTime) & (props[p]->mesh->keyframes[f + 1]->time >= props[p]->animations[a]->currentTime))
	//					{
	//						t1 = props[p]->mesh->keyframes[f]->time;
	//						t2 = props[p]->mesh->keyframes[f + 1]->time;
	//						v1 = props[p]->mesh->keyframes[f]->keys[k].rotation;
	//						v2 = props[p]->mesh->keyframes[f + 1]->keys[k].rotation;
	//						v1 = Ovgl::Vector4Lerp(v1, v2, (props[p]->animations[a]->currentTime - t1) / ( t2 - t1));
	//						if(props[p]->mesh->keyframes[0]->time > props[p]->animations[a]->currentTime)
	//						{
	//							v1 = props[p]->mesh->keyframes[0]->keys[k].rotation;
	//						}
	//						else if(props[p]->mesh->keyframes[props[p]->mesh->keyframes.size() - 1]->time <= props[p]->animations[a]->currentTime)
	//						{
	//							v1 = props[p]->mesh->keyframes[props[p]->mesh->keyframes.size() - 1]->keys[k].rotation;
	//						}
	//						Ovgl::Matrix44 m;
	//						Ovgl::Vector4 o, q;
	//						Ovgl::Matrix44 offset;
	//						DWORD i = props[p]->mesh->keyframes[f]->keys[k].index;
	//						float Pitch = (props[p]->mesh->bones[i]->min.x + ((props[p]->mesh->bones[i]->max.x - props[p]->mesh->bones[i]->min.x) / 2.0f)) * 3.14158f / 180.0f;
	//						float Roll = (props[p]->mesh->bones[i]->min.z + ((props[p]->mesh->bones[i]->max.z - props[p]->mesh->bones[i]->min.z) / 2.0f)) * 3.14158f / 180.0f;
	//						offset = Ovgl::MatrixRotationEuler( 0, Pitch, Roll );
	//						m = Ovgl::MatrixInverse( &Ovgl::Vector4Set( 0.0f, 0.0f, 0.0f, 0.0f ), &offset );
	//						o = Ovgl::QuaternionRotationMatrix( &m );
	//						q = Ovgl::QuaternionRotationAxis( &Ovgl::Vector3Set(v1[1], v1[2], v1[3]), v1[0]*2);
	//						q = q * o;
	//						NxQuat drive = NxQuat( NxVec3(-q[1], q[0], -q[2]), q[3]);
	//						props[p]->animations[a]->joints[i]->joint->setDriveOrientation( drive );
	//						NxD6JointDesc d6Desc;
	//						props[p]->animations[a]->joints[i]->joint->saveToDesc( d6Desc );
	//						d6Desc.slerpDrive.spring = 1000.0f;
	//						props[p]->animations[a]->joints[i]->joint->loadFromDesc( d6Desc );
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	// Update actor positions.
	for(UINT a = 0; a < actors.size(); a++)
	{
		NxU32 collisionFlags;
		actors[a]->controller->move(NxVec3(actors[a]->velocity.x, actors[a]->velocity.y, actors[a]->velocity.z) * ((float)(UpdateTime) * 0.01f), 1 | 2, 0.0f, collisionFlags, 0.0f);
		NxMat34 cam_pose;
		Ovgl::Matrix44 cam_mat;
		Ovgl::Matrix44 con_pos;
		con_pos = actors[a]->cmesh->getPose();
		cam_mat = Ovgl::MatrixTranslation( 0.0f, 0.0f, actors[a]->desc->radius / 2 ) * Ovgl::MatrixRotationEuler( actors[a]->direction.x, actors[a]->direction.y, actors[a]->direction.z) * Ovgl::MatrixTranslation(con_pos._41, con_pos._42, con_pos._43) * Ovgl::MatrixTranslation( 0.0f, actors[a]->controller->getHeight() / 2, 0.0f );
		cam_pose.setColumnMajor44( (float*)&cam_mat );
		actors[a]->camera->cmesh->actor->setGlobalPose(cam_pose);
		Ovgl::Matrix44 camera_pose = actors[a]->camera->getPose();
		NxRay ray;
		ray.orig = NxVec3( camera_pose._41, camera_pose._42, camera_pose._43 );
		ray.dir = NxVec3( camera_pose._31, camera_pose._32, camera_pose._33 );
		NxRaycastHit hit_data;
		NxShape* hit_shape = physics_scene->raycastClosestShape( ray, NX_ALL_SHAPES, hit_data, 2, 5);
		if( hit_shape )
		{
			NxActor* hit_actor = &hit_shape->getActor();
			
			for( UINT i = 0; i < props.size(); i++ )
			{
				for( UINT b = 0; b < props[i]->bones.size(); b++ )
				{
					if( props[i]->bones[b]->actor == hit_actor )
					{
						actors[a]->hit.actor = NULL;
						actors[a]->hit.cmesh = props[i]->bones[b];
						actors[a]->hit.prop = props[i];
					}
				}
			}
			for( UINT i = 0; i < actors.size(); i++ )
			{
				if( actors[i]->controller->getActor() == hit_actor )
				{
					actors[a]->hit.prop = NULL;
					actors[a]->hit.cmesh = actors[i]->cmesh;
					actors[a]->hit.actor = actors[i];
				}
			}
		}
		else
		{
			actors[a]->hit.actor = NULL;
			actors[a]->hit.cmesh = NULL;
			actors[a]->hit.prop = NULL;
		}
		Ovgl::Vector3 corrected_trajectory;
		corrected_trajectory = Ovgl::Vector3Transform( &actors[a]->trajectory, &Ovgl::MatrixRotationY( -actors[a]->direction.z) );
		if(actors[a]->grounded && (collisionFlags & NXCC_COLLISION_DOWN))
		{
			actors[a]->velocity = corrected_trajectory / 3;
		}
		else
		{
			actors[a]->velocity = (actors[a]->velocity + (corrected_trajectory / 50)) / 1.03f;

		}
		actors[a]->velocity = (actors[a]->velocity + (Ovgl::Vector3Set( 0.0f, -0.4f , 0.0f ) * ((float)(UpdateTime) * 0.01f)));
		float previous_height = actors[a]->controller->getHeight();
		if( (actors[a]->crouch) & (actors[a]->controller->getHeight() > actors[a]->desc->height/2) )
		{
			actors[a]->controller->setHeight(actors[a]->controller->getHeight() - (0.2f* ((float)UpdateTime * 0.01f)));
			if( actors[a]->controller->getHeight() < actors[a]->desc->height / 2 )
			{
				actors[a]->controller->setHeight( actors[a]->desc->height / 2 );
			}
			actors[a]->controller->setPosition( actors[a]->controller->getDebugPosition() + NxExtendedVec3( 0, -((actors[a]->controller->getHeight() - previous_height)/2), 0 ) );
		}
		else if( (!actors[a]->crouch) & (actors[a]->controller->getHeight() < actors[a]->desc->height) )
		{
			
			actors[a]->controller->setHeight(actors[a]->controller->getHeight() + (0.2f* ((float)UpdateTime * 0.01f)));
			if(  actors[a]->controller->getHeight() > actors[a]->desc->height)
			{
				actors[a]->controller->setHeight( actors[a]->desc->height );
			}
			if(actors[a]->grounded || (collisionFlags & (NXCC_COLLISION_DOWN | NXCC_COLLISION_SIDES)))
			{
				actors[a]->controller->setPosition( actors[a]->controller->getDebugPosition() + NxExtendedVec3( 0, -(previous_height - actors[a]->controller->getHeight()), 0 ) );
			}
			else
			{
				actors[a]->controller->setPosition( actors[a]->controller->getDebugPosition() + NxExtendedVec3( 0, ((previous_height - actors[a]->controller->getHeight())/2), 0 ) );
			}
		}
	}

	// Update camera positions.
	for( UINT c = 0; c < cameras.size(); c++ )
	{
		Ovgl::Matrix44* cmatrix = &cameras[c]->getPose();
		X3DAUDIO_LISTENER listener = {0};
		listener.OrientFront = D3DXVECTOR3( cmatrix->_31, cmatrix->_32, cmatrix->_33);
		listener.OrientTop = D3DXVECTOR3( cmatrix->_21, cmatrix->_22, cmatrix->_23);
		listener.Position = D3DXVECTOR3( cmatrix->_41, cmatrix->_42, cmatrix->_43);
		for( UINT i = 0; i < cameras[c]->voices.size(); i++ )
		{
			if(cameras[c]->voices[i] != NULL)
			{
				if(cameras[c]->voices[i]->instance->emitter != NULL)
				{
					XAUDIO2_VOICE_STATE state = {0};
					cameras[c]->voices[i]->voice->GetState(&state);
					if(state.BuffersQueued != 0)
					{
						Ovgl::Matrix44* ematrix = &cameras[c]->voices[i]->instance->emitter->getPose();
						X3DAUDIO_EMITTER emitter = {0};
						emitter.ChannelCount = 1;
						emitter.CurveDistanceScaler = 14.0f;
						emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
						emitter.OrientFront = D3DXVECTOR3( ematrix->_31, ematrix->_32, ematrix->_33);
						emitter.OrientTop = D3DXVECTOR3( ematrix->_21, ematrix->_22, ematrix->_23);
						emitter.Position = D3DXVECTOR3( ematrix->_41, ematrix->_42, ematrix->_43);
						X3DAudioCalculate( this->Inst->X3DAudio, &listener, &emitter, X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_LPF_DIRECT, this->Inst->DSPSettings );
						cameras[c]->voices[i]->voice->SetOutputMatrix( this->Inst->MasteringVoice, 2, this->Inst->DeviceDetails->OutputFormat.Format.nChannels, this->Inst->DSPSettings->pMatrixCoefficients );
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
	physics_scene->simulate((NxReal)((float)(UpdateTime) * 0.001f));
	physics_scene->flushStream();
	physics_scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
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
	scene->physics_scene->releaseJoint(*joint);
	delete this;
}