#pragma once

#include <ImathVec.h>

enum arg_type {
    type_unknown,
    type_float,
    type_color,
    type_int,
    type_string,
    type_normal,
};

struct ArgsInfo{
    char name[64];
    char label[64];
    arg_type type;

    union {
        float defaultFloat;
        Imath::V3f defaultColor;
        int defaultInt;
        char defaultString[64];
        Imath::V3f defualtNormal;
    };
};

int parseArgsFile(const char* name, ArgsInfo*& argsParams);
