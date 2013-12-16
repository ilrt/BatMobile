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

#ifndef ANALYSE_H
#define ANALYSE_H

#include "kalman.h"
#include "stats.h"
//#include "save.h"
#include "utilities.h"
#include "float.h"
#include "linearfit.h"
#include <deque>
#include <map>
#include <iterator>
#include <cmath>
#include <numeric>

class Analyse {

public:
    Analyse();
    ~Analyse();
    std::deque<double> process(Event &event, const double &Qk, const double &Rk);

private:
    std::vector<double> upsample(const std::vector<double> &x, unsigned int bins);
    std::pair<double, double> histogram(const std::deque<double> &x, double bin_to_freq,
                                        double &mean, double &sd);
    std::vector<double> histogram2(const std::deque<double> &x, double bin_to_freq);
    double smoothness(const std::deque<double> &x);
};

/******************** IMPLEMENTATION ************************/
inline Analyse::Analyse() {
    //
}

inline Analyse::~Analyse() {
    //
}

std::deque<double> Analyse::process(Event &event, const double &Qk, const double &Rk) {
    Features features;

    event.startSampleString = hoursMinsSecs(event.startSample / event.sampleRate * event.TE);

    //**************************************
    // Smooth frequency track using a
    // Kalman filter
    // To do: Kalman smoothing algorithm
    //**************************************

    Kalman kalman;
    // Qk & Rk 0.000001 & 0.0001 by default
    kalman.initialise(event.freqTrack.front(), Qk, Rk);
    kalman.setP_last(0.0);
    event.kalmanTrack.push_back(event.freqTrack.front());
    for(unsigned int i=1; i<event.freqTrack.size(); ++i) {
        event.kalmanTrack.push_back(kalman.push_back(event.freqTrack[i]));
    }
    //**************************************


    // duration in ms
    features.duration = 1000.0*event.features.duration*event.stepSize / (double)event.sampleRate;
    // Signal to Noise Ratio (SNR)
    features.SNR = to_dB(event.signal/noZero(event.noise));
    // Harmonic Distortion
    features.HD = to_dB(sumOf(event.harmonicAmpTrack) / noZero(sumOf(event.ampTrack)));
    // Crest factor
    //features.crest = to_dB(crestFactor(event.ampTrack));
    // Temporal centroid
    //features.temporalCentroid = centroid(event.ampTrack)/event.ampTrack.size();

    //**************************************
    // Power spectrum - upsample
    //**************************************
    unsigned int k = 512;
    std::vector<double> powSpectrum;
    if(event.powerSpectrum.size() < k) {
        powSpectrum = upsample(event.powerSpectrum, k);
    }
    else {
        powSpectrum = event.powerSpectrum;
    }

    std::pair<double, int> peak = findPeak(powSpectrum);
    std::pair<double, double> fmax = quadraticInterp(powSpectrum, peak.second);// in histogram bins
    features.FmaxE = fmax.first;
    double bin_to_freq = 0.5 * event.sampleRate / static_cast<double>(k*1000);
    features.FmaxE *= bin_to_freq;// in kHz
    // bandwidth in kHz from power spectrum
    //features.bandwidth_6dB = bandwidth(powSpectrum)*bin_to_freq;
    //**************************************


    //**************************************
    // Statistics from call frequency track
    //**************************************
    bin_to_freq = event.sampleRate / static_cast<double>(event.fftSize*1000);
    Stats stats;
    std::pair<double, double> minmax(DBL_MAX,-DBL_MAX);
    for(unsigned int i=0; i<event.freqTrack.size(); ++i) {
        double tmp = event.freqTrack[i]*bin_to_freq;
        stats.push_back(tmp);
        minmax.first = std::min(minmax.first, tmp);
        minmax.second = std::max(minmax.second, tmp);
    }
    features.minFreq = minmax.first;// min freq
    features.maxFreq = minmax.second;// max freq
    //features.startFreq = sampleAt(event.freqTrack, 0.0);// start freq
    //features.midFreq = sampleAt(event.freqTrack, 0.5);// mid freq
    //features.endFreq = sampleAt(event.freqTrack, 1.0);// end freq
    //features.meanFreq = stats.getMean();// mean freq
    //features.sdFreq = stats.getStandardDeviation();// sd of freq
    features.bandwidth = features.maxFreq - features.minFreq;// bandwidth in kHz
    //**************************************

    LinearFit fit;
    std::deque<double> linearFit = fit.getLinearFit(event.kalmanTrack);
    event.freqTrack = linearFit;// draw best fit line instead
    //sum = 0;
    features.curveStart = features.curveMid = features.curveEnd = 0;
    for(unsigned int i=0; i<event.kalmanTrack.size(); ++i) {
        //sum += fabs(linearFit[i] - event.kalmanTrack[i]);
        double tmp = linearFit[i] - event.kalmanTrack[i];
        if(tmp > 0) features.curveMid += tmp;
        if(tmp <= 0) {
            if(i < event.kalmanTrack.size()/2.0) {
                features.curveStart += tmp;
            }
            else features.curveEnd += tmp;
        }
    }
    //slope = (start freq - end freq) / duration
    features.midFreq = sampleAt(linearFit, 0.5) - sampleAt(event.kalmanTrack, 0.5);// mid freq
    features.midFreq *= bin_to_freq;
    //printf("mid offset: %f\n", features.midFreq);
    features.curveStart *= bin_to_freq;
    features.curveMid *= bin_to_freq;
    features.curveEnd *= bin_to_freq;
    features.slopeTotal = (sampleAt(linearFit,0.0)-sampleAt(linearFit,1.0))*bin_to_freq / features.duration;//kHz per ms

    features.smoothness = smoothness(event.kalmanTrack);
    //------------------------------------------------
    // Feature vector for svm classification
    //------------------------------------------------

    event.features = features;
    std::deque<double> results;
    //results.push_back(features.SNR);
    results.push_back(features.duration);
    results.push_back(features.HD);
    //results.push_back(features.crest);

    results.push_back(features.FmaxE);
    results.push_back(features.maxFreq);
    results.push_back(features.minFreq);
    results.push_back(features.midFreq);
    results.push_back(features.bandwidth);

    results.push_back(features.slopeTotal);

    results.push_back(features.curveStart);
    results.push_back(features.curveMid);
    results.push_back(features.curveEnd);
    results.push_back(features.smoothness);

    return results;
}

