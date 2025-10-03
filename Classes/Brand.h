#ifndef BRAND_H
#define BRAND_H

#include <string>
#include <vector>
#include <random>

// �������ݽṹ
struct CardData {
    std::string imagePath;
    std::string colorSuit;
    std::string number;
    int numericValue;  // ����ֵ���ڱȽ�
};

void BrandDisplay(std::string& imageaddr);
CardData BrandDisplayWithData();  // ����������������

// ��ȡ���Ƶ�����ֵ
int getCardNumericValue(const std::string& cardNumber);

#endif // BRAND_H
