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



#ifndef BIOACOUSTICS_H
#define BIOACOUSTICS_H

#import <cstdlib>
#import "detect.h"
#import "sndIO.h"
#import "extract.h"
#import "analyse.h"
#import "libsvm.h"
#import "labels.h"
#import "scale.h"


using namespace std;

class Bioacoustics {
public:
    deque<Event> analyse(string file, string modelPath);
    
private:
    bool checkML(string modelPath);
    // Types
    vector<LibSVM> svm;
    vector<Scale> scale;
    vector<Labels> labels;
    map<std::string, int> classes;
    deque<Event> results; // Somewhere to store the results
    bool bML = false;
    bool bDraw = false;
};


deque<Event>  Bioacoustics::analyse(string path, string modelPath) {
    
	// Initial Settings
    
	int detectThreshold = 12;
	float expDecay = 0.98;
	int TE = 10;
	float minDuration = 1.5;
    int startThreshold = -40;
    int endThreshold = -26;
    int angleThresh = 40;
    int snrThresh = 6;
	int filterCutoff = 16.0;
	int fftSize = 256;
	double overlap = 0.875;		//fft Overlap
	window_type  window_t = BlackmanHarris7;
	float smoothingGain = 0.2;
    float Qk = 0.00001;
    float Rk = 0.0001;
    int sampleRate = 50000;
    
    
	//------------------------------------------------
	// Assume low sampling rate recordings have been
	// time expanded by the user supplied factor
	//------------------------------------------------
	
	SndFile snd;
	snd.read(path, sampleRate);
	Audio audio = snd.getAudio();
	if(audio.samplerate < 192000) {
		audio.samplerate *= TE;
		audio.timeExpansionFactor = TE;
	}
    
    Detect detect(fftSize, overlap, window_t, expDecay);
	std::deque<Detection> detections = detect.process(audio, detectThreshold, filterCutoff);
	for (int i = 0; i < detections.size(); i++) {
		printf("detection[%u] = %u\n", i, detections[i].sampleLocation);
	}
    
	Extract extract(
                    fftSize,
                    overlap,
                    window_t,
                    -startThreshold,
                    -endThreshold,
                    angleThresh,
                    smoothingGain,
                    snrThresh,
                    Qk,
                    Rk
                    );
    
    
	Analyse analyse;
	string className;           // input buffer for classes
	int endSample = 0;
	double duration = 0;
	double to_ms = 1000.0*(fftSize * (1.0-overlap))/(double)audio.samplerate;
	int N = 0;
    
	bML = checkML(modelPath);		// Check if we have SVM models
    
	for(unsigned int i=0; i<detections.size(); ++i) {
		Detection detection = detections[i];
		// ignore double detections
		if(detection.sampleLocation<endSample) continue;
        
		//------------------------------------------------
		// Extract the event from the background noise
		//------------------------------------------------
        
		Event event = extract.process(audio, detection,	path, filterCutoff);
        
		// duration in ms
		double d = event.features.duration*to_ms;
		if(d < minDuration) continue;// ignore short events
        
		//------------------------------------------------
		// Analyse the extracted event to measure features
		// suitable for classification
		//------------------------------------------------
        
		std::deque<double> features = analyse.process(event, Qk, Rk);
        
		//------------------------------------------------
		// Remove calls that have extracted twice
		//------------------------------------------------
        
		if(event.startSample < endSample) {
			if(event.features.duration > duration) {
				//first call probably didn't extract properly
				if(!results.empty()) {
					results.pop_back();
                    //					draw.pop_back();
					--N;
				}
			}
			else {
				//this is a double detection
				continue;
			}
		}
		endSample = event.endSample;
		duration = event.features.duration;
        
        
        
		//------------------------------------------------
		// Classify features using SVM
		//------------------------------------------------
        
		if(bML) {
			// Classify using each model
			// To do: threshold on classifications
			std::pair<double, std::string> win(0.0,"unknown");
			for(unsigned int j=0; j < svm.size(); ++j) {
				LibSVM classifier = svm[j];
				Labels lab = labels[j];
				Scale s = scale[j];
				std::deque<double> tmp(features);
                
				if(s.scaleFeatures(tmp)) {
					std::pair<int, double> p = classifier.predict(tmp);
					className = lab.getLabel((p.first-1));
					// ignore if classified as "Other/other"
					if(!className.compare("other")) continue;
					if(!className.compare("Other")) continue;
                    
					//keep track of class names
					classes[className]++;
                    
					if(p.second > win.first) {
						win.first = p.second;
						win.second = className;
					}
				}
			}
			event.classification = win.second;
			event.p_value = win.first;
		}
		event.id = ++N;
		results.push_back(event);
        
	} // end detections loop
    
	return results;
}

bool Bioacoustics::checkML(string modelPath) {
    NSLog(@"Model Path: %s \n", modelPath.c_str());
    svm.clear();
    labels.clear();
    scale.clear();
    
	std::vector<std::string> list(6);
	list[0] = modelPath + "/Bbar_01";
	list[1] = modelPath + "/Myotis_01";
	list[2] = modelPath + "/NSL_01";
	list[3] = modelPath + "/Paur_01";
	list[4] = modelPath + "/Pip_01";
	list[5] = modelPath + "/Rhino_01";
    
    for(int i = 0; i < list.size(); ++i) {
        LibSVM classifier;
        bML = classifier.loadModel(list[i] + ".svm");
        svm.push_back(classifier);
        if(!bML) return bML;
    }
    
    for(int i = 0; i < list.size(); ++i) {
        Labels lab;
        bML = lab.read(list[i] + ".labels");
        labels.push_back(lab);
        if(!bML) return bML;
    }
    
    for(int i = 0; i < list.size(); ++i) {
        Scale s;
        bML = s.read(list[i] + ".scale");
        scale.push_back(s);
        if(!bML) return bML;
    }
    
    return bML;
}



#endif