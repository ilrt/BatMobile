#import "BMAnalyzeViewController.h"
#import "BMFileListingCell.h"
#import "BMConfig.h"
#import "Bioacoustics/bioacoustics.h"
#import "BMEvent.h"
#import "MBProgressHUD.h"
#import "BMLocalEventsListingViewController.h"
#import "BMRemoteEventsListingViewController.h"
#import "IBMain.h"
#import "BMLocalEventsSummaryViewController.h"

@interface BMAnalyzeViewController ()

@end

@implementation BMAnalyzeViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        [self loadFiles];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor colorWithRed:TABLE_BG_RED/255.0 green:TABLE_BG_GREEN/255.0 blue:TABLE_BG_BLUE/255.0 alpha:1.0];
    _tableView.opaque = NO;
    _tableView.backgroundView = nil;
    _headerLabel.backgroundColor = [UIColor colorWithRed:TABLE_BG_RED/255.0 green:TABLE_BG_GREEN/255.0 blue:TABLE_BG_BLUE/255.0 alpha:1.0];

    self.title = @"Analyse";
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setEditing:(BOOL)editing animated:(BOOL)animated {
    [super setEditing:editing animated:animated];
    [_tableView setEditing:editing animated:animated];
}

-(void)loadFiles {
    NSString *resourcePath = [[NSBundle mainBundle] bundlePath];
    NSError *error;
    examplePath = [resourcePath stringByAppendingPathComponent:@"calls"];
    exampleFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:examplePath error:&error];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    userPath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    userFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:userPath error:&error];
    
}


-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (section == 0) {
       return [exampleFiles count];
    } else {
        return [userFiles count];
    }
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    if (section ==0) {
        return @"Example Files";
    } else {
        return @"Your Files";
    }
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

-(UITableViewCell *) tableView : (UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    BMFileListingCell *cell = [tableView dequeueReusableCellWithIdentifier:@"BMFileListingCell"];
    if (!cell) {
        cell = [[BMFileListingCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"BMFileListingCell" ];
        [cell.localButton addTarget:self action:@selector(localButtonPress:) forControlEvents:UIControlEventTouchUpInside];
        [cell.remoteButton addTarget:self action:@selector(remoteButtonPress:) forControlEvents:UIControlEventTouchUpInside];
    }
    cell.localButton.tag = 100 * [indexPath section] + [indexPath row];
    cell.remoteButton.tag = 100 * [indexPath section] + [indexPath row];
    
    NSString *text;
    
    if ([indexPath section] == 0) {
        text = [exampleFiles objectAtIndex:[indexPath row]];
    } else {
        text = [userFiles objectAtIndex:[indexPath row]];
    }
    
    [[cell fileLabel] setText:text];
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return ([indexPath section] > 0);
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete && [indexPath section] > 0) {
        
        // Order matters - http://stackoverflow.com/a/9762599/1116197
        NSString *path = userPath;
        NSString *fileName = [path stringByAppendingPathComponent: [userFiles objectAtIndex:[indexPath row]]];
        NSError *error;
        [[NSFileManager defaultManager] removeItemAtPath:fileName error:&error];
        [self loadFiles];
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
        
        // Allow animation to complete before reloading data
        [self.tableView performSelector:@selector(reloadData) withObject:nil afterDelay:0.2];
    }
    
}



-(void) localButtonPress:(id)sender {
    UIButton *button = (UIButton *)sender;
    int section = button.tag / 100;
    int row = button.tag % 100;
    NSString *path;
    NSString *fileName;
    if (section == 0) {
        path = examplePath;
        fileName = [path stringByAppendingPathComponent: [exampleFiles objectAtIndex:row]];
    } else {
        path = userPath;
        fileName = [path stringByAppendingPathComponent: [userFiles objectAtIndex:row]];
    }
    

    NSLog(@"Analyzing: %@", fileName);
    MBProgressHUD * hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES ];
    hud.labelText = @"Analyzing";
    _events = [[NSMutableArray alloc] init];
    
    dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
        NSString *resourcePath = [[NSBundle mainBundle] bundlePath];
        NSString *modelPath = [resourcePath stringByAppendingPathComponent:@"models"];
  

        Bioacoustics bioacoustics;
        deque<Event> results = bioacoustics.analyse([fileName UTF8String], [modelPath UTF8String]);
        
        for (int i = 0; i < results.size(); i++) {
            BMEvent *event = [[BMEvent alloc] init];
            
            NSString *classification = [NSString stringWithUTF8String:results[i].classification.c_str()];
            
            [event setClassification:classification];
            [event setP_value:results[i].p_value];
            [event setStartSample:results[i].startSample];
            [event setEndSample:results[i].endSample];
            [event setSampleRate:results[i].sampleRate];
            [event setTE:results[i].TE];
            [_events addObject:event];
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [MBProgressHUD hideHUDForView:self.view animated:YES];

            BMLocalEventsSummaryViewController *eventsListingViewController = [[BMLocalEventsSummaryViewController alloc] init];
            
            [eventsListingViewController setEvents:_events];
            
            [[self navigationController] pushViewController:eventsListingViewController animated:YES];
        });
    });
    
}


-(void) remoteButtonPress:(id)sender {
    UIButton *button = (UIButton *)sender;
    int section = button.tag / 100;
    int row = button.tag % 100;
    NSString *path;
    NSString *fileName;
    if (section == 0) {
        path = examplePath;
        fileName = [path stringByAppendingPathComponent: [exampleFiles objectAtIndex:row]];
    } else {
        path = userPath;
        fileName = [path stringByAppendingPathComponent: [userFiles objectAtIndex:row]];
    }
    
    MBProgressHUD * hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES ];
    hud.labelText = @"Analyzing";
    //    events = [[NSMutableArray alloc] init];
    
    dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
        NSLog(@"handing to iBats");
        IBMain *ibats = [[IBMain alloc] init];
        
        NSMutableArray *events = [ibats analyse:fileName];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [MBProgressHUD hideHUDForView:self.view animated:YES];
            BMRemoteEventsListingViewController *eventsListingViewController = [[BMRemoteEventsListingViewController alloc] init];
            
            [eventsListingViewController setEvents:events];
            
            [[self navigationController] pushViewController:eventsListingViewController animated:YES];
        });
        
        
    });
}



@end
