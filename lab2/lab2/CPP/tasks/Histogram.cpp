// Задание 4, вариант 6: гистограмма частоты символов.
// Счётчики хранятся в массиве из ЛР1: индекс ячейки — код символа

#include <iostream>
#include <string>

#include "../headers/array.h"

using namespace std;

const int CODES = 128;

// Учитываются печатные символы ASCII, кроме пробела
bool Counted(int code) {
    return code > 32 && code < 127;
}

// Значение счётчика символа с кодом code
int GetCount(const Array& counts, int code) {
    string value;
    ArrayGet(counts, code, value);
    return stoi(value);
}

int main() {
    Array counts;
    ArrayInit(counts, CODES);
    for (int i = 0; i < CODES; i++) {
        ArrayPush(counts, "0");
    }

    // Подсчёт вхождений по всему вводу, переводы строк не учитываются
    char symbol = 0;
    while (cin.get(symbol)) {
        int code = static_cast<unsigned char>(symbol);
        if (code < CODES && Counted(code)) {
            ArraySet(counts, code, to_string(GetCount(counts, code) + 1));
        }
    }

    // Символы в порядке возрастания кодов и высота гистограммы
    string symbols;
    int height = 0;
    for (int code = 0; code < CODES; code++) {
        int value = GetCount(counts, code);
        if (value > 0) {
            symbols += static_cast<char>(code);
            if (value > height) {
                height = value;
            }
        }
    }

    // Строки выводятся сверху вниз: на уровне level столбик есть,
    // если символ встретился не меньше level раз
    for (int level = height; level >= 1; level--) {
        string row;
        for (size_t i = 0; i < symbols.size(); i++) {
            if (GetCount(counts, static_cast<unsigned char>(symbols[i])) >= level) {
                row += '#';
            } else {
                row += ' ';
            }
        }
        // Пробелы в конце строки не выводятся
        while (!row.empty() && row.back() == ' ') {
            row.pop_back();
        }
        cout << row << endl;
    }
    cout << symbols << endl;

    DeleteArray(counts);
    return 0;
}
