//
//  MenuData.m
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/14.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import "MenuData.h"

@interface MenuData ()
{
    NSString *_name;
    NSString *_value;
}
@end

@implementation MenuData : NSObject

@synthesize name = _name;
@synthesize value = _value;

- (id)init {
    self = [super init];
    return self;
}

- (id)initWithName:(NSString*)name {
    self = [super init];
    if(self){
        self.name = name;
        self.value = [name stringByAppendingString:@"-value"];
    }
    
    return self;
}

@end
