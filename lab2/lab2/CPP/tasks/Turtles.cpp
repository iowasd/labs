// Задание 3, вариант 6: черепахи.
// Черепаха с фразой (a, b) может говорить правду, только если a + b = N - 1
// и a, b неотрицательны. Две правдивые черепахи не могут занимать одно место,
// поэтому ответ — число различных a среди допустимых фраз

#include <iostream>
#include <string>

#include "../headers/set.h"

using namespace std;

int main() {
    int count = 0;
    if (!(cin >> count) || count < 0) {
        cout << "ERROR: expected number of turtles" << endl;
        return 1;
    }

    Set places;
    SetInit(places, 8);
    for (int i = 0; i < count; i++) {
        int ahead = 0;
        int behind = 0;
        if (!(cin >> ahead >> behind)) {
            cout << "ERROR: expected " << count << " pairs of numbers" << endl;
            DeleteSet(places);
            return 1;
        }
        if (ahead >= 0 && behind >= 0 && ahead <= count - 1 && behind == count - 1 - ahead) {
            // Место черепахи однозначно задаётся числом черепах впереди
            SetAdd(places, to_string(ahead));
        }
    }

    cout << SetSize(places) << endl;
    DeleteSet(places);
    return 0;
}
