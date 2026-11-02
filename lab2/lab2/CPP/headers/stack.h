#pragma once

#include <string>

using namespace std;

// Узел стека
struct SNode {
    string data;
    SNode* next;
};

// Стек на основе односвязного списка, top — вершина
struct Stack {
    SNode* top;
};

void InitStack(Stack& stack);
void Push(Stack& stack, const string& value);
void PopTop(Stack& stack);
string CheckFirst(const Stack& stack);
bool IsEmpty(const Stack& stack);
void PrintStack(const Stack& stack);
void DeleteStack(Stack& stack);
