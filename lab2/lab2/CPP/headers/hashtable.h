#pragma once

#include <string>

using namespace std;

// Две хеш-таблицы «ключ-строка → значение типа V»:
// метод цепочек (ChainTable) и хеширование кукушки (CuckooTable).
// Шаблоны описаны в заголовке, так как тип значения задаёт программа-пользователь

// ---------------- Хеш-функции ----------------

// Числовой код строки (полиномиальная свёртка символов)
inline unsigned long long KeyCode(const string& key) {
    unsigned long long code = 0;
    for (size_t i = 0; i < key.size(); i++) {
        code = code * 131 + static_cast<unsigned char>(key[i]);
    }
    return code;
}

// Метод деления: остаток от деления кода на размер таблицы
inline int HashDivision(const string& key, int capacity) {
    return static_cast<int>(KeyCode(key) % static_cast<unsigned long long>(capacity));
}

// Метод умножения: дробная часть произведения кода на A = (sqrt(5) - 1) / 2,
// умноженная на размер таблицы
inline int HashMultiplication(const string& key, int capacity) {
    const long double a = 0.6180339887498948482L;
    long double product = static_cast<long double>(KeyCode(key) % 1000000007ULL) * a;
    long double fraction = product - static_cast<long double>(static_cast<unsigned long long>(product));
    int index = static_cast<int>(fraction * capacity);
    if (index >= capacity) {
        index = capacity - 1;
    }
    return index;
}

// Счётчики для эмпирического анализа
struct HashStats {
    int probes;       // число просмотренных ячеек или узлов цепочек
    int kicks;        // число вытеснений (только кукушка)
    int rehashes;     // число перестроений таблицы
};

inline void ResetStats(HashStats& stats) {
    stats.probes = 0;
    stats.kicks = 0;
    stats.rehashes = 0;
}

// ---------------- Метод цепочек ----------------

// Узел цепочки — узел односвязного списка из ЛР1 с дополнительным полем значения
template <typename V>
struct ChainNode {
    string key;
    V value;
    ChainNode<V>* next;
};

template <typename V>
struct ChainTable {
    ChainNode<V>** buckets;
    int capacity;
    int size;
    bool autoGrow;     // перестраивать ли таблицу при заполнении
    HashStats stats;
};

template <typename V>
void ChainInit(ChainTable<V>& table, int capacity, bool autoGrow = true) {
    if (capacity < 1) {
        capacity = 1;
    }
    table.buckets = new ChainNode<V>*[capacity];
    for (int i = 0; i < capacity; i++) {
        table.buckets[i] = nullptr;
    }
    table.capacity = capacity;
    table.size = 0;
    table.autoGrow = autoGrow;
    ResetStats(table.stats);
}

// Перенос узлов в таблицу вдвое большего размера без копирования значений
template <typename V>
void ChainGrow(ChainTable<V>& table) {
    int newCapacity = table.capacity * 2;
    ChainNode<V>** newBuckets = new ChainNode<V>*[newCapacity];
    for (int i = 0; i < newCapacity; i++) {
        newBuckets[i] = nullptr;
    }
    for (int i = 0; i < table.capacity; i++) {
        ChainNode<V>* current = table.buckets[i];
        while (current != nullptr) {
            ChainNode<V>* next = current->next;
            int index = HashDivision(current->key, newCapacity);
            current->next = newBuckets[index];
            newBuckets[index] = current;
            current = next;
        }
    }
    delete[] table.buckets;
    table.buckets = newBuckets;
    table.capacity = newCapacity;
    table.stats.rehashes++;
}

