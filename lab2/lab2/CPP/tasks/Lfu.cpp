// Задание 7, вариант 2: LFU-кэш.
// Ввод: ёмкость cap и число запросов Q, затем Q запросов SET x y или GET x.
// Вывод: результаты GET через пробел. Параметр --trace выводит состояние после каждого запроса

#include <iostream>
#include <string>

#include "../headers/lfu.h"

using namespace std;

int main(int argc, char* argv[]) {
    bool trace = argc > 1 && string(argv[1]) == "--trace";

    int capacity = 0;
    int queries = 0;
    if (!(cin >> capacity >> queries)) {
        cout << "ERROR: expected cap and Q" << endl;
        return 1;
    }

    LfuCache cache;
    LfuInit(cache, capacity);

    string output;
    for (int i = 0; i < queries; i++) {
        string command;
        string key;
        if (!(cin >> command >> key)) {
            cout << "ERROR: expected " << queries << " queries" << endl;
            break;
        }
        if (command == "SET") {
            int value = 0;
            if (!(cin >> value)) {
                cout << "ERROR: SET needs a value" << endl;
                break;
            }
            LfuSet(cache, key, value);
        } else if (command == "GET") {
            int value = -1;
            if (!LfuGet(cache, key, value)) {
                value = -1;
            }
            if (!output.empty()) {
                output += " ";
            }
            output += to_string(value);
        } else {
            cout << "ERROR: unknown query " << command << endl;
            break;
        }
        if (trace) {
            cout << command << " " << key << ": ";
            PrintLfu(cache);
        }
    }

    cout << output << endl;
    DeleteLfu(cache);
    return 0;
}
