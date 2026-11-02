#pragma once

#include <string>

#include "array.h"

using namespace std;

// Узел дерева Хаффмана: объединение символов поддерева и суммарный вес.
// У листа нет сыновей, его поле symbols содержит ровно один символ
struct HuffNode {
    string symbols;
    int weight;
    HuffNode* left;
    HuffNode* right;
};

// Построение дерева по тексту; для пустого текста — nullptr
HuffNode* BuildHuffman(const string& text);

// Таблица кодов: ячейка с индексом, равным коду байта, хранит двоичный код символа
void BuildCodes(HuffNode* root, Array& codes);

string HuffEncode(const string& text, const Array& codes);

// Восстановление строки по коду; false, если код не соответствует дереву
bool HuffDecode(HuffNode* root, const string& bits, string& text);

void DeleteHuffman(HuffNode*& root);
