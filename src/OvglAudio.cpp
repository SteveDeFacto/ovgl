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
* @brief This part of the library has to do with anything audio related.
*/

#include "OvglContext.h"
#include "OvglMath.h"
#include "OvglResource.h"
#include "OvglGraphics.h"
#include "OvglScene.h"
#include "OvglAudio.h"
#include "OvglWindow.h"
#include <AL/al.h>
#include <AL/alc.h>

namespace Ovgl
{
AudioInstance* AudioBuffer::createAudioInstance( Emitter* emitter, bool loop )
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
    instance->voices.push_back( voice );
    if( emitter )
    {
        for( uint32_t w = 0; w < context->windows.size(); w++ )
        {
            for( uint32_t r = 0; r < context->windows[w]->renderTargets.size(); r++ )
            {
                for( uint32_t c = 0; c < emitter->scene->cameras.size(); c++ )
                {
                    if( context->windows[w]->renderTargets[r]->view == emitter->scene->cameras[c] )
                    {
                        emitter->scene->cameras[c]->voices.push_back( voice );
                    }
                }
            }
        }
    }
    audioInstances.push_back( instance );
    return instance;
}

void AudioInstance::play( bool loop )
{
    for( uint32_t i = 0; i < voices.size(); i++ )
    {
        alSourcePlay( voices[i]->source );
    }
    paused = false;
}


void AudioInstance::stop()
{
    for( uint32_t i = 0; i < voices.size(); i++ )
    {
        alSourceStop(voices[i]->source);
    }
}

void AudioInstance::pause()
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

void AudioVoice::release()
{
    alDeleteSources( 1, &source );
    delete this;
}

void AudioInstance::release()
{
    for( uint32_t i = 0; i < voices.size(); i++)
    {
        voices[i]->release();
    }
    voices.clear();
    delete this;
}

void AudioBuffer::release()
{
    for( uint32_t i = 0; i < audioInstances.size(); i++)
    {
        audioInstances[i]->release();
    }
    alDeleteBuffers(1, &mono);
    alDeleteBuffers(1, &stereo);
    data.clear();
    delete this;
}
}