// Поиск узла по ключу в его цепочке
template <typename V>
ChainNode<V>* ChainFind(ChainTable<V>& table, const string& key) {
    ChainNode<V>* current = table.buckets[HashDivision(key, table.capacity)];
    while (current != nullptr) {
        table.stats.probes++;
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Добавление или замена значения, true если ключ новый
template <typename V>
bool ChainPut(ChainTable<V>& table, const string& key, const V& value) {
    ChainNode<V>* found = ChainFind(table, key);
    if (found != nullptr) {
        found->value = value;
        return false;
    }
    // Средняя длина цепочки не превышает 1
    if (table.autoGrow && table.size + 1 > table.capacity) {
        ChainGrow(table);
    }
    int index = HashDivision(key, table.capacity);
    table.buckets[index] = new ChainNode<V>{key, value, table.buckets[index]};
    table.size++;
    return true;
}

// Получение значения, false если ключа нет
template <typename V>
bool ChainGet(ChainTable<V>& table, const string& key, V& value) {
    ChainNode<V>* found = ChainFind(table, key);
    if (found == nullptr) {
        return false;
    }
    value = found->value;
    return true;
}

// Удаление ключа, false если его нет
template <typename V>
bool ChainRemove(ChainTable<V>& table, const string& key) {
    int index = HashDivision(key, table.capacity);
    ChainNode<V>* current = table.buckets[index];
    ChainNode<V>* prev = nullptr;
    while (current != nullptr) {
        table.stats.probes++;
        if (current->key == key) {
            if (prev == nullptr) {
                table.buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            table.size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

// Длина самой длинной цепочки
template <typename V>
int ChainMaxLength(const ChainTable<V>& table) {
    int best = 0;
    for (int i = 0; i < table.capacity; i++) {
        int length = 0;
        ChainNode<V>* current = table.buckets[i];
        while (current != nullptr) {
            length++;
            current = current->next;
        }
        if (length > best) {
            best = length;
        }
    }
    return best;
}

template <typename V>
void ChainFree(ChainTable<V>& table) {
    for (int i = 0; i < table.capacity; i++) {
        ChainNode<V>* current = table.buckets[i];
        while (current != nullptr) {
            ChainNode<V>* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] table.buckets;
    table.buckets = nullptr;
    table.capacity = 0;
    table.size = 0;
}

// ---------------- Хеширование кукушки ----------------

// Ячейка таблицы: used = false означает пустую ячейку
template <typename V>
struct CuckooSlot {
    bool used;
    string key;
    V value;
};

// Две таблицы одинакового размера: первая адресуется методом деления,
// вторая — методом умножения. Ключ может лежать только в одной из двух ячеек
template <typename V>
struct CuckooTable {
    CuckooSlot<V>* first;
    CuckooSlot<V>* second;
    int capacity;  // размер каждой из двух таблиц
    int size;
    bool autoGrow;
    HashStats stats;
};

template <typename V>
void CuckooInit(CuckooTable<V>& table, int capacity, bool autoGrow = true) {
    if (capacity < 2) {
        capacity = 2;
    }
    table.first = new CuckooSlot<V>[capacity];
    table.second = new CuckooSlot<V>[capacity];
    for (int i = 0; i < capacity; i++) {
        table.first[i].used = false;
        table.second[i].used = false;
    }
    table.capacity = capacity;
    table.size = 0;
    table.autoGrow = autoGrow;
    ResetStats(table.stats);
}

// Номер ячейки ключа в первой (0) или второй (1) таблице
inline int CuckooIndex(const string& key, int which, int capacity) {
    if (which == 0) {
        return HashDivision(key, capacity);
    }
    return HashMultiplication(key, capacity);
}

// Ячейка с ключом или nullptr; просматриваются ровно две ячейки
template <typename V>
CuckooSlot<V>* CuckooFind(CuckooTable<V>& table, const string& key) {
    CuckooSlot<V>* slot = &table.first[CuckooIndex(key, 0, table.capacity)];
    table.stats.probes++;
    if (slot->used && slot->key == key) {
        return slot;
    }
    slot = &table.second[CuckooIndex(key, 1, table.capacity)];
    table.stats.probes++;
    if (slot->used && slot->key == key) {
        return slot;
    }
    return nullptr;
}

// Предел числа вытеснений: при превышении считается, что образовался цикл
inline int CuckooMaxKicks(int capacity) {
    int limit = 8;
    while (capacity > 1) {
        capacity /= 2;
        limit += 2;
    }
    return limit;
}

// Размещение нового ключа с вытеснениями.
// Если цикл не разорван, ключ, оставшийся без места, возвращается через key/value
template <typename V>
bool CuckooPlace(CuckooTable<V>& table, string& key, V& value) {
    int which = 0;
    int limit = CuckooMaxKicks(table.capacity);
    for (int step = 0; step < limit; step++) {
        CuckooSlot<V>* slots = table.first;
        if (which == 1) {
            slots = table.second;
        }
        CuckooSlot<V>& slot = slots[CuckooIndex(key, which, table.capacity)];
        table.stats.probes++;
        if (!slot.used) {
            slot.used = true;
            slot.key = key;
            slot.value = value;
            return true;
        }
        // Ячейка занята: новый ключ занимает её, старый переходит в другую таблицу
        string oldKey = slot.key;
        V oldValue = slot.value;
        slot.key = key;
        slot.value = value;
        key = oldKey;
        value = oldValue;
        table.stats.kicks++;
        which = 1 - which;
    }
    return false;
}

template <typename V>
void CuckooFree(CuckooTable<V>& table) {
    delete[] table.first;
    delete[] table.second;
    table.first = nullptr;
    table.second = nullptr;
    table.capacity = 0;
    table.size = 0;
}

// Перестроение в таблицы вдвое большего размера с повторной вставкой всех ключей
template <typename V>
void CuckooGrow(CuckooTable<V>& table) {
    CuckooTable<V> bigger;
    CuckooInit(bigger, table.capacity * 2, table.autoGrow);
    bigger.stats = table.stats;
    bigger.stats.rehashes++;
    for (int part = 0; part < 2; part++) {
        CuckooSlot<V>* slots = table.first;
        if (part == 1) {
            slots = table.second;
        }
        for (int i = 0; i < table.capacity; i++) {
            if (!slots[i].used) {
                continue;
            }
            string key = slots[i].key;
            V value = slots[i].value;
            while (!CuckooPlace(bigger, key, value)) {
                // Редкий случай: цикл при переносе, таблица растёт ещё раз
                CuckooGrow(bigger);
            }
            bigger.size++;
        }
    }
    CuckooFree(table);
    table = bigger;
}

// Добавление или замена значения, true если ключ новый.
// Через failed сообщается, что при вставке возник цикл вытеснений
template <typename V>
bool CuckooPut(CuckooTable<V>& table, const string& key, const V& value, bool* failed = nullptr) {
    if (failed != nullptr) {
        *failed = false;
    }
    CuckooSlot<V>* found = CuckooFind(table, key);
    if (found != nullptr) {
        found->value = value;
        return false;
    }
    // Заполнение двух таблиц вместе держится не выше 40 %: при 50 % и выше
    // резко растёт число вытеснений и вероятность цикла
    if (table.autoGrow && (table.size + 1) * 5 > table.capacity * 2 * 2) {
        CuckooGrow(table);
    }

    string curKey = key;
    V curValue = value;
    while (!CuckooPlace(table, curKey, curValue)) {
        // Цикл вытеснений: таблица перестраивается, и выпавший ключ
        // вставляется повторно, поэтому ни один ключ не теряется
        if (failed != nullptr) {
            *failed = true;
        }
        CuckooGrow(table);
    }
    table.size++;
    return true;
}

template <typename V>
bool CuckooGet(CuckooTable<V>& table, const string& key, V& value) {
    CuckooSlot<V>* found = CuckooFind(table, key);
    if (found == nullptr) {
        return false;
    }
    value = found->value;
    return true;
}

template <typename V>
bool CuckooRemove(CuckooTable<V>& table, const string& key) {
    CuckooSlot<V>* found = CuckooFind(table, key);
    if (found == nullptr) {
        return false;
    }
    found->used = false;
    table.size--;
    return true;
}