inline std::vector<double> Analyse::upsample(const std::vector<double> &x, unsigned int bins) {
    std::vector<double> result(bins);
    for(unsigned int i=0; i<bins; ++i)
    {
        double pos = static_cast<double>(i)/(bins-1);// range 0-1
        result[i] = sampleAt(x, pos);
    }
    return result;
}

// round frequency track to nearest kHz and store count of frequencies
inline std::pair<double, double> Analyse::histogram(const std::deque<double> &x,double bin_to_freq,
                                                    double &mean, double &sd) {
    int k = 120;// up to 120kHzzzz
    std::vector<double> hist(k,0);
    // only analyse second half of call...
    for(unsigned int i=x.size()/2.0; i<x.size(); ++i) {
        int index = round(x[i]*bin_to_freq);
        if(index >= k) continue;
        hist[index]++;
    }

    Stats stats;
    // p.first = bin; p.second = value;
    std::pair<double, double> p(0,0);
    for(unsigned int i=0; i<hist.size(); ++i) {
        stats.push_back(hist[i]);
        if(hist[i] > p.second) {
            p.second = hist[i];
            p.first = i;
        }
    }
    mean = stats.getMean();
    sd = stats.getStandardDeviation();

    // p.first = frequency; p.second = value;
    p = quadraticInterp(hist, p.first);

    return p;
}

// round frequency track to nearest kHz and store count of frequencies
inline std::vector<double> Analyse::histogram2(const std::deque<double> &x,double bin_to_freq) {
    int low = 20;
    int high = 70;
    std::vector<double> hist(high-low,0);
    for(unsigned int i=0; i<x.size(); ++i) {
        int index = round(x[i]*bin_to_freq);
        if(index >= high || index < low) continue;
        hist[index-low]++;
    }

    return hist;
}

inline double Analyse::smoothness(const std::deque<double> &x) {
    double sum = 0;
    for(unsigned int i=1; i<x.size()-1; ++i) {
        sum += fabs( (x[i+1] - x[i])-(x[i] - x[i-1]) );
    }
    return sum;
}

#endif // ANALYSE_H
