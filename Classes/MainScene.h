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
        /// みかん
        ORANGE,
        /// バナナ
        BANANA,
        /// さくらんぼ
        CHERRY,
        /// 金のリンゴ
        GOLDEN,
        /// 爆弾
        BOMB,
        /// 最大値
        COUNT
    };
    
    /** ゲームの状態を表します
     *  PLAYING : プレイ中
     *  RESULT : スコア表示
     */
    enum class GameState
    {
        /// 開始前
        READY,
        /// ゲーム中
        PLAYING,
        /// 終了演出中
        ENDING,
        /// スコア表示
        RESULT
    };

    static cocos2d::Scene* createScene();
    
    MainScene();
    virtual ~MainScene();
    
    bool init() override;
    void update(float dt);
    void onEnterTransitionDidFinish() override;
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE_PASS_BY_REF(cocos2d::Vector<cocos2d::Sprite *>, _fruits, Fruits);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE(bool, _isCrash, IsCrash);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(int, _highScore, HighScore);
    CC_SYNTHESIZE(bool, _isHighScore, IsHighScore);
    CC_SYNTHESIZE(GameState, _state, State);
    CC_SYNTHESIZE(std::mt19937, _engine, Engine);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _secondLabel, SecondLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _highscoreLabel, HighscoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode *, _fruitsBatchNode, FruitsBatchNode);
private:
    /** 画面にフルーツを新たに配置して、それを返します
     *  @return 新たに作成されたフルーツ
     */
    cocos2d::Sprite* addFruit();
    
    /** マップからフルーツを取り除きます
     *  @param  fruit 削除するフルーツ
     *  @return 正しく削除されたか
     */
    bool removeFruit(cocos2d::Sprite *fruit);
    
    /** フルーツを取得します
     *  @param Sprite* 取得するフルーツ
     */
    void catchFruit(cocos2d::Sprite* fruit);
    
    /** ゲーム開始の文字を追加します
     */
    void addReadyLabel();
    
    /** ゲームが終了したときに呼び出されます
     */
    void onResult();
    
    /** 爆弾を取ってしまったときに呼び出されます
     */
    void onCatchBomb();
    
    /** min ~ maxの乱数をfloatで返します
     *  @param min 最小値
     *  @param max 最大値
     *  @return min <= n < maxの整数値
     */
    float generateRandom(float min, float max);
    
    /** 指定位置に爆弾のエフェクトを表示します
     *  @param position 表示する位置
     */
    void addBombEffect(cocos2d::Vec2 position);
};

#endif /* defined(__KawazCatch__MainScene__) */
