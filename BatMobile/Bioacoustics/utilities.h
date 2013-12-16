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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "stats.h"
#include "float.h"
#include <algorithm>
#include <vector>
#include <deque>
#include <numeric>
#include <cmath>
#include <stdlib.h>
#include <cfloat>
#include <sstream>
using namespace std;

enum window_type { Bartlett, BlackmanHarris4, BlackmanHarris7, Hamming, Hanning, Rectangular };

static const double EPSILON = 0.000001;
static const double ZEROLOG = -120;
static const double PI = 3.14159265358979323846;
static const double TWO_PI = 6.28318530717958647693;
static const double RAD_TO_DEG = 180/PI;

static const double arr[] = {1./4096,12./4096,66./4096,220./4096,495./4096,792./4096,924./4096,792./4096,495./4096,220./4096,66./4096,12./4096,1./4096};
static const vector<double> Gaussian (arr, arr + sizeof(arr) / sizeof(arr[0]) );

struct Audio {
    std::vector<float> samples;
    int samplerate;
    int timeExpansionFactor;
    Audio() {
        samplerate = 0;
        timeExpansionFactor = 1;
    }
};

struct Sequence {
    std::string directory;
    std::string classification;
    std::string time;
    std::string date;
    double p;

    Sequence() {
        directory = classification = time = date = "";
        p = 0;
    }
};

////////////////////////////////////////////////////////////
//
// Holds audio data for extracted event
//
struct Features {
    double duration;
    double SNR, HD;
    double maxFreq, minFreq, FmaxE, midFreq;
    double bandwidth;
    double slopeTotal;
    double curveStart, curveEnd, curveMid;
    double smoothness;
    Features() {
        duration = 0;
    }
};

struct Event {
    std::string filepath;
    int id;
    std::string classification;
    double p_value;
    std::string time;
    std::string date;
    double startSample;
    std::string startSampleString;
    double endSample;
    double signal, noise;
    double peakAmp;
    double sampleRate;
    double fftSize;
    double stepSize;
    double smoothing;
    double TE;

    Features features;

    deque<double> freqTrack;
    deque<double> kalmanTrack;
    deque<double> ampTrack;
    deque<double> harmonicAmpTrack;
    vector<double> powerSpectrum;

    Event() {
        id = 0;
        p_value = signal = noise = 0;
        peakAmp = -DBL_MAX;
        filepath = classification = "unknown";
        time = date = "";
    }
};

struct Recording {
    std::string filepath;
    std::string time, date;
    int numEvents;
    std::vector<Event> events;

    Recording() {
        numEvents = 0;
        filepath = "unknown";
        time = date = "";
    }
};

struct Detection {
    int sampleLocation;
    double SNR;
    vector<double> backgroundNoiseSpectrum;

    Detection() {
        sampleLocation = 0;
        SNR = ZEROLOG;
    }
};

typedef std::pair<double,int> mypair;
inline bool comp( const mypair& l, const mypair& r)
   { return l.first > r.first; }

//Constrains a value to not exceed a maximum and minimum value
//uses conditional operator
template <typename T>
float clamp(T x, T min, T max) {
   return x < min ? min : x > max ? max : x;
}

// convert magnitude to dB
inline double to_dB(double const& x) {
    return 20. * log10(max(x, EPSILON));
}

// convert magnitude to dB
inline vector<double> to_dB(vector<double> const& x) {
    vector<double> dB(x.size());
    for(unsigned int i=0; i<x.size(); ++i) {
        dB[i] = 20. * log10(max(x[i], EPSILON));
    }
    return dB;
}

// convert magnitude to dB
inline std::deque<double> to_dB(std::deque<double> const& x) {
    std::deque<double> dB(x.size());
    for(unsigned int i=0; i<x.size(); ++i) {
        dB[i] = 20. * log10(max(x[i], EPSILON));
    }
    return dB;
}

// convert magnitude to dB
inline void from_dB(vector<double>& x)
{
    for(unsigned int i=0; i<x.size(); ++i) {
        x[i] = pow(10, 0.05*x[i]);
    }
}

// convert magnitude to dB
inline void from_dB(double &x)
{
        x = pow(10, 0.05*x);
}

inline double mapRange(double value, double inputMin, double inputMax, double outputMin, double outputMax)
{
        double outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

            if(outputMax < outputMin){
                    if( outVal < outputMax )outVal = outputMax;
                    else if( outVal > outputMin )outVal = outputMin;
            }else{
                    if( outVal > outputMax )outVal = outputMax;
                    else if( outVal < outputMin )outVal = outputMin;
            }
        return outVal;
}

