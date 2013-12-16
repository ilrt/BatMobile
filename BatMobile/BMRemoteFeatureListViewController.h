//
//  BMFeatureListViewController.h
//  batmobile
//
//  Created by Mark van Rossum on 30/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMRemoteFeatureListViewController : UIViewController {
    __weak IBOutlet UITextField *duration;
    __weak IBOutlet UITextField *fmax;
    __weak IBOutlet UITextField *fmin;
    __weak IBOutlet UITextField *fc;
    __weak IBOutlet UITextField *bandwidth;
    __weak IBOutlet UITextField *fctr;
}

@property (nonatomic, strong) NSMutableDictionary *event;

- (IBAction)explainDuration:(id)sender;
- (IBAction)explainFmax:(id)sender;
- (IBAction)explainFmin:(id)sender;
- (IBAction)explainBandwidth:(id)sender;
- (IBAction)explainFc:(id)sender;
- (IBAction)explainFctr:(id)sender;
- (IBAction)sendForAnalysis:(id)sender;

@end
