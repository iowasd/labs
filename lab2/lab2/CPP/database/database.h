#pragma once

#include <string>

using namespace std;

// Загружает файл, выполняет запрос и сохраняет файл, если данные изменились
void ExecuteQuery(const string& fileName, const string& query);