inline void normalise(vector<double>& x)
{
    double peak = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        peak = max(x[i], peak);
    }
    for(unsigned int i=0; i<x.size(); ++i) {
        x[i] /= peak;
    }
}


inline void normalise(deque<double>& x)
{
    double peak = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        peak = max(x[i], peak);
    }
    for(unsigned int i=0; i<x.size(); ++i) {
        x[i] /= peak;
    }
}

// two values a & b, float t 0-1
inline double lerp(double t, double a, double b)
{
        return a + t * (b - a);
}

inline vector<double> oversample(vector<double> const& x, unsigned int factor)
{
        vector<double> y;
        for(unsigned int i=0; i<x.size()-1; ++i) {
                double a = x[i];
                double b = x[i+1];
                for(unsigned int j=0; j<factor; ++j) {
                        double t = (double)j/factor;
                        y.push_back( lerp(t, a, b) );
                }
        }
        return y;
}


// linear interpolation - without bounds checking
inline double linearInterp(vector<double> const& x, int index, double frac)
{
    return x[index] + frac * (x[index+1] - x[index]);
}

// cubic interpolation - without bounds checking
inline double cubicInterp(vector<double> const& x, int index, double frac)
{
    double y0, y1, y2, y3;
    y0 = x[index-1];
    y1 = x[index];
    y2 = x[index+1];
    y3 = x[index+2];
    double tmp = y3 + 3.0 * y1;
    double fracsq = frac * frac;
    double fracb = frac * fracsq;

    return fracb*(- y0 - 3.0*y2 + tmp)/6.0 +
           fracsq*( (y0+y2)/2.0 - y1 ) +
           frac*(y2 + (-2.0*y0 - tmp)/6.0 ) + y1;
}

// cubic interpolation - without bounds checking
inline double cubicInterp(deque<double> const& x, int index, double frac)
{
    double y0, y1, y2, y3;
    y0 = x[index-1];
    y1 = x[index];
    y2 = x[index+1];
    y3 = x[index+2];
    double tmp = y3 + 3.0 * y1;
    double fracsq = frac * frac;
    double fracb = frac * fracsq;

    return fracb*(- y0 - 3.0*y2 + tmp)/6.0 +
           fracsq*( (y0+y2)/2.0 - y1 ) +
           frac*(y2 + (-2.0*y0 - tmp)/6.0 ) + y1;
}

// sample t along vector where t is between 0-1
// use cubic interpolation where possible, otherwise linear
inline double sampleAt(vector<double> const& x, double t)
{
    t = clamp(t, 0.0, 1.0);
    double position = t * (x.size()-1);
    unsigned int index = floor(position);
    double frac = position - index;

    if(index > 0 && index < x.size()-2) return cubicInterp(x, index, frac);
    else return (index+1) >= x.size() ? x[index] : lerp(frac, x[index], x[index+1]);
}

inline double sampleAt(deque<double> const& x, double t)
{
    t = clamp(t, 0.0, 1.0);
    double position = t * (x.size()-1);
    unsigned int index = floor(position);
    double frac = position - index;

    if(index > 0 && index < x.size()-2) return cubicInterp(x, index, frac);
    else return (index+1) >= x.size() ? x[index] : lerp(frac, x[index], x[index+1]);
}


// find min and max values in vector
// .first = min
// .second = max
inline std::pair<double, double> minMax(const vector<double>& x)
{
    std::pair<double, double> p;
    p.first = DBL_MAX;
    p.second = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        p.first = min(p.first, x[i]);
        p.second = max(p.second, x[i]);
    }
    return p;
}

inline std::pair<double, double> minMax(const deque<double>& x)
{
    std::pair<double, double> p;
    p.first = DBL_MAX;
    p.second = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        p.first = min(p.first, x[i]);
        p.second = max(p.second, x[i]);
    }
    return p;
}

template <typename T>
inline std::pair<double,int> findPeak(vector<T>& x)
{
    std::pair<double,int> max_value(-DBL_MAX, 0);
    for(unsigned int i=0; i<x.size(); ++i) {
        if(x[i] > max_value.first) {
            max_value.first = x[i];
            max_value.second = i;
        }
    }
    return max_value;
}

template <typename T>
inline std::pair<double,int> findPeak(deque<T>& x)
{
    std::pair<double,int> max_value(-DBL_MAX, 0);
    for(unsigned int i=0; i<x.size(); ++i) {
        if(x[i] > max_value.first) {
            max_value.first = x[i];
            max_value.second = i;
        }
    }
    return max_value;
}

