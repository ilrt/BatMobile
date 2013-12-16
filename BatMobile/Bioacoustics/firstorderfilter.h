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

#ifndef FIRSTORDERFILTER_H
#define FIRSTORDERFILTER_H

class FIR {

public:
        FIR();
        ~FIR();
        void initialise(const double &x);
        double push_back(const double &x);
        double back();
        void clear();

protected:
        double output;
        double previous;
};

/******************** IMPLEMENTATION ************************/
inline FIR::FIR() {
    clear();
}

inline FIR::~FIR() {
    //
}

inline void FIR::initialise(const double &x) {
    clear();
    previous = x;
}

inline double FIR::push_back(const double &x) {
    output = (x + previous) * 0.5;// filter equation
    previous = x;// previous input
    return output;
}

inline double FIR::back() {
    return output;
}

inline void FIR::clear() {
    output = 0;
    previous = 0;
}

#endif // FIRSTORDERFILTER_H
