//
//  MainViewController.m
//  BBSDK
//
//  Created by Bruce on 13-5-20.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import "MainViewController.h"
#import "BBRecorder.h"
#import "BBPlayer.h"
#import "bb_freq_util.h"

@interface MainViewController () <BBRecorderDelegate> {

    BBRecorder *_recorder;
    BBPlayer *_player;
}

@end

@implementation MainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
    if (self) {
    
        _recorder = [[BBRecorder alloc] initWithSampleRate:44100]; //758775
        [_recorder setDelegate:self];
        
        _player = [[BBPlayer alloc] initWithSampleRate:44100];
    }
    
    return self;
}

- (void)viewDidLoad {
    
    
    [self startListen];
    
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.

}

- (void)didReceiveMemoryWarning {
    
    
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)startListen {

    [_recorder start];
}

- (IBAction)stopListen {
    
    [_recorder stop];
}

- (IBAction)startPlay {
    
    NSMutableString *msg = [NSMutableString stringWithString:@""];
    
    for (int i=0; i<10; i++) {
        
        char c;
        int n = arc4random() % 32;
        
        num_to_char(n, &c);
        [msg appendFormat:@"%c", c];
    }
    
    [_player play:msg];
    
    self.myLable.text = msg;
}

- (IBAction)stopPlay {
    
    [_player stop];
}

- (void)dealloc {

    [_recorder stop];
    [_recorder release];
    
    [_player stop];
    [_player release];
    
    [super dealloc];
}

#pragma mark - BBRecorderDelegate


- (void)bbRecorder:(BBRecorder *)bbRecorder receivedMessage:(NSString *)message corrected:(BOOL)corrected {

    //NSLog(@"%@", [message substringWithRange:NSMakeRange(0, 10)]);
    
    //NSString *data = [[[message substringWithRange:NSMakeRange(0, 10)] copy] autorelease];
    
    [self alertTitle:@"收到消息:" message:[message substringWithRange:NSMakeRange(0, 10)]];
}


- (void)alertTitle:(NSString *)t message:(NSString *)m {
    
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:t
                                                        message:m
                                                       delegate:nil
                                              cancelButtonTitle:@"确定"
                                              otherButtonTitles:nil];
        [alert show];
        [alert release];
        
        
    });
    
}

@end
