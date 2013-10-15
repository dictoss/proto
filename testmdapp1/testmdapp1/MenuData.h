//
//  MenuData.h
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/14.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MenuData : NSObject

@property (nonatomic) NSString *name;
@property (nonatomic) NSString *value;

- (id)init;
- (id)initWithName:(NSString*)name;

@end
