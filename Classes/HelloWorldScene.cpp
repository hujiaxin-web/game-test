#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Brand.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 添加关闭按钮
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 初始目标牌位置（但先不创建）
    _targetPosition = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 6 + origin.y);
    _targetSprite = nullptr;

    // 生成5张可点击的牌，倒三角排列
    for (int i = 0; i < 5; i++)
    {
        CardData cardData = BrandDisplayWithData();
        _clickableCardDatas.push_back(cardData);

        // 计算位置：倒三角排列
        float x, y;
        if (i == 0)
        {
            // 顶点卡牌（最底部）
            x = origin.x + visibleSize.width / 2;
            y = origin.y + visibleSize.height / 2; // 调整 y 坐标使卡牌位于屏幕下半部分
        }
        else
        {
            // 其他卡牌，分布在两边
            int side = (i - 1) % 2; // 0 表示左边，1 表示右边
            int index = (i - 1) / 2; // 0 或 1，表示在边上的位置

            float horizontalOffset = visibleSize.width / 8; // 横向偏移量
            float verticalOffset = visibleSize.height / 12; // 纵向偏移量

            if (side == 0) // 左边
            {
                x = origin.x + visibleSize.width / 2 - horizontalOffset * (index + 1);
                y = origin.y + visibleSize.height / 2 + verticalOffset * (index + 1);
            }
            else // 右边
            {
                x = origin.x + visibleSize.width / 2 + horizontalOffset * (index + 1);
                y = origin.y + visibleSize.height / 2 + verticalOffset * (index + 1);
            }
        }

        auto sprite = createCardSprite(cardData, Vec2(x, y), false, false);
        _clickableSprites.push_back(sprite);
    }

    // 生成牌库（2张牌）
    for (int i = 0; i < 2; i++)
    {
        CardData cardData = BrandDisplayWithData();
        _deckCardDatas.push_back(cardData);

        // 牌库位置
        float deckX = _targetPosition.x - 250;
        float deckY = _targetPosition.y + (i * 15);

        auto sprite = createCardSprite(cardData, Vec2(deckX, deckY), false, true);
        _deckSprites.push_back(sprite);

        // 设置牌库牌的层级
        if (i == 1) {
            sprite->setLocalZOrder(-1);
        }
    }

    // 注册触摸事件
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 卡牌区域划分
    mainCardArea = LayerColor::create(Color4B(0, 0, 255, 100), 1080, 580);
    mainCardArea->setPosition(Vec2(origin.x, origin.y));
    mainCardArea->setAnchorPoint(Vec2::ZERO);
    mainCardArea->setName("MainCardArea");
    this->addChild(mainCardArea, 0);

    auto cardPileArea = LayerColor::create(Color4B(0, 255, 0, 100), 1080, 1500);
    cardPileArea->setPosition(Vec2(origin.x, origin.y + 580));
    cardPileArea->setAnchorPoint(Vec2::ZERO);
    this->addChild(cardPileArea, 0);

    return true;
}

cocos2d::Sprite* HelloWorld::createCardSprite(const CardData& cardData, const cocos2d::Vec2& position, bool isTarget, bool isDeck)
{
    auto sprite = Sprite::create(cardData.imagePath);
    if (sprite == nullptr)
    {
        problemLoading(cardData.imagePath.c_str());
        return nullptr;
    }

    sprite->setPosition(position);
    sprite->setScale(3.0f);

    // 不添加任何标签
    this->addChild(sprite, 1);
    return sprite;
}
void HelloWorld::revertToPreviousStep()
{
    if (_historyStack.empty()) {
        CCLOG("No previous step to revert to");
        return;
    }

    // 弹出最近的操作
    CardState previousState = _historyStack.top();
    _historyStack.pop();

    // 恢复目标牌
    _targetCardData = previousState.targetCardData;
    auto newTargetTexture = Director::getInstance()->getTextureCache()->addImage(_targetCardData.imagePath);
    if (newTargetTexture) {
        _targetSprite->setTexture(newTargetTexture);
        _targetSprite->removeAllChildren();
    }

    // 恢复可点击牌
    for (size_t i = 0; i < previousState.clickableCardDatas.size(); ++i) {
        _clickableCardDatas[i] = previousState.clickableCardDatas[i];
        auto newClickableTexture = Director::getInstance()->getTextureCache()->addImage(_clickableCardDatas[i].imagePath);
        if (newClickableTexture) {
            _clickableSprites[i]->setTexture(newClickableTexture);
            _clickableSprites[i]->setVisible(true);
            _clickableSprites[i]->setScale(3.0f);
            _clickableSprites[i]->removeAllChildren();
        }
    }

    CCLOG("Reverted to previous step");
}

