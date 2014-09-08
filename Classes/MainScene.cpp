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
#include "AudioUtils.h"

USING_NS_CC;
/// UI用Zオーダー
const int ZOrderUI = 2;

/// マージン
const int FRUIT_TOP_MERGIN = 40;
/// 制限時間
const float TIME_LIMIT_SECOND = 60;
/// 落下速度
const float FALLING_DURATION = 3.0;
/// ふつうのフルーツの数
const int NORMAL_FRUIT_COUNT = 5;
/// 黄金のフルーツが出る確率の初期値
const float GOLDEN_FRUIT_PROBABILITY_BASE = 0.02;
/// 爆弾が出る確率の初期値
const float BOMB_PROBABILITY_BASE = 0.05;
/// 黄金のフルーツが出る確率の増え幅
const float GOLDEN_FRUIT_PROBABILITY_RATE = 0.001;
/// 爆弾が出る確率の増え幅
const float BOMB_PROBABILITY_RATE = 0.003;
/// フルーツ出現頻度の初期値
const float FRUIT_SPAWN_INCREASE_BASE = 0.02;
/// フルーツ出現頻度の増加率
const float FRUIT_SPAWN_INCREASE_RATE = 1.03f;
/// フルーツ出現頻度の最大値
const float MAXIMUM_SPAWN_PROBABILITY = 0.5;
/// 爆弾を取ったときに減点される点数
const int BOMB_PENALTY_SCORE = 4;
/// ハイスコア格納用のキー
const char* HIGHSCORE_KEY = "highscore_key";

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}

MainScene::MainScene()
: _score(0)
,_isCrash(false)
,_second(TIME_LIMIT_SECOND)
,_state(GameState::READY)
, _highScore(0)
, _isHighScore(false)
,_player(NULL)
,_secondLabel(NULL)
,_scoreLabel(NULL)
,_highscoreLabel(NULL)
,_fruitsBatchNode(NULL)
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
    auto player = Sprite::create("player.png");
    this->setPlayer(player);
    
    auto playerSize =player->getContentSize();
    auto playerWidth = playerSize.width / 3.0;
    player->setTextureRect(Rect(playerWidth, 0, playerWidth, playerSize.height));
    
    player->setPosition(Vec2(size.width / 2.0, size.height - 445));
    this->addChild(player);
    
    // イベントリスナーの追加
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        // タッチされたとき
        return true;
    };
    listener->onTouchMoved = [this, size, player, playerWidth, playerSize](Touch* touch, Event* event) {
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
            
            // プレイヤーの移動方向によってグラを切り替える
            if (delta.x > 0) {
                player->setTextureRect(Rect(playerWidth * 2, 0, playerWidth, playerSize.height));
            } else if (delta.x < 0) {
                player->setTextureRect(Rect(0, 0, playerWidth, playerSize.height));
            } else {
                player->setTextureRect(Rect(playerWidth, 0, playerWidth, playerSize.height));
            }
        }
    };
    listener->onTouchEnded = [player, playerWidth, playerSize](Touch* touch, Event* event) {
        player->setTextureRect(Rect(playerWidth, 0, playerWidth, playerSize.height));
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // スコアラベルの追加
    auto scoreLabel = Label::createWithSystemFont(StringUtils::toString(_score), "Marker Felt", 16);
    scoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    scoreLabel->enableOutline(Color4B::BLACK, 1.5);
    this->setScoreLabel(scoreLabel);
    _scoreLabel->setPosition(Vec2(size.width / 2.0 * 1.5, size.height - 40));
    this->addChild(_scoreLabel, ZOrderUI);
    
    // スコアヘッダーの追加
    auto scoreLabelHeader = Label::createWithSystemFont("SCORE", "Marker Felt", 16);
    scoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    scoreLabelHeader->enableOutline(Color4B::BLACK, 1.5);
    scoreLabelHeader->setPosition(Vec2(size.width / 2.0 * 1.5, size.height - 20));
    this->addChild(scoreLabelHeader, ZOrderUI);
    
    // タイマーラベルの追加
    _second = TIME_LIMIT_SECOND;
    auto secondLabel = Label::createWithSystemFont(StringUtils::toString(static_cast<int>(_second)), "Marker Felt", 16);
    secondLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    secondLabel->enableOutline(Color4B::BLACK, 1.5);
    secondLabel->setPosition(Vec2(size.width / 2.0, size.height - 40));
    this->setSecondLabel(secondLabel);
    this->addChild(_secondLabel, ZOrderUI);
    
    // タイマーヘッダーの追加
    auto secondLabelHeader = Label::createWithSystemFont("TIME", "Marker Felt", 16);
    secondLabelHeader->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    secondLabelHeader->enableOutline(Color4B::BLACK, 1.5);
    secondLabelHeader->setPosition(Vec2(size.width / 2.0, size.height - 20));
    this->addChild(secondLabelHeader, ZOrderUI);
    
    // ハイスコアラベルの追加
    // ハイスコアの取得
    auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
    auto highscoreLabel = Label::createWithSystemFont(StringUtils::toString(static_cast<int>(highscore)), "Marker Felt", 16);
    highscoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    highscoreLabel->enableOutline(Color4B::BLACK, 1.5);
    highscoreLabel->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 40));
    this->setHighscoreLabel(highscoreLabel);
    this->addChild(_highscoreLabel, ZOrderUI);
    this->setHighScore(highscore);
    
    // ハイスコアヘッダーの追加
    auto highscoreLabelHeader = Label::createWithSystemFont("HIGHSCORE", "Marker Felt", 16);
    highscoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
    highscoreLabelHeader->enableOutline(Color4B::BLACK, 1.5);
    highscoreLabelHeader->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 20));
    this->addChild(highscoreLabelHeader, ZOrderUI);
    
    // BatchNodeの初期化
    auto fruits = SpriteBatchNode::create("fruits.png");
    this->addChild(fruits);
    this->setFruitsBatchNode(fruits);
    
    
    this->scheduleUpdate();
    
    return true;
}

