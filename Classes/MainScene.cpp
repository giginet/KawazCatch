//
//  MainScene.cpp
//  KawazCatch
//
//  Created by giginet on 5/15/14.
//
//

#include "MainScene.h"
#include "TitleScene.h"
#include <random>
#include "SimpleAudioEngine.h"

USING_NS_CC;

/// マージン
const int FRUIT_TOP_MERGIN = 40;
/// 制限時間
const float TIME_LIMIT_SECOND = 60;
/// 落下速度
const float FALLING_DURATION = 3.0;

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}

bool MainScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // 初期スコアの設定
    _score = 0;
    
    // 背景を表示する
    auto director = Director::getInstance();
    auto size = director->getWinSize();
    auto background = Sprite::create("background.png");
    background->setPosition(Point(size.width / 2.0, size.height / 2.0));
    this->addChild(background);
    
    // プレイヤーを表示する
    this->setPlayer(Sprite::create("player.png"));
    _player->setPosition(Point(size.width / 2.0, 220));
    this->addChild(_player);
    
    // イベントリスナーの追加
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        // タッチされたとき
        return true;
    };
    listener->onTouchMoved = [this, size](Touch* touch, Event* event) {
        // タッチ位置が動いたとき
        Point delta = touch->getDelta(); // 前回とのタッチ位置との差をベクトルで取得する
        Point position = _player->getPosition(); // 現在のかわずたんの座標を取得する
        Point newPosition = position + delta;
        newPosition = newPosition.getClampPoint(Point(0, position.y), Point(size.width, position.y));
        _player->setPosition(newPosition); // 現在座標 + 移動量を新たな座標にする
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // スコアラベルの追加
    auto scoreLabel = Label::createWithSystemFont(std::to_string(_score), "Helvetica", 64);
    scoreLabel->enableShadow();
    scoreLabel->enableOutline(Color4B::RED, 5);
    this->setScoreLabel(scoreLabel);
    _scoreLabel->setPosition(Point(size.width / 2.0 * 1.5, size.height - 60));
    this->addChild(_scoreLabel);
    
    // タイマーラベルの追加
    _second = TIME_LIMIT_SECOND;
    auto secondLabel = Label::createWithSystemFont(std::to_string((int)_second), "Helvetica", 64);
    secondLabel->enableShadow();
    secondLabel->enableOutline(Color4B::RED, 5);
    secondLabel->setPosition(Point(size.width / 2.0, size.height - 60));
    this->setSecondLabel(secondLabel);
    this->addChild(_secondLabel);
    this->scheduleUpdate();
    
    _lot = 0;
    std::random_device rdev;
    _engine.seed(rdev());
    
    // 初期状態をREADYにする
    _state = GameState::READY;
    
    return true;
}

MainScene::~MainScene()
{
    // デストラクタ
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
}

void MainScene::onEnterTransitionDidFinish()
{
    // シーン遷移が完了したとき
    Layer::onEnterTransitionDidFinish();
    
    // BGMを鳴らす
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);
    
    this->addReadyLabel();
}

void MainScene::update(float dt)
{
    if (_state == GameState::PLAYING) {
        // PLAYING状態の時
        if (_second > FALLING_DURATION && _lot == 0) {
            std::binomial_distribution<> dest(_second * 2.0, 0.5);
            _lot = dest(_engine);
            this->addFruit();
        } else {
            --_lot;
        }
        
        for (auto fruit : _fruits) {
            auto busketPosition = _player->getPosition() - Point(0, 10);
            bool isHit = fruit->getBoundingBox().containsPoint(busketPosition);
            if (isHit) {
                this->catchFruit(fruit);
            }
        }
        _second -= dt;
        _secondLabel->setString(std::to_string((int)_second));
        if (_second < 0) {
            _state = GameState::ENDING;
            // 終了文字の表示
            auto finish = Sprite::create("finish.png");
            auto winSize = Director::getInstance()->getWinSize();
            finish->setPosition(Point(winSize.width / 2.0, winSize.height / 2.0));
            finish->setScale(0);
            auto appear = EaseExponentialIn::create(ScaleTo::create(0.25, 1.0));
            auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25, 0));
        
            finish->runAction(Sequence::create(appear,
                                               DelayTime::create(0.5),
                                               disappear,
                                               DelayTime::create(1.0),
                                               CallFunc::create([this] {
                _state = GameState::RESULT;
                this->addResultMenu();
            }),
                                               NULL));
            this->addChild(finish);
        }
    }
}

