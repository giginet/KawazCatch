//
//  MainScene.h
//  KawazCatch
//
//  Created by giginet on 5/15/14.
//
//

#ifndef __KawazCatch__MainScene__
#define __KawazCatch__MainScene__

#include "cocos2d.h"

class MainScene :public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    bool init();
    virtual ~MainScene();
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
};

#endif /* defined(__KawazCatch__MainScene__) */
