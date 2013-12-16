#ifndef SNDIO_H
#define SNDIO_H

#include <vector>
#include <cmath>
#include <exception>
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include "utilities.h"

class SndFile {
    
public:
    SndFile();
    ~SndFile();
    bool read(std::string path, int sampleRate);
    
    void normalise();
    Audio getAudio() const;
    std::vector<float> getAudioSamples() const;
    int getSamplerate() const;
    bool resample(const int &samplerate);
    bool save();
    
private:
    Audio audio;
    ExtAudioFileRef fileRef;
    void checkResult(OSStatus error, const char *operation);
};



SndFile::SndFile() {
    //
}

SndFile::~SndFile() {
    //
}

// open a WAV file using libsndfile and store audio samples in vector
bool SndFile::read(std::string path, int sampleRate) {
    int numSamples = 1024;

    ExtAudioFileRef fileRef;

    CFStringRef str = CFStringCreateWithCString(NULL, (char *)path.c_str(), kCFStringEncodingMacRoman);
    
    CFURLRef inputFileURL = CFURLCreateWithFileSystemPath(
        kCFAllocatorDefault,
        str,
        kCFURLPOSIXPathStyle,
        false
    );
    
    checkResult(
        ExtAudioFileOpenURL(
            inputFileURL,
            &fileRef
        ),
        "ExtAudioFileOpenURL failed"
    );
    
    
    // "sample"  - An instantaneous amplitude of the signal in a single audio channel, represented as an integer, floating-point, or fixed-point number
    // "channel" - A discrete track of audio. A monaural recording or live performance has exactly one channel.
    // "frame"   - A set of samples that contains one sample from each channel in an audio data stream
    // "packet"  - An encoding-defined unit of audio data comprising one or more frames. For PCM audio, each packet corresponds to one frame.
    
    // A "packet" contains a "frame" which contains "channels" which contain "samples".
    // For mono PCM, there is one channel, so one channel per frame, and one frame per packet.  So each packet contains only one sample.
    
    // 2) Set up audio format we want the data in
    // Each sample is of type Float32
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = sampleRate;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat;
    audioFormat.mBitsPerChannel = sizeof(Float32) * 8;
    audioFormat.mChannelsPerFrame = 1; // Mono
    audioFormat.mBytesPerFrame = audioFormat.mChannelsPerFrame * sizeof(Float32);  // == sizeof(Float32)
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mBytesPerPacket = audioFormat.mFramesPerPacket * audioFormat.mBytesPerFrame; // = sizeof(Float32)
    
    // 3) Apply audio format to my Extended Audio File
    checkResult(
        ExtAudioFileSetProperty(
            fileRef,
            kExtAudioFileProperty_ClientDataFormat,
            sizeof (AudioStreamBasicDescription), //= audioFormat
            &audioFormat
        ),
        "Couldn't set client data format on input ext file"
    );
    
    
    UInt32 sizePerPacket = audioFormat.mBytesPerPacket; // = sizeof(Float32) = 32bytes
    UInt32 packetsPerBuffer = numSamples;
    UInt32 outputBufferSize = packetsPerBuffer * sizePerPacket;
    
    // So the lvalue of outputBuffer is the memory location where we have reserved space
    UInt8 *outputBuffer = (UInt8 *)malloc(sizeof(UInt8 *) * outputBufferSize);
    
    AudioBufferList convertedData;
    convertedData.mNumberBuffers = 1;       // Set this to 1 for mono
    convertedData.mBuffers[0].mNumberChannels = audioFormat.mChannelsPerFrame;  //also = 1
    convertedData.mBuffers[0].mDataByteSize = outputBufferSize;
    convertedData.mBuffers[0].mData = outputBuffer; // <-- pointer to our output buffer
    
    UInt32 frameCount = numSamples;
    while (frameCount > 0) {
        checkResult(
            ExtAudioFileRead(
                fileRef,
                &frameCount,       // <-- On "in" = how many frames to read.  On "out" = how many frames were read
                &convertedData // <-- Our ABL, which contains a pointer (at mData) where to put data.
             ),
            "ExtAudioFileRead failed"
        );
        
        if (frameCount > 0) {
            AudioBuffer audioBuffer = convertedData.mBuffers[0];
            float *samplesAsCArray = (float *)audioBuffer.mData;
            std::vector<float> samplesAsVector;
            samplesAsVector.assign(samplesAsCArray, samplesAsCArray + frameCount);
            audio.samples.insert(audio.samples.end(), samplesAsVector.begin(), samplesAsVector.end());
        }
    }
    audio.samplerate = sampleRate;
    return true;
}

// normalise the audio file to -3 dB
void SndFile::normalise() {
    float peak = 0;
    for(unsigned int i=0; i<audio.samples.size(); ++i) {
        peak = std::max(fabs(audio.samples[i]), peak);
    }
    peak *= 0.707;
    for(unsigned int i=0; i<audio.samples.size(); ++i) {
        audio.samples[i] /= peak;
    }
}

//returns audio struct
Audio SndFile::getAudio() const {
    return audio;
}

//access to the audio samples
std::vector<float> SndFile::getAudioSamples() const {
    return audio.samples;
}

int SndFile::getSamplerate() const {
    return audio.samplerate;
}

// save wav to file
bool SndFile::save() {
    return true;
}

void SndFile::checkResult(OSStatus error, const char *operation) {
    if (error == noErr) return;
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[1]) && isprint(errorString[2]) &&
        isprint(errorString[3]) && isprint(errorString[4])) {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else {
        // No, format it as an integer
        sprintf(errorString, "%d", (int)error);
    }
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
    exit(1);
}

#endif