Sprite* MainScene::addFruit()
{
    std::uniform_int_distribution<> dist(0, (int)FruitType::COUNT - 1);
    
    auto winSize = Director::getInstance()->getWinSize();
    int fruitNumber = dist(_engine);
    
    std::string filename = "fruit" + std::to_string(fruitNumber) + ".png";
    auto fruit = Sprite::create(filename);
    fruit->setTag(fruitNumber);
    
    auto fruitSize = fruit->getContentSize();
    float min = fruitSize.width / 2.0;
    float max = winSize.width - fruitSize.width / 2.0;
    std::uniform_int_distribution<float> posDist(min, max);
    float fruitXPos = posDist(_engine);
    
    fruit->setPosition(Point(fruitXPos,
                             winSize.height - FRUIT_TOP_MERGIN - fruitSize.height / 2.0));
    this->addChild(fruit);
    _fruits.pushBack(fruit);
    
    auto ground = Point(fruitXPos, 0);
    fruit->runAction(Sequence::create(MoveTo::create(FALLING_DURATION, ground),
                                      RemoveSelf::create(),
                                      NULL));
    return fruit;
}

void MainScene::catchFruit(cocos2d::Sprite *fruit)
{
    fruit->removeFromParent();
    _fruits.eraseObject(fruit);
    _score += 1;
    _scoreLabel->setString(std::to_string(_score));
}

void MainScene::addReadyLabel()
{
    auto winSize = Director::getInstance()->getWinSize();
    auto center = Point(winSize.width / 2.0, winSize.height / 2.0);
    auto ready = Sprite::create("ready.png");
    ready->setScale(0);
    ready->setPosition(center);
    this->addChild(ready);
    auto start = Sprite::create("start.png");
    start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(1.0, 5.0), 0.5),
                                                      FadeOut::create(1.0),
                                                      NULL),
                                      RemoveSelf::create(), NULL));
    start->setPosition(center);
    ready->runAction(Sequence::create(ScaleTo::create(0.25, 1),
                                      DelayTime::create(1.0),
                                      CallFunc::create([this, start] {
        this->addChild(start);
        _state = GameState::PLAYING;
    }),
                                      RemoveSelf::create(),
                                      NULL));
    
}

void MainScene::addResultMenu()
{
    // ENDING状態のとき
    _state = GameState::RESULT;
    auto winSize = Director::getInstance()->getWinSize();
    // 「もう一度遊ぶ」ボタン
    auto replayButton = MenuItemImage::create("replay_button.png",
                                              "replay_button_pressed.png",
                                              [this](Ref* ref) {
                                                  // 「もう一度遊ぶ」ボタンを押したときの処理
                                                  auto scene = MainScene::createScene();
                                                  auto transition = TransitionFade::create(0.5, scene);
                                                  Director::getInstance()->replaceScene(transition);
                                              });
    // 「タイトルへ戻る」ボタン
    auto titleButton = MenuItemImage::create("title_button.png",
                                             "title_button_pressed.png",
                                             [this](Ref* ref) {
                                                 // 「タイトルへ戻る」ボタンを押したときの処理
                                                 auto scene = TitleScene::createScene();
                                                 auto transition = TransitionCrossFade::create(0.5, scene);
                                                 Director::getInstance()->replaceScene(transition);
                                             });
    auto menu = Menu::create(replayButton, titleButton, NULL);
    menu->alignItemsVerticallyWithPadding(30);
    menu->setPosition(Point(winSize.width / 2.0, winSize.height / 2.0));
    this->addChild(menu);
}