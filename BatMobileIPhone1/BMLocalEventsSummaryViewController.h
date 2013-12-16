//
//  BMLocalEventsSummaryViewController.h
//  batmobile
//
//  Created by Mark van Rossum on 04/09/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMLocalEventsSummaryViewController : UITableViewController {
    NSMutableDictionary *eventSummaries;
    NSArray *keys;
    NSMutableDictionary *eventsByClassification;
}

@property (nonatomic, strong) NSMutableArray *events;

@end
