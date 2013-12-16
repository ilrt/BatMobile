//
//  IBTrace.h
//  batmobile
//
//  Created by Mark van Rossum on 25/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#ifndef __batmobile__IBTrace__
#define __batmobile__IBTrace__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

class IBTrace {

public:
    vector<unordered_map<string, float>> asArray;
  
    void add_entry(float time, float freq);
    
};

#endif /* defined(__batmobile__IBTrace__) */
