//
//  BMBatMobileViewController.h
//  batmobile
//
//  Created by Mark van Rossum on 22/07/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMBatMobileViewController : UIViewController
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) NSArray *icons;
@property (strong, nonatomic) NSArray *options;
@property (weak, nonatomic) IBOutlet UIView *bottomView;

@end
