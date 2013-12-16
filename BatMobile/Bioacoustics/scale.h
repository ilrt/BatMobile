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

#ifndef SCALE_H
#define SCALE_H

#include <fstream>
#include <sstream>
#include <deque>

class Scale {

public:
    Scale();
    ~Scale();
    bool read(std::string filename);
    bool scaleFeatures(std::deque<double> &features);
    std::deque<std::pair<double, double> > getScaleData() const;
    void reset();

private:
    std::deque<std::pair<double, double> > scaleData;

};

inline Scale::Scale() {
    //
}

inline Scale::~Scale() {
    //
}

// read in scale data for svm model from a text file
inline bool Scale::read(std::string filename) {
    if(filename.empty()) return false;
    if(!scaleData.empty()) reset();

    std::ifstream file(filename.c_str());
    std::string   line;

    std::pair<double, double> p;
    // p.first = center; p.second = scale;
    while(std::getline(file, line)) {
        std::stringstream       linestream(line);
        // Read the doubles using the operator >>
        linestream >> p.first >> p.second;
        scaleData.push_back(p);
    }

    return !scaleData.empty() ? true : false;
}

// center and scale features
inline bool Scale::scaleFeatures(std::deque<double> &features) {
    if(features.size() != scaleData.size()) return false;

    std::pair<double, double> p(0,0);
    for(unsigned int i=0; i<scaleData.size(); ++i) {
        p = scaleData[i];
        features[i] = (features[i] - p.first);
        if(!p.second==0) features[i] /= p.second;//avoid divide by zero
    }
    return true;
}

inline std::deque<std::pair<double, double> > Scale::getScaleData() const {
    return scaleData;
}

inline void Scale::reset() {
    scaleData.clear();
}

#endif // SCALE_H
