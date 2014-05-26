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
    
    this->scheduleUpdate();
    
    return true;
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_player);
}

void MainScene::update(float dt)
{
    this->addFruit();
    
    for (auto fruit : _fruits) {
        auto busketPosition = _player->getPosition() - Point(0, 10);
        bool isHit = fruit->getBoundingBox().containsPoint(busketPosition);
        if (isHit) {
            this->catchFruit(fruit);
        }
    }
}

Sprite* MainScene::addFruit()
{
    std::random_device rdev;
    std::mt19937 engine(rdev());
    std::uniform_int_distribution<> dist(0, (int)FruitType::COUNT - 1);
    
    auto winSize = Director::getInstance()->getWinSize();
    int fruitNumber = dist(engine);
    
    std::string filename = "fruit" + std::to_string(fruitNumber) + ".png";
    auto fruit = Sprite::create(filename);
    
    auto fruitSize = fruit->getContentSize();
    float min = fruitSize.width / 2.0;
    float max = winSize.width - fruitSize.width / 2.0;
    std::uniform_int_distribution<float> posDist(min, max);
    float fruitXPos = posDist(engine);
    
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
}