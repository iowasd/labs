#include "../headers/dlist.h"

#include <iostream>

using namespace std;

// Инициализация пустого списка
void InitList(DList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}

// Чтение от головы к хвосту
void PrintList(const DList& list) {
    if (list.head == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    DNode* current = list.head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " <-> ";
        }
        current = current->next;
    }
    cout << endl;
}

// Чтение от хвоста к голове по указателям prev
void PrintReverse(const DList& list) {
    if (list.tail == nullptr) {
        cout << "(empty)" << endl;
        return;
    }
    DNode* current = list.tail;
    while (current != nullptr) {
        cout << current->data;
        if (current->prev != nullptr) {
            cout << " <-> ";
        }
        current = current->prev;
    }
    cout << endl;
}

// Поиск первого узла с заданным значением
DNode* FindNode(const DList& list, const string& value) {
    DNode* current = list.head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Получение значения по индексу проходом с головы
bool GetByIndex(const DList& list, int index, string& result) {
    DNode* current = list.head;
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
void AddHead(DList& list, const string& data) {
    DNode* NewNode = new DNode{data, list.head, nullptr};
    if (list.head != nullptr) {
        list.head->prev = NewNode;
    } else {
        list.tail = NewNode;
    }
    list.head = NewNode;
}

// Добавление в хвост
void AddTail(DList& list, const string& data) {
    DNode* NewNode = new DNode{data, nullptr, list.tail};
    if (list.tail != nullptr) {
        list.tail->next = NewNode;
    } else {
        list.head = NewNode;
    }
    list.tail = NewNode;
}

// Благодаря prev предыдущий узел известен сразу после поиска target
bool AddBefore(DList& list, const string& target, const string& data) {
    DNode* current = FindNode(list, target);
    if (current == nullptr) {
        return false;
    }

    DNode* NewNode = new DNode{data, current, current->prev};
    if (current->prev != nullptr) {
        current->prev->next = NewNode;
    } else {
        list.head = NewNode;
    }
    current->prev = NewNode;
    return true;
}

// Вставка после target
bool AddAfter(DList& list, const string& target, const string& data) {
    DNode* current = FindNode(list, target);
    if (current == nullptr) {
        return false;
    }

    DNode* NewNode = new DNode{data, current->next, current};
    if (current->next != nullptr) {
        current->next->prev = NewNode;
    } else {
        list.tail = NewNode;
    }
    current->next = NewNode;
    return true;
}

// Отвязывает узел от соседей и удаляет его за O(1)
void RemoveNode(DList& list, DNode* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        list.head = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        list.tail = node->prev;
    }
    delete node;
}

// Удаление головы
bool DeleteHead(DList& list, string& removed) {
    if (list.head == nullptr) {
        return false;
    }
    removed = list.head->data;
    RemoveNode(list, list.head);
    return true;
}

// Удаление хвоста за O(1) благодаря prev
bool DeleteTail(DList& list, string& removed) {
    if (list.tail == nullptr) {
        return false;
    }
    removed = list.tail->data;
    RemoveNode(list, list.tail);
    return true;
}

// Удаление узла перед target
bool DeleteBefore(DList& list, const string& target, string& removed) {
    DNode* current = FindNode(list, target);
    if (current == nullptr || current->prev == nullptr) {
        return false;
    }
    removed = current->prev->data;
    RemoveNode(list, current->prev);
    return true;
}

// Удаление узла после target
bool DeleteAfter(DList& list, const string& target, string& removed) {
    DNode* current = FindNode(list, target);
    if (current == nullptr || current->next == nullptr) {
        return false;
    }
    removed = current->next->data;
    RemoveNode(list, current->next);
    return true;
}

// Удаление первого узла с заданным значением
bool DeleteByValue(DList& list, const string& value) {
    DNode* current = FindNode(list, value);
    if (current == nullptr) {
        return false;
    }
    RemoveNode(list, current);
    return true;
}

// Удаление всех узлов списка
void DeleteAll(DList& list) {
    while (list.head != nullptr) {
        DNode* tmp = list.head;
        list.head = list.head->next;
        delete tmp;
    }
    list.tail = nullptr;
}
