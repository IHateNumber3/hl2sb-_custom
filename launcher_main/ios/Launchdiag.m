/*
 Launchdialog.m - iOS launch dialog with custom UI
 Copyright (C) 2016 mittorn
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 */

#import <Foundation/Foundation.h>
#include <stdlib.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <limits.h>

int szArgc;
char **szArgv;
char *g_szLibrarySuffix;
float g_iOSVer;
bool isdark;
int g_buttonSize = 60;
bool g_devMode = false;
bool g_shouldStart = false;

#define SETTINGS_MAGIC 111

typedef struct settings_s
{
	unsigned char magic;
	char args[1024];
	unsigned int port;
	char suffix[32];
	unsigned int ftpserver;
	unsigned int devMode;
	unsigned int buttonSize;
} settings_t;

const char *IOS_GetDocsDir(void)
{
	static const char *dir = NULL;
	
	if( dir )
		return dir;
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirctory = [paths objectAtIndex:0];
	[[NSFileManager defaultManager] createDirectoryAtPath:documentsDirctory withIntermediateDirectories:YES attributes:nil error:nil];
	
	dir = [documentsDirctory fileSystemRepresentation];
	NSLog(@"IOS_GetDocsDir: %s", dir);
	
	return dir;
}

const char *IOS_GetExecDir(void)
{
	static const char *dir = NULL;
	
	if( dir )
		return dir;

	dir = [[[NSBundle mainBundle] bundleURL] fileSystemRepresentation];
	NSLog(@"IOS_GetExecDir: %s", dir);
	
	return dir;
}

@interface LaunchDialogViewController : UIViewController <UITextFieldDelegate>
{
	UITextField *argsTextField;
	UITextField *suffixTextField;
	UISwitch *devModeSwitch;
	UISlider *buttonSizeSlider;
	UIView *suffixContainer;
}

@property (nonatomic, assign) BOOL shouldStart;

@end

