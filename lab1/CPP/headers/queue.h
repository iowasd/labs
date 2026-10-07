#pragma once

#include <string>

using namespace std;

// Узел очереди
struct QNode {
    string data;
    QNode* next;
};

// Очередь: добавление в хвост, извлечение из головы
struct Queue {
    QNode* head;
    QNode* tail;
};

void InitQueue(Queue& queue);
void Push(Queue& queue, const string& value);
void PopFront(Queue& queue);
string CheckFirst(const Queue& queue);
bool IsEmpty(const Queue& queue);
void PrintQueue(const Queue& queue);
void DeleteQueue(Queue& queue);
