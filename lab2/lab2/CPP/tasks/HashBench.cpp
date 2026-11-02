// Задание 6.1, вариант 2: эмпирический анализ метода цепочек и хеширования кукушки.
// Опыт 1 — вставка, поиск и удаление N случайных ключей при автоматическом росте таблиц.
// Опыт 2 — таблицы фиксированного размера: длина поиска в зависимости от заполнения
// и заполнение, при котором у кукушки впервые возникает цикл вытеснений

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include "../headers/array.h"
#include "../headers/hashtable.h"

using namespace std;

// Массив из count различных случайных ключей вида k<число>
void MakeKeys(Array& keys, int count, unsigned int seed, const string& prefix) {
    mt19937 generator(seed);
    for (int i = 0; i < count; i++) {
        // Порядковый номер в ключе гарантирует различие ключей
        ArrayPush(keys, prefix + to_string(generator()) + "_" + to_string(i));
    }
}

// Время в миллисекундах с момента start
double ElapsedMs(chrono::steady_clock::time_point start) {
    chrono::duration<double, milli> elapsed = chrono::steady_clock::now() - start;
    return elapsed.count();
}

// Строка таблицы результатов опыта 1
void PrintRow(const string& method, int count, double insertMs, double hitMs, double missMs,
              double removeMs, double probes, int extra, int rehashes) {
    cout << left << setw(9) << method << right << setw(9) << count
         << fixed << setprecision(2)
         << setw(10) << insertMs << setw(10) << hitMs << setw(10) << missMs << setw(10) << removeMs
         << setw(9) << probes << setw(10) << extra << setw(9) << rehashes << endl;
}

// Опыт 1 для метода цепочек
void BenchChain(const Array& keys, const Array& missing) {
    int count = ArrayLength(keys);
    ChainTable<int> table;
    ChainInit(table, 16);

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        ChainPut(table, keys.data[i], i);
    }
    double insertMs = ElapsedMs(start);
    int rehashes = table.stats.rehashes;
    int maxLength = ChainMaxLength(table);

    table.stats.probes = 0;
    int value = 0;
    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        ChainGet(table, keys.data[i], value);
    }
    double hitMs = ElapsedMs(start);
    double probes = static_cast<double>(table.stats.probes) / count;

    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        ChainGet(table, missing.data[i], value);
    }
    double missMs = ElapsedMs(start);

    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        ChainRemove(table, keys.data[i]);
    }
    double removeMs = ElapsedMs(start);

    PrintRow("chain", count, insertMs, hitMs, missMs, removeMs, probes, maxLength, rehashes);
    ChainFree(table);
}

// Опыт 1 для хеширования кукушки
void BenchCuckoo(const Array& keys, const Array& missing) {
    int count = ArrayLength(keys);
    CuckooTable<int> table;
    CuckooInit(table, 16);

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        CuckooPut(table, keys.data[i], i);
    }
    double insertMs = ElapsedMs(start);
    int kicks = table.stats.kicks;
    int rehashes = table.stats.rehashes;

    table.stats.probes = 0;
    int value = 0;
    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        CuckooGet(table, keys.data[i], value);
    }
    double hitMs = ElapsedMs(start);
    double probes = static_cast<double>(table.stats.probes) / count;

    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        CuckooGet(table, missing.data[i], value);
    }
    double missMs = ElapsedMs(start);

    start = chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        CuckooRemove(table, keys.data[i]);
    }
    double removeMs = ElapsedMs(start);

    PrintRow("cuckoo", count, insertMs, hitMs, missMs, removeMs, probes, kicks, rehashes);
    CuckooFree(table);
}

