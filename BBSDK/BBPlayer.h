//
//  BBPlayer.h
//  BBSDK
//
//  Created by Littlebox on 13-4-17.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioUnit/AudioUnit.h>

#define SEND_DATA_LEN 20

@interface BBPlayer : NSObject
{
    AudioComponentInstance toneUnit;
    
@public
	unsigned int _frequency;
	double _sampleRate;
    int    _charIndex;
    BOOL   _once;
    unsigned char _messageToSend[SEND_DATA_LEN];
}

@property (nonatomic, retain) NSTimer  *timer;


- (id)initWithSampleRate:(NSInteger)rate;
- (void)createToneUnit;
- (void)play:(NSString *)message;
- (void)stop;

@end
