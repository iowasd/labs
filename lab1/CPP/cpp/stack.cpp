#include "../headers/stack.h"

#include <iostream>

using namespace std;

// Инициализация пустого стека
void InitStack(Stack& stack) {
    stack.top = nullptr;
}

// Добавление элемента на вершину
void Push(Stack& stack, const string& value) {
    SNode* NewNode = new SNode{value, nullptr};

    if (stack.top == nullptr) {
        stack.top = NewNode;
        return;
    }

    NewNode->next = stack.top;
    stack.top = NewNode;
}

// Удаление элемента с вершины
void PopTop(Stack& stack) {
    if (stack.top == nullptr) {
        return;
    }

    SNode* tmp = stack.top;
    stack.top = stack.top->next;
    delete tmp;
}

// Значение на вершине, пустая строка для пустого стека
string CheckFirst(const Stack& stack) {
    if (stack.top == nullptr) {
        return "";
    }
    return stack.top->data;
}

// Проверка стека на пустоту
bool IsEmpty(const Stack& stack) {
    if (stack.top == nullptr) {
        return true;
    }
    return false;
}

// Чтение от вершины ко дну
void PrintStack(const Stack& stack) {
    if (stack.top == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    cout << "(top)";
    SNode* current = stack.top;
    while (current != nullptr) {
        cout << " " << current->data;
        current = current->next;
    }
    cout << endl;
}

// Удаление всех элементов стека
void DeleteStack(Stack& stack) {
    while (stack.top != nullptr) {
        SNode* tmp = stack.top;
        stack.top = stack.top->next;
        delete tmp;
    }
}
