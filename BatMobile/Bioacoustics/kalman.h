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

/* A C++ implementation of the example given in
 * 'An introduction to the Kalman filter' (Welch & Bishop, 2006)
 * http://www.cs.unc.edu/~welch/media/pdf/kalman_intro.pdf
 * and based on Python code by Andrew D. Straw
 * http://www.scipy.org/Cookbook/KalmanFiltering
 */

#ifndef KALMAN_H
#define KALMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

class Kalman {

public:
    Kalman();
    void initialise(const double &x, const double &Q, const double &R);
    double push_back(const double &x);
    void clear();
    void setP_last(const double &x);
    ~Kalman();

private:
    double xHatLast, P_last;
    double Q, R;
    double K, P, P_temp;
    double xTempEst, xHat;
    double z_measured, z_real;
};

inline Kalman::Kalman() {
    clear();
}

inline Kalman::~Kalman() {
    //
}

// Q process variance
// R estimate of measurement variance
inline void Kalman::initialise(const double &x, const double &Q, const double &R) {
    clear();
    this->Q = Q; this->R = R;
    xHatLast = x;
}

inline double Kalman::push_back(const double &x) {
    // time update
    xTempEst = xHatLast;
    P_temp = P_last + Q;

    // measurement update
    K = P_temp * (1.0/(P_temp + R));

    z_measured = x;

    xHat = xTempEst + K * (z_measured - xTempEst);
    P = (1 - K) * P_temp;

    P_last = P;
    xHatLast = xHat;

    return xHat;
}

inline void Kalman::clear() {
    //initial values for the kalman filter
    xHatLast = 0;
    P_last = 1;
    Q = R = 0;
}

inline void Kalman::setP_last(const double &x) {
    P_last = x;
}

#endif // KALMAN_H
