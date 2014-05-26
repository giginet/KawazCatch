//
//  MainScene.cpp
//  KawazCatch
//
//  Created by giginet on 5/15/14.
//
//

#include "MainScene.h"

USING_NS_CC;

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
    
    return true;
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_player);
}