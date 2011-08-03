/**
* @file OvglAudio.h
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

// Forward declare external classes

namespace Ovgl
{
	extern "C"
	{
		class Instance;
		class AudioBuffer;
		class AudioInstance;
		class AudioVoice;
		class Emitter;

		class __declspec(dllexport) AudioBuffer
		{
		public:
			Instance*							Inst;
			int									format;
			int									frequency;
			unsigned int						stereo;
			unsigned int						mono;
			std::vector<signed short>			data;
			AudioInstance* CreateAudioInstance( Emitter* emitter, bool loop );
		};

		class __declspec(dllexport) AudioInstance
		{
		public:
			Emitter*							emitter;
			std::vector<AudioVoice*>			voices;
			bool								paused;
			void Play( bool loop );
			void Stop();
			void Pause();
			void Set();
		};

		class __declspec(dllexport) AudioVoice
		{
		public:
			AudioInstance*						instance;
			unsigned int						source;
		};
	}
}