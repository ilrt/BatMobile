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

#ifndef FILTER_H
#define FILTER_H

#include <vector>

class Filter {

public:
    Filter();
    ~Filter();
    void initialise(const int &sampleRate, int fftSize);
    std::vector<double> highPass(const std::vector<double> &x,
                                 const double &cutoff_kHz, const double &width);

private:
    bool bEnabled;
    double frequencyResolution;
};

inline Filter::Filter() {
    bEnabled = false;
    frequencyResolution = 1.0;
}

inline void Filter::initialise(const int &sampleRate, int fftSize) {
    bEnabled = true;
    frequencyResolution = sampleRate / static_cast<double>(fftSize);
}

inline std::vector<double> Filter::highPass(const std::vector<double> &x,
                                            const double &cutoff_kHz, const double &width=4) {
    std::vector<double> out = x;
    if(!bEnabled) return out;
    double c = 1000.0 * cutoff_kHz/frequencyResolution;
    double scaling = 0;
    for(unsigned int i=0; i<x.size(); ++i) {
        scaling = (i - c)/(width/8.0);
        scaling = 1.0 / (1.0 + pow(2.718281828, -scaling)); //sigmoid function
        out[i] *= scaling;
    }
    return out;
}

inline Filter::~Filter() {
    //
}

#endif // FILTER_H
