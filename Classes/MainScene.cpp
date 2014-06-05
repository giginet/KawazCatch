//
//  MainScene.cpp
//  KawazCatch
//
//  Created by giginet on 5/15/14.
//
//
#include "SimpleAudioEngine.h"

#include "MainScene.h"
#include "TitleScene.h"

USING_NS_CC;

/// マージン
const int FRUIT_TOP_MERGIN = 40;
/// 制限時間
const float TIME_LIMIT_SECOND = 60;
/// 落下速度
const float FALLING_DURATION = 3.0;
/// ふつうのフルーツの数
const int NORMAL_FRUIT_COUNT = 5;
/// 黄金のフルーツが出る確率の初期値
const float GOLDEN_FRUIT_PROBABILITY_BASE = 2;
/// 爆弾が出る確率の初期値
const float BOMB_PROBABILITY_BASE = 5;
/// 黄金のフルーツが出る確率の増え幅
const float GOLDEN_FRUIT_PROBABILITY_RATE = 0.1;
/// 爆弾が出る確率の増え幅
const int BOMB_PROBABILITY_RATE = 0.15;
/// フルーツ出現頻度の増加率
const float FRUIT_SPAWN_INCREASE_RATE = 1.1f;
/// フルーツ出現頻度の最大値
const int MINIMUM_SPAWN_PROBABILITY = 12;


Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}

MainScene::MainScene() :
_score(0),
_isCrash(false),
_addFrameCount(0),
_second(0),
_state(GameState::READY),
_player(NULL),
_secondLabel(NULL),
_scoreLabel(NULL)
{
    // 乱数の初期化
    std::random_device rdev;
    _engine.seed(rdev());
}

bool MainScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // 背景を表示する
    auto director = Director::getInstance();
    auto size = director->getWinSize();
    auto background = Sprite::create("background.png");
    background->setPosition(Vec2(size.width / 2.0, size.height / 2.0));
    this->addChild(background);
    
    // プレイヤーを表示する
    this->setPlayer(Sprite::create("player.png"));
    _player->setPosition(Vec2(size.width / 2.0, size.height - 455));
    this->addChild(_player);
    
    // イベントリスナーの追加
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        // タッチされたとき
        return true;
    };
    listener->onTouchMoved = [this, size](Touch* touch, Event* event) {
        // タッチ位置が動いたとき
        if (!this->getIsCrash()) { // クラッシュしてないとき
            // 前回とのタッチ位置との差をベクトルで取得する
            Vec2 delta = touch->getDelta();
            
            // 現在のかわずたんの座標を取得する
            Vec2 position = _player->getPosition();
            
            // 現在座標 + 移動量を新たな座標にする
            Vec2 newPosition = position + delta;
            
            // 画面外に飛び出していたら補正する
            newPosition = newPosition.getClampPoint(Vec2(0, position.y), Vec2(size.width, position.y));
            _player->setPosition(newPosition);
        }
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // スコアラベルの追加
    auto scoreLabel = Label::createWithSystemFont(std::to_string(_score), "Helvetica", 32);
    scoreLabel->enableShadow();
    scoreLabel->enableOutline(Color4B::RED, 2.5);
    this->setScoreLabel(scoreLabel);
    _scoreLabel->setPosition(Vec2(size.width / 2.0 * 1.5, size.height - 30));
    this->addChild(_scoreLabel);
    
    // タイマーラベルの追加
    _second = TIME_LIMIT_SECOND;
    auto secondLabel = Label::createWithSystemFont(std::to_string(static_cast<int>(_second)), "Helvetica", 32);
    secondLabel->enableShadow();
    secondLabel->enableOutline(Color4B::RED, 2.5);
    secondLabel->setPosition(Vec2(size.width / 2.0, size.height - 30));
    this->setSecondLabel(secondLabel);
    this->addChild(_secondLabel);
    this->scheduleUpdate();
    
    return true;
}

MainScene::~MainScene()
{
    // デストラクタ
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
}

void MainScene::onEnterTransitionDidFinish()
{
    // シーン遷移が完了したとき
    Layer::onEnterTransitionDidFinish();
    
    // BGMを鳴らす
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);
    
    // 「READY」演出を行う
    this->addReadyLabel();
}

