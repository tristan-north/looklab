#include "argsfile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#pragma warning(disable : 4996) // Disable unsafe string warnings

int getNumParams(const char* argsBuf, const int argsBufSize) {
    const char* pBuf = argsBuf;
    int paramCount = 0;
    while (true) {
        const char* pStartOfName = strstr(pBuf, "<param name=");
        if (pStartOfName == NULL)
            break;

        paramCount++;
        pBuf = pStartOfName + sizeof("<param name=");
    }
    return paramCount;
}

int parseArgsFile(const char* name, ArgsInfo*& argsParams) {
    char pathFromTree[] = R"(lib\plugins\Args\)";
    char fileExt[] = ".args";
    char* rmanTree = getenv("RMANTREE");
    assert(strlen(pathFromTree) + strlen(rmanTree) + strlen(name) + strlen(fileExt) < 255 && "Args filepath too long for buffer.");
    
    char argsFilePath[256];
    strcpy(argsFilePath, rmanTree);
    strcat(argsFilePath, pathFromTree);
    strcat(argsFilePath, name);
    strcat(argsFilePath, fileExt);

#if _WIN32
    struct __stat64 Stat = {};
    _stat64(argsFilePath, &Stat);
#else
    struct stat Stat = {};
    stat(FileName, &Stat);
#endif

    size_t argsFileSize = Stat.st_size;

    char* argsFileBuf = (char*)malloc(argsFileSize);

    FILE* f = fopen(argsFilePath, "r");
    assert(f && "Couldn't open args file.");
    fread(argsFileBuf, 1, argsFileSize, f);
    fclose(f);

    int numParams = getNumParams(argsFileBuf, argsFileSize);

    argsParams = (ArgsInfo*)calloc(numParams, sizeof(ArgsInfo));

    const char* pBuf = argsFileBuf;
    for (int i = 0; i < numParams; i++) {
        // Get name
        const char* pStartOfName = strstr(pBuf, "<param name=");
        pStartOfName += sizeof("<param name=");
        const char* pEndOfName = strchr(pStartOfName + 1, '"');
        strncpy(argsParams[i].name, pStartOfName, pEndOfName - pStartOfName);

        pBuf = pEndOfName;

        // Get type
        const char* pStartOfType = strstr(pBuf, "type=");
        pStartOfType += sizeof("type=");
        const char* pEndOfType = strchr(pStartOfType + 1, '"');
        if (strncmp(pStartOfType, "float", pEndOfType - pStartOfType) == 0)
            argsParams[i].type = type_float;

        else if (strncmp(pStartOfType, "color", pEndOfType - pStartOfType) == 0)
            argsParams[i].type = type_color;

        else if (strncmp(pStartOfType, "int", pEndOfType - pStartOfType) == 0)
            argsParams[i].type = type_int;

        else if (strncmp(pStartOfType, "string", pEndOfType - pStartOfType) == 0)
            argsParams[i].type = type_string;

        else if (strncmp(pStartOfType, "normal", pEndOfType - pStartOfType) == 0)
            argsParams[i].type = type_normal;

        // Get default
        const char* pStartOfDefault = strstr(pBuf, "default=");
        pStartOfDefault += sizeof("default=");
        const char* pEndOfDefault = strchr(pStartOfDefault, '"');
        switch (argsParams[i].type) {
        case type_unknown:
            break;
        case type_float: {
            char valueChar[32];
            strncpy(valueChar, pStartOfDefault, pEndOfDefault - pStartOfDefault);
            valueChar[pEndOfDefault - pStartOfDefault] = '\0';
            argsParams[i].defaultFloat = atof(valueChar);
            break;
        }
        case type_normal:
        case type_color: {
            const char* startOfFloat = pStartOfDefault;
            argsParams[i].defaultColor.x = atof(startOfFloat);
            startOfFloat = strchr(startOfFloat, ' ') + 1;
            argsParams[i].defaultColor.y = atof(startOfFloat);
            startOfFloat = strchr(startOfFloat, ' ') + 1;
            argsParams[i].defaultColor.z = atof(startOfFloat);
            break;
        }

        case type_int: {
            if (pStartOfDefault[0] == '0')
                argsParams[i].defaultInt = false;
            else
                argsParams[i].defaultInt = true;
            break;
        }
        case type_string:
            int strLen = pEndOfDefault - pStartOfDefault;
            assert(strLen < sizeof(argsParams[i].defaultString)+1 && "Default string too long.");
            strncpy(argsParams[i].defaultString, pStartOfDefault, strLen);
            argsParams[i].defaultString[strLen] = '\0';
            break;
        }
    }

    return numParams;
}
