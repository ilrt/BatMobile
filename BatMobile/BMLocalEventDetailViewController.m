//
//  BMEventDetailViewController.m
//  BatMobileIPhone1
//
//  Created by Mark van Rossum on 12/03/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMLocalEventDetailViewController.h"
#import "BMConfig.h"

@implementation BMLocalEventDetailViewController
@synthesize event;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.BackgroundColor = [UIColor colorWithRed:TABLE_BG_RED/255.0 green:TABLE_BG_GREEN/255.0 blue:TABLE_BG_BLUE/255.0 alpha:1.0];

    [classification setText:[event classification]];
    [confidence setText:[NSString stringWithFormat:@"\%.4f",[event p_value]]];
    [callStart setText:[self convertSecondsToTime:[event startSample] / [event sampleRate] * [event TE]]];
    [callEnd setText:[self convertSecondsToTime:[event endSample]/[event sampleRate] * [event TE]]];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(NSString *)convertSecondsToTime:(double) seconds {
    int hours = (int) seconds / 3600;
    seconds = seconds - (3600 * hours);
    int minutes = (int) seconds / 60;
    seconds = seconds - (60 * minutes);
    return [NSString stringWithFormat:@"%uhr %umin %.2fsec",hours,minutes,seconds];
    
}

@end
