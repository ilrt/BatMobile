//
//  BMFileListingCell.h
//  batmobile
//
//  Created by Mark van Rossum on 12/08/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMFileListingCell : UITableViewCell

@property (nonatomic, retain) UILabel *fileLabel;
@property (nonatomic, retain) UIButton  *localButton;
@property (nonatomic, retain) UIButton  *remoteButton;

@end
