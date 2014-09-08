#include "AppDelegate.h"
#include "TitleScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // ゲーム全体の設定
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        // Windows版やMac版を作るときにウィンドウの名前を変更できる
        glview = GLView::create("かわずたんキャッチ！");
        director->setOpenGLView(glview);
    }
    
    // デバッグ用の情報を表示させる設定。ここでは表示させていない
    director->setDisplayStats(false);
    
    // FPS (Frame per Second)の設定。ここでは1秒間に60回更新される
    director->setAnimationInterval(1.0 / 60);
    
    // Resourceの探索パスを設定する
    auto addPath = [](const char* path) {
        FileUtils::getInstance()->addSearchPath(path);
    };
    addPath("bgm");
    addPath("se");
    
    // 画像リソースを振り分ける
    // 画像リソースを検索する順番を定義する配列
    std::vector<std::string> searchResolutionOrder;
    auto platform = this->getTargetPlatform(); // 現在のプラットフォーム
    auto frameSize = glview->getFrameSize(); // 現在の端末の画面サイズ
    
    if (platform == Platform::OS_IPHONE) { // もし、iPhoneなら
        addPath("bgm/caf");
        addPath("se/caf");
        if (frameSize.height > 480.f) {
            // Retinaディスプレイのとき
            // 高解像度画像を有効にする
            director->setContentScaleFactor(2.0f);
            if (frameSize.height == 1136) {
                // iPhone 4inchのとき
                // 4インチ対応の画面サイズに変更する
                glview->setDesignResolutionSize(320, 568, ResolutionPolicy::NO_BORDER);
                // Resources/4inchフォルダに画像ファイルがあれば、最優先で利用する
                searchResolutionOrder.push_back("images/4inch");
            } else {
                // Retina 3.5インチのとき
                glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
            }
            searchResolutionOrder.push_back("images/retina");
        } else { // non-Retina 3.5インチ
            glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
        }
    } else if (platform == Platform::OS_IPAD) {
        addPath("bgm/caf");
        addPath("se/caf");
        if (frameSize.width > 1024.f) {
            director->setContentScaleFactor(2.0f);
            searchResolutionOrder.push_back("images/retina");
        } else {
            searchResolutionOrder.push_back("images/nonretina");
        }
        glview->setDesignResolutionSize(320, 480, ResolutionPolicy::SHOW_ALL);
    } else if (platform == Platform::OS_ANDROID) {
        // Android端末のとき
        addPath("bgm/ogg");
        addPath("se/ogg");
        glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
    }
    searchResolutionOrder.push_back("images/nonretina");
    // 画像の読み込み順を設定する
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);
    
    // 最初のシーンを作成する
    auto scene = TitleScene::createScene();

    // ゲームを開始する
    director->runWithScene(scene);
    
    return true;
}

// このメソッドはアプリケーションがバックグラウンドに移動した時に呼び出されます
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// このメソッドはバックグラウンドに移動したアプリケーションが再び実行されたときに呼び出されます
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
