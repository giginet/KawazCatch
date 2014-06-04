//
//  TitleScene.cpp
//  KawazCatch
//
//  Created by giginet on 5/29/14.
//
//

#include "TitleScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Title::createScene()
{
    auto scene = Scene::create();
    auto layer = Title::create();
    scene->addChild(layer);
    return scene;
}

bool Title::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    auto director = Director::getInstance();
    auto winSize = director->getWinSize();
    
    // 背景の追加
    auto background = Sprite::create("title_background.png");
    background->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
    this->addChild(background);
    
    // ロゴの追加
    auto logo = Sprite::create("logo.png");
    logo->setPosition(Vec2(winSize.width / 2.0, winSize.height - 150));
    this->addChild(logo);
    
    // Touch to Startの追加
    auto touchToStart = Label::createWithSystemFont("Touch to Start", "Helvetica", 28);
    touchToStart->setPosition(Vec2(winSize.width / 2.0, 75));
    auto blink = Sequence::create(FadeTo::create(0.5, 127),
                                  FadeTo::create(0.5, 255),
                                  NULL);
    touchToStart->runAction(RepeatForever::create(blink));
    this->addChild(touchToStart);
    
    // 画面をタッチしたときにメイン画面へ遷移
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
        this->getEventDispatcher()->removeAllEventListeners();
        this->runAction(Sequence::create(DelayTime::create(0.5),
            
                                         CallFunc::create([]{
            auto scene = Main::createScene();
            auto transition = TransitionPageTurn::create(0.5, scene, true);
            Director::getInstance()->replaceScene(transition);
        }),
                                         NULL));
        return true;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void Title::onEnterTransitionDidFinish()
{
    // BGMの再生
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("title.wav", true);
}