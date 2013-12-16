
#include "IBFFT.h"

void IBFFT::setPlan(const int &fftSize, const window_type &window_t) {
    this->fftSize = fftSize;
    vDSP_Length log2n = log2f(fftSize);
    
    // Calculate the weights array.  This is a one-off operation for a given fftSize.
    fftSetup = vDSP_create_fftsetupD(log2n, FFT_RADIX2);

    // Use BioAcoustics Windows
    this->win.setup(fftSize, window_t);
    this->windowBlckman = win.getWindow();
    
    /* Allocate space for the samples in COMPLEX_SPLIT form:
     DSPSplitComplex (aka COMPLEX_SPLIT)
     Used to represent a complex number when the real and imaginary parts are stored in separate arrays.
     
     struct DSPSplitComplex {
     float *realp;
     float *imagp;
     };
     Because our samples are all real, imagp will be left empty but we still need to allocate it.
     */
    this->complexSplitSamples.realp = (double *) malloc(fftSize * sizeof(double));
	this->complexSplitSamples.imagp = (double *) malloc(fftSize * sizeof(double));
}

void IBFFT::process(const std::vector<float> &audioSamples, const int &start) {
    std::vector<double> samplesVector(this->fftSize);
    
    // Put the current samples into a seperate vector, and window it.
    for (int i = 0; i < this->fftSize; i++) {
        if ((start+i) >= audioSamples.size()) {
            samplesVector[i] = 0;
        } else {
            samplesVector[i]=audioSamples[start+i] * windowBlckman[i];
        }
    }
    //    printf("\n\n *************** START FFT *****************\n");
    //    for (int i = 0; i < samplesVector.size(); i++) {
    //        printf("samplesVector[%u] = %f \n", i, samplesVector[i]);
    //    }
    
    // samples is a C++ vector, but the DSP functions need C arrays.  To convert a C++ vector to a
    // C array, you simply take the memory address (i.e. pointer) of the 1st element.
    // http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
    double *samplesArray = &samplesVector[0];
    
    // Multiply the samples by the window generated in the constructor.  "Stride" is always 1, and we put the output
    // back into the samples array
    // vDSP_vmulD(samplesArray, 1, this->window, 1, samplesArray, 1, this->fftSize);
    
    
    /*  
     Reinterpret the real signal (samplesArray) as an interleaved-data complex
     vector and use vDSP_ctoz to move the data to a separated-data
     complex vector (complexSplitSamples).  This puts the even-indexed elements of samplesArray
     in complexSplitSamples and the odd-indexed elements in
     Observed.imagp.
     
     Note that we pass vDSP_ctoz two times samplesArray's normal stride,
     because ctoz skips through a complex vector from real to real,
     skipping imaginary elements.  Considering this as a stride of
     two real-sized elements rather than one complex element is a
     legacy use.
     
     In the destination array, a stride of one is used regardless of
     the source stride.  Since the destination is a buffer allocated
     just for this purpose, there is no point in replicating the
     source stride.
     http://developer.apple.com/library/mac/#samplecode/vDSPExamples/Listings/DemonstrateFFT_c.html
     */
    vDSP_ctozD((DOUBLE_COMPLEX *) samplesArray, 2, &this->complexSplitSamples, 1, this->fftSize/2);
    
    
    vDSP_Length log2n = log2f(this->fftSize);
    
    /*  
     Run the FFT.  It returns the output in the same array as the input.  Although the output is complex, and
     so would appear to require 2x as much space, because our inputs are real, the 2nd half of the FFT is just the
     complex conjugates, and so redundant information.  This means for speed we can pack the output into the input.
     
     Apple docs:
     "The result of the real FFT of AEvenOdd of dimension n is a  complex
     array of the dimension 2n, with a special format:
     
     {[DC,0],C[1],C[2],...,C[n/2],[NY,0],Cc[n/2],...,Cc[2],Cc[1]}
     
     where
     
     1. DC and NY are the dc and nyquist components (real valued),
     2. C is complex in a split representation,
     3. Cc is the complex conjugate of C in a split representation.
     
     For an n size real array A, the complex results require 2n
     spaces.  In order to fit the 2n size result into an n size  input and
     since the complex conjugates are duplicate information, the  real
     FFT produces its results as follows:
     
     {[DC,NY],C[1],C[2],...,C[n/2]}."
     
     https://developer.apple.com/library/mac/#documentation/Performance/Conceptual/vDSP_Programming_Guide/SampleCode/SampleCode.html
     */
    vDSP_fft_zripD(fftSetup, &this->complexSplitSamples, 1, log2n, FFT_FORWARD);
    
    double windowSum = win.getWindowSum();
    
    //calculate Coherent Power Gain
    double cpGain  = static_cast<double>(fftSize) / windowSum;
    this->normalise = cpGain / fftSize; //multiplier to normalise FFT mags by FFT size
    
    
    
    this->amplitudes.resize(this->fftSize / 2);
    // From above:  FFT output = {[DC,NY],C[1],C[2],...,C[n/2]}.
    // We aren't interested in the DC or Nyquist values, so skip them (hence i=1 in loop below)
    // and calculate the amplitudes of the FFT output bins
    this->amplitudes[0]=0;
    for(int i=1; i < this->fftSize / 2; i++) {
        this->amplitudes[i] = sqrt(complexSplitSamples.realp[i] * complexSplitSamples.realp[i] +
                                   complexSplitSamples.imagp[i] * complexSplitSamples.imagp[i]) * normalise / 2;
    }
    
}

std::vector<double> IBFFT::getSpectrum() {
    return this->amplitudes;
}


