//
//  BMLocalEventsSummaryViewController.m
//  batmobile
//
//  Created by Mark van Rossum on 04/09/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import "BMLocalEventsSummaryViewController.h"
#import "BMEvent.h"
#import "BMLocalEventsListingViewController.h"

@interface BMLocalEventsSummaryViewController ()

@end

@implementation BMLocalEventsSummaryViewController

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
    self.title = @"Call Summaries";
    eventSummaries = [[NSMutableDictionary alloc] initWithCapacity:20];
    eventsByClassification = [[NSMutableDictionary alloc] initWithCapacity:20];

    for (BMEvent *event in _events) {
        if ([eventSummaries objectForKey:event.classification] == nil) {
            NSMutableDictionary *eventSummary = [[NSMutableDictionary alloc] initWithCapacity:2];
            // Can but dream of Javas autoboxing.... :(
            [eventSummary setObject:[[NSNumber alloc] initWithInt:0] forKey:@"count"];
            [eventSummary setObject:[[NSNumber alloc] initWithFloat:0.0] forKey:@"average_confidence"];
            [eventSummaries setObject:eventSummary forKey:event.classification];
            
            [eventsByClassification setObject:[[NSMutableArray alloc] init] forKey:event.classification];
        }
        
        

        NSMutableDictionary *eventSummary = [eventSummaries objectForKey:event.classification];

        int count = [[eventSummary objectForKey:@"count"] intValue] + 1;
        float average_confidence = ([[eventSummary objectForKey:@"average_confidence"] floatValue] * (count - 1) + event.p_value) / count;
        [eventSummary setObject:[[NSNumber alloc] initWithInt:count] forKey:@"count"];
        [eventSummary setObject:[[NSNumber alloc] initWithFloat:average_confidence] forKey:@"average_confidence"];
        
        [[eventsByClassification objectForKey:event.classification] addObject:event];
        
    }
    keys = [eventSummaries allKeys];
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
    // Return the number of rows in the section.
    return [eventSummaries count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier];
    }

    NSMutableDictionary *event = [eventSummaries objectForKey:keys[ [indexPath row] ]];

    [cell.textLabel setText:keys[[indexPath row]]];
    [cell.detailTextLabel
        setText:[NSString stringWithFormat:@"%u calls with %u%% average confidence",
                 [[event objectForKey:@"count"] intValue],
                 (int) ([[event objectForKey:@"average_confidence"] floatValue] * 100)
        ]
     ];
    
    // Configure the cell...
    
    return cell;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    BMLocalEventsListingViewController *eventsListingViewController = [[BMLocalEventsListingViewController alloc] init];
    [eventsListingViewController setEvents:[eventsByClassification objectForKey:[keys objectAtIndex:[indexPath row]]]];
    [[self navigationController] pushViewController:eventsListingViewController animated:YES];
}

@end
