#include "IBFeatures.h"
/**
 * Class to calculate various features of the audio.  Max frequency, etc.  Constructor is supplied
 * with the raw audio, then the other methods return the features.
 */

IBFeatures::IBFeatures(IBAudio audio) {
    IBFFT fft;
    this->fftSize = 256;
    this->hopSize = 32;
    
    window_type  window_t = BlackmanHarris7;
    fft.setPlan(fftSize, window_t);
    
    int binWidth = audio.sampleRate / fftSize;
    int sampleRate = audio.sampleRate;
    
    IBTrace trace;
    
    IBFFTSpectrum fftSpectrum;
    fftSpectrum.fftTimeLength = fftSize / sampleRate;
    fftSpectrum.binFreq = sampleRate / fftSize;
    
    for (int sampleNum = 0; sampleNum < audio.samples.size(); sampleNum += hopSize) {
        fft.process(audio.samples, sampleNum);
        std::vector<double> spectrum = fft.getSpectrum();
        
        fftSpectrum.spectrum.push_back(spectrum);
        
        float maxBinValue = 0;
        int maxBin = 0;
        
        for (int fftBin = 0; fftBin < spectrum.size(); fftBin++) {
            if (spectrum[fftBin] > maxBinValue) {
                maxBinValue = spectrum[fftBin];
                maxBin = fftBin;
            }
        }
        trace.add_entry((float)sampleNum / audio.sampleRate, maxBin * binWidth);
    }
    
    this->audio = audio;
    this->spectrum = fftSpectrum;
    this->trace = trace;
}


float IBFeatures::FMax() {
    this->maxFreq = 0;
    for (int i=0; i<trace.asArray.size(); i++) {
        if (trace.asArray[i]["freq"] > this->maxFreq) {
            this->maxFreq = trace.asArray[i]["freq"];
            this->timeAtMaxFreq = trace.asArray[i]["time"];
        }
    }
    return this->maxFreq;
}

float IBFeatures::timeAtFMax() {
    if (this->timeAtMaxFreq == -1) this->FMax();
    return this->timeAtMaxFreq;
}


float IBFeatures::FMin() {
    this->minFreq = 1e99;
    for (int i=0; i<trace.asArray.size(); i++) {
        if (trace.asArray[i]["freq"] < this->minFreq) {
            this->minFreq = trace.asArray[i]["freq"];
            this->timeAtMinFreq = trace.asArray[i]["time"];
        }
    }
    return this->minFreq;
}

float IBFeatures::BW() {
    return (this->FMax() - this->FMin());
}

float IBFeatures::Duration() {
    return (float) this->audio.sampleCount / this->audio.sampleRate;
}

float IBFeatures::FC() {
    float startTime = ((float) this->audio.sampleCount / this->audio.sampleRate) * (float)(1-0.4);
    float lowestSlope = (float)1e6;
    float freqAtLowestSlope = 0;
    float prevFreq = 0;
    
    for (int i = 0; i < trace.asArray.size(); i++) {
        float freq = trace.asArray[i]["freq"];
        float time = trace.asArray[i]["time"];
        if (time > startTime) {
            if (abs(freq-prevFreq) < lowestSlope) {
                lowestSlope= abs(freq-prevFreq);
                freqAtLowestSlope = freq;
            };
        };
    };
    return freqAtLowestSlope;
};

float IBFeatures::FCtr() {
    float halfTime = ((float)this->audio.sampleCount / this->audio.sampleRate) / 2;
    float halfTimeFreq = 0;
    for (int i = 0; i < trace.asArray.size(); i++) {
        float time = trace.asArray[i]["time"];
        if (time > halfTime) {
            // We've gone past it, so use the previous value;
            halfTimeFreq = trace.asArray[i-1]["freq"];
            break;
        }
    }
    return halfTimeFreq;
    
}

