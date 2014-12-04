/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#include "chronotext/sound/AudioLoopImplCocoa.h"

#if TARGET_OS_IPHONE
#define AUDIO_UNIT_COMPONENT_SUB_TYPE kAudioUnitSubType_RemoteIO
#else
#define AUDIO_UNIT_COMPONENT_SUB_TYPE kAudioUnitSubType_DefaultOutput
#endif

bool AudioLoopImplCocoa::init()
{
    if (!initialized)
    {
        AudioComponentDescription defaultOutputDescription;
        defaultOutputDescription.componentType = kAudioUnitType_Output;
        defaultOutputDescription.componentSubType = AUDIO_UNIT_COMPONENT_SUB_TYPE;
        defaultOutputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
        defaultOutputDescription.componentFlags = 0;
        defaultOutputDescription.componentFlagsMask = 0;
        
        AudioComponent defaultOutput = AudioComponentFindNext(NULL, &defaultOutputDescription);
        if (!defaultOutput)
        {
            printf("Can't find default output\n");
            return false;
        }
        
        OSStatus err = AudioComponentInstanceNew(defaultOutput, &audioUnit);
        if (err)
        {
            printf("AudioComponentInstanceNew ERROR: %d\n", (int)err);
            return false;
        }
        
        AURenderCallbackStruct input;
        input.inputProc = staticRenderCallback;
        input.inputProcRefCon = this;
        err = AudioUnitSetProperty(audioUnit, 
                                   kAudioUnitProperty_SetRenderCallback, 
                                   kAudioUnitScope_Input,
                                   0, 
                                   &input, 
                                   sizeof(input));
        if (err)
        {
            printf("AudioUnitSetProperty/kAudioUnitProperty_SetRenderCallback ERROR: %d\n", (int)err);
            return false;
        }
        
        AudioStreamBasicDescription streamFormat;
        streamFormat.mSampleRate = 44100;
        streamFormat.mFormatID = kAudioFormatLinearPCM;
        streamFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
        streamFormat.mBytesPerPacket = 4;
        streamFormat.mFramesPerPacket = 1;    
        streamFormat.mBytesPerFrame = 4;        
        streamFormat.mChannelsPerFrame = 1;    
        streamFormat.mBitsPerChannel = 32;
        err = AudioUnitSetProperty(audioUnit,
                                   kAudioUnitProperty_StreamFormat,
                                   kAudioUnitScope_Input,
                                   0,
                                   &streamFormat,
                                   sizeof(AudioStreamBasicDescription));
        if (err)
        {
            printf("AudioUnitSetProperty/kAudioUnitProperty_StreamFormat ERROR: %d\n", (int)err);
            return false;
        }
        
        err = AudioUnitInitialize(audioUnit);
        if (err)
        {
            printf("AudioUnitInitialize ERROR: %d\n", (int)err);
            return false;
        }
        
        initialized = true;
    }
    
    return true;
}

void AudioLoopImplCocoa::shutdown()
{
    if (initialized)
    {
        AudioOutputUnitStop(audioUnit);
        AudioUnitUninitialize(audioUnit);
        AudioComponentInstanceDispose(audioUnit);
        audioUnit = NULL;
    }
}

void AudioLoopImplCocoa::start()
{
    if (initialized)
    {
        OSStatus err = AudioOutputUnitStart(audioUnit);
        if (err) printf("AudioOutputUnitStart ERROR: %d\n", (int)err);
    }
}

void AudioLoopImplCocoa::stop()
{
    if (initialized)
    {
        OSStatus err = AudioOutputUnitStop(audioUnit);
        if (err) printf("AudioOutputUnitStop ERROR: %d\n", (int)err);
    }
}

/*
 * FOR SETTING VOLUME ON iOS, AudioQueue SHOULD BE USED INSTEAD OF AudioUnit
 */
void AudioLoopImplCocoa::setVolume(float volume)
{
#if !TARGET_OS_IPHONE
    if (initialized)
    {
        OSStatus err = AudioUnitSetParameter(audioUnit, kHALOutputParam_Volume, kAudioUnitScope_Output, 0, volume, 0);
        if (err) printf("AudioUnitSetParameter ERROR: %d\n", (int)err);
    }
#endif
}
