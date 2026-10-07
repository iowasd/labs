#include <iostream>
#include <string>

#include "database/database.h"

using namespace std;

// Разбор аргументов --file и --query и выполнение запроса
int main(int argc, char* argv[]) {
    string fileName;
    string query;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            i++;
            fileName = argv[i];
        } else if (arg == "--query" && i + 1 < argc) {
            i++;
            query = argv[i];
        }
    }

    if (fileName.empty() || query.empty()) {
        cout << "Usage: ./dbms --file <file> --query '<COMMAND> <name> [args]'" << endl;
        return 1;
    }

    ExecuteQuery(fileName, query);
    return 0;
}
