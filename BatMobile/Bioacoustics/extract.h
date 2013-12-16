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

#ifndef EXTRACT_H
#define EXTRACT_H

#include "fft.h"
#include "filter.h"
#include "kalman.h"
#include "float.h"
#include "utilities.h"
#include <vector>
#include <deque>
#include <cmath>
#include <numeric>

class Extract {

public:
    Extract(const int &fftSize, const double &overlap, window_type win,
            const double &startThresh, const double &endThresh,
            const double &angle, const double &smoothing,
            const double &snrThresh, const double &Qk, const double &Rk);
    ~Extract();
    Event process(const Audio &audio, Detection &detection,
                  const std::string &filepath, const double &cutoff);

private:

    void forwardExtract(const std::vector<float> &audioSamples,
                        Detection &detection, Event &event);
    void backExtract(const std::vector<float> &audioSamples,
                     Detection &detection, Event &event);
    int isStart(Event &event, const double &ampDiff, const double &angle,
                const double &localSNR);
    int isEnd(Event &event, const double &ampDiff, const double &angle,
              const double &localSNR);
    void storeBack(Event &event,const double &sig, const double &noise,
                   const std::vector<double> &spectrum,
                   const std::vector<double> &masked);
    void storeFront(Event &event,const double &sig, const double &noise,
                    const std::vector<double> &spectrum,
                    const std::vector<double> &masked);
    std::pair<double, double> SNR(const std::vector<double> &spectrum,
                             const std::vector<double> &noiseEstimate);
    std::pair<double, int> getPeak(const std::vector<double> &x);
    std::vector<double> mask(const int &peak, const std::vector<double> &x);
    double getAngularDifference(const std::deque<double> &x);

    Filter filter;
    double cutoff;
    Kalman kalman;
    double Qk, Rk;

    FFT fft;
    int fftSize, stepSize, binN, seekPoint;

    double startThresh, endThresh, SNR_thresh, angleThresh;
    int fundBin, harmBin;
    double energy, centroidBin, kalmanBin;
    double bin_to_freq;
    double gain;
    std::deque<double> track;
};

/******************** IMPLEMENTATION ************************/
inline Extract::Extract(const int &fftSize, const double &overlap, window_type win,
                        const double &startThresh, const double &endThresh,
                        const double &angle, const double &smoothing,
                        const double &snrThresh, const double &Qk, const double &Rk) {
    // user settings:
    this->fftSize = fftSize;
    stepSize = fftSize * (1.0-overlap);
    fft.setPlan(fftSize, win);
    binN = fftSize*0.5;
    this->startThresh = startThresh;
    this->endThresh = endThresh;
    SNR_thresh = snrThresh;
    angleThresh = angle;
    gain = smoothing;
    // these settings  are fixed for call extraction:
    this->Qk = 0.00001;
    this->Rk = 0.0001;
}


inline Extract::~Extract() {
    //
}


// Short-term Fourier transform through detected audio event
Event Extract::process(const Audio &audio, Detection &detection,
                       const std::string &filepath, const double &cutoff) {
    Event event;
    event.filepath = filepath;// store file path
    event.fftSize = fftSize;
    event.stepSize = stepSize;
    event.smoothing = gain;
    event.sampleRate = audio.samplerate;
    event.startSample = detection.sampleLocation;
    event.powerSpectrum.resize(binN, 0);
    event.TE = audio.timeExpansionFactor;

    bin_to_freq = event.sampleRate / static_cast<double>(event.fftSize*1000);

    filter.initialise(event.sampleRate, fftSize);
    this->cutoff = cutoff;
    seekPoint = detection.sampleLocation;

    // Send audio samples to FFT
    fft.process(audio.samples, seekPoint);
    seekPoint += stepSize;

    // FFT spectrum
    std::vector<double> spectrum = fft.getSpectrum();

    // Filter low-end of spectrum
    spectrum = filter.highPass(spectrum, 16.0);
    smoothSpectrum(spectrum, gain);
    smoothSpectrum(detection.backgroundNoiseSpectrum, gain);

    // Calculate local SNR
    std::pair<double,double> sig_noise = SNR(spectrum, detection.backgroundNoiseSpectrum);

    // subtract background
    for(unsigned int i=0; i<spectrum.size(); ++i) {
        spectrum[i] -= detection.backgroundNoiseSpectrum[i];
        spectrum[i] = (spectrum[i] + fabs(spectrum[i])) * 0.5;// half wave rectify
    }

    pair<double, int> p = getPeak(spectrum);
    // mask tracked spectral peak
    std::vector<double> masked = mask(p.second, spectrum);
    fundBin = p.second;
    energy = sumOf(masked);

    // tracked frequency - weighted average
    centroidBin = centroid(masked);

    // Harmonic octave above tracked
    double harm = 2*centroidBin;
    harmBin = round(harm);

    //***************************************
    // Forwards iteration to event end
    track.clear();
    kalman.initialise(centroidBin, Qk, Rk);
    kalmanBin = centroidBin;
    // store data for first frame
    storeBack(event, sig_noise.first, sig_noise.second, spectrum, masked);
    forwardExtract(audio.samples, detection, event);
    //***************************************

    //***************************************
    // Back iteration to event start
    track.clear();
    kalman.initialise(centroidBin, Qk, Rk);
    backExtract(audio.samples, detection, event);
    //***************************************

    // extracted event
    return event;
}


