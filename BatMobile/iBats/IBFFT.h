#ifndef IBFFT_H
#define IBFFT_H

#include <iostream>
#include <vector>
#include <Accelerate/Accelerate.h>
#include "utilities.h"
#include "fftwindows.h"     // BioAcoustics windows

class IBFFT {
public:
    void setPlan(const int &fftSize, const window_type &window_t);
    void process(const std::vector<float> &audioSamples, const int &start);
    std::vector<double> getSpectrum();
    
private:
    int fftSize;
    
    /*
     DSPSplitComplex (aka COMPLEX_SPLIT)
     Used to represent a complex number when the real and imaginary parts are stored in separate arrays.
     
     struct DSPSplitComplex {
     float *realp;
     float *imagp;
     };
     */
    DOUBLE_COMPLEX_SPLIT complexSplitSamples;
    
    FFTSetupD fftSetup;
    double *window;
    std::vector<double> spectrum;
    std::vector<double> amplitudes;
    
    std::vector<double> windowBlckman;
    double windowSum;
    double normalise;
    FFT_Window win; // Use the Windows from BioAcoustics
};


#endif