@implementation LaunchDialogViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	
	// Set background image
	UIImage *bgImage = [UIImage imageNamed:@"launcher_bg"];
	if(!bgImage) {
		bgImage = [UIImage imageNamed:@"launcher_bg.png"];
	}
	if(bgImage) {
		UIImageView *bgView = [[UIImageView alloc] initWithImage:bgImage];
		bgView.frame = self.view.bounds;
		bgView.contentMode = UIViewContentModeScaleAspectFill;
		[self.view insertSubview:bgView atIndex:0];
	} else {
		self.view.backgroundColor = [UIColor colorWithRed:0.3 green:0.5 blue:0.3 alpha:1.0];
	}
	
	// Logo Image + MODDED (top left)
	UIImage *logoImg = [UIImage imageNamed:@"logo.png"];
	if(logoImg) {
		UIImageView *logoView = [[UIImageView alloc] initWithImage:logoImg];
		// Налаштовуємо позицію та розмір під верхній лівий кут
		logoView.frame = CGRectMake(10, 10, 60, 60);
		logoView.contentMode = UIViewContentModeScaleAspectFit;
		[self.view addSubview:logoView];
	} else {
		// Резервний варіант, якщо logo.png відсутній
		UILabel *logoLabel = [[UILabel alloc] initWithFrame:CGRectMake(15, 20, 100, 25)];
		logoLabel.text = @"HL2SB+++";
		logoLabel.font = [UIFont boldSystemFontOfSize:16];
		logoLabel.textColor = [UIColor whiteColor];
		[self.view addSubview:logoLabel];
	}

	
	// Settings button (top right)
	UIButton *settingsBtn = [[UIButton alloc] initWithFrame:CGRectMake(self.view.bounds.size.width - 60, 40, 50, 50)];
	[settingsBtn setTitle:@"⚙" forState:UIControlStateNormal];
	[settingsBtn.titleLabel setFont:[UIFont systemFontOfSize:30]];
	[settingsBtn addTarget:self action:@selector(showSettings) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:settingsBtn];
	
	// Main content centered
	CGFloat centerY = self.view.bounds.size.height / 2 - 80;
	CGFloat centerX = self.view.bounds.size.width / 2;
	
	// Command-line arguments label
	UILabel *argsLabel = [[UILabel alloc] initWithFrame:CGRectMake(30, centerY, 300, 25)];
	argsLabel.text = @"Command-line arguments:";
	argsLabel.font = [UIFont systemFontOfSize:14];
	argsLabel.textColor = [UIColor whiteColor];
	[self.view addSubview:argsLabel];
	
	// Command-line arguments textfield
	argsTextField = [[UITextField alloc] initWithFrame:CGRectMake(30, centerY + 30, self.view.bounds.size.width - 60, 45)];
	argsTextField.placeholder = @"-game hl2sbpp";
	argsTextField.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.9];
	argsTextField.layer.cornerRadius = 8;
	argsTextField.layer.borderColor = [UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0].CGColor;
	argsTextField.layer.borderWidth = 1.0;
	argsTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 10, 0)];
	argsTextField.leftViewMode = UITextFieldViewModeAlways;
	argsTextField.delegate = self;
	[self.view addSubview:argsTextField];
	
	// Library suffix container (hidden until dev mode)
	suffixContainer = [[UIView alloc] initWithFrame:CGRectMake(30, centerY + 85, self.view.bounds.size.width - 60, 80)];
	suffixContainer.hidden = !g_devMode;
	
	UILabel *suffixLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 150, 25)];
	suffixLabel.text = @"Library suffix:";
	suffixLabel.font = [UIFont systemFontOfSize:14];
	suffixLabel.textColor = [UIColor whiteColor];
	[suffixContainer addSubview:suffixLabel];
	
	suffixTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, 30, suffixContainer.bounds.size.width, 45)];
	suffixTextField.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.9];
	suffixTextField.layer.cornerRadius = 8;
	suffixTextField.layer.borderColor = [UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0].CGColor;
	suffixTextField.layer.borderWidth = 1.0;
	suffixTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 10, 0)];
	suffixTextField.leftViewMode = UITextFieldViewModeAlways;
	suffixTextField.delegate = self;
	[suffixContainer addSubview:suffixTextField];
	
	[self.view addSubview:suffixContainer];
	
	// Exit button (red)
	UIButton *exitBtn = [[UIButton alloc] initWithFrame:CGRectMake(30, centerY + 170, (self.view.bounds.size.width - 60) / 2 - 7, g_buttonSize)];
	[exitBtn setTitle:@"Exit" forState:UIControlStateNormal];
	[exitBtn setBackgroundColor:[UIColor colorWithRed:1.0 green:0.2 alpha:0.2 alpha:1.0]];
	[exitBtn.titleLabel setFont:[UIFont boldSystemFontOfSize:16]];
	exitBtn.layer.cornerRadius = 8;
	[exitBtn addTarget:self action:@selector(exitPressed) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:exitBtn];
	
	// Start button (green)
	UIButton *startBtn = [[UIButton alloc] initWithFrame:CGRectMake(30 + (self.view.bounds.size.width - 60) / 2 + 7, centerY + 170, (self.view.bounds.size.width - 60) / 2 - 7, g_buttonSize)];
	[startBtn setTitle:@"Start" forState:UIControlStateNormal];
	[startBtn setBackgroundColor:[UIColor colorWithRed:0.2 green:0.8 alpha:0.2 alpha:1.0]];
	[startBtn.titleLabel setFont:[UIFont boldSystemFontOfSize:16]];
	startBtn.layer.cornerRadius = 8;
	[startBtn addTarget:self action:@selector(startPressed) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:startBtn];
	
	// Load settings
	[self loadSettings];
}

- (void)loadSettings
{
	const char *docsDir = IOS_GetDocsDir();
	char settingspath[256];
	snprintf(settingspath, sizeof(settingspath), "%s/settings.bin", docsDir);
	settingspath[255] = 0;
	
	FILE *settingsfile = fopen(settingspath, "rb");
	settings_t settings = {0};
	
	if(settingsfile && (fread(&settings, sizeof(settings), 1, settingsfile) == 1) && (settings.magic == SETTINGS_MAGIC)) {
		settings.args[1023] = 0;
		settings.suffix[31] = 0;
		[argsTextField setText:@(settings.args)];
		[suffixTextField setText:@(settings.suffix)];
		g_devMode = settings.devMode != 0;
		g_buttonSize = settings.buttonSize > 0 ? settings.buttonSize : 60;
		fclose(settingsfile);
	} else {
		[argsTextField setText:@"-game hl2sbpp"];
		g_devMode = false;
		g_buttonSize = 60;
	}
	
	suffixContainer.hidden = !g_devMode;
}

- (void)saveSettings
{
	const char *docsDir = IOS_GetDocsDir();
	char settingspath[256];
	snprintf(settingspath, sizeof(settingspath), "%s/settings.bin", docsDir);
	settingspath[255] = 0;
	
	FILE *settingsfile = fopen(settingspath, "wb");
	if(settingsfile) {
		settings_t settings;
		strlcpy(settings.args, [argsTextField.text UTF8String], 1024);
		strlcpy(settings.suffix, [suffixTextField.text UTF8String], 32);
		settings.magic = SETTINGS_MAGIC;
		settings.devMode = g_devMode ? 1 : 0;
		settings.buttonSize = g_buttonSize;
		fwrite(&settings, sizeof(settings), 1, settingsfile);
		fclose(settingsfile);
	}
}

