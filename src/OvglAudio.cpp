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

#include "OvglInstance.h"
#include "OvglMath.h"
#include "OvglMedia.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglWindow.h"

namespace Ovgl
{
AudioInstance* AudioBuffer::CreateAudioInstance( Emitter* emitter, bool loop )
{
    AudioInstance* instance = new AudioInstance;
    instance->paused = false;
    instance->emitter = emitter;
    AudioVoice* voice = new AudioVoice;
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
        for( uint32_t w = 0; w < Inst->Windows.size(); w++ )
        {
            for( uint32_t r = 0; r < Inst->Windows[w]->RenderTargets.size(); r++ )
            {
                for(uint32_t c = 0; c < emitter->scene->cameras.size(); c++)
                {
                    if( Inst->Windows[w]->RenderTargets[r]->View == emitter->scene->cameras[c] )
                    {
                        emitter->scene->cameras[c]->voices.push_back(voice);
                    }
                }
            }
        }
    }
    return instance;
}

void AudioInstance::Play( bool loop )
{
    for( uint32_t i = 0; i < voices.size(); i++ )
    {
        alSourcePlay( voices[i]->source );
    }
    paused = false;
}


void AudioInstance::Stop()
{
    for( uint32_t i = 0; i < voices.size(); i++ )
    {
        alSourceStop(voices[i]->source);
    }
}

void AudioInstance::Pause()
{
    //if( paused )
    //{
    //	for( uint32_t v = 0; v < voices.size(); v++ )
    //	{
    //		voices[v]->voice->Start( 0 );
    //	}
    //	paused = false;
    //}
    //else
    //{
    //	for( uint32_t v = 0; v < voices.size(); v++ )
    //	{
    //		voices[v]->voice->Stop( 0 );
    //	}
    //	paused = true;
    //}
}

void AudioVoice::Release()
{
    alDeleteSources( 1, &source );
    delete this;
}

void AudioInstance::Release()
{
    for( uint32_t i = 0; i < 0; i++)
    {
        voices[i]->Release();
    }
    voices.clear();
    delete this;
}

void AudioBuffer::Release()
{
    data.clear();
    delete this;
}
}
