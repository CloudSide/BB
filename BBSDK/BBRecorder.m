//
//  BBRecorder.m
//  BBSDK
//
//  Created by Littlebox on 13-4-17.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import "BBRecorder.h"

#include "bb_header.h"

@implementation BBRecorder

static void myInputBufferHandler(void *inUserData,
                          AudioQueueRef inAQ,
                          AudioQueueBufferRef inBuffer,
                          const AudioTimeStamp *inStartTime,
                          UInt32 inNumPackets,
                          const AudioStreamPacketDescription *inPacketDesc)
{
	BBRecorder *recorder = (BBRecorder *)inUserData;

    if (inNumPackets > 0) {
        
        int numFrequencies = pow(2,floor(log2(inBuffer->mAudioDataByteSize)));
        
        /*
        void *temp = (void *)malloc(numFrequencies);
        memcpy(temp, inBuffer->mAudioData, numFrequencies);
        */
        
        // soundCode:0-31
        int soundCode = decode_sound(inBuffer->mAudioData, numFrequencies);
        
        
        // 监听
        if (recorder->_listenState == LISTEN) {
            
            [recorder listenning:soundCode];
        }
        
        // 录音
        if (recorder->_listenState == RECORD) {
            
            @synchronized(recorder) {
                
                if (recorder->_recordedCounter >= nNumberOfCodeRecorded+nNumberOfCodeRecorded/18) {
                    
                    printf("--------------   Record finished !  ---------------\n");
                    [recorder recordFinished];

                } else {
                    
                    [recorder recording:soundCode];
                }
            
            }
        }
         
        
        //free(temp);
    }
    
    
    
    // if we're not stopping, re-enqueue the buffe so that it gets filled again
    if (recorder->mIsRunning==1) {
        
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}


//void interruptionListener(void *inClientData, UInt32 inInterruptionState)
//{
//	//BBRecorder *recorder = (BBRecorder *)inClientData;
//	//[recorder stop];
//}

- (void)dealloc
{
    [super dealloc];
}

- (id)initWithSampleRate:(NSInteger)rate {

    if (self = [super init]) {
        
        mIsRunning = 0;
        mRecordPacket = 0;
        
        _recordedCounter = 0;
        
        for (int i=0; i<nNumberOfWatchDog; i++) {
            _watchDog[i] = -1;
        }
        
        for (int i=0; i<nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18; i++) {
            _allData[i] = -1;
        }
        
        _sampleRate = rate;
        _listenState = LISTEN;
        
        OSStatus result = AudioSessionInitialize(NULL, NULL, NULL, self);
        if (result==kAudioSessionNoError) {
            
            UInt32 category = kAudioSessionCategory_RecordAudio;
            AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        }
        
        AudioSessionSetActive(true);
    }
    
    return self;
}

- (void)start
{
    int i, bufferByteSize;
	UInt32 size;
    
    [self setupAudioFormat:kAudioFormatLinearPCM];
    
    AudioQueueNewInput(
                       &mRecordFormat,
                       myInputBufferHandler,
                       self /* userData */,
                       NULL /* run loop */, NULL /* run loop mode */,
                       0 /* flags */, &mQueue);
    
    mRecordPacket = 0;
    
    size = sizeof(mRecordFormat);
    AudioQueueGetProperty(mQueue, kAudioQueueProperty_StreamDescription, &mRecordFormat, &size);
    
    // allocate and enqueue buffers
    bufferByteSize = [self computeRecordBufferSize:&mRecordFormat withSecond:kBufferDurationSeconds];// enough bytes for half a second
    for (i = 0; i < kNumberRecordBuffers; ++i) {
        
        AudioQueueAllocateBuffer(mQueue, bufferByteSize, &mBuffers[i]);
        AudioQueueEnqueueBuffer(mQueue, mBuffers[i], 0, NULL);
    }
    // start the queue
    mIsRunning = 1;
    AudioQueueStart(mQueue, NULL);

}

- (void)stop
{
    
}

- (void)setupAudioFormat:(UInt32) inFormatID
{
	memset(&mRecordFormat, 0, sizeof(mRecordFormat));
    
	UInt32 size = sizeof(mRecordFormat.mSampleRate);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, &size, &mRecordFormat.mSampleRate);
    
	size = sizeof(mRecordFormat.mChannelsPerFrame);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &mRecordFormat.mChannelsPerFrame);
    
	mRecordFormat.mFormatID = inFormatID;
	if (inFormatID == kAudioFormatLinearPCM)
	{
		// if we want pcm, default to signed 16-bit little-endian
		mRecordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
		mRecordFormat.mBitsPerChannel = 16;
		mRecordFormat.mBytesPerPacket = mRecordFormat.mBytesPerFrame = (mRecordFormat.mBitsPerChannel / 8) * mRecordFormat.mChannelsPerFrame;
		mRecordFormat.mFramesPerPacket = 1;
        
        // Littlebox-XXOO
        mRecordFormat.mSampleRate = _sampleRate;
	}
}

