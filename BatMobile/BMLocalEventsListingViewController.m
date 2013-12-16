#import "BMLocalEventsListingViewController.h"
#import "BMEvent.h"
#import "BMLocalEventDetailViewController.h"

@implementation BMLocalEventsListingViewController
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
     self.title = @"Call Details";
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
    BMEvent *event = [events objectAtIndex:index];
    
    [[cell textLabel] setText:[event classification] ];

    NSString *detailTextLabel = [NSString stringWithFormat:@"Confidence: %.4f", [event p_value]];
    [[cell detailTextLabel] setText: detailTextLabel];

    return cell;
}



- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    BMLocalEventDetailViewController *detailViewController = [[BMLocalEventDetailViewController alloc] initWithNibName:@"BMLocalEventDetailViewController" bundle:nil];
    BMEvent *event = [events objectAtIndex:[indexPath row]];
    [detailViewController setEvent:event];
    [self.navigationController pushViewController:detailViewController animated:YES];

}


@end
