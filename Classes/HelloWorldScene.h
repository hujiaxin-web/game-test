#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Brand.h"

USING_NS_CC;

class HelloWorld : public Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(HelloWorld);

    void menuCloseCallback(Ref* pSender);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void handleDeckCardClick(int deckIndex);
    void moveDeckCardToTargetPosition(int deckIndex);
    void handleCardClick(cocos2d::Sprite* clickedSprite, CardData clickedCardData, int cardIndex);
    void replaceClickedCard(int cardIndex);
    void replaceDeckCard(int deckIndex);

    cocos2d::Sprite* createCardSprite(const CardData& cardData, const cocos2d::Vec2& position, bool isTarget, bool isDeck);

private:
    Vec2 _targetPosition;
    cocos2d::Sprite* _targetSprite;
    std::vector<CardData> _clickableCardDatas;
    std::vector<cocos2d::Sprite*> _clickableSprites;
    std::vector<CardData> _deckCardDatas;
    std::vector<cocos2d::Sprite*> _deckSprites;
    CardData _targetCardData;

    // 添加操作历史结构体和栈
    struct CardState {
        CardData targetCardData;
        std::vector<CardData> clickableCardDatas;
    };

    std::stack<CardState> _historyStack;

    void revertToPreviousStep();

    // 添加 mainCardArea 作为成员变量
    cocos2d::LayerColor* mainCardArea;
};

#endif // __HELLOWORLD_SCENE_H__