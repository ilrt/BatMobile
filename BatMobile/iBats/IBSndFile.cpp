#include "IBSndFile.h"

IBSndFile::IBSndFile() {
    //
}

IBSndFile::~IBSndFile() {
    //
}

// open a WAV file using IBSndfile and store audio samples in vector
bool IBSndFile::read(std::string path) {

    
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
    
    AudioStreamBasicDescription audioFormatIn;
    UInt32 propertySize = sizeof(audioFormatIn);

    checkResult(
        ExtAudioFileGetProperty( fileRef, kExtAudioFileProperty_FileDataFormat , &propertySize, &audioFormatIn ),
        "Couldn't get AudioFileProerty for input file"
    );
    
   
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = audioFormatIn.mSampleRate;
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
            sizeof (AudioStreamBasicDescription), 
            &audioFormat
        ),
        "Couldn't set client data format on input ext file"
    );
    
    UInt32 packetsPerBuffer = 1024; // The number of packets to read in at a time
    UInt32 outputBufferSize = packetsPerBuffer * audioFormat.mBytesPerPacket;
    
    // So the lvalue of outputBuffer is the memory location where we have reserved space
    UInt8 *outputBuffer = (UInt8 *)malloc(sizeof(UInt8 *) * outputBufferSize);
    
    AudioBufferList convertedData;
    convertedData.mNumberBuffers = 1;       // Set this to 1 for mono
    convertedData.mBuffers[0].mNumberChannels = audioFormat.mChannelsPerFrame;  //also = 1
    convertedData.mBuffers[0].mDataByteSize = outputBufferSize;
    convertedData.mBuffers[0].mData = outputBuffer; // <-- pointer to our output buffer
    
    UInt32 frameCount = packetsPerBuffer;
    while (frameCount > 0) {
        checkResult(
            ExtAudioFileRead(
                 fileRef,
                 &frameCount,       // <-- On "in" = how many frames to read.  On "out" = how many frames were actually read
                 &convertedData // <-- Our ABL, which contains a pointer (at mData) where to put data.
            ),
            "ExtAudioFileRead failed"
        );
        
        if (frameCount > 0) {
            AudioBuffer audioBuffer = convertedData.mBuffers[0];    // Our audio data
            float *samplesAsCArray = (float *)audioBuffer.mData;    // Convert it to a C array
            std::vector<float> samplesAsVector;
            samplesAsVector.assign(samplesAsCArray, samplesAsCArray + frameCount); // Convert the C array to a C++ vector
            audio.samples.insert(audio.samples.end(), samplesAsVector.begin(), samplesAsVector.end()); // Put the samples into the Audio object
        }
    }
    audio.sampleRate = audioFormat.mSampleRate;
    if (audio.sampleRate <= 50000) {    // If the sampleRate is less than 50kHz, assume it has been time expanded
        audio.timeExpansionFactor = 10;
    } else {
        audio.timeExpansionFactor = 1;
    }
    audio.sampleCount = audio.samples.size();
    return true;
}

//returns audio struct
IBAudio IBSndFile::getAudio() const {
    return audio;
}

//access to the audio samples
std::vector<float> IBSndFile::getAudioSamples() const {
    return audio.samples;
}

int IBSndFile::getSamplerate() const {
    return audio.sampleRate;
}


void IBSndFile::checkResult(OSStatus error, const char *operation) {
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
