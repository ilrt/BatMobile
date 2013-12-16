//
//  IBDetect.h
//  batmobile
//
//  Created by Mark van Rossum on 30/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#ifndef __batmobile__IBDetect__
#define __batmobile__IBDetect__

#import <iostream>
#import <vector>
#import "IBAudio.h"
#import "IBFFT.h"
#import "IBTrace.h"
#import "IBFFTSpectrum.h"

using namespace std;
class IBDetect {
    IBAudio audio;
    vector<IBAudio> calls;
    float lowerFreqCutoff;
    float higherFreqCutoff;
    float threshold;
    int runningAvLength;
    int minSampleSize;
    int fftSize;
    int hopSize;

public:
    IBDetect(IBAudio audio);
    vector<IBAudio> getCalls();
};
#endif /* defined(__batmobile__IBDetect__) */
