#include "../headers/array.h"

#include <iostream>

using namespace std;

// Инициализация пустого массива с заданной ёмкостью (минимум 1)
void ArrayInit(Array& arr, int capacity) {
    if (capacity < 1) {
        capacity = 1;
    }
    arr.data = new string[capacity];
    arr.size = 0;
    arr.capacity = capacity;
}

// Увеличение ёмкости в 2 раза, когда массив заполнен
void ArrayGrow(Array& arr) {
    int newCapacity = arr.capacity * 2;
    string* newData = new string[newCapacity];
    for (int i = 0; i < arr.size; i++) {
        newData[i] = arr.data[i];
    }
    delete[] arr.data;
    arr.data = newData;
    arr.capacity = newCapacity;
}

// Добавление в конец
void ArrayPush(Array& arr, const string& value) {
    if (arr.size == arr.capacity) {
        ArrayGrow(arr);
    }
    arr.data[arr.size] = value;
    arr.size++;
}

// Добавление по индексу: элементы правее сдвигаются на одну позицию
bool ArrayInsert(Array& arr, int index, const string& value) {
    if (index < 0 || index > arr.size) {
        return false;
    }
    if (arr.size == arr.capacity) {
        ArrayGrow(arr);
    }
    for (int i = arr.size; i > index; i--) {
        arr.data[i] = arr.data[i - 1];
    }
    arr.data[index] = value;
    arr.size++;
    return true;
}

// Получение элемента по индексу, false при выходе за границы
bool ArrayGet(const Array& arr, int index, string& result) {
    if (index < 0 || index >= arr.size) {
        return false;
    }
    result = arr.data[index];
    return true;
}

// Замена элемента по индексу, false при выходе за границы
bool ArraySet(Array& arr, int index, const string& value) {
    if (index < 0 || index >= arr.size) {
        return false;
    }
    arr.data[index] = value;
    return true;
}

// Удаление по индексу: элементы правее сдвигаются влево
bool ArrayDelete(Array& arr, int index, string& removed) {
    if (index < 0 || index >= arr.size) {
        return false;
    }
    removed = arr.data[index];
    for (int i = index; i < arr.size - 1; i++) {
        arr.data[i] = arr.data[i + 1];
    }
    arr.size--;
    return true;
}

// Количество элементов в массиве
int ArrayLength(const Array& arr) {
    return arr.size;
}

// Вывод массива в формате [a, b, c]
void PrintArray(const Array& arr) {
    cout << "[";
    for (int i = 0; i < arr.size; i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << arr.data[i];
    }
    cout << "]" << endl;
}

// Освобождение памяти и обнуление полей
void DeleteArray(Array& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}
