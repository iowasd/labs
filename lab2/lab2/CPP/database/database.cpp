#include "database.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../headers/array.h"
#include "../headers/avl.h"
#include "../headers/dlist.h"
#include "../headers/flist.h"
#include "../headers/queue.h"
#include "../headers/set.h"
#include "../headers/stack.h"

using namespace std;

// ---------------- Вспомогательные функции ----------------

// Перевод строки в верхний регистр
string ToUpper(string text) {
    for (size_t i = 0; i < text.size(); i++) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// Разбивает строку на слова по пробелам
void SplitWords(const string& text, Array& words) {
    istringstream stream(text);
    string word;
    while (stream >> word) {
        ArrayPush(words, word);
    }
}

// Дописывает слово в строку через пробел
void AppendWord(string& text, const string& word) {
    if (!text.empty()) {
        text += " ";
    }
    text += word;
}

// Преобразование строки в число, false если строка не целиком число
bool ParseInt(const string& text, int& value) {
    try {
        size_t pos = 0;
        value = stoi(text, &pos);
        return pos == text.size();
    } catch (const exception&) {
        return false;
    }
}

// Вывод результата команды
void PrintResult(const string& text) {
    cout << "-> " << text << endl;
}

// Вывод сообщения об ошибке
void PrintError(const string& text) {
    cout << "-> ERROR: " << text << endl;
}

// ---------------- Работа с файлом ----------------

// Чтение непустых строк файла в массив.
// Строка файла: <тип> <имя> <элемент1> <элемент2> ...
void LoadFile(const string& fileName, Array& lines) {
    ifstream file(fileName);
    if (!file.is_open()) {
        return;
    }
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            ArrayPush(lines, line);
        }
    }
}

// Перезапись файла строками из массива
bool SaveFile(const string& fileName, const Array& lines) {
    ofstream file(fileName);
    if (!file.is_open()) {
        return false;
    }
    for (int i = 0; i < lines.size; i++) {
        file << lines.data[i] << endl;
    }
    return true;
}

// Индекс строки со структурой name или -1
int FindLine(const Array& lines, const string& name) {
    for (int i = 0; i < lines.size; i++) {
        istringstream stream(lines.data[i]);
        string type;
        string lineName;
        stream >> type >> lineName;
        if (lineName == name) {
            return i;
        }
    }
    return -1;
}

// Разбор строки файла на тип структуры и её элементы
void ParseLine(const string& line, string& type, Array& items) {
    istringstream stream(line);
    string name;
    stream >> type >> name;
    string word;
    while (stream >> word) {
        ArrayPush(items, word);
    }
}

// Тип структуры по первой букве команды
string CommandType(const string& command) {
    if (command == "ISMEMBER") {
        return "T";
    }
    // Команды множества начинаются с SET, поэтому проверяются раньше стека (S)
    if (command.substr(0, 3) == "SET") {
        return "H";
    }
    string first = command.substr(0, 1);
    if (first == "M" || first == "F" || first == "L" || first == "Q" || first == "S" || first == "T") {
        return first;
    }
    return "";
}

// ---------------- Массив (M) ----------------

