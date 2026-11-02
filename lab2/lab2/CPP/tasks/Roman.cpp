// Задание 6.2, вариант 3: перевод целого числа в римскую запись.
// Соответствие «значение → символы» хранится в хеш-таблице с цепочками

#include <iostream>
#include <stdexcept>
#include <string>

#include "../headers/array.h"
#include "../headers/hashtable.h"

using namespace std;

// Значения от большего к меньшему, включая вычитательные пары (CM, CD, XC, XL, IX, IV)
void FillTable(ChainTable<string>& table, Array& order) {
    const int count = 13;
    const int values[count] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    const string symbols[count] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    for (int i = 0; i < count; i++) {
        ChainPut(table, to_string(values[i]), symbols[i]);
        ArrayPush(order, to_string(values[i]));
    }
}

// Жадный перевод: пока число не меньше значения, дописываются его символы.
// Так цифры I, X, C, M повторяются не более трёх раз подряд
string ToRoman(int number, ChainTable<string>& table, const Array& order) {
    string result;
    for (int i = 0; i < ArrayLength(order); i++) {
        string key;
        ArrayGet(order, i, key);
        int value = stoi(key);
        string symbol;
        ChainGet(table, key, symbol);
        while (number >= value) {
            result += symbol;
            number -= value;
        }
    }
    return result;
}

int main() {
    ChainTable<string> table;
    ChainInit(table, 16);
    Array order;
    ArrayInit(order, 16);
    FillTable(table, order);

    string word;
    while (cin >> word) {
        int number = 0;
        size_t pos = 0;
        bool parsed = true;
        try {
            number = stoi(word, &pos);
        } catch (const exception&) {
            parsed = false;
        }
        if (!parsed || pos != word.size()) {
            cout << word << ": ERROR: not an integer" << endl;
        } else if (number < 1 || number > 3999) {
            // Без черты над цифрами записываются только числа от 1 до 3999
            cout << word << ": ERROR: number must be in range 1..3999" << endl;
        } else {
            cout << number << " = \"" << ToRoman(number, table, order) << "\"" << endl;
        }
    }

    DeleteArray(order);
    ChainFree(table);
    return 0;
}
