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
* @brief None.
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

		/**
        * This class contains pre-rendered text characters.
        * @brief The style of text to render.
        */
        class DLLEXPORT Font
        {
        public:
			Font( Resource* resource, const std::string& file, uint32_t size );
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
			Resource*								MLibrary;
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
			Resource*								MLibrary;
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
			Resource*								MLibrary;
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

        class DLLEXPORT Resource
		{
		public:
            Resource( Ovgl::Context* context, const std::string& file );
			~Resource();
            Context* context;
			std::vector< Font* > Fonts;
			std::vector< Scene* > Scenes;
			std::vector< Shader* > Shaders;
			std::vector< Material* > Materials;
			std::vector< Mesh* > Meshes;
			std::vector< Texture* > Textures;
			std::vector< AudioBuffer* > AudioBuffers;
			Mesh* ImportModel( const std::string& file, bool z_up );
			Shader* ImportShader( const std::string& file );
			Texture* ImportTexture( const std::string& file );
			Texture* ImportCubeMap( const std::string& front, const std::string& back, const std::string& top, const std::string& bottom, const std::string& left, const std::string& right );
			AudioBuffer* ImportAudio( const std::string& file );
			Scene* CreateScene();
			Shader* CreateShader( const std::string& code );
			Mesh* CreateMesh();
			Material* CreateMaterial();
			Texture* CreateTexture( uint32_t width, uint32_t height );
			Texture* CreateCubemap( uint32_t width, uint32_t height );
			AudioBuffer* CreateAudioBuffer();
			void Save( const std::string& file );
			void Load( const std::string& file );
		};
	}
}