bool HelloWorld::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return true;
}

void HelloWorld::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Vec2 touchLocation = touch->getLocation();

    // 检查是否点击了牌库牌（从最上面的牌开始检查）
    for (int i = _deckSprites.size() - 1; i >= 0; i--)
    {
        if (_deckSprites[i] &&
            _deckSprites[i]->getBoundingBox().containsPoint(touchLocation) &&
            _deckSprites[i]->isVisible())
        {
            CCLOG("Deck card %d touched", i);
            handleDeckCardClick(i);
            return;
        }
    }

    // 检查点击了哪张可点击的牌
    for (int i = 0; i < _clickableSprites.size(); i++)
    {
        if (_clickableSprites[i] &&
            _clickableSprites[i]->getBoundingBox().containsPoint(touchLocation) &&
            _clickableSprites[i]->isVisible())
        {
            handleCardClick(_clickableSprites[i], _clickableCardDatas[i], i);
            return;
        }
    }

    // 检查是否点击了 mainCardArea 的空白区域
    if (mainCardArea->getBoundingBox().containsPoint(touchLocation))
    {
        CCLOG("Clicked on mainCardArea blank space, reverting to previous step");
        revertToPreviousStep();
    }
}

void HelloWorld::handleDeckCardClick(int deckIndex)
{
    CCLOG("Handling deck card click: %d", deckIndex);
    moveDeckCardToTargetPosition(deckIndex);
}

void HelloWorld::moveDeckCardToTargetPosition(int deckIndex)
{
    if (deckIndex < 0 || deckIndex >= _deckSprites.size()) {
        CCLOG("Invalid deck index: %d", deckIndex);
        return;
    }

    auto deckSprite = _deckSprites[deckIndex];
    if (!deckSprite || !deckSprite->isVisible()) {
        CCLOG("Deck sprite not available or not visible: %d", deckIndex);
        return;
    }

    CCLOG("Moving deck card %d to target position", deckIndex);

    // 获取目标位置
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    Vec2 targetPosition = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 6 + origin.y);

    // 保存牌库牌的数据
    CardData deckCardData = _deckCardDatas[deckIndex];

    // 如果已经有目标牌，先移除它
    if (_targetSprite && _targetSprite->isVisible()) {
        _targetSprite->setVisible(false);
        _targetSprite->removeFromParent();
        _targetSprite = nullptr;
    }

    // 重置牌的层级，确保它显示在最上面（正常颜色）
    deckSprite->setLocalZOrder(1); // 设置为正数，确保显示在最上面

    // 移除牌库牌上的所有标签（"Deck"标签等）
    deckSprite->removeAllChildren();

    // 移动牌库牌到目标位置
    auto moveAction = MoveTo::create(0.5f, targetPosition);
    auto easeOut = EaseBackOut::create(moveAction);

    deckSprite->runAction(Sequence::create(
        easeOut,
        CallFunc::create([this, deckSprite, deckCardData, deckIndex]() {
            // 更新目标牌引用
            _targetSprite = deckSprite;
            _targetCardData = deckCardData;

            CCLOG("Successfully set deck card as target: %s, Value: %d",
                _targetCardData.number.c_str(), _targetCardData.numericValue);

            // 重新生成牌库牌
            this->replaceDeckCard(deckIndex);
            }),
        nullptr
    ));
}

