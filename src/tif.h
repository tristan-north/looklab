#pragma once

#include <vector>


void writeTif(const std::vector<unsigned char> pixels, const int res);
void createTestImage(std::vector<unsigned char>& imageData, int res);
