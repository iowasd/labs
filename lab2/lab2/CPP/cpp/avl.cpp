#include "../headers/avl.h"

#include <iostream>

using namespace std;

// Высота поддерева, у пустого — 0
int Height(TreeNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

// Пересчёт высоты узла по высотам его сыновей
void UpdateHeight(TreeNode* node) {
    int leftHeight = Height(node->left);
    int rightHeight = Height(node->right);
    if (leftHeight > rightHeight) {
        node->height = leftHeight + 1;
    } else {
        node->height = rightHeight + 1;
    }
}

// Баланс-фактор: высота левого поддерева минус высота правого
int BalanceFactor(TreeNode* node) {
    return Height(node->left) - Height(node->right);
}

// Правый поворот: левый сын становится корнем поддерева
void RotateRight(TreeNode*& root) {
    TreeNode* newRoot = root->left;
    root->left = newRoot->right;
    newRoot->right = root;
    UpdateHeight(root);
    UpdateHeight(newRoot);
    root = newRoot;
}

// Левый поворот: правый сын становится корнем поддерева
void RotateLeft(TreeNode*& root) {
    TreeNode* newRoot = root->right;
    root->right = newRoot->left;
    newRoot->left = root;
    UpdateHeight(root);
    UpdateHeight(newRoot);
    root = newRoot;
}

// Восстановление баланса узла после вставки или удаления
void Balance(TreeNode*& root) {
    UpdateHeight(root);
    int balance = BalanceFactor(root);

    if (balance > 1) {
        // Перевес слева, если у левого сына перевес справа — случай LR
        if (BalanceFactor(root->left) < 0) {
            RotateLeft(root->left);
        }
        RotateRight(root);
    } else if (balance < -1) {
        // Перевес справа, если у правого сына перевес слева — случай RL
        if (BalanceFactor(root->right) > 0) {
            RotateRight(root->right);
        }
        RotateLeft(root);
    }
}

// Возвращает false, если такое значение уже есть
bool Insert(TreeNode*& root, int value) {
    if (root == nullptr) {
        root = new TreeNode{value, 1, nullptr, nullptr};
        return true;
    }

    bool inserted = false;
    if (value < root->data) {
        inserted = Insert(root->left, value);
    } else if (value > root->data) {
        inserted = Insert(root->right, value);
    }

    if (inserted) {
        Balance(root);
    }
    return inserted;
}

// Узел с минимальным значением — самый левый в поддереве
TreeNode* FindMin(TreeNode* root) {
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

// Возвращает false, если значения нет в дереве
bool Remove(TreeNode*& root, int value) {
    if (root == nullptr) {
        return false;
    }

    bool removed = false;
    if (value < root->data) {
        removed = Remove(root->left, value);
    } else if (value > root->data) {
        removed = Remove(root->right, value);
    } else if (root->left == nullptr || root->right == nullptr) {
        // Ноль или один сын: узел заменяется своим сыном
        TreeNode* tmp = root;
        if (root->left != nullptr) {
            root = root->left;
        } else {
            root = root->right;
        }
        delete tmp;
        return true;
    } else {
        // Два сына: значение заменяется минимумом правого поддерева,
        // а сам минимум удаляется из правого поддерева
        TreeNode* successor = FindMin(root->right);
        root->data = successor->data;
        Remove(root->right, successor->data);
        removed = true;
    }

    if (removed) {
        Balance(root);
    }
    return removed;
}

// Поиск значения в дереве
bool Contains(TreeNode* root, int value) {
    if (root == nullptr) {
        return false;
    }
    if (root->data == value) {
        return true;
    }
    if (value < root->data) {
        return Contains(root->left, value);
    }
    return Contains(root->right, value);
}

// Количество узлов в дереве
int TreeSize(TreeNode* root) {
    if (root == nullptr) {
        return 0;
    }
    return TreeSize(root->left) + TreeSize(root->right) + 1;
}

// Дописывает число в строку через пробел
void AppendValue(string& result, int value) {
    if (!result.empty()) {
        result += " ";
    }
    result += to_string(value);
}

// Левое поддерево, корень, правое поддерево: ключи по возрастанию
void InOrder(TreeNode* root, string& result) {
    if (root == nullptr) {
        return;
    }
    InOrder(root->left, result);
    AppendValue(result, root->data);
    InOrder(root->right, result);
}

// Корень, левое поддерево, правое поддерево
void PreOrder(TreeNode* root, string& result) {
    if (root == nullptr) {
        return;
    }
    AppendValue(result, root->data);
    PreOrder(root->left, result);
    PreOrder(root->right, result);
}

// Левое поддерево, правое поддерево, корень
void PostOrder(TreeNode* root, string& result) {
    if (root == nullptr) {
        return;
    }
    PostOrder(root->left, result);
    PostOrder(root->right, result);
    AppendValue(result, root->data);
}

// Обход в ширину (по уровням) с очередью на массиве
void LevelOrder(TreeNode* root, string& result) {
    if (root == nullptr) {
        return;
    }

    int count = TreeSize(root);
    TreeNode** queue = new TreeNode*[count];
    int head = 0;
    int tail = 0;
    queue[tail] = root;
    tail++;

    while (head < tail) {
        TreeNode* current = queue[head];
        head++;
        AppendValue(result, current->data);
        if (current->left != nullptr) {
            queue[tail] = current->left;
            tail++;
        }
        if (current->right != nullptr) {
            queue[tail] = current->right;
            tail++;
        }
    }

    delete[] queue;
}

// Дерево, повёрнутое на 90 градусов: правое поддерево сверху, левое снизу.
// В скобках высота поддерева узла
void PrintTree(TreeNode* root, int depth) {
    if (root == nullptr) {
        return;
    }
    PrintTree(root->right, depth + 1);
    cout << string(depth * 4, ' ') << root->data << " (h=" << root->height << ")" << endl;
    PrintTree(root->left, depth + 1);
}

// Удаление всех узлов дерева (сначала сыновья, потом сам узел)
void DeleteAll(TreeNode*& root) {
    if (root == nullptr) {
        return;
    }
    DeleteAll(root->left);
    DeleteAll(root->right);
    delete root;
    root = nullptr;
}
