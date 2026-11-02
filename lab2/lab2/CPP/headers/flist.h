#pragma once

#include <string>

using namespace std;

// Узел односвязного списка
struct Node {
    string data;
    Node* next;
};

// Односвязный список, указатель на хвост даёт добавление в конец за O(1)
struct FList {
    Node* head;
    Node* tail;
};

void InitList(FList& list);

// Чтение и поиск
void PrintList(const FList& list);
void PrintReverse(const FList& list);
Node* FindNode(const FList& list, const string& value);
bool GetByIndex(const FList& list, int index, string& result);

// Добавление
void AddHead(FList& list, const string& data);
void AddTail(FList& list, const string& data);
bool AddBefore(FList& list, const string& target, const string& data);
bool AddAfter(FList& list, const string& target, const string& data);

// Удаление
bool DeleteHead(FList& list, string& removed);
bool DeleteTail(FList& list, string& removed);
bool DeleteBefore(FList& list, const string& target, string& removed);
bool DeleteAfter(FList& list, const string& target, string& removed);
bool DeleteByValue(FList& list, const string& value);
void DeleteAll(FList& list);