- (NSInteger)computeRecordBufferSize:(AudioStreamBasicDescription *)format withSecond:(float)seconds
{
	int packets, frames, bytes = 0;
    
    frames = (int)ceil(seconds * format->mSampleRate);
    
    if (format->mBytesPerFrame > 0)
        bytes = frames * format->mBytesPerFrame;
    else {
        UInt32 maxPacketSize;
        if (format->mBytesPerPacket > 0)
            maxPacketSize = format->mBytesPerPacket;	// constant packet size
        else {
            UInt32 propertySize = sizeof(maxPacketSize);
            AudioQueueGetProperty(mQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize, &propertySize);
        }
        if (format->mFramesPerPacket > 0)
            packets = frames / format->mFramesPerPacket;
        else
            packets = frames;	// worst-case scenario: 1 frame in a packet
        if (packets == 0)		// sanity check
            packets = 1;
        bytes = packets * maxPacketSize;
    }
    
	return bytes;
}

- (void)listenning:(int)code
{
    for (int i=0; i<nNumberOfWatchDog-1; i++) {
        
        _watchDog[i] = _watchDog[i+1];
    }
    _watchDog[nNumberOfWatchDog-1] = code;
    
    
    if (_watchDog[0] == 17) {
        
        for (int i=1; i<nNumberOfWatchDog; i++) {
            
            if (_watchDog[i]==19) {
                
                [self listenFinished];
                break;
            }
        }
    }
}

- (void)listenFinished
{
    for (int i=0; i<nNumberOfWatchDog; i++) {
        
        _allData[i] = _watchDog[i];
        _watchDog[i] = 0;
    }
    _listenState = RECORD;
}

- (void)recording:(int)code
{
    _allData[nNumberOfWatchDog + _recordedCounter] = code;
    _recordedCounter++;
}

- (void)recordFinished
{
    mIsRunning = 0;
    
    for (int i=0; i<nNumberOfWatchDog + nNumberOfCodeRecorded+nNumberOfCodeRecorded/18; i++) {
        
        printf("%d ~ ", _allData[i]);
        
        if ((i+1)%(nNumberOfWatchDog/2)==0) {
        
            printf("\n");
        }
    }
    /*
    for (int i=0; i<nNumberOfWatchDog + nNumberOfCodeRecorded+nNumberOfCodeRecorded/18; i++) {
        
        printf("%d, ", _allData[i]);
    }
     */
    
    printf("\n");
    
    int resultCode[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    statistics(_allData, nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18, resultCode, 20);
    
    printf("\n");
    
    printf("(%d-%d-)", resultCode[0], resultCode[1]);
    
    for (int i=2; i<20; i++) {
        
        printf("%d-", resultCode[i]);
    }
    
    
    
    RS *rs = init_rs(RS_SYMSIZE, RS_GFPOLY, RS_FCR, RS_PRIM, RS_NROOTS, RS_PAD);
    int eras_pos[RS_TOTAL_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    unsigned char data1[RS_TOTAL_LEN];
    
    for (int i=0; i<RS_TOTAL_LEN; i++) {
        data1[i] = resultCode[i+2];
    }
    
    int count = decode_rs_char(rs, data1, eras_pos, 0);//(unsigned char*)(resultCode+2)
    
    printf("\n");
    printf("count  =  %d\n", count);
    for (int i=0; i<18; i++) {
        printf("%d-", data1[i]);
    }
    
    printf("\n\n\n");
    
    
    for (int i=0; i<nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18; i++) {
        _allData[i] = -1;
    }

    _listenState = LISTEN;
    _recordedCounter = 0;
    mIsRunning = 1;
}

@end
