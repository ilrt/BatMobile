//
//  BMAboutViewController.h
//  batmobile
//
//  Created by Mark van Rossum on 10/07/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BMAboutViewController : UIViewController <UIWebViewDelegate> {
    UIWebView *webView;

}
@property (nonatomic) IBOutlet UIWebView *webView;

@end
