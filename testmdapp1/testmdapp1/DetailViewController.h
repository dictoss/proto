//
//  DetailViewController.h
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/13.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController <UISplitViewControllerDelegate>

@property (strong, nonatomic) id detailItem;

//@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel1;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel2;
@end
