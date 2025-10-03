#ifndef BRAND_H
#define BRAND_H

#include <string>
#include <vector>
#include <random>

// 卡牌数据结构
struct CardData {
    std::string imagePath;
    std::string colorSuit;
    std::string number;
    int numericValue;  // 数字值用于比较
};

void BrandDisplay(std::string& imageaddr);
CardData BrandDisplayWithData();  // 返回完整卡牌数据

// 获取卡牌的数字值
int getCardNumericValue(const std::string& cardNumber);

#endif // BRAND_H
