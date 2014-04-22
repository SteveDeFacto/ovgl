/**
 * @file OvglMedia.h
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
 * @brief This header defines all resource functions and classes used in Ovgl.
 */

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

namespace Ovgl
{
extern "C"
{
	class Mesh;
	class AudioBuffer;
	class Scene;
	class Mesh;
	class Context;
	class Shader;
	class Texture;
	class ResourceManager;

	/**
	 * This class contains pre-rendered text characters.
	 * @brief The style of text to render.
	 */
	class DLLEXPORT Font
	{
		public:
			Font( ResourceManager* resource_manager, const std::string& file, uint32_t size );
			uint32_t                                charSet[256];
			uint32_t                                charOffsets[256];
			uint32_t                                size;
	};

	/**
	 * A texture is a two dimensional image stored in a chuck of memory on the graphics card.
	 * @brief Two dimensional image.
	 */
	class DLLEXPORT Texture
	{
		public:
			ResourceManager*                        mLibrary;
			uint32_t                                image;
			std::string                             file;
			bool                                    hasAlpha;
			void release();
	};

	/**
	 * A shader is a small program used to process geometry and pixel information.
	 * In other words is decides how objects are rendered to the screen.
	 * @brief Small program used for rendering surfaces.
	 */
	class DLLEXPORT Shader
	{
		public:
			ResourceManager*                        mLibrary;
			CGeffect                                effect;
			void release();
	};

	/**
	 * Stores the all the informaton to render a surface such as textures, variables, and shaders.
	 * You can think if it as the material which the surface is made out of.
	 * @brief The material
	 */
	class DLLEXPORT Material
	{
		public:
			ResourceManager*                        mLibrary;
			Shader*                                 shaderProgram;
			bool                                    postRender;
			bool                                    noZBuffer;
			bool                                    noZWrite;
			std::vector< std::pair< CGparameter, std::vector< float > > > variables;
			std::vector< std::pair< CGparameter, Ovgl::Texture* > > textures;
			void setEffectVariable(const std::string& variable, const std::vector< float >& data);
			void setEffectTexture(const std::string& variable, Texture* texture);
			void release();
	};

	class DLLEXPORT ResourceManager
	{
		public:
			ResourceManager( Ovgl::Context* context, const std::string& file );
			~ResourceManager();
			Context*                                context;
			std::vector< Font* >                    fonts;
			std::vector< Scene* >                   scenes;
			std::vector< Shader* >                  shaders;
			std::vector< Material* >                materials;
			std::vector< Mesh* >                    meshes;
			std::vector< Texture* >                 textures;
			std::vector< AudioBuffer* >             sounds;
			Mesh* importModel( const std::string& file, bool zUp );
			Shader* importShader( const std::string& file );
			Texture* importTexture( const std::string& file );
			Texture* importCubemap( const std::string& front, const std::string& back, const std::string& top, const std::string& bottom, const std::string& left, const std::string& right );
			AudioBuffer* importAudio( const std::string& file );
			Scene* createScene();
			Shader* createShader( const std::string& code );
			Mesh* createMesh();
			Material* createMaterial();
			Texture* createTexture( uint32_t width, uint32_t height );
			Texture* createCubemap( uint32_t width, uint32_t height );
			AudioBuffer* createAudioBuffer();
			void saveResources( const std::string& file );
			void loadResources( const std::string& file );
	};
}
}
