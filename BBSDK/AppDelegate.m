//
//  AppDelegate.m
//  BBSDK
//
//  Created by Littlebox on 13-4-17.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import "AppDelegate.h"

#import "BBPlayer.h"

#import "BBRecorder.h"
#import "bb_freq_util.h"

@interface AppDelegate () {
    
    BBRecorder *_recorder;
}

@end

@implementation AppDelegate

- (void)dealloc
{
    [_recorder release];
    [_window release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
//    BBPlayer *player = [[[BBPlayer alloc] init] autorelease];
//    [player initBBPlayerWithSampleRate:44100];
//    [player play:@"1234567890"];

    
    /*
    int data[nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18] = {17, -1, -1, -1, -1, 19, 19, 26, 26, 26, 23, -1, -1, 26, 26, 26, 25, 25, -1, -1, -1, -1, 4, 4, -1, 8, -1, 8, -1, -1, 16, -1, -1, -1, 17, -1, -1, 2, 2, 2, -1, -1, 3, 3, -1, 14, -1, -1, -1, 29, -1, 15, 15, -1, 3, 3, 21, -1, 21, 3, 3, 3, 3};
    int resultCode[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    statistics(data, nNumberOfWatchDog + nNumberOfCodeRecorded + nNumberOfCodeRecorded/18, resultCode, 20);
    
    printf("\n");
    
    printf("(%d-%d-)", resultCode[0], resultCode[1]);
    
    for (int i=2; i<20; i++) {
        
        printf("%d-", resultCode[i]);
    }
     */
    
    
    _recorder = [[BBRecorder alloc] initWithSampleRate:44100]; //758775
    [_recorder start];
         
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
