#import "BMBrowseDBListingViewController.h"
#import "CHCSVParser.h"
#import "GRMustache/include/GRMustache.h"
#import "BMConfig.h"
#import "CustomRootCell.h"

@interface BMBrowseDBListingViewController ()
@end

@implementation BMBrowseDBListingViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    NSString *CSVfile = [[NSBundle mainBundle] bundlePath];
    CSVfile = [CSVfile stringByAppendingString:@"/batDB.csv"];
    _batDB = [NSArray arrayWithContentsOfCSVFile:CSVfile options:CHCSVParserOptionsSanitizesFields];
    NSLog(@"%@", _batDB);
}


- (void)viewWillAppear:(BOOL)animated {
    // Update the view with current data before it is displayed.
    [super viewWillAppear:animated];
    self.tableView.backgroundColor = [UIColor colorWithRed:TABLE_BG_RED/255.0 green:TABLE_BG_GREEN/255.0 blue:TABLE_BG_BLUE/255.0 alpha:1.0];
    self.tableView.opaque = NO;
    self.tableView.backgroundView = nil;
    self.title = @"Bat Database";

    
    // Scroll the table view to the top before it appears
    [self.tableView reloadData];
    [self.tableView setContentOffset:CGPointZero animated:NO];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [_batDB count];
}

// @implementation
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return @"Bat Database";
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    static NSString *cellIdentifier = @"BDBCell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];

    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    }
   
    NSArray *entry = [_batDB objectAtIndex:[indexPath row]];
    NSString *title = [entry objectAtIndex:0];
    cell.textLabel.text = title;
    [cell.imageView setImage:[UIImage imageNamed:[entry objectAtIndex:1]]];
     cell.imageView.contentMode = UIViewContentModeScaleToFill;
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    [[cell textLabel] setText:title];
    
    // Configure the cell...
    
    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    UIWebView *webView = [[UIWebView alloc] init];
    
    NSArray *entry = [_batDB objectAtIndex:[indexPath row]];

    NSString *resources = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resources stringByAppendingPathComponent:[entry objectAtIndex:2]];
    
    NSError *error = nil;
    NSString *resourceFileContent = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:&error];
    
    if (error != nil) {
        @throw [NSException exceptionWithName:[error domain] reason:[error localizedDescription] userInfo:[error userInfo]];
    }
    
    NSURL *baseURL = [NSURL fileURLWithPath:resources];
    [webView loadHTMLString:resourceFileContent baseURL:baseURL];

    
    UIViewController *viewController = [[UIViewController alloc] init];
    viewController.view = webView;
    viewController.title = [entry objectAtIndex:0];
    [[self navigationController] pushViewController:viewController animated:YES];

    
    
}

@end
