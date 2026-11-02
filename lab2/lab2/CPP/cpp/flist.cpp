#include "../headers/flist.h"

#include <iostream>

using namespace std;

// Инициализация пустого списка
void InitList(FList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}

// Чтение от головы к хвосту
void PrintList(const FList& list) {
    if (list.head == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    Node* current = list.head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

// Рекурсия: сначала выводится остаток списка, потом текущий узел
void PrintReverseRec(Node* node) {
    if (node == nullptr) {
        return;
    }
    PrintReverseRec(node->next);
    if (node->next != nullptr) {
        cout << " <- ";
    }
    cout << node->data;
}

// Чтение от хвоста к голове
void PrintReverse(const FList& list) {
    if (list.head == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    PrintReverseRec(list.head);
    cout << endl;
}

// Поиск первого узла с заданным значением
Node* FindNode(const FList& list, const string& value) {
    Node* current = list.head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Получение значения по индексу проходом с головы
bool GetByIndex(const FList& list, int index, string& result) {
    Node* current = list.head;
    int i = 0;
    while (current != nullptr) {
        if (i == index) {
            result = current->data;
            return true;
        }
        current = current->next;
        i++;
    }
    return false;
}

// Добавление в голову
void AddHead(FList& list, const string& data) {
    Node* NewNode = new Node{data, list.head};
    list.head = NewNode;
    if (list.tail == nullptr) {
        list.tail = NewNode;
    }
}

// Добавление в хвост за O(1) благодаря указателю tail
void AddTail(FList& list, const string& data) {
    Node* NewNode = new Node{data, nullptr};
    if (list.tail == nullptr) {
        list.head = NewNode;
        list.tail = NewNode;
        return;
    }
    list.tail->next = NewNode;
    list.tail = NewNode;
}

// Вставка перед target: нужен предыдущий узел, поэтому идём с головы
bool AddBefore(FList& list, const string& target, const string& data) {
    if (list.head == nullptr) {
        return false;
    }
    if (list.head->data == target) {
        AddHead(list, data);
        return true;
    }

    Node* prev = list.head;
    while (prev->next != nullptr && prev->next->data != target) {
        prev = prev->next;
    }
    if (prev->next == nullptr) {
        return false;
    }

    Node* NewNode = new Node{data, prev->next};
    prev->next = NewNode;
    return true;
}

// Вставка после target
bool AddAfter(FList& list, const string& target, const string& data) {
    Node* current = FindNode(list, target);
    if (current == nullptr) {
        return false;
    }

    Node* NewNode = new Node{data, current->next};
    current->next = NewNode;
    if (current == list.tail) {
        list.tail = NewNode;
    }
    return true;
}

// Удаление головы
bool DeleteHead(FList& list, string& removed) {
    if (list.head == nullptr) {
        return false;
    }
    Node* tmp = list.head;
    removed = tmp->data;
    list.head = tmp->next;
    if (list.head == nullptr) {
        list.tail = nullptr;
    }
    delete tmp;
    return true;
}

// Хвост знаем сразу, но для удаления нужен предпоследний узел — проход O(n)
bool DeleteTail(FList& list, string& removed) {
    if (list.head == nullptr) {
        return false;
    }
    if (list.head == list.tail) {
        return DeleteHead(list, removed);
    }

    Node* prev = list.head;
    while (prev->next != list.tail) {
        prev = prev->next;
    }
    removed = list.tail->data;
    delete list.tail;
    prev->next = nullptr;
    list.tail = prev;
    return true;
}

// Удаление узла перед target: ищем узел, у которого через один стоит target
bool DeleteBefore(FList& list, const string& target, string& removed) {
    if (list.head == nullptr || list.head->next == nullptr || list.head->data == target) {
        return false;
    }
    if (list.head->next->data == target) {
        return DeleteHead(list, removed);
    }

    Node* prev = list.head;
    while (prev->next->next != nullptr && prev->next->next->data != target) {
        prev = prev->next;
    }
    if (prev->next->next == nullptr) {
        return false;
    }

    Node* tmp = prev->next;
    removed = tmp->data;
    prev->next = tmp->next;
    delete tmp;
    return true;
}

// Удаление узла после target
bool DeleteAfter(FList& list, const string& target, string& removed) {
    Node* current = FindNode(list, target);
    if (current == nullptr || current->next == nullptr) {
        return false;
    }

    Node* tmp = current->next;
    removed = tmp->data;
    current->next = tmp->next;
    if (tmp == list.tail) {
        list.tail = current;
    }
    delete tmp;
    return true;
}

// Удаление первого узла с заданным значением
bool DeleteByValue(FList& list, const string& value) {
    if (list.head == nullptr) {
        return false;
    }
    if (list.head->data == value) {
        string removed;
        return DeleteHead(list, removed);
    }

    Node* prev = list.head;
    while (prev->next != nullptr && prev->next->data != value) {
        prev = prev->next;
    }
    if (prev->next == nullptr) {
        return false;
    }

    Node* tmp = prev->next;
    prev->next = tmp->next;
    if (tmp == list.tail) {
        list.tail = prev;
    }
    delete tmp;
    return true;
}

// Удаление всех узлов списка
void DeleteAll(FList& list) {
    while (list.head != nullptr) {
        Node* tmp = list.head;
        list.head = list.head->next;
        delete tmp;
    }
    list.tail = nullptr;
}
