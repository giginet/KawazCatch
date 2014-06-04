#include "AppDelegate.h"
#include "TitleScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }
    
    // turn on display FPS
    director->setDisplayStats(true);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    // 画像リソースをセットする
    std::vector<std::string> searchResolutionOrder;
    auto platform = this->getTargetPlatform(); // 現在のプラットフォーム
    auto frameSize = glview->getFrameSize();
    if (platform == Platform::OS_IPHONE) {
        if (frameSize.height > 480.f) { // Retina
            director->setContentScaleFactor(2.0f);
            if (frameSize.height == 1136) { // iPhone 4inch
                glview->setDesignResolutionSize(320, 568, ResolutionPolicy::NO_BORDER);
                searchResolutionOrder.push_back("4inch");
            } else { // 3.5インチ
                glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
            }
            searchResolutionOrder.push_back("retina");
        } else { // non-Retina
            glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
        }
    } else if (platform == Platform::OS_ANDROID) {
        glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
    }
    searchResolutionOrder.push_back("image");
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);
    
    // create a scene. it's an autorelease object
    auto scene = Title::createScene();
    
    // run
    director->runWithScene(scene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
