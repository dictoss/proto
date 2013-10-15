//
//  DiaryData.h
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/28.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DiaryData : NSObject

@property (nonatomic) NSInteger _id;
@property (nonatomic) NSString *_subject;
@property (nonatomic) NSString *_content;
@property (nonatomic) NSString *_create_date;
@property (nonatomic) NSString *_update_date;

- (id)init;
- (id)initWithData:(NSString*) subject:(NSString*) content;

@end