// Опыт 2а: средняя длина успешного поиска в цепочках при заполнении load
double ChainProbesAtLoad(int capacity, double load, unsigned int seed) {
    int count = static_cast<int>(capacity * load);
    Array keys;
    ArrayInit(keys, count);
    MakeKeys(keys, count, seed, "k");

    ChainTable<int> table;
    ChainInit(table, capacity, false);
    for (int i = 0; i < count; i++) {
        ChainPut(table, keys.data[i], i);
    }
    table.stats.probes = 0;
    int value = 0;
    for (int i = 0; i < count; i++) {
        ChainGet(table, keys.data[i], value);
    }
    double probes = static_cast<double>(table.stats.probes) / count;

    ChainFree(table);
    DeleteArray(keys);
    return probes;
}

// Опыт 2б: заполнение двух таблиц кукушки в момент первого цикла вытеснений
// и среднее число вытеснений на одну вставку до этого момента
void CuckooFailLoad(int capacity, unsigned int seed, double& load, double& kicksPerInsert) {
    int limit = capacity * 2;
    Array keys;
    ArrayInit(keys, limit);
    MakeKeys(keys, limit, seed, "k");

    CuckooTable<int> table;
    CuckooInit(table, capacity, false);
    int inserted = 0;
    for (int i = 0; i < limit; i++) {
        bool failed = false;
        CuckooPut(table, keys.data[i], i, &failed);
        if (failed) {
            break;
        }
        inserted++;
    }
    load = static_cast<double>(inserted) / (capacity * 2);
    kicksPerInsert = static_cast<double>(table.stats.kicks) / (inserted + 1);

    CuckooFree(table);
    DeleteArray(keys);
}

int main() {
    cout << "Experiment 1: N random keys, tables grow automatically" << endl;
    cout << "(chain: load <= 1; cuckoo: load of two tables <= 0.4)" << endl;
    cout << left << setw(9) << "method" << right << setw(9) << "N"
         << setw(10) << "insert,ms" << setw(10) << "hit,ms" << setw(10) << "miss,ms" << setw(10) << "del,ms"
         << setw(9) << "probes" << setw(10) << "max/kick" << setw(9) << "rehash" << endl;

    const int sizes[4] = {1000, 10000, 100000, 1000000};
    for (int s = 0; s < 4; s++) {
        Array keys;
        ArrayInit(keys, sizes[s]);
        MakeKeys(keys, sizes[s], 42 + s, "k");
        Array missing;
        ArrayInit(missing, sizes[s]);
        MakeKeys(missing, sizes[s], 1042 + s, "m");

        BenchChain(keys, missing);
        BenchCuckoo(keys, missing);

        DeleteArray(keys);
        DeleteArray(missing);
    }

    const int capacity = 10007;
    const int runs = 10;
    cout << endl << "Experiment 2a: chaining without growth, " << capacity << " buckets" << endl;
    cout << setw(8) << "load" << setw(12) << "probes" << setw(14) << "1 + load/2" << endl;
    const double loads[6] = {0.25, 0.5, 0.75, 1.0, 2.0, 4.0};
    for (int i = 0; i < 6; i++) {
        double sum = 0;
        for (int run = 0; run < runs; run++) {
            sum += ChainProbesAtLoad(capacity, loads[i], 7 + run);
        }
        cout << fixed << setprecision(2) << setw(8) << loads[i]
             << setw(12) << sum / runs << setw(14) << 1 + loads[i] / 2 << endl;
    }

    cout << endl << "Experiment 2b: cuckoo without growth, first cycle of evictions" << endl;
    cout << setw(10) << "table" << setw(12) << "fail load" << setw(14) << "kicks/insert" << endl;
    const int tables[3] = {1009, 10007, 100003};
    for (int i = 0; i < 3; i++) {
        double loadSum = 0;
        double kicksSum = 0;
        for (int run = 0; run < runs; run++) {
            double load = 0;
            double kicks = 0;
            CuckooFailLoad(tables[i], 100 + run, load, kicks);
            loadSum += load;
            kicksSum += kicks;
        }
        cout << setw(10) << tables[i] << fixed << setprecision(3)
             << setw(12) << loadSum / runs << setw(14) << kicksSum / runs << endl;
    }
    return 0;
}
