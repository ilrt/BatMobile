/**
 * Class to do a very simplistic call detection from the raw audio.  It calculates FFTs and for each one:
 * Calculates the average energy between a defined upper and lower frequency range.
 * If the energy is over the defined threshold, it assumes it's in a call.
 * There is some smoothing of the averages to avoid spurious detections on the edges of calls.
 */


#include "IBDetect.h"
IBDetect::IBDetect(IBAudio audio) {
    this->audio = audio;                // Audio supplied from caller

    // Lower frequency threshold.  Frequencies below this are ignored when calculating the average energy
    this->lowerFreqCutoff = 30000 / audio.timeExpansionFactor;

    // Upper frequency cutoff threshold. Frequencies above this are ignored when calculating the average energy
    this->higherFreqCutoff = 150000 / audio.timeExpansionFactor;

    this->threshold = 20;               // Average energy threshold. Greater energy than this is considered to be a call.
    this->minSampleSize = 512;          // Minimum raw number of samples a call must be.
    this->fftSize = 256;                // The size of the FFT. 
    this->hopSize = 32;                 // The number of samples the FFT will advance through the audio by.
};

vector<IBAudio> IBDetect::getCalls() {
    IBFFT fft;
    window_type  window_t = BlackmanHarris7;
    fft.setPlan(this->fftSize, window_t);
    
    IBFFTSpectrum fftSpectrum;
    fftSpectrum.fftTimeLength = (float)fftSize / audio.sampleRate;
    fftSpectrum.binFreq = (float)audio.sampleRate / fftSize;
    
    int startBin = (int) this->lowerFreqCutoff / fftSpectrum.binFreq;
    
    // The highest freq we can detect is the Nyqust freq, = sampleRate / 2.
    if (this->higherFreqCutoff > audio.sampleRate / 2) this->higherFreqCutoff = audio.sampleRate / 2;
    int endBin = (int) this->higherFreqCutoff / fftSpectrum.binFreq;
  
    bool inCall = false;

    vector<float> call;
    
    // We loop through all the samples, and calculate the FFT spectrum.
    // The average bin energy between the high and low cutoffs is then calculated.
    // If the average is over "threshold" it's considered to be a call and the samples are saved.
    // Once the average drops below "threshold", if the number of samples was over "minSampleSize" the call is saved into the
    // calls vector, which is ultimately returned to the caller.
    for (int sampleNum = 0; sampleNum < audio.samples.size(); sampleNum += this->hopSize) {
        fft.process(audio.samples, sampleNum);
        std::vector<double> spectrum = fft.getSpectrum();
        
        // Calculate average energy between the bin limits
        float sum = 0;
        for (int bin = startBin; bin <= endBin; bin++) {
            sum += spectrum[bin];
        }
        float averageLevel = sum / (endBin - startBin);

        averageLevel *= 1e5;
        if (averageLevel > this->threshold) {
            // We're in a call, so save the samples into 'call'
            inCall = true;
            for (int sample = sampleNum; sample < sampleNum + hopSize; sample++) {
                call.push_back(audio.samples[sample]);
            }
        } else {
            // We're not in a call...
            if (inCall == true) { // But if we were...
                if (call.size() > this->minSampleSize) { // ... and the call was longer than the threshold ...
                    // ... then save the call
                    IBAudio callAudio;
                    callAudio.samples = call;
                    callAudio.sampleRate = audio.sampleRate;
                    callAudio.sampleCount = call.size();
                    callAudio.startTime = (float)(sampleNum + hopSize - call.size()) / audio.sampleRate ;
                    this->calls.push_back(callAudio);
                }
            }
            inCall = false;
            call.clear();
        }
    }
    return this->calls;
};