void HelloWorld::handleCardClick(cocos2d::Sprite* clickedSprite, CardData clickedCardData, int cardIndex)
{
    // 检查是否有目标牌
    if (!_targetSprite || !_targetSprite->isVisible()) {
        CCLOG("No target card available. Please select a card from deck first.");
        return;
    }

    // 比较卡牌逻辑
    int valueDiff = std::abs(clickedCardData.numericValue - _targetCardData.numericValue);
    bool canReplace = (valueDiff == 1 || valueDiff == 0);

    CCLOG("Comparing: %s(%d) vs %s(%d), Difference: %d, Can Replace: %s",
        clickedCardData.number.c_str(), clickedCardData.numericValue,
        _targetCardData.number.c_str(), _targetCardData.numericValue,
        valueDiff, canReplace ? "Yes" : "No");

    if (canReplace)
    {
        // 匹配逻辑
        auto scaleDown = ScaleTo::create(0.2f, 0.1f);
        auto scaleUp = ScaleTo::create(0.2f, 3.0f);

        clickedSprite->runAction(Sequence::create(
            scaleDown,
            CallFunc::create([clickedSprite]() {
                clickedSprite->setVisible(false);
                }),
            nullptr
        ));

        _targetSprite->runAction(Sequence::create(
            scaleDown,
            CallFunc::create([this, clickedCardData]() {
                auto newTexture = Director::getInstance()->getTextureCache()->addImage(clickedCardData.imagePath);
                if (newTexture) {
                    _targetSprite->setTexture(newTexture);
                    _targetCardData = clickedCardData;

                    // 确保移除所有子节点（包括之前可能添加的标签）
                    _targetSprite->removeAllChildren();
                }
                }),
            scaleUp,
            nullptr
        ));

        this->runAction(Sequence::create(
            DelayTime::create(1.5f),
            CallFunc::create([this, cardIndex]() {
                this->replaceClickedCard(cardIndex);
                }),
            nullptr
        ));

        // 保存当前状态到操作历史栈
        CardState currentState;
        currentState.targetCardData = _targetCardData;
        for (const auto& cardData : _clickableCardDatas) {
            currentState.clickableCardDatas.push_back(cardData);
        }
        _historyStack.push(currentState);
    }
    else
    {
        // 不匹配的抖动效果
        auto moveLeft = MoveBy::create(0.05f, Vec2(-10, 0));
        auto moveRight = MoveBy::create(0.05f, Vec2(20, 0));
        auto moveBack = MoveBy::create(0.05f, Vec2(-10, 0));
        clickedSprite->runAction(Sequence::create(moveLeft, moveRight, moveBack, nullptr));
    }
}
void HelloWorld::replaceClickedCard(int cardIndex)
{
    CardData newCardData = BrandDisplayWithData();
    _clickableCardDatas[cardIndex] = newCardData;

    auto sprite = _clickableSprites[cardIndex];
    auto newTexture = Director::getInstance()->getTextureCache()->addImage(newCardData.imagePath);
    if (newTexture && sprite) {
        sprite->setTexture(newTexture);
        sprite->setVisible(true);
        sprite->setScale(3.0f);

        // 确保移除所有子节点（包括之前可能添加的标签）
        sprite->removeAllChildren();

        CCLOG("Replaced clickable card %d: %s, Value: %d", cardIndex, newCardData.number.c_str(), newCardData.numericValue);
    }
}

void HelloWorld::replaceDeckCard(int deckIndex)
{
    CardData newCardData = BrandDisplayWithData();
    _deckCardDatas[deckIndex] = newCardData;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    Vec2 targetPosition = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 6 + origin.y);
    Vec2 deckPosition = Vec2(targetPosition.x - 250, targetPosition.y + (deckIndex * 15));

    // 创建新的牌库牌
    auto newDeckSprite = Sprite::create(newCardData.imagePath);
    if (newDeckSprite) {
        newDeckSprite->setPosition(deckPosition);
        newDeckSprite->setScale(3.0f);

        // 确保移除所有子节点（包括之前可能添加的标签）
        newDeckSprite->removeAllChildren();

        this->addChild(newDeckSprite, 1);
        _deckSprites[deckIndex] = newDeckSprite;

        if (deckIndex == 1) {
            newDeckSprite->setLocalZOrder(-1);
        }
    }

    CCLOG("Replaced deck card %d: %s, Value: %d", deckIndex, newCardData.number.c_str(), newCardData.numericValue);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