template <typename T>
inline std::pair<int,int> findStartEnd(vector<T>& x, int peak, double thresh = 0.05)//26dB default threshold
{
    vector<double> norm(x);
    normalise(norm);
    std::pair<int,int> start_end(0,0);
    for(unsigned int i=peak; i<norm.size(); ++i) {
        if(norm[i] <= thresh) {
            start_end.first = i;
            break;
        }
    }
    for(unsigned int i=peak; i>=0; --i) {
        if(norm[i] <= thresh) {
            start_end.second = i;
            break;
        }
    }
    return start_end;
}


template <typename T>
inline std::pair<int,int> findStartEnd(deque<T>& x, int peak, double thresh = 0.05)//26dB default threshold
{
    deque<double> norm(x);
    normalise(norm);
    std::pair<int,int> start_end(0,0);
    for(unsigned int i=peak; i<norm.size(); ++i) {
        if(norm[i] <= thresh) {
            start_end.first = i;
            break;
        }
    }
    for(unsigned int i=peak; i>=0; --i) {
        if(norm[i] <= thresh) {
            start_end.second = i;
            break;
        }
    }
    return start_end;
}

//linear interpolation - calculates a number between two numbers at a specific increment
inline float lerp(float start, float stop, float amt) {
    return start + (stop-start) * amt;
}

//round to nearest integer
template <typename T>
int round(T x)
{
    return int(x + 0.5);
}

//double x = mapRange(i, 0, width, -5, 5);
inline double sampleGaussian(double x, double mean, double variance) {
  return (1 / sqrt(TWO_PI * variance)) * exp(-pow(x - mean, 2.0) / (2 * variance));
}

template <typename T>
inline double interpPeakBin(vector<T>& sequence, int k)
{
    k = clamp(k, 1, (int)sequence.size()-2);

    double delta = (sequence[k+1]-sequence[k-1]) / (sequence[k]+sequence[k+1]+sequence[k-1]);
    return k + delta;
}

// symmetric exponential smoothing filter
inline void smoothSpectrum(deque<double>& x, double gain)
{
   double oneMinusGain = 1.0 - gain;
   int i;
   int size = x.size();

   // reverse filter
   for (i=size-2; i>=0; --i) {
      x[i] = oneMinusGain*x[i] + gain*x[i+1];
   }

   // forward filter
   for (i=1; i<size; ++i) {
      x[i] = oneMinusGain*x[i] + gain*x[i-1];
   }

}

// symmetric exponential smoothing filter
inline void smoothSpectrum(vector<double>& x, double gain)
{
   double oneMinusGain = 1.0 - gain;
   int i;
   int size = x.size();

   // reverse filter
   for (i=size-2; i>=0; --i) {
      x[i] = oneMinusGain*x[i] + gain*x[i+1];
   }

   // forward filter
   for (i=1; i<size; ++i) {
      x[i] = oneMinusGain*x[i] + gain*x[i-1];
   }

}

static inline double computeSquare (double x) { return x*x; }
// convert amplitude spectrum to power spectrum
inline void power(vector<double> & x)
{
    transform(x.begin(), x.end(), x.begin(), computeSquare);
}

// return closest value in the vector values to the target value
inline int closestValueIndex(double const& target, vector<pair<double, int> > const& values)
{
    int result = 0;
    if(values.empty()) {
        return result;
    }

    double min = DBL_MAX;
    pair<double, int> p;
    unsigned int maxPeaks = 3;
    for(unsigned int i=0; i<values.size(); ++i) {
        if(i >= maxPeaks) break;
        p = values[i];
        double dist = fabs(target-p.second);
        if(dist < min) {
            min = dist;
            result = p.second;
        }
    }
    return result;
}

// return closest value in the vector values to the target value
inline pair<double, int> closestValueIndex(double const& target, vector<double> const& values)
{
    pair<double, int> result;
    result.first = 0;
    result.second = 0;
    if(values.empty()) {
        return result;
    }

    double min = DBL_MAX;
    for(unsigned int i=0; i<values.size(); ++i)
    {
        double dist = fabs(target-values[i]);
        if(dist < min) {
            min = dist;
            result.first = values[i];
            result.second = i;
        }
    }
    return result;
}

// Crest Factor
// for +ve only arrays
inline double crestFactor(const vector<double>& x)
{
    Stats stats;
    double peak = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        peak = std::max(peak, x[i]);
        stats.push_back(x[i]);
    }
    return peak / stats.getMean();
}

// Crest factor
// for +ve only arrays
inline double crestFactor(const deque<double>& x)
{
    Stats stats;
    double peak = -DBL_MAX;
    for(unsigned int i=0; i<x.size(); ++i) {
        peak = std::max(peak, x[i]);
        stats.push_back(x[i]);
    }
    return peak / stats.getMean();
}