// Выполнение команды над массивом.
// Возвращает true, если массив изменился и его нужно сохранить
bool ArrayCommand(const string& command, const Array& words, Array& arr) {
    int count = words.size;
    int index = 0;
    string value;

    if (command == "MCREATE" && count == 2) {
        PrintResult(words.data[1]);
        return true;
    }
    if (command == "MPUSH" && count == 3) {
        ArrayPush(arr, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "MINSERT" && count == 4) {
        if (!ParseInt(words.data[2], index) || !ArrayInsert(arr, index, words.data[3])) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "MGET" && count == 3) {
        if (!ParseInt(words.data[2], index) || !ArrayGet(arr, index, value)) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(value);
        return false;
    }
    if (command == "MSET" && count == 4) {
        if (!ParseInt(words.data[2], index) || !ArraySet(arr, index, words.data[3])) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "MDEL" && count == 3) {
        if (!ParseInt(words.data[2], index) || !ArrayDelete(arr, index, value)) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "MLEN" && count == 2) {
        PrintResult(to_string(ArrayLength(arr)));
        return false;
    }
    if (command == "PRINT") {
        cout << "-> ";
        PrintArray(arr);
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка массива из элементов файла, выполнение команды и запись результата в saved
bool RunArray(const string& command, const Array& words, const Array& items, string& saved) {
    Array arr;
    ArrayInit(arr, items.size);
    for (int i = 0; i < items.size; i++) {
        ArrayPush(arr, items.data[i]);
    }

    bool changed = ArrayCommand(command, words, arr);
    if (changed) {
        for (int i = 0; i < arr.size; i++) {
            AppendWord(saved, arr.data[i]);
        }
    }

    DeleteArray(arr);
    return changed;
}

// ---------------- Односвязный список (F) ----------------

// Выполнение команды над односвязным списком, true если список изменился
bool FListCommand(const string& command, const Array& words, FList& list) {
    int count = words.size;
    int index = 0;
    string value;

    // FPUSH без уточнения — добавление в хвост, как в таблице задания
    if ((command == "FPUSH" || command == "FPUSHTAIL") && count == 3) {
        AddTail(list, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "FPUSHHEAD" && count == 3) {
        AddHead(list, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "FPUSHBEFORE" && count == 4) {
        if (!AddBefore(list, words.data[2], words.data[3])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "FPUSHAFTER" && count == 4) {
        if (!AddAfter(list, words.data[2], words.data[3])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "FDELHEAD" && count == 2) {
        if (!DeleteHead(list, value)) {
            PrintError("list is empty");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "FDELTAIL" && count == 2) {
        if (!DeleteTail(list, value)) {
            PrintError("list is empty");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "FDELBEFORE" && count == 3) {
        if (!DeleteBefore(list, words.data[2], value)) {
            PrintError("nothing to delete before " + words.data[2]);
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "FDELAFTER" && count == 3) {
        if (!DeleteAfter(list, words.data[2], value)) {
            PrintError("nothing to delete after " + words.data[2]);
            return false;
        }
        PrintResult(value);
        return true;
    }
    // FDEL — удаление по значению
    if (command == "FDEL" && count == 3) {
        if (!DeleteByValue(list, words.data[2])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "FGET" && count == 3) {
        if (!ParseInt(words.data[2], index) || !GetByIndex(list, index, value)) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(value);
        return false;
    }
    if (command == "FFIND" && count == 3) {
        if (FindNode(list, words.data[2]) != nullptr) {
            PrintResult("TRUE");
        } else {
            PrintResult("FALSE");
        }
        return false;
    }
    if (command == "FPRINTREV" && count == 2) {
        cout << "-> ";
        PrintReverse(list);
        return false;
    }
    if (command == "PRINT") {
        cout << "-> ";
        PrintList(list);
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка односвязного списка из элементов файла, выполнение команды и запись результата в saved
bool RunFList(const string& command, const Array& words, const Array& items, string& saved) {
    FList list;
    InitList(list);
    for (int i = 0; i < items.size; i++) {
        AddTail(list, items.data[i]);
    }

    bool changed = FListCommand(command, words, list);
    if (changed) {
        Node* current = list.head;
        while (current != nullptr) {
            AppendWord(saved, current->data);
            current = current->next;
        }
    }

    DeleteAll(list);
    return changed;
}

// ---------------- Двусвязный список (L) ----------------

// Выполнение команды над двусвязным списком, true если список изменился
bool DListCommand(const string& command, const Array& words, DList& list) {
    int count = words.size;
    int index = 0;
    string value;

    // LPUSH без уточнения — добавление в хвост, как в таблице задания
    if ((command == "LPUSH" || command == "LPUSHTAIL") && count == 3) {
        AddTail(list, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "LPUSHHEAD" && count == 3) {
        AddHead(list, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "LPUSHBEFORE" && count == 4) {
        if (!AddBefore(list, words.data[2], words.data[3])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "LPUSHAFTER" && count == 4) {
        if (!AddAfter(list, words.data[2], words.data[3])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[3]);
        return true;
    }
    if (command == "LDELHEAD" && count == 2) {
        if (!DeleteHead(list, value)) {
            PrintError("list is empty");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "LDELTAIL" && count == 2) {
        if (!DeleteTail(list, value)) {
            PrintError("list is empty");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "LDELBEFORE" && count == 3) {
        if (!DeleteBefore(list, words.data[2], value)) {
            PrintError("nothing to delete before " + words.data[2]);
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "LDELAFTER" && count == 3) {
        if (!DeleteAfter(list, words.data[2], value)) {
            PrintError("nothing to delete after " + words.data[2]);
            return false;
        }
        PrintResult(value);
        return true;
    }
    // LDEL — удаление по значению
    if (command == "LDEL" && count == 3) {
        if (!DeleteByValue(list, words.data[2])) {
            PrintError("element " + words.data[2] + " not found");
            return false;
        }
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "LGET" && count == 3) {
        if (!ParseInt(words.data[2], index) || !GetByIndex(list, index, value)) {
            PrintError("invalid index");
            return false;
        }
        PrintResult(value);
        return false;
    }
    if (command == "LFIND" && count == 3) {
        if (FindNode(list, words.data[2]) != nullptr) {
            PrintResult("TRUE");
        } else {
            PrintResult("FALSE");
        }
        return false;
    }
    if (command == "LPRINTREV" && count == 2) {
        cout << "-> ";
        PrintReverse(list);
        return false;
    }
    if (command == "PRINT") {
        cout << "-> ";
        PrintList(list);
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка двусвязного списка из элементов файла, выполнение команды и запись результата в saved
bool RunDList(const string& command, const Array& words, const Array& items, string& saved) {
    DList list;
    InitList(list);
    for (int i = 0; i < items.size; i++) {
        AddTail(list, items.data[i]);
    }

    bool changed = DListCommand(command, words, list);
    if (changed) {
        DNode* current = list.head;
        while (current != nullptr) {
            AppendWord(saved, current->data);
            current = current->next;
        }
    }

    DeleteAll(list);
    return changed;
}

// ---------------- Стек (S) ----------------

// Выполнение команды над стеком, true если стек изменился
bool StackCommand(const string& command, const Array& words, Stack& stack) {
    if (command == "SPUSH" && words.size == 3) {
        Push(stack, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "SPOP" && words.size == 2) {
        if (IsEmpty(stack)) {
            PrintError("stack is empty");
            return false;
        }
        string value = CheckFirst(stack);
        PopTop(stack);
        PrintResult(value);
        return true;
    }
    if (command == "PRINT") {
        cout << "-> ";
        PrintStack(stack);
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка стека из элементов файла, выполнение команды и запись результата в saved
bool RunStack(const string& command, const Array& words, const Array& items, string& saved) {
    Stack stack;
    InitStack(stack);
    // В файле стек записан от вершины ко дну, поэтому кладём с конца
    for (int i = items.size - 1; i >= 0; i--) {
        Push(stack, items.data[i]);
    }

    bool changed = StackCommand(command, words, stack);
    if (changed) {
        SNode* current = stack.top;
        while (current != nullptr) {
            AppendWord(saved, current->data);
            current = current->next;
        }
    }

    DeleteStack(stack);
    return changed;
}

// ---------------- Очередь (Q) ----------------

// Выполнение команды над очередью, true если очередь изменилась
bool QueueCommand(const string& command, const Array& words, Queue& queue) {
    if (command == "QPUSH" && words.size == 3) {
        Push(queue, words.data[2]);
        PrintResult(words.data[2]);
        return true;
    }
    if (command == "QPOP" && words.size == 2) {
        if (IsEmpty(queue)) {
            PrintError("queue is empty");
            return false;
        }
        string value = CheckFirst(queue);
        PopFront(queue);
        PrintResult(value);
        return true;
    }
    if (command == "PRINT") {
        cout << "-> ";
        PrintQueue(queue);
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка очереди из элементов файла, выполнение команды и запись результата в saved
bool RunQueue(const string& command, const Array& words, const Array& items, string& saved) {
    Queue queue;
    InitQueue(queue);
    for (int i = 0; i < items.size; i++) {
        Push(queue, items.data[i]);
    }

    bool changed = QueueCommand(command, words, queue);
    if (changed) {
        QNode* current = queue.head;
        while (current != nullptr) {
            AppendWord(saved, current->data);
            current = current->next;
        }
    }

    DeleteQueue(queue);
    return changed;
}

// ---------------- АВЛ-дерево (T) ----------------

// Выполнение команды над АВЛ-деревом, true если дерево изменилось
bool TreeCommand(const string& command, const Array& words, TreeNode*& root) {
    int count = words.size;
    int value = 0;
    string result;

    if (command == "PRINT") {
        if (root == nullptr) {
            PrintResult("(empty)");
            return false;
        }
        cout << "->" << endl;
        PrintTree(root, 1);
        return false;
    }
    if ((command == "TINORDER" || command == "TPREORDER" || command == "TPOSTORDER" || command == "TBFS") && count == 2) {
        if (command == "TINORDER") {
            InOrder(root, result);
        } else if (command == "TPREORDER") {
            PreOrder(root, result);
        } else if (command == "TPOSTORDER") {
            PostOrder(root, result);
        } else {
            LevelOrder(root, result);
        }
        if (result.empty()) {
            result = "(empty)";
        }
        PrintResult(result);
        return false;
    }

    bool known = command == "TINSERT" || command == "TDEL" || command == "TGET" || command == "ISMEMBER";
    if (!known || count != 3 || !ParseInt(words.data[2], value)) {
        PrintError("unknown command or wrong arguments");
        return false;
    }

    if (command == "TINSERT") {
        if (!Insert(root, value)) {
            PrintError("value already exists");
            return false;
        }
        PrintResult(to_string(value));
        return true;
    }
    if (command == "TDEL") {
        if (!Remove(root, value)) {
            PrintError("value not found");
            return false;
        }
        PrintResult(to_string(value));
        return true;
    }
    // TGET и ISMEMBER — поиск элемента
    if (Contains(root, value)) {
        PrintResult("TRUE");
    } else {
        PrintResult("FALSE");
    }
    return false;
}

// Сборка АВЛ-дерева из элементов файла, выполнение команды и запись результата в saved
bool RunTree(const string& command, const Array& words, const Array& items, string& saved) {
    TreeNode* root = nullptr;
    for (int i = 0; i < items.size; i++) {
        int value = 0;
        if (ParseInt(items.data[i], value)) {
            Insert(root, value);
        }
    }

    bool changed = TreeCommand(command, words, root);
    if (changed) {
        // Сохранение в ширину: при загрузке в этом порядке АВЛ-дерево
        // собирается без поворотов и получает ту же форму
        LevelOrder(root, saved);
    }

    DeleteAll(root);
    return changed;
}

// ---------------- Множество (H) ----------------

// Выполнение команды над множеством, true если множество изменилось
bool SetCommand(const string& command, const Array& words, Set& set) {
    if (command == "PRINT") {
        cout << "-> ";
        PrintSet(set);
        return false;
    }
    if (words.size != 3) {
        PrintError("unknown command or wrong arguments");
        return false;
    }

    const string& value = words.data[2];
    if (command == "SETADD") {
        if (!SetAdd(set, value)) {
            PrintError("value already exists");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "SETDEL") {
        if (!SetDel(set, value)) {
            PrintError("value not found");
            return false;
        }
        PrintResult(value);
        return true;
    }
    if (command == "SET_AT") {
        if (SetAt(set, value)) {
            PrintResult("TRUE");
        } else {
            PrintResult("FALSE");
        }
        return false;
    }

    PrintError("unknown command or wrong arguments");
    return false;
}

// Сборка множества из элементов файла, выполнение команды и запись результата в saved
bool RunSet(const string& command, const Array& words, const Array& items, string& saved) {
    Set set;
    SetInit(set, 8);
    for (int i = 0; i < items.size; i++) {
        SetAdd(set, items.data[i]);
    }

    bool changed = SetCommand(command, words, set);
    if (changed) {
        saved = SetToString(set);
    }

    DeleteSet(set);
    return changed;
}

// ---------------- Выполнение запроса ----------------

// Разбор запроса, загрузка структуры из файла, выполнение команды и сохранение изменений
void ExecuteQuery(const string& fileName, const string& query) {
    Array words;
    ArrayInit(words, 4);
    SplitWords(query, words);

    if (words.size < 2) {
        PrintError("query format: <COMMAND> <name> [args]");
        DeleteArray(words);
        return;
    }

    string command = ToUpper(words.data[0]);
    string name = words.data[1];

    Array lines;
    ArrayInit(lines, 8);
    LoadFile(fileName, lines);

    Array items;
    ArrayInit(items, 8);
    string storedType;
    int index = FindLine(lines, name);
    if (index != -1) {
        ParseLine(lines.data[index], storedType, items);
    }

    // PRINT работает с любой структурой, тип берётся из файла
    string type = CommandType(command);
    if (command == "PRINT") {
        type = storedType;
    }

    if (command == "PRINT" && index == -1) {
        PrintError("structure " + name + " not found");
    } else if (type.empty()) {
        PrintError("unknown command " + command);
    } else if (command == "MCREATE" && index != -1) {
        PrintError("structure " + name + " already exists");
    } else if (index != -1 && storedType != type) {
        PrintError("name " + name + " is already used by structure of type " + storedType);
    } else {
        string saved;
        bool changed = false;

        if (type == "M") {
            changed = RunArray(command, words, items, saved);
        } else if (type == "F") {
            changed = RunFList(command, words, items, saved);
        } else if (type == "L") {
            changed = RunDList(command, words, items, saved);
        } else if (type == "S") {
            changed = RunStack(command, words, items, saved);
        } else if (type == "Q") {
            changed = RunQueue(command, words, items, saved);
        } else if (type == "T") {
            changed = RunTree(command, words, items, saved);
        } else if (type == "H") {
            changed = RunSet(command, words, items, saved);
        } else {
            PrintError("unknown structure type " + type + " in file");
        }

        // Файл перезаписывается только при изменении состава элементов
        if (changed) {
            string line = type + " " + name;
            if (!saved.empty()) {
                line += " " + saved;
            }
            if (index == -1) {
                ArrayPush(lines, line);
            } else {
                ArraySet(lines, index, line);
            }
            if (!SaveFile(fileName, lines)) {
                PrintError("cannot write file " + fileName);
            }
        }
    }

    DeleteArray(items);
    DeleteArray(lines);
    DeleteArray(words);
}
