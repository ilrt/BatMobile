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

#ifndef LABELS_H
#define LABELS_H

#include <fstream>
#include <sstream>
#include <string>
#include <deque>

class Labels {

public:
    Labels();
    ~Labels();
    bool read(std::string filename);
    std::string getLabel(int index);
    std::deque<std::string> getLabels() const;
    void reset();

private:
    std::deque<std::string> labels;

};

inline Labels::Labels() {
    //
}

inline Labels::~Labels() {
    //
}

// read in Labels data for svm model from a text file
inline bool Labels::read(std::string filename) {
    if(filename.empty()) return false;
    if(!labels.empty()) reset();

    std::ifstream file(filename.c_str());
    std::string   line;

    std::string str;
    while(std::getline(file, line)) {
        std::stringstream       linestream(line);
        linestream >> str;
        labels.push_back(str);
    }

    return !labels.empty() ? true : false;
}

inline std::string Labels::getLabel(int index) {
    return (index<0 || index>=(int)labels.size()) ?
                "unknown" : labels[index];
}

inline std::deque<std::string> Labels::getLabels() const {
    return labels;
}

inline void Labels::reset() {
    labels.clear();
}

#endif // LABELS_H