void MainScene::update(float dt)
{
    if (_state == GameState::PLAYING) { // PLAYING状態の時
        
        // フルーツの出現を判定する
        int p = MAX(static_cast<int>(powf(FRUIT_SPAWN_INCREASE_RATE, _second)),
                    MINIMUM_SPAWN_PROBABILITY);
        int random = rand() % p;
        if (random == 0) {
            this->addFruit();
        }
        
        // フルーツの当たり判定を行う
        for (auto fruit : _fruits) {
            auto busketPosition = _player->getPosition() - Vec2(0, 10);
            bool isHit = fruit->getBoundingBox().containsPoint(busketPosition);
            if (isHit) {
                this->catchFruit(fruit);
            }
        }
        
        // 制限時間を更新する
        _second -= dt;
        _secondLabel->setString(std::to_string(static_cast<int>(_second)));
        if (_second < 0) { // 制限時間が0未満になったとき
            _state = GameState::ENDING; // ゲーム状態をENDINGに移行
            // 終了文字の表示
            auto finish = Sprite::create("finish.png");
            auto winSize = Director::getInstance()->getWinSize();
            finish->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
            finish->setScale(0);
            
            // アクションの作成
            auto appear = EaseExponentialIn::create(ScaleTo::create(0.25, 1.0));
            auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25, 0));
            
            finish->runAction(Sequence::create(appear,
                                               DelayTime::create(2.0),
                                               disappear,
                                               DelayTime::create(1.0),
                                               CallFunc::create([this] {
                _state = GameState::RESULT;
                this->addResultMenu();
            }),
                                               NULL));
            this->addChild(finish);
        }
    }
}

Sprite* MainScene::addFruit()
{
    
    auto winSize = Director::getInstance()->getWinSize();
    // フルーツの種類を選択する
    int fruitType = 0;
    int r = this->generateRandom(100);
    int pastSecond = TIME_LIMIT_SECOND - _second; // 経過時間
    float goldenFruitProbability = GOLDEN_FRUIT_PROBABILITY_BASE + GOLDEN_FRUIT_PROBABILITY_RATE * pastSecond;
    float bombProbability = BOMB_PROBABILITY_BASE + BOMB_PROBABILITY_RATE * pastSecond;
    
    if (r <= goldenFruitProbability) { // 黄金のフルーツ
        fruitType = static_cast<int>(FruitType::GOLDEN);
    } else if (r <= goldenFruitProbability + bombProbability) { // 爆弾
        fruitType = static_cast<int>(FruitType::BOMB);
    } else { // その他のフルーツ
        fruitType = this->generateRandom(NORMAL_FRUIT_COUNT - 1);
    }
    
    // フルーツを作成する
    std::string filename = "fruit" + std::to_string(fruitType) + ".png";
    auto fruit = Sprite::create(filename);
    fruit->setTag(fruitType);
    
    auto fruitSize = fruit->getContentSize();
    float min = fruitSize.width / 2.0;
    float max = winSize.width - fruitSize.width / 2.0;
    std::uniform_int_distribution<float> posDist(min, max);
    float fruitXPos = posDist(_engine);
    
    fruit->setPosition(Vec2(fruitXPos,
                            winSize.height - FRUIT_TOP_MERGIN - fruitSize.height / 2.0));
    this->addChild(fruit);
    _fruits.pushBack(fruit);
    
    // フルーツに動きをつける
    auto ground = Vec2(fruitXPos, 0);
    fruit->setScale(0);
    auto swing = Sequence::create(RotateTo::create(0.25, -30),
                                  RotateTo::create(0.25, 30),
                                  NULL); // 左右に揺れるアクション
    fruit->runAction(Sequence::create(ScaleTo::create(0.25, 1),
                                      Repeat::create(swing, 2),
                                      RotateTo::create(0, 0.125),
                                      MoveTo::create(FALLING_DURATION, ground),
                                      CallFuncN::create([this](Node *n) {
        this->removeFruit(dynamic_cast<Sprite *>(n));
    }),
                                      NULL));
    return fruit;
}

