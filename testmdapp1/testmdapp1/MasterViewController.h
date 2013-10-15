//
//  MasterViewController.h
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/13.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface MasterViewController : UITableViewController

@property (strong, nonatomic) DetailViewController *detailViewController;

@end
