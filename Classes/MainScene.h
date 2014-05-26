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
#include <random>

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
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _timerLabel, TimerLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
private:
    /// フルーツを新たに生成して、生成したフルーツを返します
    /// @return 新たに作成されたフルーツ
    cocos2d::Sprite* addFruit();
    
    /// フルーツを取得します
    /// @param Sprite* 取得するフルーツ
    void catchFruit(cocos2d::Sprite* fruit);
    float _lot;
    std::mt19937 _engine;
};

#endif /* defined(__KawazCatch__MainScene__) */
