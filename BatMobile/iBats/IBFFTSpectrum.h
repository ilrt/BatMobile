//
//  IBFFTSpectrum.h
//  batmobile
//
//  Created by Mark van Rossum on 26/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#ifndef __batmobile__IBFFTSpectrum__
#define __batmobile__IBFFTSpectrum__

#include <vector>
#include <iostream>
struct  IBFFTSpectrum {

    float binFreq;
    float fftTimeLength;
    std::vector<std::vector<double>> spectrum;
    
};
#endif /* defined(__batmobile__IBFFTSpectrum__) */