- (void)showSettings
{
	UIAlertController *settingsAlert = [UIAlertController alertControllerWithTitle:@"Settings" message:@"" preferredStyle:UIAlertControllerStyleAlert];
	
	// Developer Mode switch
	[settingsAlert addTextFieldWithConfigurationHandler:^(UITextField *textField) {
		textField.hidden = YES;
	}];
	
	UIView *switchContainer = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 270, 60)];
	UILabel *devLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 15, 150, 30)];
	devLabel.text = @"Developer Mode";
	devLabel.font = [UIFont systemFontOfSize:14];
	[switchContainer addSubview:devLabel];
	
	devModeSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(220, 15, 50, 30)];
	devModeSwitch.on = g_devMode;
	[devModeSwitch addTarget:self action:@selector(devModeChanged:) forControlEvents:UIControlEventValueChanged];
	[switchContainer addSubview:devModeSwitch];
	
	[settingsAlert.view addSubview:switchContainer];
	
	// Button Size slider
	UIView *sliderContainer = [[UIView alloc] initWithFrame:CGRectMake(0, 60, 270, 60)];
	UILabel *sliderLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 15, 150, 30)];
	sliderLabel.text = @"Button Size";
	sliderLabel.font = [UIFont systemFontOfSize:14];
	[sliderContainer addSubview:sliderLabel];
	
	buttonSizeSlider = [[UISlider alloc] initWithFrame:CGRectMake(20, 45, 230, 20)];
	buttonSizeSlider.minimumValue = 40;
	buttonSizeSlider.maximumValue = 100;
	buttonSizeSlider.value = g_buttonSize;
	[sliderContainer addSubview:buttonSizeSlider];
	
	[settingsAlert.view addSubview:sliderContainer];
	
	settingsAlert.preferredContentSize = CGSizeMake(270, 130);
	
	UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
		g_buttonSize = (int)buttonSizeSlider.value;
		[self saveSettings];
		suffixContainer.hidden = !g_devMode;
	}];
	
	[settingsAlert addAction:okAction];
	[self presentViewController:settingsAlert animated:YES completion:nil];
}

- (void)devModeChanged:(UISwitch *)sender
{
	g_devMode = sender.on;
}

- (void)exitPressed
{
	[self saveSettings];
	exit(0);
}

- (void)startPressed
{
	[self saveSettings];
	_shouldStart = YES;
	
	// Setup args
	NSArray *argv = [argsTextField.text componentsSeparatedByString:@" "];
	int count = [argv count];
	szArgv = calloc(count + 2, sizeof(char*));
	
	for(int i = 0; i < count; i++) {
		szArgv[i + 1] = strdup([[argv objectAtIndex:i] UTF8String]);
	}
	
	szArgc = count + 1;
	szArgv[count + 1] = 0;
	szArgv[0] = strdup(IOS_GetExecDir());
	
	// Stop runloop
	CFRunLoopStop(CFRunLoopGetCurrent());
}

@end

void IOS_PrepareView(void)
{
	// Stub for compatibility
}

void IOS_LaunchDialog(void)
{
	NSLog(@"System Version is %@",[[UIDevice currentDevice] systemVersion]);
	NSString *ver = [[UIDevice currentDevice] systemVersion];
	g_iOSVer = [ver floatValue];

	char exec_dir[PATH_MAX];
	char extras_path[PATH_MAX];
	strcpy(exec_dir, IOS_GetExecDir());
	setenv("APP_LIB_PATH", exec_dir, 1);
	setenv("APP_MOD_LIB", exec_dir, 1);
	snprintf(extras_path, sizeof(extras_path), "%s/extras_dir.vpk", IOS_GetExecDir());
	setenv("EXTRAS_VPK_PATH", extras_path, 1);
	setenv("VALVE_GAME_PATH", IOS_GetDocsDir(), 1);

	const char *docsDir = IOS_GetDocsDir();
	NSString *workingDir = [NSString stringWithUTF8String:docsDir];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:workingDir];

	UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	LaunchDialogViewController *controller = [[LaunchDialogViewController alloc] init];
	window.rootViewController = controller;
	[window makeKeyAndVisible];
	
	@autoreleasepool {
		while(!controller.shouldStart) {
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
		}
	}
}

int IOS_GetArgs(char ***out)
{
	if(szArgv != NULL) {
		*out = szArgv;
		return szArgc;
	}
	return 0;
}