// extract event to end
void Extract::forwardExtract(std::vector<float> const& audioSamples,
                                    Detection &detection, Event &event) {
    unsigned int frames = 1 + floor( static_cast<double>( detection.sampleLocation - binN ) / stepSize );
    for(unsigned int i=0; i<frames; ++i) {
        // Send audio samples to FFT
        fft.process(audioSamples, seekPoint);
        seekPoint += stepSize;// advance STFT

        // FFT spectrum
        std::vector<double> spectrum = fft.getSpectrum();
        spectrum = filter.highPass(spectrum, 16.0);
        smoothSpectrum(spectrum, gain);
        // Calculate local SNR
        std::pair<double,double> sig_noise = SNR(spectrum, detection.backgroundNoiseSpectrum);
        double localSNR = to_dB(sig_noise.first/noZero(sig_noise.second));

        // subtract background
        for(unsigned int j=0; j<spectrum.size(); ++j) {
            spectrum[j] -= detection.backgroundNoiseSpectrum[j];
            spectrum[j] = (spectrum[j] + fabs(spectrum[j])) * 0.5;// half wave rectify
        }

        pair<double, int> p = getPeak(spectrum);
        std::vector<double> masked = mask(p.second, spectrum);
        fundBin = p.second;

        energy = sumOf(masked);
        centroidBin = centroid(masked);

        kalmanBin = kalman.push_back(centroidBin);
        track.push_back(kalmanBin);
        double angularChange = getAngularDifference(track);

        // Harmonic octave above tracked
        double harm = 2*centroidBin;
        harmBin = round(harm);

        double ampDiff = to_dB(event.peakAmp) - to_dB(energy);

        if(i>1) if(isEnd(event, ampDiff, angularChange, localSNR)) break;
        storeBack(event, sig_noise.first, sig_noise.second, spectrum, masked);
    }
}


// extract event to start
void Extract::backExtract(std::vector<float> const& audioSamples,
                                 Detection &detection, Event &event) {
    seekPoint = detection.sampleLocation-stepSize;
    while(seekPoint >= 0)
    {
        // Send audio samples to FFT
        fft.process(audioSamples, seekPoint);
        seekPoint -= stepSize;// advance STFT

        // FFT spectrum
        std::vector<double> spectrum = fft.getSpectrum();
        spectrum = filter.highPass(spectrum, 16.0);
        smoothSpectrum(spectrum, gain);
        // Calculate local SNR
        std::pair<double,double> sig_noise = SNR(spectrum, detection.backgroundNoiseSpectrum);
        double localSNR = to_dB(sig_noise.first/noZero(sig_noise.second));

        // subtract background
        for(unsigned int i=0; i<spectrum.size(); ++i)
        {
            spectrum[i] -= detection.backgroundNoiseSpectrum[i];
            spectrum[i] = (spectrum[i] + fabs(spectrum[i])) * 0.5;// half wave rectify
        }

        pair<double, int> p = getPeak(spectrum);
        std::vector<double> masked = mask(p.second, spectrum);
        fundBin = p.second;

        energy = sumOf(masked);
        centroidBin = centroid(masked);

        kalmanBin = kalman.push_back(centroidBin);
        track.push_back(kalmanBin);
        double angularChange = getAngularDifference(track);

        // Harmonic octave above tracked
        double harm = 2*centroidBin;
        harmBin = round(harm);

        double ampDiff = to_dB(event.peakAmp) - to_dB(energy);
        if(isStart(event, ampDiff, angularChange, localSNR)) break;
        storeFront(event, sig_noise.first, sig_noise.second, spectrum, masked);
    }
}


