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

/* Adapted code from John Cook's post on comparing two ways to fit a line to data
 * http://www.johndcook.com/blog/2008/10/20/comparing-two-ways-to-fit-a-line-to-data/
 */

#ifndef LINEARFIT_H
#define LINEARFIT_H

#include <deque>

class LinearFit {

public:
    LinearFit();
    ~LinearFit();
    std::deque<double> getLinearFit(const std::deque<double> &y);
    double getSlope() const;
    double getIntercept() const;

protected:
    double slope, intercept;
};

inline LinearFit::LinearFit() {
    slope = intercept = 0;
}

// linear regression line fit to deque y and returned as deque for plotting etc.
inline std::deque<double> LinearFit::getLinearFit(const std::deque<double> &y) {
    double sx = 0.0, sy = 0.0, stt = 0.0, sts = 0.0;
    unsigned int n = y.size();
    for (unsigned int i = 0; i < n; ++i) {
        sx += i;
        sy += y[i];
    }
    for (unsigned int i = 0; i < n; ++i) {
        double t = i - sx/n;
        stt += t*t;
        sts += t*y[i];
    }

    slope = sts/stt;
    intercept = (sy - sx*slope)/n;

    std::deque<double> fit;
    for(unsigned int i=0; i<n; ++i) {
        fit.push_back(intercept);
        intercept += slope;
    }

    return fit;
}

inline double LinearFit::getSlope() const {
    return slope;
}

inline double LinearFit::getIntercept() const {
    return intercept;
}

inline LinearFit::~LinearFit() {
    //
}

#endif // LINEARFIT_H
