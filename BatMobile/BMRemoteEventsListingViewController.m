//
//  BMLocalEventsListingViewController.m
//  BatMobileIPhone1
//
//  Created by Mark van Rossum on 11/03/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMRemoteEventsListingViewController.h"
#import "BMRemoteFeatureListViewController.h"

@implementation BMRemoteEventsListingViewController
@synthesize events;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [events count];
}

// @implementation
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    
    return [NSString stringWithFormat:@"Detected %u calls",[events count]];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"UITableViewCell"];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"UITabViewCell" ];
    }
    
    int index = [indexPath row];
    NSMutableDictionary *event = [events objectAtIndex:index];
    
    NSString *mainTextLabel = [NSString stringWithFormat:@"Start Time: %.4f", [[event objectForKey:@"starttime"] floatValue]];
    [[cell textLabel] setText: mainTextLabel];
    
    NSString *detailTextLabel = [NSString stringWithFormat:@"Duration: %.4f", [[event objectForKey:@"duration"] floatValue]];
    [[cell detailTextLabel] setText: detailTextLabel];
    
    return cell;
}



- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    BMRemoteFeatureListViewController *detailViewController = [[BMRemoteFeatureListViewController alloc] initWithNibName:@"BMRemoteFeatureListViewController" bundle:nil];
    NSMutableDictionary *event = [events objectAtIndex:[indexPath row]];
    [detailViewController setEvent:event];
    [self.navigationController pushViewController:detailViewController animated:YES];
    
}


@end
