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

/* Python pyo code by Olivier Belanger was a useful resource for this
 * http://code.google.com/p/pyo/source/browse/trunk/src/engine/wind.c?r=803
 */

#ifndef FFT_WINDOWS_H
#define FFT_WINDOWS_H

#include <vector>
#include <cmath>
#include "utilities.h"

class FFT_Window {

public:
    FFT_Window();
    void setup(const int &fftSize, window_type win);
    std::vector<double> getWindow() const;
    double getWindowSum() const;
    ~FFT_Window();

private:
    void Bartlett(const int &fftSize);
    void BlackmanHarris_4(const int &fftSize);
    void BlackmanHarris_7(const int &fftSize);
    void Hamming(const int &fftSize);
    void Hanning(const int &fftSize);
    void Rectangular(const int &fftSize);

    std::vector<double> window;
    double sum;

};

inline FFT_Window::FFT_Window() {
    sum = 0;
}

inline void FFT_Window::setup(const int &fftSize, window_type win) {
    switch ( win ) {
      case 0:
        Bartlett(fftSize);
        break;
      case 1:
        BlackmanHarris_4(fftSize);
        break;
      case 2:
        BlackmanHarris_7(fftSize);
        break;
      case 3:
        Hamming(fftSize);
        break;
    case 4:
        Hanning(fftSize);
        break;
    case 5:
        Rectangular(fftSize);
        break;
      default:
        Rectangular(fftSize);
        break;
      }
}

inline void FFT_Window::Bartlett(const int &fftSize) {
    window.resize(fftSize);
    sum = 0;
    double arg = 2.0 / (fftSize-1);
    int i;
    for (i=0; i<(fftSize-1)*0.5; ++i) {
        window[i] = i * arg;
        sum += window[i];
    }
    for (; i<fftSize; ++i) {
        window[i] = 2.0 - i * arg;
        sum += window[i];
    }
}

inline void FFT_Window::BlackmanHarris_4(const int &fftSize) {
    // 4-term Blackman-harris FFT_Window
    window.resize(fftSize);
    sum = 0;
    double arg = TWO_PI / (fftSize-1);
    for (int i=0; i<fftSize; ++i) {
        window[i] = 0.35875 - 0.48829 * cos(arg*i) + 0.14128 * cos(2*arg*i) - \
                    0.01168 * cos(3*arg*i);
        sum += window[i];
    }
}

inline void FFT_Window::BlackmanHarris_7(const int &fftSize) {
    // 7-term Blackman-harris FFT_Window
    window.resize(fftSize);
    sum = 0;
    double arg = TWO_PI / (fftSize-1);
    for (int i=0; i<fftSize; ++i) {
        window[i] = 0.2712203606 - 0.4334446123*cos(arg*i) + \
                    0.21800412*cos(2*arg*i) - 0.0657853433*cos(3*arg*i) + \
                    0.0107618673*cos(4*arg*i) - 0.0007700127*cos(5*arg*i) + \
                    0.00001368088*cos(6*arg*i);
        sum += window[i];
    }
}

inline void FFT_Window::Hamming(const int &fftSize) {
    window.resize(fftSize);
    sum = 0;
    double arg = TWO_PI / (fftSize-1);
    for (int i=0; i<fftSize; ++i) {
        window[i] = 0.54 - 0.46*cos(arg*i);
        sum += window[i];
    }
}

inline void FFT_Window::Hanning(const int &fftSize) {
    window.resize(fftSize);
    sum = 0;
    double arg = TWO_PI / (fftSize-1);
    for (int i=0; i<fftSize; ++i) {
        window[i] = 0.5 - 0.5*cos(arg*i);
        sum += window[i];
    }
}

inline void FFT_Window::Rectangular(const int &fftSize) {
    window.resize(fftSize, 1.0);
    sum = fftSize;
}

inline std::vector<double> FFT_Window::getWindow() const {
    return window;
}

inline double FFT_Window::getWindowSum() const {
    return sum;
}

inline FFT_Window::~FFT_Window() {
    //
}

#endif // FFT_WINDOWS_H
