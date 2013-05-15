//
//  BBPlayer.m
//  BBSDK
//
//  Created by Littlebox on 13-4-17.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>

#import "BBPlayer.h"
#include "bb_header.h"

@implementation BBPlayer

@synthesize timer   = _timer;

OSStatus RenderTone(
                    void                        *inRefCon,
                    AudioUnitRenderActionFlags 	*ioActionFlags,
                    const AudioTimeStamp 		*inTimeStamp,
                    UInt32 						inBusNumber,
                    UInt32 						inNumberFrames,
                    AudioBufferList 			*ioData)

{
    BBPlayer *player = (BBPlayer *)inRefCon;
    float *a = (Float32 *)ioData->mBuffers[0].mData;
    encode_sound(player->_frequency, a, inNumberFrames);
    
	return noErr;
}

void interruptionListener(void *inClientData, UInt32 inInterruptionState)
{
	BBPlayer *player = (BBPlayer *)inClientData;
	[player stop];
}

- (void)dealloc {
    
    [_timer release];
    
    [super dealloc];
}

- (void)initBBPlayerWithSampleRate:(NSInteger)rate
{
    _once = YES;
    _charIndex = 0;
	_sampleRate = rate;
    
	OSStatus result = AudioSessionInitialize(NULL, NULL, interruptionListener, self);
	if (result == kAudioSessionNoError)
	{
		UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);
	}
	AudioSessionSetActive(true);
}

- (void)createToneUnit
{
	// Configure the search parameters to find the default playback output unit
	// (called the kAudioUnitSubType_RemoteIO on iOS but
	// kAudioUnitSubType_DefaultOutput on Mac OS X)
	AudioComponentDescription defaultOutputDescription;
	defaultOutputDescription.componentType = kAudioUnitType_Output;
	defaultOutputDescription.componentSubType = kAudioUnitSubType_RemoteIO;
	defaultOutputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
	defaultOutputDescription.componentFlags = 0;
	defaultOutputDescription.componentFlagsMask = 0;
	
	// Get the default playback output unit
	AudioComponent defaultOutput = AudioComponentFindNext(NULL, &defaultOutputDescription);
	NSAssert(defaultOutput, @"Can't find default output");
	
	// Create a new unit based on this that we'll use for output
	OSErr err = AudioComponentInstanceNew(defaultOutput, &toneUnit);
	NSAssert1(toneUnit, @"Error creating unit: %hd", err);
	
	// Set our tone rendering function on the unit
	AURenderCallbackStruct input;
	input.inputProc = RenderTone;
	input.inputProcRefCon = self;
	err = AudioUnitSetProperty(toneUnit,
                               kAudioUnitProperty_SetRenderCallback,
                               kAudioUnitScope_Input,
                               0,
                               &input,
                               sizeof(input));
    
	NSAssert1(err == noErr, @"Error setting callback: %hd", err);
	
	// Set the format to 32 bit, single channel, floating point, linear PCM
	const int four_bytes_per_float = 4;
	const int eight_bits_per_byte = 8;
	AudioStreamBasicDescription streamFormat;
	streamFormat.mSampleRate = _sampleRate;
	streamFormat.mFormatID = kAudioFormatLinearPCM;
	streamFormat.mFormatFlags =
    kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
	streamFormat.mBytesPerPacket = four_bytes_per_float;
	streamFormat.mFramesPerPacket = 1;
	streamFormat.mBytesPerFrame = four_bytes_per_float;
	streamFormat.mChannelsPerFrame = 1;
	streamFormat.mBitsPerChannel = four_bytes_per_float * eight_bits_per_byte;
	err = AudioUnitSetProperty (toneUnit,
                                kAudioUnitProperty_StreamFormat,
                                kAudioUnitScope_Input,
                                0,
                                &streamFormat,
                                sizeof(AudioStreamBasicDescription));
	NSAssert1(err == noErr, @"Error setting stream format: %hd", err);
}

- (void)play:(NSString *)message
{
    if ([message isEqualToString:@""]) {
        return;
    }
    
    // 初始化字符串
    
    unsigned char *data = (unsigned char *)[message UTF8String];

    create_sending_code(data, _messageToSend, SEND_DATA_LEN);
    
    self.timer = [NSTimer scheduledTimerWithTimeInterval: 0.0872
                                                  target: self
                                                selector: @selector(handleTimer:)
                                                userInfo: nil
                                                 repeats: YES];
}

- (void)stop
{
    AudioOutputUnitStop(toneUnit);
    AudioUnitUninitialize(toneUnit);
    AudioComponentInstanceDispose(toneUnit);
    toneUnit = nil;
    
    [self.timer invalidate];
    _once = YES;
}

- (void)handleTimer:(id)sender {
    
    if (_charIndex <= SEND_DATA_LEN-1) {
        
        char charToSend = _messageToSend[_charIndex];
        _charIndex++;
        
        freq_init();
        num_to_freq((int)charToSend, &_frequency);
        
        if (_once) {
            
            _once = NO;
            [self createToneUnit];
            
            // Stop changing parameters on the unit
            OSErr err = AudioUnitInitialize(toneUnit);
            NSAssert1(err == noErr, @"Error initializing unit: %hd", err);
            
            // Start playback
            err = AudioOutputUnitStart(toneUnit);
            NSAssert1(err == noErr, @"Error starting unit: %hd", err);
        }
        
    } else {
        
        _charIndex = 0;
        [self stop];
    }
}


@end
