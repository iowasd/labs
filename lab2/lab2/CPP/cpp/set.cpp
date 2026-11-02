#include "../headers/set.h"

#include <iostream>

using namespace std;

// Полиномиальный хеш строки, сводится к размеру таблицы методом деления
unsigned int SetHash(const string& value, int capacity) {
    unsigned int hash = 0;
    for (size_t i = 0; i < value.size(); i++) {
        hash = hash * 31 + static_cast<unsigned char>(value[i]);
    }
    return hash % static_cast<unsigned int>(capacity);
}

// Создание пустого множества с заданным числом корзин (минимум 1)
void SetInit(Set& set, int capacity) {
    if (capacity < 1) {
        capacity = 1;
    }
    set.buckets = new FList[capacity];
    for (int i = 0; i < capacity; i++) {
        InitList(set.buckets[i]);
    }
    set.capacity = capacity;
    set.size = 0;
}

// Перенос всех элементов в таблицу с вдвое большим числом корзин
void SetGrow(Set& set) {
    Set bigger;
    SetInit(bigger, set.capacity * 2);
    for (int i = 0; i < set.capacity; i++) {
        Node* current = set.buckets[i].head;
        while (current != nullptr) {
            FList& bucket = bigger.buckets[SetHash(current->data, bigger.capacity)];
            AddHead(bucket, current->data);
            bigger.size++;
            current = current->next;
        }
    }
    DeleteSet(set);
    set = bigger;
}

// Добавление элемента, false если он уже есть
bool SetAdd(Set& set, const string& value) {
    if (SetAt(set, value)) {
        return false;
    }
    // Коэффициент заполнения не превышает 0.75, поэтому цепочки короткие
    if ((set.size + 1) * 4 > set.capacity * 3) {
        SetGrow(set);
    }
    AddHead(set.buckets[SetHash(value, set.capacity)], value);
    set.size++;
    return true;
}

// Удаление элемента, false если его нет
bool SetDel(Set& set, const string& value) {
    if (!DeleteByValue(set.buckets[SetHash(value, set.capacity)], value)) {
        return false;
    }
    set.size--;
    return true;
}

// Проверка принадлежности: просматривается только одна корзина
bool SetAt(const Set& set, const string& value) {
    return FindNode(set.buckets[SetHash(value, set.capacity)], value) != nullptr;
}

// Количество элементов
int SetSize(const Set& set) {
    return set.size;
}

// Элементы через пробел в порядке обхода корзин
string SetToString(const Set& set) {
    string result;
    for (int i = 0; i < set.capacity; i++) {
        Node* current = set.buckets[i].head;
        while (current != nullptr) {
            if (!result.empty()) {
                result += " ";
            }
            result += current->data;
            current = current->next;
        }
    }
    return result;
}

// Вывод в формате {a, b, c}
void PrintSet(const Set& set) {
    cout << "{";
    bool first = true;
    for (int i = 0; i < set.capacity; i++) {
        Node* current = set.buckets[i].head;
        while (current != nullptr) {
            if (!first) {
                cout << ", ";
            }
            cout << current->data;
            first = false;
            current = current->next;
        }
    }
    cout << "}" << endl;
}

// Освобождение всех корзин
void DeleteSet(Set& set) {
    for (int i = 0; i < set.capacity; i++) {
        DeleteAll(set.buckets[i]);
    }
    delete[] set.buckets;
    set.buckets = nullptr;
    set.capacity = 0;
    set.size = 0;
}
