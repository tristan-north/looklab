#pragma once

#include <ImathVec.h>

namespace geo {

/*
OpenGL needs:
 - numIndices
 - numPositions
 - positions
 - indices
 - UVs

*/

size_t getNumIndices();
size_t getNumPoints();
const unsigned int* getIndices();
const Imath::V3f* getPositions();
const Imath::V2f* getUVs();
const Imath::V3f* getNormals();


} // namespace geo