// SNR = (signal plus noise) / noise;
inline std::pair<double, double> Extract::SNR(const std::vector<double> &spectrum,
                           const std::vector<double> &noiseEstimate) {
    std::pair<double, double> sig_noise(0,0);
    for(unsigned int i=0; i<spectrum.size(); ++i) {
        if(spectrum[i] > noiseEstimate[i]) {
            sig_noise.first += spectrum[i];// signal and noise
            sig_noise.second += noiseEstimate[i];// noise
        }
    }
    return sig_noise;
}


// find spectral peak with greatest magnitude
inline std::pair<double, int> Extract::getPeak(const std::vector<double> &x) {
    std::pair<double, int> p(0,1);
    for(unsigned int i=1; i<x.size()-1; ++i) {
        // look for spectral peaks
        if(x[i] >= x[i-1] && x[i] > x[i+1]) {
            if(x[i] > p.first) {
                p.first = x[i];
                p.second = i;
            }
        }
    }
    return p;
}


// Return a spectrum consisting of only one specified spectral peak
inline std::vector<double> Extract::mask(const int &peak, const std::vector<double> &x) {
    std::vector<double> result;
    result.resize(x.size(), 0);

    // multiply peak with a Gaussian kernel
    int index = -floor(Gaussian.size()/2.0);
    index += peak;
    for(unsigned int i=0; i<Gaussian.size(); ++i) {
        int seekPoint = index+i;
        if(seekPoint < 0 || seekPoint >= (int)x.size()) {
            // do nothing - out of bounds
        }
        else {
            double mag = x[seekPoint] * Gaussian[i];
            result[seekPoint] += mag;
        }
    }
    return result;
}


inline int Extract::isEnd(Event &event, const double &ampDiff, const double &angle,
                          const double &localSNR) {
    if( ampDiff > endThresh ||
        fabs(angle) > angleThresh ||
        localSNR < SNR_thresh )
    {
        event.endSample = seekPoint - (2*stepSize);
        return 1;
    }
    return 0;
}


inline int Extract::isStart(Event &event, const double &ampDiff, const double &angle,
                            const double &localSNR) {
    if( ampDiff > startThresh ||
        fabs(angle) > angleThresh ||
        localSNR < SNR_thresh )
    {
        event.startSample = seekPoint+(2*stepSize);
        return 1;
    }
    return 0;
}


inline void Extract::storeBack(Event &event, const double &sig, const double &noise,
                               const std::vector<double> &spectrum,
                               const std::vector<double> &masked) {
    // store data
    event.signal += sig;
    event.noise += noise;// avoid divide by zero
    event.ampTrack.push_front(spectrum[fundBin]);// sum of fundamental
    event.freqTrack.push_back(centroidBin);// frequency time course
    //event.kalmanTrack.push_back(kalmanBin);
    event.peakAmp = std::max(event.peakAmp, energy);
    event.features.duration++;
    // adds fundamental to event power spectrum
    transform(masked.begin(),masked.end(),event.powerSpectrum.begin(),event.powerSpectrum.begin(),plus<double>());
    event.harmonicAmpTrack.push_back(spectrum[min(harmBin,binN-1)]);
}


inline void Extract::storeFront(Event &event, const double &sig, const double &noise,
                                const std::vector<double> &spectrum,
                                const std::vector<double> &masked) {
    // store data
    event.signal += sig;
    event.noise += noise;// avoid divide by zero
    event.ampTrack.push_front(spectrum[fundBin]);// sum of fundamental
    event.freqTrack.push_front(centroidBin);// frequency time course
    //event.kalmanTrack.push_front(kalmanBin);
    event.peakAmp = std::max(event.peakAmp, energy);
    event.features.duration++;
    // adds fundamental to event power spectrum
    transform(masked.begin(),masked.end(),event.powerSpectrum.begin(),event.powerSpectrum.begin(),plus<double>());
    event.harmonicAmpTrack.push_front(spectrum[min(harmBin,binN-1)]);
}


inline double Extract::getAngularDifference(const std::deque<double> &x) {
    if(x.size()<=2) return 0;

    std::deque<double>::const_iterator it = x.end();
    double r = *(--it); double m = *(--it); double l = *(--it);
    return bearingDegrees(atan2(r-m,1.0) - atan2(m-l,1.0));
}

#endif // EXTRACT_H
