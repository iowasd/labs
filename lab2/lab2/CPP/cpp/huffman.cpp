#include "../headers/huffman.h"

#include "../headers/dlist.h"

using namespace std;

const int BYTES = 256;

// Вставка индекса узла в список свободных узлов, упорядоченный по возрастанию веса.
// Узел ставится после всех узлов с тем же весом, поэтому при равных весах
// раньше выбираются узлы, попавшие в список раньше
void InsertFree(DList& freeList, HuffNode** pool, int index) {
    int weight = pool[index]->weight;
    DNode* current = freeList.head;
    while (current != nullptr && pool[stoi(current->data)]->weight <= weight) {
        current = current->next;
    }
    if (current == nullptr) {
        AddTail(freeList, to_string(index));
    } else {
        AddBefore(freeList, current->data, to_string(index));
    }
}

// Извлечение узла с наименьшим весом — головы списка
int TakeMin(DList& freeList) {
    string removed;
    DeleteHead(freeList, removed);
    return stoi(removed);
}

HuffNode* BuildHuffman(const string& text) {
    if (text.empty()) {
        return nullptr;
    }

    // Шаг 1: частоты символов; order — символы в порядке первого появления
    int counts[BYTES] = {0};
    string order;
    for (size_t i = 0; i < text.size(); i++) {
        int code = static_cast<unsigned char>(text[i]);
        if (counts[code] == 0) {
            order += text[i];
        }
        counts[code]++;
    }

    // Шаг 2: список свободных узлов. Узлы лежат в пуле, в списке DList
    // хранятся их индексы (список из ЛР1 хранит строки)
    int total = static_cast<int>(order.size()) * 2;
    HuffNode** pool = new HuffNode*[total];
    int used = 0;
    DList freeList;
    InitList(freeList);
    for (size_t i = 0; i < order.size(); i++) {
        int code = static_cast<unsigned char>(order[i]);
        pool[used] = new HuffNode{string(1, order[i]), counts[code], nullptr, nullptr};
        InsertFree(freeList, pool, used);
        used++;
    }

    // Шаги 3–7: два узла с наименьшим весом объединяются, пока не останется один.
    // Левой дуге соответствует бит 0, правой — 1
    while (freeList.head != freeList.tail) {
        HuffNode* left = pool[TakeMin(freeList)];
        HuffNode* right = pool[TakeMin(freeList)];
        pool[used] = new HuffNode{left->symbols + right->symbols, left->weight + right->weight, left, right};
        InsertFree(freeList, pool, used);
        used++;
    }

    HuffNode* root = pool[TakeMin(freeList)];
    DeleteAll(freeList);
    delete[] pool;
    return root;
}

// Обход дерева в глубину: путь от корня до листа и есть код символа
void CollectCodes(HuffNode* node, const string& path, Array& codes) {
    if (node->left == nullptr) {
        int code = static_cast<unsigned char>(node->symbols[0]);
        ArraySet(codes, code, path);
        return;
    }
    CollectCodes(node->left, path + "0", codes);
    CollectCodes(node->right, path + "1", codes);
}

void BuildCodes(HuffNode* root, Array& codes) {
    ArrayInit(codes, BYTES);
    for (int i = 0; i < BYTES; i++) {
        ArrayPush(codes, "");
    }
    if (root == nullptr) {
        return;
    }
    if (root->left == nullptr) {
        // Единственный символ: дерево из одного листа, код назначается равным 0
        CollectCodes(root, "0", codes);
        return;
    }
    CollectCodes(root, "", codes);
}

// Кодирование: коды символов записываются подряд
string HuffEncode(const string& text, const Array& codes) {
    string bits;
    for (size_t i = 0; i < text.size(); i++) {
        string code;
        ArrayGet(codes, static_cast<unsigned char>(text[i]), code);
        bits += code;
    }
    return bits;
}

// Декодирование: спуск от корня по битам, в листе выводится символ и спуск начинается заново
bool HuffDecode(HuffNode* root, const string& bits, string& text) {
    text.clear();
    if (root == nullptr) {
        return bits.empty();
    }
    if (root->left == nullptr) {
        for (size_t i = 0; i < bits.size(); i++) {
            if (bits[i] != '0') {
                return false;
            }
            text += root->symbols;
        }
        return true;
    }

    HuffNode* current = root;
    for (size_t i = 0; i < bits.size(); i++) {
        if (bits[i] == '0') {
            current = current->left;
        } else if (bits[i] == '1') {
            current = current->right;
        } else {
            return false;
        }
        if (current->left == nullptr) {
            text += current->symbols;
            current = root;
        }
    }
    // Код должен закончиться ровно на границе символа
    return current == root;
}

void DeleteHuffman(HuffNode*& root) {
    if (root == nullptr) {
        return;
    }
    DeleteHuffman(root->left);
    DeleteHuffman(root->right);
    delete root;
    root = nullptr;
}
