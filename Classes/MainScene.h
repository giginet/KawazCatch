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
    /// フルーツの種類を表します
    enum class FruitType
    {
        /// リンゴ
        APPLE,
        /// ぶどう
        GRAPE,
        /// 最大値
        COUNT
    };

    static cocos2d::Scene* createScene();
    bool init() override;
    virtual ~MainScene();
    void update(float dt);
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _fruits, Fruits);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
private:
    /// フルーツを新たに生成して、生成したフルーツを返します
    /// @return 新たに作成されたフルーツ
    cocos2d::Sprite* addFruit();
    
    /// フルーツを取得します
    /// @param Sprite* 取得するフルーツ
    void catchFruit(cocos2d::Sprite* fruit);
};

#endif /* defined(__KawazCatch__MainScene__) */
