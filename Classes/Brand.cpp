#include "Brand.h"
#include <map>

// �������ֵ���ֵ��ӳ��
std::map<std::string, int> cardValueMap = {
    {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7},
    {"8", 8}, {"9", 9}, {"10", 10}, {"J", 11}, {"Q", 12}, {"K", 13}, {"A", 14}
};

void BrandDisplay(std::string& imageaddr) {
    static std::mt19937 rng(std::random_device{}());

    // ��ɫ�Ͷ�Ӧ��ǰ׺ӳ��
    std::vector<std::pair<std::string, std::string>> suits = {
        {"Black Hearts/", "H"},
        {"Red Hearts/", "H"},
        {"Clubs/", "H"},
        {"Diamonds/", "H"}
    };

    // �����������
    std::vector<std::string> numbers = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };

    // ���ѡ��
    std::uniform_int_distribution<int> suitDist(0, suits.size() - 1);
    std::uniform_int_distribution<int> numberDist(0, numbers.size() - 1);

    auto selectedSuit = suits[suitDist(rng)];
    std::string number = numbers[numberDist(rng)];

    // ���·��
    imageaddr = "Pixel Classic Cards/" + selectedSuit.first + selectedSuit.second + number + ".png";
}

CardData BrandDisplayWithData() {
    static std::mt19937 rng(std::random_device{}());

    std::vector<std::pair<std::string, std::string>> suits = {
        {"Black Hearts/", "H"},
        {"Red Hearts/", "H"},
        {"Clubs/", "H"},
        {"Diamonds/", "H"}
    };

    std::vector<std::string> numbers = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };

    std::uniform_int_distribution<int> suitDist(0, suits.size() - 1);
    std::uniform_int_distribution<int> numberDist(0, numbers.size() - 1);

    auto selectedSuit = suits[suitDist(rng)];
    std::string number = numbers[numberDist(rng)];

    CardData card;
    card.colorSuit = selectedSuit.first;
    card.number = number;
    card.numericValue = cardValueMap[number];
    card.imagePath = "Pixel Classic Cards/" + selectedSuit.first + selectedSuit.second + number + ".png";

    return card;
}

int getCardNumericValue(const std::string& cardNumber) {
    auto it = cardValueMap.find(cardNumber);
    if (it != cardValueMap.end()) {
        return it->second;
    }
    return 0; // Ĭ��ֵ
}