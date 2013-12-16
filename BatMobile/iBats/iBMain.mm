#import "IBMain.h"
#import "IBSndFile.h"
#import "IBFeatures.h"
#import "IBDetect.h"

@implementation IBMain

/**
 * This class manages other classes that ultimately:
 *  - Read in the raw audio samples from the file
 *  - Detect (and extract) call from the audio
 *  - Analyses the audio to extract audio "features"
 *  - Returns these features to the caller
 *
 * It's inefficient in that both the detection and extraction classes calculate FFTs for the same samples.
 * This could be done once and passed around, at the expense of making the coupling more complex.
 */

- (NSMutableArray*) analyse:(NSString*) file {
   
    IBSndFile sndfile;          // Sound library to read the audio - essentially wraps Core Audio.
	sndfile.read([file UTF8String]);

    IBAudio audio;              // Object to hold raw audio samples and metadata
    audio = sndfile.getAudio();


    IBDetect detect(audio);
    vector<IBAudio> calls = detect.getCalls();
    NSLog(@"Found %lu calls", calls.size());
    
    
    NSMutableArray *callFeaturesList = [[NSMutableArray alloc] init];
    for (int i = 0; i < calls.size(); i++) {
        IBFeatures features(calls[i]);

        NSMutableDictionary *featuresList = [[NSMutableDictionary alloc] init];
        [featuresList setObject:[NSNumber numberWithFloat:calls[i].startTime] forKey:@"starttime"];
        [featuresList setObject:[NSNumber numberWithFloat:features.FMax()] forKey:@"fmax"];
        [featuresList setObject:[NSNumber numberWithFloat:features.FMin()] forKey:@"fmin"];
        [featuresList setObject:[NSNumber numberWithFloat:features.BW()] forKey:@"bandwidth"];
        [featuresList setObject:[NSNumber numberWithFloat:features.Duration()] forKey:@"duration"];
        [featuresList setObject:[NSNumber numberWithFloat:features.FCtr()] forKey:@"fctr"];
        [featuresList setObject:[NSNumber numberWithFloat:features.FC()] forKey:@"fc"];
        [callFeaturesList addObject: featuresList];
    }
    
    return callFeaturesList;
}

@end