MainScene::~MainScene()
{
    // デストラクタ
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_secondLabel);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_highscoreLabel);
    CC_SAFE_RELEASE_NULL(_fruitsBatchNode);
}

void MainScene::onEnterTransitionDidFinish()
{
    // シーン遷移が完了したとき
    Layer::onEnterTransitionDidFinish();
    
    // 「READY」演出を行う
    this->addReadyLabel();
}

void MainScene::update(float dt)
{
    if (_state == GameState::PLAYING) { // PLAYING状態の時
        
        // フルーツの出現を判定する
        float pastTime = TIME_LIMIT_SECOND - _second;
        float p = FRUIT_SPAWN_INCREASE_BASE * (1 + powf(FRUIT_SPAWN_INCREASE_RATE, pastTime));
        p = MIN(p, MAXIMUM_SPAWN_PROBABILITY); // pが最大値以上なら丸める
        float random = this->generateRandom(0, 1);
        if (random < p && _second >= 5) {
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
        _secondLabel->setString(StringUtils::toString(static_cast<int>(_second)));
        if (_second < 0) { // 制限時間が0未満になったとき
            _state = GameState::ENDING; // ゲーム状態をENDINGに移行
            // 終了文字の表示
            auto finish = Sprite::create("finish.png");
            auto winSize = Director::getInstance()->getWinSize();
            finish->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
            finish->setScale(0);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("finish").c_str());
            
            // アクションの作成
            auto appear = EaseExponentialIn::create(ScaleTo::create(0.25, 1.0));
            auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25, 0));
            
            finish->runAction(Sequence::create(appear,
                                               DelayTime::create(2.0),
                                               disappear,
                                               DelayTime::create(1.0),
                                               CallFunc::create([this] {
                _state = GameState::RESULT;
                this->onResult();
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
    float r = generateRandom(0, 1);
    int pastSecond = TIME_LIMIT_SECOND - _second; // 経過時間
    float goldenFruitProbability = GOLDEN_FRUIT_PROBABILITY_BASE + GOLDEN_FRUIT_PROBABILITY_RATE * pastSecond;
    float bombProbability = BOMB_PROBABILITY_BASE + BOMB_PROBABILITY_RATE * pastSecond;
    if (r <= goldenFruitProbability) { // 黄金のフルーツ
        fruitType = static_cast<int>(FruitType::GOLDEN);
    } else if (r <= goldenFruitProbability + bombProbability) { // 爆弾
        fruitType = static_cast<int>(FruitType::BOMB);
    } else { // その他のフルーツ
        fruitType = round(generateRandom(0, NORMAL_FRUIT_COUNT));
    }
    
    // フルーツを作成する
    // テクスチャのサイズを取り出す
    auto textureSize = _fruitsBatchNode->getTextureAtlas()->getTexture()->getContentSize();
    // テクスチャの横幅を個数で割った物がフルーツ1個の幅になる
    auto fruitWidth = textureSize.width / static_cast<int>(FruitType::COUNT);
    auto fruit = Sprite::create("fruits.png", Rect(fruitType *
                                                   fruitWidth,
                                                   0,
                                                   fruitWidth,
                                                   textureSize.height));
    fruit->setTag(fruitType);
    auto fruitSize = fruit->getContentSize();
    
    float min = fruitSize.width / 2.0;
    float max = winSize.width - fruitSize.width / 2.0;
    float fruitXPos = generateRandom(min, max);
    
    fruit->setPosition(Vec2(fruitXPos,
                            winSize.height - FRUIT_TOP_MERGIN - fruitSize.height / 2.0));
    _fruitsBatchNode->addChild(fruit);
    _fruits.pushBack(fruit);
    
    // フルーツに動きをつける
    auto ground = Vec2(fruitXPos, 0);
    fruit->setScale(0);
    auto swing = Sequence::create(RotateTo::create(0.25, -30),
                                  RotateTo::create(0.25, 30),
                                  NULL); // 左右に揺れるアクション
    auto fall = MoveTo::create(FALLING_DURATION, ground);
    auto remove = CallFuncN::create([this](Node *n) {
        this->removeFruit(dynamic_cast<Sprite *>(n));
    });
    auto playSound = CallFunc::create([fruitType] {
        if (fruitType == static_cast<int>(FruitType::GOLDEN)) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("golden").c_str());
            
        } else if (fruitType == static_cast<int>(FruitType::BOMB)) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("bomb").c_str());
        }
    });
    fruit->runAction(Sequence::create(ScaleTo::create(0.25, 1),
                                      Repeat::create(swing, 2),
                                      RotateTo::create(0, 0.125),
                                      playSound,
                                      fall,
                                      remove,
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
    auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();
    // フルーツタイプの取得
    FruitType fruitType = static_cast<FruitType>(fruit->getTag());
    switch (fruitType) {
        case MainScene::FruitType::GOLDEN:
            // 黄金のフルーツのとき
            _score += 5;
            audioEngine->playEffect(AudioUtils::getFileName("catch_golden").c_str());
            break;
        case MainScene::FruitType::BOMB:
            // 爆弾のとき
            this->onCatchBomb();
            audioEngine->playEffect(AudioUtils::getFileName("catch_bomb").c_str());
            this->addBombEffect(fruit->getPosition());
            break;
        default:
            // その他のフルーツのとき
            _score += 1;
            audioEngine->playEffect(AudioUtils::getFileName("catch_fruit").c_str());
            break;
    }
    
    // ハイスコアの判定
    if (_highScore > 0 && !_isHighScore && _score > _highScore) {
        // ハイスコアを出したとき、1度だけSEをならす
        _isHighScore = true;
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("highscore").c_str());
    }
    
    // フルーツの削除
    this->removeFruit(fruit);
    
    // スコアの更新
    _scoreLabel->setString(StringUtils::toString(_score));
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
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("start").c_str());
        // BGMを鳴らす
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(AudioUtils::getFileName("main").c_str());
    }),
                                      RemoveSelf::create(), // 自分を削除する
                                      NULL));
}

