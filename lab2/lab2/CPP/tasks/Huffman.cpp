// Задание 5, вариант *: алгоритм Хаффмана.
// Первая строка ввода — исходная строка, по ней строится дерево и таблица кодов.
// Вторая строка (необязательная) — двоичный код, который восстанавливается по этому дереву;
// если её нет, восстанавливается код самой исходной строки

#include <iostream>
#include <string>

#include "../headers/huffman.h"

using namespace std;

// Отображение символа в таблице: пробел и непечатные байты выводятся кодом
string ShowSymbol(char symbol) {
    int code = static_cast<unsigned char>(symbol);
    if (code > 32 && code < 127) {
        return string(1, symbol);
    }
    return "#" + to_string(code);
}

// Вывод таблицы в порядке убывания частот, при равных частотах — в порядке появления
void PrintTable(const string& text, const Array& codes) {
    int counts[256] = {0};
    string order;
    for (size_t i = 0; i < text.size(); i++) {
        int code = static_cast<unsigned char>(text[i]);
        if (counts[code] == 0) {
            order += text[i];
        }
        counts[code]++;
    }

    // Сортировка вставками: символов не больше 256
    for (size_t i = 1; i < order.size(); i++) {
        char symbol = order[i];
        size_t j = i;
        while (j > 0 && counts[static_cast<unsigned char>(order[j - 1])] < counts[static_cast<unsigned char>(symbol)]) {
            order[j] = order[j - 1];
            j--;
        }
        order[j] = symbol;
    }

    cout << "Table (symbol, frequency, code):" << endl;
    for (size_t i = 0; i < order.size(); i++) {
        string code;
        ArrayGet(codes, static_cast<unsigned char>(order[i]), code);
        cout << "  " << ShowSymbol(order[i]) << "  " << counts[static_cast<unsigned char>(order[i])] << "  " << code << endl;
    }
}

int main() {
    string text;
    if (!getline(cin, text)) {
        cout << "ERROR: expected a line of text" << endl;
        return 1;
    }

    HuffNode* root = BuildHuffman(text);
    Array codes;
    BuildCodes(root, codes);
    string bits = HuffEncode(text, codes);

    cout << "Code: " << bits << endl;
    PrintTable(text, codes);
    cout << "Size: " << bits.size() << " bits instead of " << text.size() * 8 << endl;

    string input = bits;
    string line;
    if (getline(cin, line)) {
        input = line;
    }

    string restored;
    if (HuffDecode(root, input, restored)) {
        cout << "Decoded: " << restored << endl;
    } else {
        cout << "Decoded: ERROR: code does not match the tree" << endl;
    }

    DeleteArray(codes);
    DeleteHuffman(root);
    return 0;
}
