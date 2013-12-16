//
//  BMBatMobileViewController.m
//  batmobile
//
//  Created by Mark van Rossum on 22/07/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMBatMobileViewController.h"

#import <QuartzCore/QuartzCore.h>
#import "CustomRootCell.h"
#import "BMAnalyzeViewController.h"
#import "BMAboutViewController.h"
#import "BMBrowseDBListingViewController.h"
#import "BMRecordViewController.h"

@interface BMBatMobileViewController ()

@end

@implementation BMBatMobileViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        _options = [[NSArray alloc] initWithObjects:@"Record", @"Analyse", @"Bat Database",@"About",nil];

    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.navigationController.navigationBar.hidden = YES;
    _icons = [NSArray arrayWithObjects:@"record.png", @"analyze.png", @"database.png", @"about.png", nil];
    // Do any additional setup after loading the view from its nib.
}

- (void)viewWillAppear:(BOOL)animated {
    [self.navigationController setNavigationBarHidden:YES animated:animated];
    
    // This fixes a bug where the cell text has disappeared (it's white on white) when the user returns to the main menu
    if ([self.tableView indexPathForSelectedRow]) {
        CustomRootCell *cell = (CustomRootCell *) [self.tableView cellForRowAtIndexPath:[self.tableView indexPathForSelectedRow]];
        cell.nameLabel.textColor = [UIColor blackColor];
        [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:YES];
    }
    
    // background gradient below the table cell contents
    CAGradientLayer *gradient = [CAGradientLayer layer];
    gradient.frame = _bottomView.bounds;
    gradient.colors = [NSArray arrayWithObjects:
        (id)[[UIColor colorWithRed:139.0/255.0 green:132.0/255.0 blue:124.0/255.0 alpha:1.0] CGColor],
        (id)[[UIColor colorWithRed:198.0/255.0 green:188.0/255.0 blue:179.0/255.0 alpha:1.0] CGColor],
    nil];
    [_bottomView.layer insertSublayer:gradient atIndex:0];
    
    
    // shadow to appear below the table contents
    UIView *footerShadow = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 10)];

    // This looks like we convert from UIColor to CGColor and back again for no reason, but it does need
    // to be done this way.  App crashes if just use UIColor directly.
    // http://stackoverflow.com/a/7802088/1116197
    UIColor *darkColor = (id)[[UIColor darkGrayColor] CGColor];
    UIColor *lightColor = (id)[[UIColor clearColor] CGColor];
    
    CAGradientLayer *bottomShadow = [[CAGradientLayer alloc] init] ;
    bottomShadow.frame = CGRectMake(0,0, self.view.frame.size.width, 10);
    bottomShadow.colors = [NSArray arrayWithObjects:darkColor, lightColor, nil];
    
    [footerShadow.layer addSublayer:bottomShadow];
    [_bottomView addSubview:footerShadow];

    [super viewWillAppear:animated];
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Only one section.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [_options count] ;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"MenuCell";
    
    CustomRootCell *cell = (CustomRootCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[CustomRootCell alloc]  initWithStyle:UITableViewCellStyleDefault
                                        reuseIdentifier:CellIdentifier];
    }
    
    NSString *title = [_options objectAtIndex: [indexPath row]];
    [cell.nameLabel setText:title];
    [cell.imageView setImage:[UIImage imageNamed:[_icons objectAtIndex:indexPath.row]]];
    cell.imageView.contentMode = UIViewContentModeCenter;
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [self.navigationController setNavigationBarHidden:NO ];

    
    CustomRootCell *cell = (CustomRootCell *) [tableView cellForRowAtIndexPath:indexPath];
    cell.nameLabel.textColor = [UIColor whiteColor];

    UIViewController *viewController;
    switch ([indexPath row]) {
        case 0:         // Record
            viewController = [[BMRecordViewController alloc] init];
            break;

        case 1:         // Analyse
            viewController = [[BMAnalyzeViewController alloc] init];
            break;
        
        case 2:         // Database
            viewController = [[BMBrowseDBListingViewController alloc] init];
            break;
            
        case 3:         // About
            viewController = [[BMAboutViewController alloc] init];
            break;
    }
    
    [[self navigationController] pushViewController:viewController animated:YES];
       
}


@end
