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

#ifndef STATS_H
#define STATS_H

#include <cmath>

class Stats {
public:
    Stats();
    void clear();
    void push_back(const double &x);
    double getMean() const;
    double getVariance() const;
    double getStandardDeviation() const;

private:
    int m_n;
    double m_oldM, m_newM, m_oldS, m_newS;
};

inline Stats::Stats() : m_n(0) {
    m_oldM=m_newM=m_oldS=m_newS=0;
}

inline void Stats::clear() {
    m_n = 0;
}

inline void Stats::push_back(const double &x) {
    m_n++;

    // See Knuth TAOCP vol 2, 3rd edition, page 232
    if (m_n == 1) {
        m_oldM = m_newM = x;
        m_oldS = 0.0;
    }
    else {
        m_newM = m_oldM + (x - m_oldM)/m_n;
        m_newS = m_oldS + (x - m_oldM)*(x - m_newM);

        // set up for next iteration
        m_oldM = m_newM;
        m_oldS = m_newS;
    }
}

inline double Stats::getMean() const {
    return (m_n > 0) ? m_newM : 0.0;
}

inline double Stats::getVariance() const {
    return ( (m_n > 1) ? m_newS/(m_n - 1) : 0.0 );
}

inline double Stats::getStandardDeviation() const {
    return sqrt( getVariance() );
}

#endif // STATS_H
