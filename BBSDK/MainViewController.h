//
//  MainViewController.h
//  BBSDK
//
//  Created by Bruce on 13-5-20.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainViewController : UIViewController

- (IBAction)startPlay;
- (IBAction)stopPlay;


@property (nonatomic, retain) IBOutlet UILabel *myLable;

@end