bool MainScene::removeFruit(cocos2d::Sprite *fruit)
{
    if (_fruits.contains(fruit)) {
        // _fruits配列から削除する
        _fruits.eraseObject(fruit);
        // 親ノードから削除する
        fruit->removeFromParent();
        return true;
    }
    return false;
}

void MainScene::catchFruit(cocos2d::Sprite *fruit)
{
    // フルーツタイプの取得
    FruitType fruitType = static_cast<FruitType>(fruit->getTag());
    switch (fruitType) {
        case MainScene::FruitType::GOLDEN:
            // 黄金のフルーツのとき
            _score += 5;
            break;
        case MainScene::FruitType::BOMB:
            // 爆弾のとき
            this->onCatchBomb();
            break;
        default:
            // その他のフルーツのとき
            _score += 1;
            break;
    }
    
    // フルーツの削除
    this->removeFruit(fruit);
    
    // スコアの更新
    _scoreLabel->setString(std::to_string(_score));
}

void MainScene::addReadyLabel()
{
    auto winSize = Director::getInstance()->getWinSize();
    auto center = Vec2(winSize.width / 2.0, winSize.height / 2.0);
    
    // Readyの文字を定義する
    auto ready = Sprite::create("ready.png");
    ready->setScale(0); // 最初に大きさを0にしておく
    ready->setPosition(center);
    this->addChild(ready);
    
    // STARTの文字を定義する
    auto start = Sprite::create("start.png");
    start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(0.5, 5.0), 0.5),
                                                      FadeOut::create(0.5),
                                                      NULL), // 0.5秒かけて拡大とフェードアウトを同時に行う
                                      RemoveSelf::create(), // 自分を削除する
                                      NULL));
    start->setPosition(center);
    
    // READYにアニメーションを追加する
    ready->runAction(Sequence::create(ScaleTo::create(0.25, 1), // 0.25秒かけて等倍に拡大される
                                      DelayTime::create(1.0), // 1.0秒待つ
                                      CallFunc::create([this, start] { // ラムダの中でthisとstart変数を使っているのでキャプチャに加える
        this->addChild(start); // 「スタート」のラベルを追加する（この時点でスタートのアニメーションが始まる）
        _state = GameState::PLAYING; // ゲーム状態をPLAYINGに切り替える
    }),
                                      RemoveSelf::create(), // 自分を削除する
                                      NULL));
}

void MainScene::addResultMenu()
{
    // ENDING状態のとき
    _state = GameState::RESULT;
    auto winSize = Director::getInstance()->getWinSize();
    
    // 「もう一度遊ぶ」ボタン
    auto replayButton = MenuItemImage::create("replay_button.png",
                                              "replay_button_pressed.png",
                                              [this](Ref* ref) {
                                                  // 「もう一度遊ぶ」ボタンを押したときの処理
                                                  auto scene = MainScene::createScene();
                                                  auto transition = TransitionFade::create(0.5, scene);
                                                  Director::getInstance()->replaceScene(transition);
                                              });
    // 「タイトルへ戻る」ボタン
    auto titleButton = MenuItemImage::create("title_button.png",
                                             "title_button_pressed.png",
                                             [this](Ref* ref) {
                                                 // 「タイトルへ戻る」ボタンを押したときの処理
                                                 auto scene = TitleScene::createScene();
                                                 auto transition = TransitionCrossFade::create(0.5, scene);
                                                 Director::getInstance()->replaceScene(transition);
                                             });
    
    // メニューの作成
    auto menu = Menu::create(replayButton, titleButton, NULL);
    menu->alignItemsVerticallyWithPadding(15); // ボタンを縦に並べる
    menu->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
    this->addChild(menu);
}

void MainScene::onCatchBomb()
{
    _isCrash = true; // クラッシュ状態
    // ToDo アニメーション
    _player->runAction(Sequence::create(DelayTime::create(3.0),
                                        CallFunc::create([this] {
        _isCrash = false;
    }),
                                        NULL));
    _score = MAX(0, _score - 4); // 0未満になったら0点にする
}

int MainScene::generateRandom(int n)
{
    std::uniform_int_distribution<> dist(0, n);
    return dist(_engine);
}