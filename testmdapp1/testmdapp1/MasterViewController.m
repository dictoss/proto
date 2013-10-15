//
//  MasterViewController.m
//  testmdapp1
//
//  Created by SUGIMOTO Norimitsu on 2013/07/13.
//  Copyright (c) 2013年 SUGIMOTO Norimitsu. All rights reserved.
//

#import "MasterViewController.h"
#import "DetailViewController.h"

@interface MasterViewController () {
    NSMutableArray *_objects;
}
@end

@implementation MasterViewController

- (void)awakeFromNib
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        self.clearsSelectionOnViewWillAppear = NO;
        self.contentSizeForViewInPopover = CGSizeMake(320.0, 600.0);
    }
    [super awakeFromNib];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    if (!_objects) {
        _objects = [[NSMutableArray alloc] init];
    }
    
	// Do any additional setup after loading the view, typically from a nib.
    //self.navigationItem.leftBarButtonItem = self.editButtonItem;

    //UIBarButtonItem *addButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(insertNewObject:)];
    //self.navigationItem.rightBarButtonItem = addButton;
    
    self.detailViewController = (DetailViewController *)[[self.splitViewController.viewControllers lastObject] topViewController];
    
    /*
    NSJSONSerialization
     */
    NSString *apiurl = @"http://192.168.234.50/api/rest/diary/find/";
    NSURL *url = [NSURL URLWithString: apiurl];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:60.0];
    [request setHTTPMethod:@"POST"];
    
    NSMutableDictionary *reqcontrol = [NSMutableDictionary dictionary];
    NSInteger apiver = 1;
    [reqcontrol setObject:[NSNumber numberWithInteger:apiver] forKey:@"version"];
    [reqcontrol setObject:@"testuser" forKey:@""];
    [reqcontrol setObject:@"testpass" forKey:@""];

    NSMutableDictionary *reqquery = [NSMutableDictionary dictionary];
    NSMutableDictionary *keywords = [NSMutableDictionary dictionary];

    NSInteger maxrecord = 10;
    [reqquery setObject:[NSNumber numberWithInteger:maxrecord] forKey:@"maxrecord"];
    [reqquery setObject:keywords forKey:@"keyword"];
    
    NSMutableDictionary *reqjson = [NSMutableDictionary dictionary];
    [reqjson setObject:reqcontrol forKey:@"control"];
    [reqjson setObject:reqquery forKey:@"query"];
    
    NSError *error = nil;
    NSData *content = [NSJSONSerialization dataWithJSONObject:reqjson
                                                      options:NSJSONWritingPrettyPrinted
                                                        error:&error];

    [request setHTTPBody:content];

    
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
        
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
        
        NSString *code = [jsonDict objectForKey:@"code"];
        
        if([ code isEqualToString:@"200"] ){
            _objects = [jsonDict objectForKey:@"result"];
        }
        else{
            _objects = [[NSMutableArray alloc] init];
        }
        
        // reload TableView
        [self.tableView reloadData];
    }];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)insertNewObject:(id)sender
{
}

#pragma mark - Table View

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return _objects.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell" forIndexPath:indexPath];

    NSDictionary *object = _objects[indexPath.row];
    //cell.textLabel.text = [object description];
    cell.textLabel.text = object[@"subject"];
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return NO;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [_objects removeObjectAtIndex:indexPath.row];
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view.
    }
}

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        NSDictionary *object = _objects[indexPath.row];
        self.detailViewController.detailItem = object;
    }
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    //if ([[segue identifier] isEqualToString:@"showDetail"]) {
    //    NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];
    //    NSDictionary *object = _objects[indexPath.row];
    //
    //    [[segue destinationViewController] setDetailItem:object];
    //}
    NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];
    NSDictionary *object = _objects[indexPath.row];
    
    [[segue destinationViewController] setDetailItem:object];
}

@end
