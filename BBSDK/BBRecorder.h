//
//  BBRecorder.h
//  BBSDK
//
//  Created by Littlebox on 13-4-17.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

#define kNumberRecordBuffers    5

#define test 3

#if (test==7)

#define kBufferDurationSeconds  0.01245//0.01245//0.01744//0.0872
#define nNumberOfCodeRecorded  126//162//126
#define nNumberOfWatchDog      14//18//14

#elif(test==5)

#define kBufferDurationSeconds  0.01744//0.01245//0.01744//0.0872
#define nNumberOfCodeRecorded  90//162//126
#define nNumberOfWatchDog      10//18//14

#elif(test==3)

#define kBufferDurationSeconds  0.029066 //0.029066667//0.01245//0.01744//0.0872//0.02906
#define nNumberOfCodeRecorded  54//162//126
#define nNumberOfWatchDog      6//18//14

#elif(test==1)

#define kBufferDurationSeconds  0.0872//0.01245//0.01744//0.0872
#define nNumberOfCodeRecorded  18//162//126
#define nNumberOfWatchDog      2//18//14

#endif

enum RecordingState {
    LISTEN = 0,
    RECORD = 1
    };

@interface BBRecorder : NSObject
{
    
@public
    CFStringRef					mFileName;
    AudioQueueRef				mQueue;
    AudioQueueBufferRef			mBuffers[kNumberRecordBuffers];
    AudioFileID					mRecordFile;
    SInt64						mRecordPacket; // current packet number in record file
    AudioStreamBasicDescription	mRecordFormat;
    int			     			mIsRunning;

    
    double                      _sampleRate;
    int                         _recordedCounter;
    int                         _watchDog[nNumberOfWatchDog];
    int                         _allData[nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18];
    enum RecordingState         _listenState;
}

- (void)setupAudioFormat:(UInt32)inFormatID;
- (NSInteger)computeRecordBufferSize:(AudioStreamBasicDescription *)format withSecond:(float)seconds;

- (void)start;
- (void)stop;

- (void)listenning:(int)code;
- (void)listenFinished;

- (void)recording:(int)code;
- (void)recordFinished;

- (id)initWithSampleRate:(NSInteger)rate;

@end
