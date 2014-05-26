//
//  MainScene.cpp
//  KawazCatch
//
//  Created by giginet on 5/15/14.
//
//

#include "MainScene.h"
#include <random>

USING_NS_CC;

const int FRUIT_TOP_MERGIN = 40;
const float TIME_LIMIT_SECOND = 60;

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
    
    _score = 0;
    _second = TIME_LIMIT_SECOND;
    
    auto director = Director::getInstance();
    auto size = director->getWinSize();
    auto background = Sprite::create("background.png");
    background->setPosition(Point(size.width / 2.0, size.height / 2.0));
    this->addChild(background);
    
    this->setPlayer(Sprite::create("player.png"));
    _player->setPosition(Point(size.width / 2.0, 70));
    this->addChild(_player);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        log("Touch at (%f, %f)", touch->getLocation().x, touch->getLocation().y);
        return true;
    };
    listener->onTouchMoved = [this, size](Touch* touch, Event* event) {
        Point delta = touch->getDelta(); // 前回とのタッチ位置との差をベクトルで取得する
        Point position = _player->getPosition(); // 現在のかわずたんの座標を取得する
        Point newPosition = position + delta;
        newPosition = newPosition.getClampPoint(Point(0, position.y), Point(size.width, position.y));
        _player->setPosition(newPosition); // 現在座標 + 移動量を新たな座標にする
    };
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // スコアラベルの追加
    auto scoreLabel = Label::createWithSystemFont(std::to_string(_score), "Helvetica", 64);
    scoreLabel->enableShadow();
    scoreLabel->enableOutline(Color4B::RED, 5);
    this->setScoreLabel(scoreLabel);
    _scoreLabel->setPosition(Point(size.width / 2.0 * 1.5, size.height - 60));
    this->addChild(_scoreLabel);
    
    // タイマーラベルの追加
    auto timerLabel = Label::createWithSystemFont(std::to_string((int)_second), "Helvetica", 64);
    timerLabel->enableShadow();
    timerLabel->enableOutline(Color4B::RED, 5);
    this->setTimerLabel(timerLabel);
    _timerLabel->setPosition(Point(size.width / 2.0, size.height - 60));
    this->addChild(_timerLabel);
    this->scheduleUpdate();
    
    _lot = 0;
    std::random_device rdev;
    _engine.seed(rdev());
    
    return true;
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_timerLabel);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_player);
}

void MainScene::update(float dt)
{
    if (_second > 0 && _lot == 0) {
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
    _scoreLabel->setString(std::to_string(_score));
    _second -= dt;
    _timerLabel->setString(std::to_string((int)_second));
}

Sprite* MainScene::addFruit()
{
    std::uniform_int_distribution<> dist(0, (int)FruitType::COUNT - 1);
    
    auto winSize = Director::getInstance()->getWinSize();
    int fruitNumber = dist(_engine);
    
    std::string filename = "fruit" + std::to_string(fruitNumber) + ".png";
    auto fruit = Sprite::create(filename);
    
    auto fruitSize = fruit->getContentSize();
    float min = fruitSize.width / 2.0;
    float max = winSize.width - fruitSize.width / 2.0;
    std::uniform_int_distribution<float> posDist(min, max);
    float fruitXPos = posDist(_engine);
    
    fruit->setUserData((void *)fruitNumber);
    fruit->setPosition(Point(fruitXPos, winSize.height - FRUIT_TOP_MERGIN - fruitSize.height / 2.0));
    this->addChild(fruit);
    _fruits.pushBack(fruit);
    
    auto ground = Point(fruitXPos, 0);
    fruit->runAction(Sequence::create(MoveTo::create(3.0, ground),
                                      RemoveSelf::create(),
                                      NULL));
    return fruit;
}

void MainScene::catchFruit(cocos2d::Sprite *fruit)
{
    fruit->removeFromParent();
    _fruits.eraseObject(fruit);
    _score += 1;
}