// Mean and standard deviation of a vector of values
inline std::pair<double, double> stats(const vector<double>& x)
{
    Stats stats;
    for(unsigned int i=0; i<x.size(); ++i) {
        stats.push_back(x[i]);
    }
    std::pair<double, double> p;
    p.first = stats.getMean();
    p.second = stats.getStandardDeviation();
    return p;
}

// Mean and standard deviation of a vector of values
inline std::pair<double, double> stats(const deque<double>& x)
{
    Stats stats;
    for(unsigned int i=0; i<x.size(); ++i) {
        stats.push_back(x[i]);
    }
    std::pair<double, double> p;
    p.first = stats.getMean();
    p.second = stats.getStandardDeviation();
    return p;
}

inline double centroid(const vector<double>& x)
{
    if(x.empty()) return 0.0;

    double indexSum = 0.0;
    double magSum  = 0.0;
    for(unsigned int i=0; i<x.size(); ++i) {
        indexSum += i * x[i];
        magSum += x[i];
    }
    return (magSum==0) ? 0 : indexSum / magSum;
}

inline double centroid(const std::deque<double>& x)
{
    if(x.empty()) return 0.0;

    double indexSum = 0.0;
    double magSum  = 0.0;
    for(unsigned int i=0; i<x.size(); ++i) {
        //squared magnitude represents signal power
        indexSum += i * x[i] * x[i];
        magSum += x[i] * x[i];
    }
    return (magSum==0) ? 0 : indexSum / magSum;
}

// bandwidth above threshold (default 6dB)
inline double bandwidth(const vector<double>& x, double threshold = 0.5)
{
    vector<double> norm(x);
    normalise(norm);
    return count_if (norm.begin(), norm.end(), bind2nd(greater_equal<double>(), threshold));
}

// bandwidth above threshold (default 6dB)
inline double bandwidth(const deque<double>& x, double threshold = 0.5)
{
    deque<double> norm(x);
    normalise(norm);
    return count_if (norm.begin(), norm.end(), bind2nd(greater_equal<double>(), threshold));
}


// Sum of a vector
inline double sumOf(vector<double> const& x)
{
    if(x.empty()) return 0.0;
    return accumulate(x.begin(),x.end(),0.0);
}

inline double sumOf(deque<double> const& x)
{
    if(x.empty()) return 0.0;
    return accumulate(x.begin(),x.end(),0.0);
}


// central difference
inline std::deque<double> diff(std::deque<double> const& x)
{
    // deque must have enough content to do difference
    if(x.size() <= 2) {
        std::deque<double> blank;
        return blank;
    }

    std::deque<double> result;
    for(unsigned int i=1; i<x.size()-1; ++i) {
        result.push_back( (x[i-1] - x[i+1]) / 2 );
    }
    return result;
}

inline std::deque<double> difference(std::deque<double> const& x, const unsigned int &differences)
{
    std::deque<double> result(x);
    for(unsigned int i=0; i<differences; ++i) {
        result = diff(result);
    }
    return result;
}


inline std::deque<double> OPS(std::deque<double> const& x)
{
    // deque must have enough content to do difference
    if(x.size() < 2) {
        std::deque<double> blank;
        return blank;
    }

    std::deque<double> result(x.size()-1, 0);
    for(unsigned int i=0; i<x.size()-1; ++i) {
        result[i] = log2(fabs(x[i] / x[i+1]));
    }
    return result;
}

// first = x; second = y;
inline std::pair<double, double> quadraticInterp(std::vector<double> const& x, unsigned int n)
{
    std::pair<double, double> p(0,0);
    if(n >= x.size()-1 || n < 2) return p;

    double y1 = x[n-1];
    double y2 = x[n];
    double y3 = x[n+1];

    p.first = n + (y3 - y1)/(2*(2*y2 - y1 - y3));
    p.second = y2 + (2*y2 - y1 - y3) / 2.0 *((p.first-n)*(p.first-n));
    return p;
}

// won't return less than EPSILON
inline double noZero(const double &x)
{
    return std::max(x, EPSILON);
}

inline double bearingDegrees(const double &radians)
{
    return radians*RAD_TO_DEG;
    //return (radians >= 0 ? radians : (TWO_PI + radians)) * 360 / TWO_PI;
}

inline std::string hoursMinsSecs(const int &x)
{
    int secRem = x % 60;
    int min = x / 60;
    int minRem = min % 60;
    int hours = min / 60;

    stringstream ss;
    if(hours < 10) ss << 0;
    ss << hours << ":";
    if(minRem < 10) ss << 0;
    ss << minRem << ":";
    if(secRem < 10) ss << 0;
    ss << secRem;

    return ss.str();
}

#endif // UTILITIES_H
