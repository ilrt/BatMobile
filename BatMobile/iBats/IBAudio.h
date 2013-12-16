//
//  IBAudio.h
//  batmobile
//
//  Created by Mark van Rossum on 25/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#ifndef __batmobile__IBSamples__
#define __batmobile__IBSamples__

#include <iostream>
#include <vector>

using namespace std;

/**
 * Class used to hold raw audio data.
 */

struct IBAudio {
    
    vector<float> samples;  // Raw sample data.
    int sampleRate;         // sampleRate (kHz)
    int sampleCount;        // Number of samples
    float startTime;
    int timeExpansionFactor;    //
};


#endif /* defined(__batmobile__IBSamples__) */
