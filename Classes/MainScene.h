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
    CREATE_FUNC(MainScene);
};

#endif /* defined(__KawazCatch__MainScene__) */
