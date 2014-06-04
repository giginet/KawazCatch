//
//  TitleScene.h
//  KawazCatch
//
//  Created by giginet on 5/29/14.
//
//

#ifndef __KawazCatch__TitleScene__
#define __KawazCatch__TitleScene__

#include "cocos2d.h"

class Title :public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    void onEnterTransitionDidFinish() override;
    CREATE_FUNC(Title);
};

#endif /* defined(__KawazCatch__TitleScene__) */
