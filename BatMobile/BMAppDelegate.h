//
//  BMAppDelegate.h
//  BatMobileIPhone1
//
//  Created by Mark van Rossum on 08/03/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BMViewController;

@interface BMAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) BMViewController *viewController;

@end
