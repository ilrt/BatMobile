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

/* Adapted code from John Cook's post on accurately computing running variance
 * http://www.johndcook.com/standard_deviation.html
 */

#ifndef STATSVEC_H
#define STATSVEC_H

#include <vector>
#include <cmath>

class StatsVec
{
public:
    StatsVec(int size);
    ~StatsVec();
    void reset(int size);
    void push_back(std::vector<double> const& x);
    std::vector<double> getMean() const;
    std::vector<double> getStandardDeviation();

private:
    int m_n;
    std::vector<double> m_oldM, m_newM, m_oldS, m_newS;
    std::vector<double> ss;

};

inline StatsVec::StatsVec(int _size) {
    reset(_size);
}

inline StatsVec::~StatsVec() {
    //
}

inline void StatsVec::reset(int size) {
    m_n = 0;
    ss.clear();// sum of squares
    ss.resize(size, 0);

    m_oldM.clear();
    m_oldM.resize(size, 0);
    m_newM.clear();
    m_newM.resize(size, 0);
    m_oldS.clear();
    m_oldS.resize(size, 0);
    m_newS.clear();
    m_newS.resize(size, 0);
}

inline void StatsVec::push_back(std::vector<double> const& x) {
    m_n++;
//    ss.resize(64,0);
//    ss[0]=0;
    for(unsigned int i=0; i<x.size(); ++i) {
       // printf("%u \n",i);
        
     //   ss[i] = 0;

        ss[i] += (x[i] * x[i]);

        // See Knuth TAOCP vol 2, 3rd edition, page 232
        if (m_n == 1) {
            m_oldM[i] = m_newM[i] = x[i];
            m_oldS[i] = 0.0;
        }
        else {
            m_newM[i] = m_oldM[i] + (x[i] - m_oldM[i])/m_n;
            m_newS[i] = m_oldS[i] + (x[i] - m_oldM[i])*(x[i] - m_newM[i]);

            // set up for next iteration
            m_oldM[i] = m_newM[i];
            m_oldS[i] = m_newS[i];
        }

    }
}

inline std::vector<double> StatsVec::getMean() const {
    return m_newM;
}

inline std::vector<double> StatsVec::getStandardDeviation() {
    std::vector<double> result;
    result.resize(m_newM.size(), 0);
    if(m_n <= 1) return result;
    for(unsigned int i=0; i<m_newM.size(); ++i) {
        result[i] = sqrt(m_newS[i]/(m_n - 1));
    }
    return result;
}


#endif // STATSVEC_H
