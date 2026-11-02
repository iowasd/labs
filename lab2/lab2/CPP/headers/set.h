#pragma once

#include <string>

#include "flist.h"

using namespace std;

// Множество строк на хеш-таблице с цепочками.
// Каждая корзина — односвязный список FList из ЛР1
struct Set {
    FList* buckets;
    int capacity;
    int size;
};

void SetInit(Set& set, int capacity);
bool SetAdd(Set& set, const string& value);
bool SetDel(Set& set, const string& value);
bool SetAt(const Set& set, const string& value);
int SetSize(const Set& set);

// Элементы через пробел в порядке обхода корзин
string SetToString(const Set& set);
void PrintSet(const Set& set);
void DeleteSet(Set& set);
