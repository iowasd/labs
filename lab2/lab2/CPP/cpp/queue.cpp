#include "../headers/queue.h"

#include <iostream>

using namespace std;

// Инициализация пустой очереди
void InitQueue(Queue& queue) {
    queue.head = nullptr;
    queue.tail = nullptr;
}

// Указатель на хвост даёт добавление за O(1)
void Push(Queue& queue, const string& value) {
    QNode* NewNode = new QNode{value, nullptr};

    if (queue.tail == nullptr) {
        queue.head = NewNode;
        queue.tail = NewNode;
        return;
    }

    queue.tail->next = NewNode;
    queue.tail = NewNode;
}

// Удаление элемента из головы
void PopFront(Queue& queue) {
    if (queue.head == nullptr) {
        return;
    }

    QNode* tmp = queue.head;
    queue.head = queue.head->next;
    if (queue.head == nullptr) {
        queue.tail = nullptr;
    }
    delete tmp;
}

// Значение первого элемента, пустая строка для пустой очереди
string CheckFirst(const Queue& queue) {
    if (queue.head == nullptr) {
        return "";
    }
    return queue.head->data;
}

// Проверка очереди на пустоту
bool IsEmpty(const Queue& queue) {
    if (queue.head == nullptr) {
        return true;
    }
    return false;
}

// Чтение от головы к хвосту
void PrintQueue(const Queue& queue) {
    if (queue.head == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    cout << "(head)";
    QNode* current = queue.head;
    while (current != nullptr) {
        cout << " " << current->data;
        current = current->next;
    }
    cout << " (tail)" << endl;
}

// Удаление всех элементов очереди
void DeleteQueue(Queue& queue) {
    while (queue.head != nullptr) {
        QNode* tmp = queue.head;
        queue.head = queue.head->next;
        delete tmp;
    }
    queue.tail = nullptr;
}
