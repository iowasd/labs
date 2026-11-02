#include "../headers/lfu.h"

#include <iostream>

using namespace std;

// Список ключей с заданной частотой, при отсутствии создаётся
DList* FreqList(LfuCache& cache, int freq) {
    DList* list = nullptr;
    if (ChainGet(cache.freqs, to_string(freq), list)) {
        return list;
    }
    list = new DList;
    InitList(*list);
    ChainPut(cache.freqs, to_string(freq), list);
    return list;
}

// Исключение ключа из списка его частоты; пустой список удаляется
void DetachItem(LfuCache& cache, LfuItem* item) {
    DList* list = FreqList(cache, item->freq);
    RemoveNode(*list, item->node);
    item->node = nullptr;
    if (list->head == nullptr) {
        delete list;
        ChainRemove(cache.freqs, to_string(item->freq));
        if (cache.minFreq == item->freq) {
            cache.minFreq++;
        }
    }
}

// Добавление ключа в голову списка его частоты
void AttachItem(LfuCache& cache, LfuItem* item) {
    DList* list = FreqList(cache, item->freq);
    AddHead(*list, item->key);
    item->node = list->head;
}

// Обращение к элементу: счётчик увеличивается, ключ переходит в следующий список
void TouchItem(LfuCache& cache, LfuItem* item) {
    DetachItem(cache, item);
    item->freq++;
    AttachItem(cache, item);
}

// Удаление элемента с наименьшей частотой, при равенстве — давнего (хвост списка)
void EvictItem(LfuCache& cache) {
    DList* list = FreqList(cache, cache.minFreq);
    string key = list->tail->data;
    LfuItem* item = nullptr;
    ChainGet(cache.items, key, item);
    DetachItem(cache, item);
    ChainRemove(cache.items, key);
    delete item;
}

// Ёмкость задаётся при создании кэша
void LfuInit(LfuCache& cache, int capacity) {
    cache.capacity = capacity;
    cache.minFreq = 0;
    ChainInit(cache.items, 16);
    ChainInit(cache.freqs, 16);
}

// GET: значение ключа или false, если ключа нет; обращение увеличивает частоту
bool LfuGet(LfuCache& cache, const string& key, int& value) {
    LfuItem* item = nullptr;
    if (!ChainGet(cache.items, key, item)) {
        return false;
    }
    TouchItem(cache, item);
    value = item->value;
    return true;
}

// SET: замена значения существующего ключа или добавление нового с вытеснением
void LfuSet(LfuCache& cache, const string& key, int value) {
    if (cache.capacity <= 0) {
        return;
    }

    LfuItem* item = nullptr;
    if (ChainGet(cache.items, key, item)) {
        item->value = value;
        TouchItem(cache, item);
        return;
    }

    if (cache.items.size == cache.capacity) {
        EvictItem(cache);
    }
    item = new LfuItem{key, value, 1, nullptr};
    ChainPut(cache.items, key, item);
    AttachItem(cache, item);
    // Новый ключ всегда имеет наименьшую частоту
    cache.minFreq = 1;
}

// Состояние кэша: для каждого ключа значение и частота
void PrintLfu(LfuCache& cache) {
    cout << "[";
    bool first = true;
    for (int i = 0; i < cache.items.capacity; i++) {
        ChainNode<LfuItem*>* current = cache.items.buckets[i];
        while (current != nullptr) {
            if (!first) {
                cout << ", ";
            }
            LfuItem* item = current->value;
            cout << item->key << "->" << item->value << " (freq " << item->freq << ")";
            first = false;
            current = current->next;
        }
    }
    cout << "]" << endl;
}

// Освобождение элементов, списков и обеих хеш-таблиц
void DeleteLfu(LfuCache& cache) {
    for (int i = 0; i < cache.items.capacity; i++) {
        ChainNode<LfuItem*>* current = cache.items.buckets[i];
        while (current != nullptr) {
            delete current->value;
            current = current->next;
        }
    }
    for (int i = 0; i < cache.freqs.capacity; i++) {
        ChainNode<DList*>* current = cache.freqs.buckets[i];
        while (current != nullptr) {
            DeleteAll(*current->value);
            delete current->value;
            current = current->next;
        }
    }
    ChainFree(cache.items);
    ChainFree(cache.freqs);
}
