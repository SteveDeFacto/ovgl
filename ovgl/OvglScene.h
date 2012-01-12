/**
* @file OvglScene.h
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
* @brief This Header defines each class that makes up the OvglScene class.
*/

// Forward declare external classes
class btGeneric6DofConstraint;
class btKinematicCharacterController;
class btDiscreteDynamicsWorld;

namespace Ovgl
{
	enum ObjectFlags
	{
		OBJECT_DISABLE_COLLISION = 1,
		OBJECT_DISABLE_GRAVITY = 2,
		OBJECT_KINEMATIC = 4,
	};

	extern "C"
	{
		class Actor;
		class Camera;
		class Light;
		class Prop;
		class Scene;
		class Mesh;
		class CMesh;
		class AudioEmitter;
		class AudioVoice;
		class Instance;
		class Shader;
		class Joint;
		class Vector3;

		/**
		* This class can be used to get the object that a ray is cast onto, the world space point of collision, and the local space point of collision. 
		* @brief Used to return ray collision information.
		*/
		class __declspec(dllexport) RayHit
		{
		public:
			DWORD type;
			Ovgl::CMesh* CollisionMesh;
			Ovgl::Prop* prop;
			Ovgl::Actor* actor;
			Ovgl::Vector3 loc_point;
			Ovgl::Vector3 ws_point;
		};

		/**
		* Joints form bounds between objects in the physics scene.
		* @brief Scene joint class.
		*/
		class __declspec(dllexport) Joint
		{
		public:
			Scene* scene;
			CMesh* obj[1];
			btGeneric6DofConstraint* joint;
			void Release();
		};

		/**
		* By setting it as the viewport of a Ovgl::RenderTarget you can render what this camera sees to the render target.
		* @brief This class represents a camera within a Ovgl::Scene.
		*/
		class __declspec(dllexport) Camera
		{
		public:
			/**
			* This is a pointer to the scene that this camera was created by and resides in.
			*/
			Scene* scene;
			/**
			* This is a pointer to the physics scene object that represents the camera within the scene.
			*/
			CMesh* CollisionMesh;
			/**
			* This matrix defines the perspective of the camera.
			*/
			Matrix44 projMat;
			/**
			* This is an array of all audio voices that the camera can hear.
			*/
			std::vector< AudioVoice* > voices;
			/**
			* Sets the pose of this camera.
			* @param matrix The matrix which defines the new pose for this camera.
			*/
			void setPose( Matrix44* matrix );
			/**
			* Returns the current pose of the camera.
			*/
			Matrix44 getPose();
			/**
			* This function will release control of all memory associated with the camera and it will also remove any reference to it from the scene.
			*/
			void Release();
		};
		
		/**
		* @brief This class represents a light within a Ovgl::Scene.
		*/
		class __declspec(dllexport) Light
		{
		public:
			/**
			* This is a pointer to the scene that this light was created by and resides in.
			*/
			Scene* scene;
			/**
			* This is a pointer to the physics scene object that represents the light with in the scene.
			*/
			CMesh* CollisionMesh;
			/**
			* This is the color of the light that will be cast on objects.
			*/
			Vector3 color;
			/**
			* Specifies what type of light is emitted.
			*/
			DWORD type;
			/**
			* Sets the pose of this light.
			* @param matrix The matrix which defines the new pose for this light.
			*/
			void setPose( Matrix44* matrix );
			/**
			* Returns the current pose of this light.
			*/
			Matrix44 getPose();
			/**
			* This function will release control of all memory associated with the light and it will also remove any reference to it from the scene.
			*/
			void Release();
		};
		
		/**
		* Props are dynamic objects such as vehicles, rag dolls, and any other objects that react to kinetic forces within the scene.
		* @brief This class represents a prop within a Ovgl::Scene. 
		*/
		class __declspec(dllexport) Prop
		{
		public:
			/**
			* This is a pointer to the scene that this prop was created by and resides in.
			*/
			Scene*									scene;
			/**
			* The mesh that is displayed for this prop.
			*/
			Mesh*									mesh;
			/**
			* List of bones that are used to distort the mesh that is displayed for the prop.
			*/
			std::vector< CMesh* >					bones;
			/**
			* List of materials that are used for each subset of the mesh.
			*/
			std::vector< Material* >				materials;
			/**
			* List of matrices that are taken from the bones and can be passed directly to the shaders to be rendered.
			*/
			std::vector< Matrix44 >					matrices;
			/**
			* List of joints that are holding together the bones of this prop.
			*/
			std::vector< Joint* >					joints;
			/**
			* List of animations affecting this prop.
			*/
			std::vector< Animation* >				animations;
			/**
			* Used by Ovgl::Scene to update the bone orientations that are displayed to the Ovgl::RenderTarget. This function will recursivly apply the offset of a parent bone to all child bones and their children until it has gone through the entire tree.
			* @param bone Index of the first bone to update. Generally the meshes root_bone.
			* @param matrix This matrix is the amount of offset that is to be applied to the bone.
			*/
			void Update( int bone, Matrix44* matrix );
			/**
			* This function creates the joints that fuse together bones within the prop. This function will recursivly joints together a parent bone to all child bones and their children until it has gone through the entire tree.
			* @param bone Index of first bone to joint to it's children. This should generally be the meshes root_bone.
			*/
			void CreateJoints( DWORD bone );
			/**
			* Creates an animation clip.
			* @param current The current time.
			* @param start The time in which the clip begins.
			* @param start The time in which the clip ends.
			*/
			Animation* CreateAnimation( DWORD current, DWORD start, DWORD end );
			/**
			* Sets the pose of this prop.
			* @param matrix The matrix which defines the new pose for this prop.
			*/
			void setPose( Matrix44* matrix );
			/**
			* Returns the current pose of this prop.
			*/
			Matrix44 getPose();
			/**
			* This function will release control of all memory associated with the prop and it will also remove any reference to it from the scene.
			*/
			void Release();
		};
	
		/**
		* Objects are static and cannot react to kinetic forces within the scene.
		* @brief This class represents a object within a Ovgl::Scene. 
		*/
		class __declspec(dllexport) Object
		{
		public:
			/**
			* This is a pointer to the scene that this object resides in.
			*/
			Scene* Scene;
			/**
			* This is a pointer to the physics object that represents this object within the scene.
			*/
			CMesh* CollisionMesh;
			/**
			* The mesh that is displayed for this object.
			*/
			Mesh* mesh;
			/**
			* List of materials that are used for each subset of the mesh.
			*/
			std::vector< Material* > materials;
			/**
			* Sets the pose of this object.
			* @param matrix The matrix which defines the new pose for this object.
			*/
			void setPose( Matrix44* matrix );
			/**
			* Returns the current pose of this object.
			*/
			Matrix44 getPose();
			/**
			* This function will release control of all memory associated with the object and it will also remove any reference to it from the scene.
			*/
			void Release();
		};
	
		/**
		* Emitters act as a reference points that provides the location and oriantation of the source of an emission for other classes such as Ovgl::AudioInstance and Ovgl::ParticleInstance.
		* @brief This class represents a emitter within a Ovgl::Scene. 
		*/
		class __declspec(dllexport) Emitter
		{
		public:
			/**
			* This is a pointer to the scene that this emitter was created by and resides in.
			*/
			Scene* scene;
			/**
			* This is a pointer to the physics scene object that represents the emitter with in the scene.
			*/
			CMesh* CollisionMesh;
			/**
			* Sets the pose of this emitter.
			* @param matrix The matrix which defines the new pose for this emitter.
			*/
			void setPose( Matrix44* matrix );
			/**
			* Returns the current post of this emitter.
			*/
			Matrix44 getPose();
			/**
			* This function will release control of all memory associated with the emitter and it will also remove any reference to it from the scene.
			*/
			void Release();
		};

		class __declspec(dllexport) Actor
		{
		public:
			std::vector< Ovgl::Animation* > animations;
			/**
			* This is a pointer to the scene that this actor was created by and resides in.
			*/
			Scene* scene;
			/**
			* Bullet character controller.
			*/
			btKinematicCharacterController* controller;
			/**
			* Bullet ghost object.
			*/
			btPairCachingGhostObject* ghostObject;
			/**
			* This is the view of the actor. You can set it as the view to a render target to see out of the actor's eyes.
			*/
			Camera* camera;
			/**
			* This offsets the actor's view and can be used for third person view.
			*/
			Matrix44 CameraOffset;
			/**
			* Mesh offset
			*/		
			Matrix44 offset;
			/**
			* This is what defines the appearance of the actor.
			*/			
			Mesh* mesh;
			/**
			* List of materials that are used for each subset of the character's mesh.
			*/
			std::vector< Material* > materials;
			/**
			* List of matrices that are taken from active animations.
			*/
			std::vector< Matrix44 > matrices;
			/**
			* Which direction the actor is currently moving.
			*/
			Vector3 walkDirection;
			/**
			* Which direction the actor is currently looking.
			*/
			Vector3 lookDirection;
			/**
			* When the actor is not on the gound we need to know what his velocity is in the air.
			*/
			Vector3 velocity;
			/**
			*
			*/
			Vector3 position;
			/**
			* The maximum slope the actor can climb
			*/
			float maxSlope;
			/**
			* The height of the character's collision shape
			*/
			float height;
			/**
			* The radius of the character's collision shape
			*/
			float radius;
			/**
			* The actor's current crouch state
			*/
			bool crouch;
			/**
			* This will be true if the actor is standing on the ground.
			*/
			bool onGround;
			/**
			* Returns the current post of this actor.
			*/
			Matrix44 getPose();

			void UpdateAnimation( int bone, Ovgl::Matrix44* matrix, DWORD time );

			Ovgl::Animation* CreateAnimation( DWORD start, DWORD end, bool repeat );
			/**
			* Tells the actor where to look.
			* @param vec The direction to look.
			*/
			void SetDirection( Vector3* vec );
			/**
			* Makes the actor jump.
			* @param force This tells the actor how high to jump.
			*/
			void Jump( float force );
			/**
			* Releases the actor from memory and creates a ragdoll in it's place.
			*/
			Prop* Kill();
			/**
			* This function will release control of all memory associated with the actor and it will also remove any reference to it from the scene.
			*/
			void Release();
		};

		/**
		* Scenes contain all the 3D objects that you will see on the screen such as lights, cameras, props, and actors. They also maintain the physics scene and objects.
		* @brief This class contains a set of objects that make up a 3D scene.
		*/
		class __declspec(dllexport) Scene
		{
		public:
			/**
			* This is a pointer to the Ovgl instance that created the scene.
			*/
			Instance*								Inst;
			/**
			* This is a pointer to the bullet scene.
			*/
			btDiscreteDynamicsWorld*				DynamicsWorld;
			/**
			* This texture is the cubemap for the skybox.
			*/
			Texture*								skybox;
			/**
			* This array contains all static objects within the scene.
			*/
			std::vector< Object* >					objects;
			/**
			* This array contains all lights within the scene.
			*/
			std::vector< Light* >					lights;
			/**
			* This array contains all cameras within the scene.
			*/
			std::vector< Camera* >					cameras;
			/**
			* This array contains all props within the scene.
			*/
			std::vector< Prop* >					props;
			/**
			* This array contains all actors within the scene.
			*/
			std::vector< Actor* >					actors;
			/**
			* This array contains all emitters within the scene.
			*/
			std::vector< Emitter* >					emitters;
			/**
			* This array contains all joints within the scene.
			*/
			std::vector< Joint* >					joints;
			/**
			* This array contains all markers within the scene.
			*/
			std::vector< Matrix44* >				markers;
			/**
			* This function adds a Ovgl::Light to the scene.
			* @param matrix The matrix which defines the the starting pose of the light.
			* @param color The color of the light.
			* @param type This option can be used to specify what type of light to emit. There are three types Point Light, Spot Light, and a Directional Light.
			*/
			Light* CreateLight( Matrix44* matirx, Vector4* color );
			/**
			* This function adds a Ovgl::Camera to the scene.
			* @param matrix The matrix which defines the the starting pose of the camera.
			*/
			Camera* CreateCamera( Matrix44* view_matrix );
			/**
			* This function adds a Ovgl::Actor to the scene.
			* @param mesh The mesh object that will be displayed for the actor.
			* @param radius The radius of the actor's capsule within the physics scene which is the distance at which a character comes in contact with objects in the character's vertical plane.
			* @param height The hieght of the actor's capsule within the physics scene which is the distance at which a character comes in contact with objects in the character's horizontal plane.
			* @param matrix The matrix which defines the the starting pose of the actor.
			*/
			Actor* CreateActor( Mesh* mesh, float radius, float height, Matrix44* matirx, Matrix44* offset );
			/**
			* This function adds a Ovgl::Prop to the scene.
			* @param mesh The mesh that will be displayed for the prop.
			* @param matrix The matrix which defines the the starting pose of the prop.
			*/
			Prop* CreateProp( Mesh* mesh, Matrix44* matrix);
			/**
			* This function adds a Ovgl::Prop to the scene.
			* @param mesh The mesh that will be displayed for the object.
			* @param matrix The matrix which defines the pose of the object.
			*/
			Object* CreateObject( Mesh* mesh, Matrix44* matrix);
			/**
			* This function adds a Ovgl::Emitter to the scene.
			* @param matrix The matrix which defines the starting pose of the emitter.
			*/
			Emitter* CreateEmitter( Matrix44* matrix );
			/**
			* This function creates a joint which binds two Ovgl::CMesh together. 
			* @param obj1 First Ovgl::CMesh.
			* @param obj2 Second Ovgl::CMesh.
			* @param anchor This is the point in which the two Ovgl::CMesh are anchored. If this is set to NULL the anchor will be placed at the position of the first Ovgl::CMesh;
			*/
			Joint* CreateJoint( CMesh* obj1, CMesh* obj2);
			/**
			* This function updates the animations, audio emition points, and the physics objects of the scene.
			* @param update_time The amount of time that has passed since the last scene update.
			*/
			void Update( DWORD update_time );
			/**
			* This function will release control of all memory associated with the scene and any objects within it. It will also remove any reference to it from the Ovgl::Instance.
			*/
			void Release();
		};
	}
}