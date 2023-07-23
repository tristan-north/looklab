#include "argsfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

int getNumParams(const char* argsBuf, const int argsBufSize) {
    const char* pBuf = argsBuf;
    int paramCount = 0;
    while (true) {
        const char* pStartOfName = strstr(pBuf, "<param name=");
        if(pStartOfName == NULL)
            break;

        paramCount++;
        pBuf = pStartOfName + sizeof("<param name=");
    }
    return paramCount;
}

int parseArgs(ArgsInfo*& argsParams) {
    char pathFromTree[] = R"(\lib\plugins\Args\LamaDiffuse.args)";
    char* rmanTree = getenv("RMANTREE");
    assert(sizeof(pathFromTree) + strlen(rmanTree) < 255 && "Args filepath too long for buffer.");
    char argsFilePath[256];
    strcpy(argsFilePath, rmanTree);

    strcat(argsFilePath, pathFromTree);

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
    for(int i=0; i<numParams; i++) {
        const char* pStartOfName = strstr(pBuf, "<param name=");
        pStartOfName += sizeof("<param name=");
        const char* pEndOfName = strchr(pStartOfName+1, '"');
        strncpy(argsParams[i].name, pStartOfName, pEndOfName-pStartOfName);

        pBuf = pEndOfName;
    }

    return numParams;
}