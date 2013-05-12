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
            uint32_t								charset[256];
            uint32_t								charoffsets[256];
            uint32_t                                size;
        };

		        /**
        * A texture is a two dimensional image stored in a chuck of memory on the graphics card.
        * @brief Two dimensional image.
        */
        class DLLEXPORT Texture
		{
		public:
			ResourceManager*						MLibrary;
			uint32_t								Image;
			std::string								File;
			bool									HasAlpha;
			void Release();
		};

        /**
        * A shader is a small program used to process geometry and pixel information.
        * In other words is decides how objects are rendered to the screen.
        * @brief Small program used for rendering surfaces.
        */
        class DLLEXPORT Shader
		{
		public:
			ResourceManager*						MLibrary;
            CGeffect                                effect;
			void Release();
		};

        /**
        * Stores the all the informaton to render a surface such as textures, variables, and shaders.
        * You can think if it as the material which the surface is made out of.
        * @brief The material
        */
        class DLLEXPORT Material
		{
		public:
			ResourceManager*						MLibrary;
			Shader*									ShaderProgram;
			bool									PostRender;
			bool									NoZBuffer;
			bool									NoZWrite;
			std::vector< std::pair< CGparameter, std::vector< float > > > Variables;
			std::vector< std::pair< CGparameter, Ovgl::Texture* > > Textures;
            void setEffectVariable(const std::string& variable, const std::vector< float >& data);
            void setEffectTexture(const std::string& variable, Texture* texture);
			void Release();
		};

        class DLLEXPORT ResourceManager
		{
		public:
            ResourceManager( Ovgl::Context* context, const std::string& file );
			~ResourceManager();
            Context* context;
			std::vector< Font* > fonts;
			std::vector< Scene* > scenes;
			std::vector< Shader* > shaders;
			std::vector< Material* > materials;
			std::vector< Mesh* > meshes;
			std::vector< Texture* > textures;
			std::vector< AudioBuffer* > sounds;
			Mesh* import_model( const std::string& file, bool z_up );
			Shader* import_shader( const std::string& file );
			Texture* import_texture( const std::string& file );
			Texture* import_cubemap( const std::string& front, const std::string& back, const std::string& top, const std::string& bottom, const std::string& left, const std::string& right );
			AudioBuffer* import_audio( const std::string& file );
			Scene* create_scene();
			Shader* create_shader( const std::string& code );
			Mesh* create_mesh();
			Material* create_material();
			Texture* create_texture( uint32_t width, uint32_t height );
			Texture* create_cubemap( uint32_t width, uint32_t height );
			AudioBuffer* create_audio_buffer();
			void save_resources( const std::string& file );
			void load_resources( const std::string& file );
		};
	}
}
