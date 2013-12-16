//
//  IBTrace.cpp
//  batmobile
//
//  Created by Mark van Rossum on 25/04/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#include "IBTrace.h"

void IBTrace::add_entry(float time, float freq) {
    unordered_map<string, float> entry;
    entry["time"] = time;
    entry["freq"] = freq;
    asArray.push_back(entry);
}
