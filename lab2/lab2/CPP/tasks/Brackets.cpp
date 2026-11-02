#include <iostream>
#include <string>

#include "../headers/stack.h"

using namespace std;

// Возвращает строку без лишних скобок.
// В стеке хранятся индексы ещё не закрытых открывающих скобок
string FixBrackets(const string& text) {
    int length = static_cast<int>(text.size());
    bool* removed = new bool[length + 1];
    for (int i = 0; i < length; i++) {
        removed[i] = false;
    }

    Stack open;
    InitStack(open);
    for (int i = 0; i < length; i++) {
        if (text[i] == '(') {
            Push(open, to_string(i));
        } else if (text[i] == ')') {
            if (IsEmpty(open)) {
                // Закрывающей скобке нет пары
                removed[i] = true;
            } else {
                PopTop(open);
            }
        }
    }
    // Открывающие скобки, оставшиеся в стеке, не закрыты
    while (!IsEmpty(open)) {
        removed[stoi(CheckFirst(open))] = true;
        PopTop(open);
    }
    DeleteStack(open);

    string result;
    for (int i = 0; i < length; i++) {
        if (!removed[i]) {
            result += text[i];
        }
    }
    delete[] removed;
    return result;
}

int main() {
    string line;
    while (getline(cin, line)) {
        cout << FixBrackets(line) << endl;
    }
    return 0;
}
