//
//  BMFeatureListViewController.m
//  batmobile
//
//  Created by Mark van Rossum on 30/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMRemoteFeatureListViewController.h"
#import "BMConfig.h"

@interface BMRemoteFeatureListViewController ()

@end

@implementation BMRemoteFeatureListViewController
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

    [duration setText:[NSString stringWithFormat:@"\%.4fsec", [[event objectForKey:@"duration"] floatValue]] ];
    [fmax setText:[NSString stringWithFormat:@"\%.4fHz", [[event objectForKey:@"fmax"] floatValue]] ];
    [fmin setText:[NSString stringWithFormat:@"\%.4fHz", [[event objectForKey:@"fmin"] floatValue]] ];
    [bandwidth setText:[NSString stringWithFormat:@"\%.4fHz", [[event objectForKey:@"bandwidth"] floatValue]] ];
    [fc setText:[NSString stringWithFormat:@"\%.4fHz", [[event objectForKey:@"fc"] floatValue]] ];
    [fctr setText:[NSString stringWithFormat:@"\%.4fHz", [[event objectForKey:@"fctr"] floatValue]] ];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)explainDuration:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Duration"
                          message:@"The number of seconds the call lasted"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}

- (IBAction)explainFmax:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Maximum Frequency"
                          message:@"The maximum frequency in the call"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}
- (IBAction)explainFmin:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Minimum Frequency"
                          message:@"The minimum frequency in the call"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}

- (IBAction)explainBandwidth:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Bandwidth"
                          message:@"The frequency range in the call.  FMax - FMin"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}

- (IBAction)explainFc:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Characteristic Frequency (Hz)"
                          message:@"Frequency of the instantaneous point in the final 40% of the call with lowest slope"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}

- (IBAction)explainFctr:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"FCtr"
                          message:@"Frequency at half the duration of the call"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}

- (IBAction)sendForAnalysis:(id)sender {
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Send For Analysis"
                          message:@"Not yet implemented"
                          delegate:self
                          cancelButtonTitle:@"Ok"
                          otherButtonTitles: nil];
    [alert show];
}
@end
