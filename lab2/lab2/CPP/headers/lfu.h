#pragma once

#include <string>

#include "dlist.h"
#include "hashtable.h"

using namespace std;

// Элемент кэша: значение, счётчик обращений и узел в списке своей частоты
struct LfuItem {
    string key;
    int value;
    int freq;
    DNode* node;
};

// LFU-кэш: хеш-таблица «ключ → элемент» и хеш-таблица «частота → двусвязный список ключей».
// В каждом списке ключи упорядочены от недавно использованных (голова) к давним (хвост)
struct LfuCache {
    int capacity;
    int minFreq;
    ChainTable<LfuItem*> items;
    ChainTable<DList*> freqs;
};

void LfuInit(LfuCache& cache, int capacity);
bool LfuGet(LfuCache& cache, const string& key, int& value);
void LfuSet(LfuCache& cache, const string& key, int value);
void PrintLfu(LfuCache& cache);
void DeleteLfu(LfuCache& cache);
