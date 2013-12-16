//
//  Event.h
//  BatMobileIPhone1
//
//  Created by Mark van Rossum on 11/03/2013.
//  Copyright (c) 2013 Mark van Rossum. All rights reserved.
//

#import <Foundation/Foundation.h>
 
@interface BMEvent : NSObject

@property (nonatomic, strong) NSString *classification;
@property (nonatomic) double p_value;
@property (nonatomic) double startSample;
@property (nonatomic) double endSample;
@property (nonatomic) double sampleRate;
@property (nonatomic) int TE; //time expansion factor
 
@end


