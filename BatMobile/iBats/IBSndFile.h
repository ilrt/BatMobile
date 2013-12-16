#ifndef SND_H
#define SND_H

#include <exception>
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <deque>
#include <numeric>
#include <cmath>
#include <stdlib.h>
#include <cfloat>

#include "stats.h"
#include "float.h"

#include "IBAudio.h"

class IBSndFile {
    
public:
    IBSndFile();
    ~IBSndFile();
    bool read(std::string path);
    
    void normalise();
    IBAudio getAudio() const;
    std::vector<float> getAudioSamples() const;
    int getSamplerate() const;
    bool resample(const int &samplerate);
    bool save();
    
private:
    IBAudio audio;
    ExtAudioFileRef fileRef;
    void checkResult(OSStatus error, const char *operation);
};



#endif