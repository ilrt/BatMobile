//
//  BMAnalyzeViewController.h
//  batmobile
//
//  Created by Mark van Rossum on 13/08/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMAnalyzeViewController : UIViewController <UITableViewDelegate>{
    NSString *examplePath;
    NSArray *exampleFiles;
    NSString *userPath;
    NSArray *userFiles;
}

@property (weak, nonatomic) IBOutlet UILabel *headerLabel;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) NSMutableArray *events;

-(void) remoteButtonPress:(id)sender;
-(void) localButtonPress:(id)sender;


@end
