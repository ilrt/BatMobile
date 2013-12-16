#ifndef __batmobile__IBFeatures__
#define __batmobile__IBFeatures__

#include <iostream>
#include <unordered_map>
#include <string>
#include <stdlib.h>

#include "IBFFTSpectrum.h"
#include "IBAudio.h"
#include "IBTrace.h"
#include "IBFFT.h"


class IBFeatures {
    IBAudio audio;
    IBFFTSpectrum spectrum;
    IBTrace trace;
    
    float maxFreq;
    float timeAtMaxFreq = -1;
    float minFreq;
    float timeAtMinFreq = -1;
    int fftSize;
    int hopSize;

public:
    IBFeatures(IBAudio audio);
    float FMax();
    float FMin();
    float timeAtFMax();
    float BW();
    float Duration();
    float FC();
    float FCtr();
};

#endif /* defined(__batmobile__IBFeatures__) */
