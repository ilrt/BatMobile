#import "BMFileListingCell.h"

@implementation BMFileListingCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        _fileLabel = [[UILabel alloc] init];
        [_fileLabel setBackgroundColor:[UIColor colorWithRed:100 green: 50 blue:50 alpha:0.5]];
        [_fileLabel setOpaque:YES];
        [[self contentView] addSubview:_fileLabel];
        

        _localButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [[self contentView] addSubview:_localButton];
        
        _remoteButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];

        [[self contentView] addSubview:_remoteButton];
        

    }
    return self;
}



- (void)layoutSubviews {
    [super layoutSubviews];
    
    int localButtonWidth = 55;
    int remoteButtonWidth = 65;
    int spacing = 5;
    
    CGRect bounds = [[self contentView] bounds]; 
    if (super.editing) {
        _localButton.hidden = YES;
        _remoteButton.hidden = YES;
        CGRect fileLabelFrame = CGRectMake(4.0, 8.0, bounds.size.width - spacing, 30);
        [_fileLabel setFrame:fileLabelFrame];
    } else {
        _localButton.hidden = NO;
        _remoteButton.hidden = NO;
        CGRect fileLabelFrame = CGRectMake(4.0, 8.0, bounds.size.width - (spacing + localButtonWidth + spacing + remoteButtonWidth + spacing), 30);
        [_fileLabel setFrame:fileLabelFrame];
        
        [_localButton setTitle:@"Locally" forState:UIControlStateNormal];
        _localButton.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        _localButton.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
        _localButton.frame = CGRectMake(fileLabelFrame.size.width + spacing, 8.0, localButtonWidth, 30.0);
        
        [_remoteButton setTitle:@"Remotely" forState:UIControlStateNormal];
        _remoteButton.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        _remoteButton.titleLabel.lineBreakMode = NSLineBreakByWordWrapping;
        _remoteButton.frame = CGRectMake(fileLabelFrame.size.width + spacing + localButtonWidth + spacing, 8.0, remoteButtonWidth, 30.0);
        
        
    }
   
}

//-(void)setEditing:(BOOL)editing animated:(BOOL)animated {
//    [super setEditing:editing animated:animated];
//}




@end
