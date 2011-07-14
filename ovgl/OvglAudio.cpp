/**
* @file OvglAudio.cpp
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
#include "OvglScene.h"
#include "OvglAudio.h"

Ovgl::AudioInstance* Ovgl::AudioBuffer::CreateAudioInstance( Ovgl::Emitter* emitter, bool loop )
{
	Ovgl::AudioInstance* instance = new Ovgl::AudioInstance;
	instance->paused = false;
	instance->emitter = emitter;
	Ovgl::AudioVoice* voice = new Ovgl::AudioVoice;
	voice->instance = instance;
	alGenSources(1, &voice->source);
	if( !emitter && format == AL_FORMAT_STEREO16 )
	{
		alSourcei( voice->source, AL_BUFFER, stereo );
	}
	else
	{
		alSourcei( voice->source, AL_BUFFER, mono );
	}
	if( !emitter )
	{
		alSourcei( voice->source, AL_SOURCE_RELATIVE, AL_TRUE );
	}
	alSourcef( voice->source, AL_PITCH,	1.0f );
	alSourcef( voice->source, AL_GAIN, 1.0f );
	alSourcei( voice->source, AL_LOOPING, loop );
	alSourcePlay( voice->source );
	instance->voices.push_back(voice);
	if( emitter )
	{
		for( UINT r = 0; r < this->Inst->RenderTargets.size(); r++ )
		{
			for(UINT c = 0; c < emitter->scene->cameras.size(); c++)
			{
				if( Inst->RenderTargets[r]->view == emitter->scene->cameras[c] )
				{
					emitter->scene->cameras[c]->voices.push_back(voice);
				}
			}
		}
	}
	return instance;
}

void Ovgl::AudioInstance::Play( bool loop )
{
	//for( UINT v = 0; v < voices.size(); v++ )
	//{
	//	voices[v]->voice->Start( 0 );
	//}
	//paused = false;
}


void Ovgl::AudioInstance::Stop()
{
	//for( UINT v = 0; v < voices.size(); v++ )
	//{
	//	voices[v]->voice->Stop( 0 );
	//	voices[v]->voice->FlushSourceBuffers();
	//	voices[v]->voice->DestroyVoice();
	//}
}

void Ovgl::AudioInstance::Pause()
{
	//if( paused )
	//{
	//	for( UINT v = 0; v < voices.size(); v++ )
	//	{
	//		voices[v]->voice->Start( 0 );
	//	}
	//	paused = false;
	//}
	//else
	//{
	//	for( UINT v = 0; v < voices.size(); v++ )
	//	{
	//		voices[v]->voice->Stop( 0 );
	//	}
	//	paused = true;
	//}
}