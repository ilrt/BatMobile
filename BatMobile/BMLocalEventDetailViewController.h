//
//  BMLocalEventDetailViewController.h
//  BatMobileIPhone1
//
//  Created by Mark van Rossum on 12/03/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BMEvent.h"

@interface BMLocalEventDetailViewController : UIViewController{
    
    __weak IBOutlet UITextField *callEnd;
    __weak IBOutlet UITextField *callStart;
    __weak IBOutlet UITextField *confidence;
    __weak IBOutlet UITextField *classification;
    
}
@property (nonatomic, strong) BMEvent *event;

-(NSString *)convertSecondsToTime:(double) seconds;


@end
