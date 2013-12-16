#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@interface BMRecordViewController : UIViewController <AVAudioRecorderDelegate>{
    AVAudioRecorder *audioRecorder;
    long startTime;
    NSTimer *timer;
}

@property (weak, nonatomic) IBOutlet UITextField *fileName;
@property (weak, nonatomic) IBOutlet UIButton *startRecording;
@property (weak, nonatomic) IBOutlet UIButton *stopRecording;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *recordingSpinner;
@property (weak, nonatomic) IBOutlet UIButton *analyzeRecording;
@property (weak, nonatomic) IBOutlet UIButton *nwButton;

@property (weak, nonatomic) IBOutlet UILabel *recordingText;
- (IBAction)startRecordingAction:(id)sender;
- (IBAction)stopRecordingAction:(id)sender;
- (IBAction)analyzeRecordingAction:(id)sender;
- (IBAction)newButtonAction:(id)sender;

@end
