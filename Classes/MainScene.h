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
    CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _fruits, Fruits);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
private:
    /// フルーツを新たに生成して、生成したフルーツを返します
    /// @return 新たに作成されたフルーツ
    cocos2d::Sprite* addFruit();
};

#endif /* defined(__KawazCatch__MainScene__) */
