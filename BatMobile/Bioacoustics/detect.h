/*
 * Copyright (c) 2012, Chris Scott, C.D.Scott@leeds.ac.uk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DETECT_H
#define DETECT_H

#include "statsvec.h"
#include "float.h"
#include "fft.h"
#include "firstorderfilter.h"
#include "filter.h"
#include "utilities.h"
#include <vector>
#include <deque>
#include <iterator>
#include <cmath>
#include <numeric>

class Detect {

public:
    Detect(const int &_fftSize, const double &overlap,
           window_type win, const double &_alpha);
    ~Detect();
    deque<Detection> process(const Audio &audio, const double &snrThresh,
                             const double &cutoff);

private:
    double getSNR(const std::vector<double> &noiseReducedSpectrum,
                  const std::vector<double> &noiseEstimate);

    FFT fft;
    int fftSize, stepSize, binN;
    double alpha, oneMinusAlpha;
};

inline Detect::Detect(const int &_fftSize, const double &overlap,
                      window_type win, const double &_alpha) {
    fftSize = _fftSize;
    stepSize = fftSize*(1.0-overlap);
    fft.setPlan(fftSize, win);
    binN = fftSize*0.5;

    alpha = _alpha;
    oneMinusAlpha = 1.0 - alpha;
}

inline Detect::~Detect() {
    //
}

std::deque<Detection> Detect::process(const Audio &audio, const double &snrThresh, const double &cutoff) {
    std::deque<Detection> detections;
    StatsVec backgroundNoiseUnfiltered(binN);
    StatsVec backgroundNoise(binN);
    
    std::vector<FIR> average(binN);


    Filter filter;

    filter.initialise(audio.samplerate, fftSize);

    int frames = 1 + floor( static_cast<double>( audio.samples.size() - fftSize ) / stepSize );
    int seekPoint = -binN;
    seekPoint += 500;// to avoid D500x header
    double expThresh = -DBL_MAX;

    // First frame
    // check if recording is silent
    double ss = 0;
    while(ss < 0.00001 && seekPoint < (int)audio.samples.size()) {
        for (int j=0; j<fftSize; ++j) {
            if ((seekPoint+j<0) || (seekPoint+j) >= (int)audio.samples.size()) {
                //
            }
            else {
                ss += (audio.samples[seekPoint+j]*audio.samples[seekPoint+j]);
            }
        }
        seekPoint += stepSize;
    }


    fft.process(audio.samples, seekPoint);
    std::vector<double> spectrum = fft.getSpectrum();

    spectrum = filter.highPass(spectrum, cutoff);
    for(unsigned int j=0; j<spectrum.size(); ++j) {
        average[j].initialise(spectrum[j]);
    }
    std::vector<double> background;
    
    backgroundNoise.push_back(spectrum);
    
    double localSNR;
    bool bTrigger = false;
    Detection d;

    // ms per analysis step
    double step_ms = 1000.0*stepSize / (double)audio.samplerate;
    double duration = 0;

    // STFT
    std::vector<double> filteredSpectrum;
    for(int i=0; i<frames; ++i) {

        // check if recording is silent
        double ss = 0;//sum square
        for (int j=0; j<fftSize; ++j) {
            if ((seekPoint+j<0) || (seekPoint+j) >= (int)audio.samples.size()) {
                //
            }
            else {
                ss += (audio.samples[seekPoint+j]*audio.samples[seekPoint+j]);
            }
        }
        if(ss < 0.00001) {
            seekPoint += stepSize;// advance
            continue;
        }
        //printf("ss: %.9f\n", ss);

        // Send audio samples to FFT
        fft.process(audio.samples, seekPoint);
        seekPoint += stepSize;// advance STFT

        // FFT spectrum
        spectrum = fft.getSpectrum();
        filteredSpectrum = filter.highPass(spectrum,cutoff);

        // averaging first order filter
        for(unsigned int j=0; j<spectrum.size(); ++j) {
            filteredSpectrum[j] = average[j].push_back(filteredSpectrum[j]);
        }

        // Background noise spectrum
        background = backgroundNoise.getMean();
        // Stop measuring background noise during a call,
        // unless duration gets very long in which case it is probably just noise
        if(!bTrigger || duration > 80.0) {
            backgroundNoise.push_back(filteredSpectrum);
            backgroundNoiseUnfiltered.push_back(spectrum);
        }

        // Calculate local SNR
        localSNR = getSNR(filteredSpectrum, background);

        // exponential decay threshold/echo masker
        expThresh = std::max(localSNR, alpha*expThresh + oneMinusAlpha*localSNR);

        if(localSNR >= snrThresh && localSNR >= expThresh && !bTrigger) {
            // trigger start of detected event
            bTrigger = true;
        }

        if(bTrigger) {
            duration += step_ms;
            // want to record highest SNR during trigger
            if(localSNR > d.SNR) {
                d.sampleLocation = seekPoint - 2*stepSize;
                d.SNR = localSNR;
            }

            if(localSNR < snrThresh) {
                // end of detection
                duration = 0;
                bTrigger = false;
                // store detection
                d.backgroundNoiseSpectrum = backgroundNoiseUnfiltered.getMean();
                detections.push_back(d);
                // reset snr
                d.SNR = ZEROLOG;
            }
        }
    }

    return detections;
}

// SNR = (signal plus noise) / noise;
inline double Detect::getSNR(const std::vector<double> &spectrum,
                             const std::vector<double> &noiseEstimate) {
    std::pair<double, double> result(0,0);
    for(unsigned int i=0; i<spectrum.size(); ++i) {
        if(spectrum[i] > noiseEstimate[i]) {
            result.first += spectrum[i];// signal and noise
            result.second += noiseEstimate[i];// noise
        }
    }
    return to_dB(result.first/noZero(result.second));
}

#endif // DETECT_H
