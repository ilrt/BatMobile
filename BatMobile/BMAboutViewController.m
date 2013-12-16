//
//  BMAboutViewController.m
//  batmobile
//
//  Created by Mark van Rossum on 10/07/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMAboutViewController.h"


@implementation BMAboutViewController
@synthesize webView;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad {

    [super viewDidLoad];
    
    NSString *resources = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resources stringByAppendingPathComponent:@"about.html"];
    
    NSError *error = nil;
    NSString * resourceFileContent = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:&error];
    
    if (error != nil) {
        @throw [NSException exceptionWithName:[error domain] reason:[error localizedDescription] userInfo:[error userInfo]];
    }
    
    NSURL *baseURL = [NSURL fileURLWithPath:resources];
    [webView loadHTMLString:resourceFileContent baseURL:baseURL];

    
    
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



@end