void MainScene::onResult()
{
    _state = GameState::RESULT;
    auto winSize = Director::getInstance()->getWinSize();
    
    // 「もう一度遊ぶ」ボタン
    auto replayButton = MenuItemImage::create("replay_button.png",
                                              "replay_button_pressed.png",
                                              [](Ref* ref) {
                                                  // 「もう一度遊ぶ」ボタンを押したときの処理
                                                  // BGMを停止する
                                                  CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
                                                  // 決定音を鳴らす
                                                  CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("decide").c_str());
                                                  auto scene = MainScene::createScene();
                                                  auto transition = TransitionFade::create(0.5, scene);
                                                  Director::getInstance()->replaceScene(transition);
                                              });
    // 「タイトルへ戻る」ボタン
    auto titleButton = MenuItemImage::create("title_button.png",
                                             "title_button_pressed.png",
                                             [](Ref* ref) {
                                                 // 「タイトルへ戻る」ボタンを押したときの処理
                                                 CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("decide").c_str());
                                                 auto scene = TitleScene::createScene();
                                                 auto transition = TransitionCrossFade::create(0.5, scene);
                                                 Director::getInstance()->replaceScene(transition);
                                             });
    
    // メニューの作成
    auto menu = Menu::create(replayButton, titleButton, NULL);
    menu->alignItemsVerticallyWithPadding(30); // ボタンを縦に並べる
    menu->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
    this->addChild(menu);
    
    // ハイスコアの更新
    auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
    if (_score > highscore) {
        _highscoreLabel->setString(StringUtils::toString(_score));
        UserDefault::getInstance()->setIntegerForKey(HIGHSCORE_KEY, _score);
    }
    
    // BGMを鳴らす
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(AudioUtils::getFileName("result").c_str(), true);
}

