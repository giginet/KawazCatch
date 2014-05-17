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
    
    return true;
}