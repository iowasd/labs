#pragma once

#include <string>

using namespace std;

// Динамический массив строк
struct Array {
    string* data;
    int size;
    int capacity;
};

void ArrayInit(Array& arr, int capacity);
void ArrayPush(Array& arr, const string& value);
bool ArrayInsert(Array& arr, int index, const string& value);
bool ArrayGet(const Array& arr, int index, string& result);
bool ArraySet(Array& arr, int index, const string& value);
bool ArrayDelete(Array& arr, int index, string& removed);
int ArrayLength(const Array& arr);
void PrintArray(const Array& arr);
void DeleteArray(Array& arr);