void MainScene::onCatchBomb()
{
    if (!this->getIsCrash()) {
        _isCrash = true; // クラッシュ状態
        // アニメーションの作成
        Vector<SpriteFrame *> frames;
        const auto playerSize = _player->getContentSize();
        const int animationFrameCount = 3;
        for (int i = 0; i < animationFrameCount; ++i) {
            auto rect = Rect(playerSize.width * i, 0, playerSize.width, playerSize.height);
            auto frame = SpriteFrame::create("player_crash.png", rect) ;
            frames.pushBack(frame);
        }
        auto animation = Animation::createWithSpriteFrames(frames, 10.0 / 60.0);
        animation->setLoops(3);
        animation->setRestoreOriginalFrame(true);
        _player->runAction(Sequence::create(Animate::create(animation),
                                            CallFunc::create([this] {
            _isCrash = false;
        }),
                                            NULL));
        _score = MAX(0, _score - BOMB_PENALTY_SCORE); // 0未満になったら0点にする
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("crash").c_str());
    }
}

float MainScene::generateRandom(float min, float max)
{
    std::uniform_real_distribution<float> dest(min, max);
    return dest(_engine);
}

void MainScene::addBombEffect(cocos2d::Vec2 position)
{
    auto effect = Sprite::create("bomb_effect.png");
    auto size = effect->getContentSize();
    const int frameCount = 3;
    auto frameWidth = size.width / frameCount;
    
    effect->setTextureRect(Rect(0, 0, frameWidth, size.height));
    effect->setPosition(position);
    effect->setOpacity(0);
    Vector<SpriteFrame *> frames;
    for (int i = 0; i < frameCount; ++i) {
        auto frame = SpriteFrame::create("bomb_effect.png",
                                         Rect(frameWidth * i, 0, frameWidth, size.height));
        frames.pushBack(frame);
    }
    auto animation = Animation::createWithSpriteFrames(frames, 0.05);
    effect->runAction(Sequence::create(FadeIn::create(0.1),
                                       Animate::create(animation),
                                       FadeOut::create(0.1),
                                       RemoveSelf::create(),
                                       NULL));
    this->addChild(effect);
}
