/*
 Launchdialog.m - iOS launch dialog with dynamic button sizing, transparent UI, and background selection
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
int g_buttonSize = 45;
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
	if( dir ) return dir;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirctory = [paths objectAtIndex:0];
	[[NSFileManager defaultManager] createDirectoryAtPath:documentsDirctory withIntermediateDirectories:YES attributes:nil error:nil];
	dir = [documentsDirctory fileSystemRepresentation];
	return dir;
}

const char *IOS_GetExecDir(void)
{
	static const char *dir = NULL;
	if( dir ) return dir;
	dir = [[[NSBundle mainBundle] bundleURL] fileSystemRepresentation];
	return dir;
}

@interface LaunchDialogViewController : UIViewController <UITextFieldDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate>
{
	UITextField *argsTextField;
	UITextField *suffixTextField;
	UISwitch *devModeSwitch;
	UISlider *buttonSizeSlider;
	UIView *cardView;
	UIImageView *bgView;
	
	NSString *savedArgsText;
	NSString *savedSuffixText;
}

@property (nonatomic, assign) BOOL shouldStart;

@end

@implementation LaunchDialogViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	
	savedArgsText = @"-game hl2sbpp";
	savedSuffixText = @"";
	
	[self loadSettings];
	
	// Закриття клавіатури за тапом
	UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
	tap.cancelsTouchesInView = NO;
	[self.view addGestureRecognizer:tap];
	
	// 1. Фоновій малюнок
	NSString *docsDir = [NSString stringWithUTF8String:IOS_GetDocsDir()];
	NSString *customBgPath = [docsDir stringByAppendingPathComponent:@"launcher_bg.png"];
	UIImage *bgImage = nil;
	
	// Спочатку шукаємо користувацький фон
	if ([[NSFileManager defaultManager] fileExistsAtPath:customBgPath]) {
		bgImage = [UIImage imageWithContentsOfFile:customBgPath];
	}
	
	// Якщо нема, шукаємо вбудований
	if (!bgImage) {
		NSString *bgPath = [[NSBundle mainBundle] pathForResource:@"launcher_bg" ofType:@"png"];
		if (!bgPath) bgPath = [[NSBundle mainBundle] pathForResource:@"launcher_bg" ofType:@"PNG"];
		bgImage = bgPath ? [UIImage imageWithContentsOfFile:bgPath] : nil;
	}
	
	if (bgImage) {
		bgView = [[UIImageView alloc] initWithImage:bgImage];
		bgView.frame = self.view.bounds;
		bgView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
		bgView.contentMode = UIViewContentModeScaleAspectFill;
		bgView.clipsToBounds = YES;
		[self.view insertSubview:bgView atIndex:0];
	} else {
		self.view.backgroundColor = [UIColor colorWithRed:0.15 green:0.15 blue:0.15 alpha:1.0];
	}
	
	// 2. Лого + Назва додатка
	CGFloat currentX = 15.0;
	NSString *logoPath = [[NSBundle mainBundle] pathForResource:@"logo" ofType:@"png"];
	if (!logoPath) logoPath = [[NSBundle mainBundle] pathForResource:@"logo" ofType:@"PNG"];
	UIImage *logoImg = logoPath ? [UIImage imageWithContentsOfFile:logoPath] : nil;
	
	if (logoImg) {
		UIImageView *logoView = [[UIImageView alloc] initWithImage:logoImg];
		logoView.frame = CGRectMake(currentX, 15, 45, 45);
		logoView.contentMode = UIViewContentModeScaleAspectFit;
		[self.view addSubview:logoView];
		currentX += 55.0;
	}
	
	UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(currentX, 15, 300, 45)];
	titleLabel.text = @"HL2SB+++";
	titleLabel.textColor = [UIColor whiteColor];
	
	UIFont *customFont = [UIFont fontWithName:@"AppleSDGothicNeo-SemiBold" size:22.0];
	if (!customFont) {
		customFont = [UIFont systemFontOfSize:22.0 weight:UIFontWeightSemibold];
	}
	titleLabel.font = customFont;
	[self.view addSubview:titleLabel];

	// Кнопка налаштувань
	UIButton *settingsBtn = [[UIButton alloc] initWithFrame:CGRectMake(self.view.bounds.size.width - 50, 15, 40, 40)];
	[settingsBtn setTitle:@"⚙" forState:UIControlStateNormal];
	[settingsBtn.titleLabel setFont:[UIFont systemFontOfSize:26]];
	[settingsBtn addTarget:self action:@selector(showSettings) forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:settingsBtn];
	
	// 3. Побудова картки з кнопками
	[self rebuildCardView];
}

- (void)rebuildCardView
{
	if (argsTextField) savedArgsText = argsTextField.text;
	if (suffixTextField) savedSuffixText = suffixTextField.text;
	
	if (cardView) {
		[cardView removeFromSuperview];
	}
	
	CGFloat cardWidth = 340;
	CGFloat btnHeight = g_buttonSize;
	CGFloat contentHeight = g_devMode ? 145 : 80;
	CGFloat cardHeight = contentHeight + btnHeight;
	
	cardView = [[UIView alloc] initWithFrame:CGRectMake((self.view.bounds.size.width - cardWidth) / 2, (self.view.bounds.size.height - cardHeight) / 2, cardWidth, cardHeight)];
	cardView.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.35];
	cardView.layer.cornerRadius = 14;
	cardView.clipsToBounds = YES;
	cardView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin;
	[self.view addSubview:cardView];
	
	// Arguments label
	UILabel *argsLabel = [[UILabel alloc] initWithFrame:CGRectMake(15, 10, cardWidth - 30, 20)];
	argsLabel.text = @"Command-line arguments:";
	argsLabel.font = [UIFont systemFontOfSize:13 weight:UIFontWeightMedium];
	argsLabel.textColor = [UIColor blackColor];
	[cardView addSubview:argsLabel];
	
	// Arguments TextField
	argsTextField = [[UITextField alloc] initWithFrame:CGRectMake(15, 32, cardWidth - 30, 36)];
	argsTextField.placeholder = @"-game hl2sbpp";
	argsTextField.text = savedArgsText;
	argsTextField.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.4];
	argsTextField.textColor = [UIColor blackColor];
	argsTextField.font = [UIFont systemFontOfSize:14];
	argsTextField.layer.cornerRadius = 6;
	argsTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 8, 0)];
	argsTextField.leftViewMode = UITextFieldViewModeAlways;
	argsTextField.delegate = self;
	[cardView addSubview:argsTextField];
	
	// Dev Mode suffix block
	if (g_devMode) {
		UILabel *suffixLabel = [[UILabel alloc] initWithFrame:CGRectMake(15, 75, cardWidth - 30, 20)];
		suffixLabel.text = @"Library suffix:";
		suffixLabel.font = [UIFont systemFontOfSize:13 weight:UIFontWeightMedium];
		suffixLabel.textColor = [UIColor blackColor];
		[cardView addSubview:suffixLabel];
		
		suffixTextField = [[UITextField alloc] initWithFrame:CGRectMake(15, 97, cardWidth - 30, 36)];
		suffixTextField.text = savedSuffixText;
		suffixTextField.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.4];
		suffixTextField.textColor = [UIColor blackColor];
		suffixTextField.font = [UIFont systemFontOfSize:14];
		suffixTextField.layer.cornerRadius = 6;
		suffixTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 8, 0)];
		suffixTextField.leftViewMode = UITextFieldViewModeAlways;
		suffixTextField.delegate = self;
		[cardView addSubview:suffixTextField];
	}
	
	// Кнопки Exit та Start
	CGFloat btnWidth = cardWidth / 2;
	
	UIButton *exitBtn = [[UIButton alloc] initWithFrame:CGRectMake(0, cardHeight - btnHeight, btnWidth, btnHeight)];
	[exitBtn setTitle:@"Exit" forState:UIControlStateNormal];
	[exitBtn setBackgroundColor:[UIColor colorWithRed:1.0 green:0.2 blue:0.2 alpha:0.65]];
	[exitBtn.titleLabel setFont:[UIFont systemFontOfSize:15 weight:UIFontWeightMedium]];
	[exitBtn addTarget:self action:@selector(exitPressed) forControlEvents:UIControlEventTouchUpInside];
	[cardView addSubview:exitBtn];
	
	UIButton *startBtn = [[UIButton alloc] initWithFrame:CGRectMake(btnWidth, cardHeight - btnHeight, btnWidth, btnHeight)];
	[startBtn setTitle:@"Start" forState:UIControlStateNormal];
	[startBtn setBackgroundColor:[UIColor colorWithRed:0.2 green:0.9 blue:0.3 alpha:0.65]];
	[startBtn.titleLabel setFont:[UIFont systemFontOfSize:15 weight:UIFontWeightMedium]];
	[startBtn addTarget:self action:@selector(startPressed) forControlEvents:UIControlEventTouchUpInside];
	[cardView addSubview:startBtn];
}

- (void)dismissKeyboard
{
	[self.view endEditing:YES];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	return YES;
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
		savedArgsText = @(settings.args);
		savedSuffixText = @(settings.suffix);
		g_devMode = settings.devMode != 0;
		g_buttonSize = (settings.buttonSize >= 30 && settings.buttonSize <= 90) ? settings.buttonSize : 45;
		fclose(settingsfile);
	} else {
		savedArgsText = @"-game hl2sbpp";
		g_devMode = false;
		g_buttonSize = 45;
	}
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
		strlcpy(settings.suffix, [suffixTextField.text ? suffixTextField.text : @"" UTF8String], 32);
		settings.magic = SETTINGS_MAGIC;
		settings.devMode = g_devMode ? 1 : 0;
		settings.buttonSize = g_buttonSize;
		fwrite(&settings, sizeof(settings), 1, settingsfile);
		fclose(settingsfile);
	}
}

- (void)showSettings
{
	UIAlertController *settingsAlert = [UIAlertController alertControllerWithTitle:@"Settings" message:@"\n\n\n\n\n\n" preferredStyle:UIAlertControllerStyleAlert];
	
	UIViewController *customVC = [[UIViewController alloc] init];
	customVC.preferredContentSize = CGSizeMake(270, 180);
	
	// BG: SET button
	UIButton *bgButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 10, 250, 35)];
	[bgButton setTitle:@"SET BACKGROUND" forState:UIControlStateNormal];
	[bgButton setBackgroundColor:[UIColor colorWithRed:0.2 green:0.5 blue:0.9 alpha:0.7]];
	bgButton.layer.cornerRadius = 6;
	[bgButton.titleLabel setFont:[UIFont systemFontOfSize:14 weight:UIFontWeightMedium]];
	[bgButton addTarget:self action:@selector(selectBackground) forControlEvents:UIControlEventTouchUpInside];
	[customVC.view addSubview:bgButton];
	
	// Developer Mode switch
	UILabel *devLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 55, 150, 30)];
	devLabel.text = @"Developer Mode";
	devLabel.font = [UIFont systemFontOfSize:14];
	[customVC.view addSubview:devLabel];
	
	devModeSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(180, 55, 50, 30)];
	devModeSwitch.on = g_devMode;
	[devModeSwitch addTarget:self action:@selector(devModeChanged:) forControlEvents:UIControlEventValueChanged];
	[customVC.view addSubview:devModeSwitch];
	
	// Button Height slider
	UILabel *sliderLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 100, 150, 20)];
	sliderLabel.text = @"Button Height";
	sliderLabel.font = [UIFont systemFontOfSize:14];
	[customVC.view addSubview:sliderLabel];
	
	buttonSizeSlider = [[UISlider alloc] initWithFrame:CGRectMake(10, 130, 220, 20)];
	buttonSizeSlider.minimumValue = 35;
	buttonSizeSlider.maximumValue = 80;
	buttonSizeSlider.value = g_buttonSize;
	[customVC.view addSubview:buttonSizeSlider];
	
	[settingsAlert setValue:customVC forKey:@"contentViewController"];
	
	UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
		g_buttonSize = (int)buttonSizeSlider.value;
		[self saveSettings];
		[self rebuildCardView];
	}];
	
	[settingsAlert addAction:okAction];
	[self presentViewController:settingsAlert animated:YES completion:nil];
}

- (void)selectBackground
{
	UIImagePickerController *picker = [[UIImagePickerController alloc] init];
	picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
	picker.delegate = self;
	
	UIViewController *presentedController = self.presentedViewController;
	if (presentedController) {
		[presentedController presentViewController:picker animated:YES completion:nil];
	} else {
		[self presentViewController:picker animated:YES completion:nil];
	}
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
	UIImage *image = [info objectForKey:UIImagePickerControllerOriginalImage];
	
	NSString *docsDir = [NSString stringWithUTF8String:IOS_GetDocsDir()];
	NSString *imagePath = [docsDir stringByAppendingPathComponent:@"launcher_bg.png"];
	
	NSData *imageData = UIImagePNGRepresentation(image);
	[imageData writeToFile:imagePath atomically:YES];
	
	[picker dismissViewControllerAnimated:YES completion:^{
		[self dismissViewControllerAnimated:YES completion:^{
			if (image && bgView) {
				bgView.image = image;
			} else if (image) {
				bgView = [[UIImageView alloc] initWithImage:image];
				bgView.frame = self.view.bounds;
				bgView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
				bgView.contentMode = UIViewContentModeScaleAspectFill;
				bgView.clipsToBounds = YES;
				[self.view insertSubview:bgView atIndex:0];
			}
		}];
	}];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	[picker dismissViewControllerAnimated:YES completion:nil];
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
	
	NSArray *argv = [argsTextField.text componentsSeparatedByString:@" "];
	int count = [argv count];
	szArgv = calloc(count + 2, sizeof(char*));
	
	for(int i = 0; i < count; i++) {
		szArgv[i + 1] = strdup([[argv objectAtIndex:i] UTF8String]);
	}
	
	szArgc = count + 1;
	szArgv[count + 1] = 0;
	szArgv[0] = strdup(IOS_GetExecDir());
	
	CFRunLoopStop(CFRunLoopGetCurrent());
}

@end

void IOS_PrepareView(void) {}

void IOS_LaunchDialog(void)
{
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
