#pragma once

#include <string>

using namespace std;

// Узел АВЛ-дерева, height — высота поддерева с корнем в этом узле
struct TreeNode {
    int data;
    int height;
    TreeNode* left;
    TreeNode* right;
};

bool Insert(TreeNode*& root, int value);
bool Remove(TreeNode*& root, int value);
bool Contains(TreeNode* root, int value);
int TreeSize(TreeNode* root);

// Обходы: значения дописываются в строку через пробел
void InOrder(TreeNode* root, string& result);
void PreOrder(TreeNode* root, string& result);
void PostOrder(TreeNode* root, string& result);
void LevelOrder(TreeNode* root, string& result);

void PrintTree(TreeNode* root, int depth);
void DeleteAll(TreeNode*& root);
