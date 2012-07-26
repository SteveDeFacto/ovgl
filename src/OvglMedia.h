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

namespace Ovgl
{
	extern "C"
	{
		class Mesh;
		class AudioBuffer;
		class Scene;
		class Mesh;
		class Instance;
		class Shader;
		class Texture;

        class DLLEXPORT MediaLibrary
		{
		public:
			MediaLibrary( Ovgl::Instance* instance, const std::string& file );
			Instance* Inst;
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
			void Release();
		};
	}
}
