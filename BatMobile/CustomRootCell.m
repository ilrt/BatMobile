#import "CustomRootCell.h"

@implementation CustomRootCell

@synthesize nameLabel, imageView;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {

        nameLabel = [[UILabel alloc] initWithFrame:CGRectZero];
        [nameLabel setFont:[UIFont boldSystemFontOfSize:20.0f]];
        [[self contentView] addSubview:nameLabel];
        
        imageView = [[UIImageView alloc] initWithFrame:CGRectZero];
        [[self contentView] addSubview:imageView];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    float inset = 0;
    
    CGRect bounds = [[self contentView] bounds];
    
    CGRect imageFrame = CGRectMake(4.0, 1.0, 40.0, 40.0);
    [imageView setFrame:imageFrame];
    
    CGRect nameFrame = CGRectMake(imageFrame.size.width + imageFrame.origin.x + 10.0, inset, 
                                  bounds.size.width - imageFrame.size.width, bounds.size.height - inset * 2.0);
    [nameLabel setFrame:nameFrame];
    
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
}


@end
