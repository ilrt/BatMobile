#import "BMRecordViewController.h"
#import "UIImage+animatedGIF.h"
#import "BMConfig.h"

@interface BMRecordViewController ()

@end

@implementation BMRecordViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {

    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.view setBackgroundColor:[UIColor colorWithRed:TABLE_BG_RED/255.0 green:TABLE_BG_GREEN/255.0 blue:TABLE_BG_BLUE/255.0 alpha:1.0]];
    [self setTitle:@"Record Audio"];
    
    NSDateFormatter *format = [[NSDateFormatter alloc] init];
    [format setDateFormat:@"yyyy-MM-dd-HHmmss"];
    
    NSDate *now = [[NSDate alloc] init];
    
    NSString *dateString = [format stringFromDate:now];
    
    [_fileName setText:[dateString stringByAppendingString:@".caf"]];

    [_startRecording setHidden: NO];
    [_stopRecording setHidden: YES];
    [_nwButton setHidden:YES];
    
    [_recordingSpinner setHidden:YES];
    [_recordingText setText:@"stopped"];
    
       
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)startRecordingAction:(id)sender {
   
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *userPath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    NSString *soundFilePath = [userPath stringByAppendingPathComponent:[_fileName text]];
    
    NSURL *soundFileURL = [NSURL fileURLWithPath:soundFilePath];
    NSDictionary *recordSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithFloat: 44100.0],AVSampleRateKey,
                                    [NSNumber numberWithInt: kAudioFormatLinearPCM],AVFormatIDKey,// kAudioFormatLinearPCM
                                    [NSNumber numberWithInt:16],AVLinearPCMBitDepthKey,
                                    [NSNumber numberWithInt: 2], AVNumberOfChannelsKey,
                                    [NSNumber numberWithBool:NO],AVLinearPCMIsBigEndianKey,
                                    [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey,
                                    [NSNumber numberWithInt: AVAudioQualityMedium],AVEncoderAudioQualityKey,nil];
    NSError *error = nil;
    
    audioRecorder = [[AVAudioRecorder alloc]
                     initWithURL:soundFileURL
                     settings:recordSettings
                     error:&error];
    
    if (error) {
        NSLog(@"error: %@", [error localizedDescription]);
    } else {
        [audioRecorder prepareToRecord];
        _fileName.enabled = NO;
        [_recordingText setText:@"recording"];
    }

    if (!audioRecorder.recording) {
        startTime = [[[NSDate alloc] init ] timeIntervalSince1970];

        timer = [NSTimer scheduledTimerWithTimeInterval:1
            target:self
            selector:@selector(tick:)
            userInfo:nil
            repeats:YES
         ];

        [_startRecording setHidden: YES];
        [_stopRecording setHidden: NO];
        [_recordingSpinner setHidden:NO];
        [_recordingText setHidden:NO];
        [audioRecorder record];
    }
}

- (void) tick:(NSTimer *) timer {
    int time = [[[NSDate alloc] init ] timeIntervalSince1970] - startTime;
    int hours = floor(time / 3600.0);
    int min = floor( (time % 3600) / 60);
    int sec = floor( ((time % 3600) % 60));
    
    NSString *string = @"recording...";
    NSString *timeString = [NSString stringWithFormat:@"%uhrs %umin %usec",hours, min, sec];
    string = [string stringByAppendingString:timeString];
    [_recordingText setText:string];
   
}


- (IBAction)stopRecordingAction:(id)sender {
    [audioRecorder stop];
    [timer invalidate];
    timer = nil;
    
    [_recordingText setText:@"saved"];
    [_recordingSpinner setHidden:YES];
    [_startRecording setHidden: YES];
    [_stopRecording setHidden: YES];
    [_nwButton setHidden:NO];
}

- (IBAction)analyzeRecordingAction:(id)sender {
    
}

- (IBAction)newButtonAction:(id)sender {
    [self viewDidLoad];
 
}
@end
