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

#ifndef D500X_H
#define D500X_H

#include "utilities.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iomanip>
#include <sys/time.h>
#include <sstream>

class D500X {

public:
    D500X();
    ~D500X();

    long unsigned int readTime(std::string path);

    int getSec();
    int getMin();
    int getHour();
    int getDay();
    int getMonth();
    int getYear();

    std::string getTime();
    std::string getDate();
    std::string getNight();

protected:
    bool isD500X(std::string path);
    struct tm dt;
};

inline D500X::D500X() {
    //
}

inline D500X::~D500X() {
    //
}

//returns true if file is a D500X recording
bool D500X::isD500X(std::string path) {
    const char * astr = path.c_str();

    fstream callTime( astr, std::ios::in | std::ios::binary);
    if (callTime.is_open() ){
        callTime.seekg( 240, std::ios::beg ); // 208 for filename, 240 for D500X - checked with HexEdit
        std::string waveChunk ("D500X");
        char chunk1[6] = {0,0,0,0,0};
        callTime.get(chunk1, 6);
        return !waveChunk.compare(chunk1) ? true : false;
    }

    return false;
}

long unsigned int D500X::readTime(std::string path) {
    if(!isD500X(path)) return 0;

    long unsigned int epoch = -1;
    const char * astr = path.c_str();

    fstream callTime( astr, std::ios::in | std::ios::binary);
    if (callTime.is_open() ){
        callTime.seekg( 240, std::ios::beg ); // 208 for filename, 240 for D500X - checked with HexEdit
        std::string waveChunk ("D500X");
        char chunk1[6] = {0,0,0,0,0};
        callTime.get(chunk1, 6);

        // extract date and time if its a D500X recording
        if(!waveChunk.compare(chunk1)) {
            callTime.seekg( 224, std::ios::beg ); // 224 for date and time
            // e.g. 100829 21:53:56
            char chunk[16];
            callTime.get(chunk, 16); // read date and time from D500X

            int dateTime[16];
            int day;
            for(int i=0; i<16; i++) {
                std::stringstream ss;
                ss << std::hex << chunk[i];
                ss >> day;
                dateTime[i] = day;
            }

            // Unix time:
            // number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC
            int dec[2] = {1,10};
            int tmp = 2000 + dateTime[0]*dec[1] + dateTime[1]*dec[0];
            dt.tm_year = tmp - 1900; // year - 1900 (years since 1900!)
            tmp = dateTime[2]*dec[1] + dateTime[3]*dec[0];
            dt.tm_mon = tmp - 1; // month - 1 (months since January 0-11)
            tmp = dateTime[4]*dec[1] + dateTime[5]*dec[0];
            dt.tm_mday = tmp;
            dt.tm_hour = dateTime[7]*dec[1] + dateTime[8]*dec[0];
            dt.tm_min = dateTime[10]*dec[1] + dateTime[11]*dec[0];
            dt.tm_sec = dateTime[13]*dec[1] + dateTime[14]*dec[0];
            // Daylight Saving Time
            dt.tm_isdst = -1; // less than zero if the information is not available

            dt.tm_wday = 0;
            dt.tm_yday = 0;
            epoch = mktime(&dt);
            callTime.close();
        }
    }
    return epoch;
}

inline int D500X::getSec() {
    return dt.tm_sec;
}

inline int D500X::getMin() {
    return dt.tm_min;
}

inline int D500X::getHour() {
    return dt.tm_hour;
}

inline int D500X::getDay() {
    return dt.tm_mday;
}

//inline int D500X::getNight();

inline int D500X::getMonth() {
    return dt.tm_mon+1;
}

inline int D500X::getYear() {
    return dt.tm_year+1900;//adjust from years since 1900
}

//time runs from mid-day (0) to next day (+24)
inline std::string D500X::getTime() {
    std::string s;
    std::stringstream out;
    out << dt.tm_hour << ":" << dt.tm_min << ":" << dt.tm_sec;
    s = out.str();

    return s;
}

//return date as string
inline std::string D500X::getDate() {
    std::string s;
    std::stringstream out;
    // day/month/year
    out << dt.tm_mday << "/" << dt.tm_mon+1 << "/" << (dt.tm_year+1900);
    s = out.str();

    return s;
}

//return date as string
inline std::string D500X::getNight() {
    std::string s;
    std::stringstream out;
    struct tm tmp = dt;
    if(tmp.tm_hour < 12) tmp.tm_mday--;//if before mid-day, count as day before
    time_t next = mktime(&tmp);
    struct tm * timeinfo;
    timeinfo = localtime ( &next );
    out << timeinfo->tm_mday << "/" << timeinfo->tm_mon+1 << "/" << (timeinfo->tm_year+1900);
    s = out.str();

    return s;
}

#endif // D500X_H
