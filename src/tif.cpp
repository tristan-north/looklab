#include "tif.h"
#include <cstdint>
#include <QDebug>
#include <fstream>

// Tif format ref https://www.fileformat.info/format/tiff/egff.htm
// More info about the tags https://www.awaresystems.be/imaging/tiff/tifftags/search.html

using namespace std;

static const uint16_t NUM_TAGS = 10;

// Data Types
#define BYTE 1
#define ASCII 2
#define SHORT 3
#define LONG 4

#define UNCOMPRESSED 1

#define BLACKISZERO 1
#define RGB 2


#pragma pack(push, 1)  // Need to ensure the structs are packed without padding
struct TifHead
{
    uint16_t Identifier;  /* Byte-order Identifier */
    uint16_t Version;     /* TIFF version number (always 2A hex) */
    uint32_t IFDOffset;   /* Offset of the first Image File Directory*/
};

struct TifTag
{
    uint16_t  TagId;        /* The tag identifier  */
    uint16_t  DataType;     /* The scalar type of the data items  */
    uint32_t  DataCount;    /* The number of items in the tag data  */
    uint32_t  DataOffset;   /* The byte offset to the data items  */
};

struct TifIFD
{
    uint16_t NumDirEntries;       /* Number of Tags in IFD  */
    TifTag  TagList[NUM_TAGS];    /* Array of Tags  */
    uint32_t NextIFDOffset;       /* Offset to next IFD, or 0 if no more images in file  */
};
#pragma pack(pop) // Restore original struct packing

struct ImageData
{
    ImageData(uint res)
    {
        pData = new uint8_t(res * res);
    }

    ~ImageData()
    {
        delete[] pData;
    }

    uint8_t *pData;
};

static void createTestImage(uint res, uint8_t* pImgData)
{
    for (int y = 0; y < res; ++y) {
        for (int x = 0; x < res; ++x) {
            pImgData[y * res + x] = 255;
        }
    }
}



void writeTif()
{
    const uint imageWidthLength = 512;
    const uint tileSize = 64; // Rman expects 64 tile size

    assert(imageWidthLength % tileSize == 0 && "Image resolution doesn't divide by tile size.");

//    ImageData imageData(imageWidthLength);
//    createTestImage(imageWidthLength, imageData.pData);

    const uint numTiles = (imageWidthLength / tileSize) * (imageWidthLength / tileSize);

    uint8_t tileData[tileSize * tileSize];
    for (int i = 0; i < tileSize * tileSize; i++) {
        tileData[i] = i / float(tileSize * tileSize) * 255;
    }

    uint16_t tileByteCount[numTiles];
    for (uint16_t &i: tileByteCount)
        i = tileSize * tileSize;

    // TODO: This will need to be unique per tile
    uint32_t tileOffsets[numTiles];
    for (int i = 0; i < numTiles; i++) {
        tileOffsets[i] = sizeof(TifHead) + sizeof(TifIFD) + sizeof(tileOffsets) + sizeof(tileByteCount); // Point to the data itself
    }


    // Tif header
    TifHead tifHead = {};
    tifHead.Identifier = 0x4949;  // Little endian, aka II
    tifHead.Version = 42;
    tifHead.IFDOffset = sizeof(tifHead);  // Ie offset right after header

    TifIFD tifIFD = {};
    tifIFD.NumDirEntries = NUM_TAGS;

    // Tags should be specified in acending order of TagId
    // Width Tag
    tifIFD.TagList[0].TagId = 256;
    tifIFD.TagList[0].DataType = SHORT;
    tifIFD.TagList[0].DataCount = 1;
    tifIFD.TagList[0].DataOffset = imageWidthLength;

    // Length Tag
    tifIFD.TagList[1].TagId = 257;
    tifIFD.TagList[1].DataType = SHORT;
    tifIFD.TagList[1].DataCount = 1;
    tifIFD.TagList[1].DataOffset = imageWidthLength;

    // Bits per sample Tag
    tifIFD.TagList[2].TagId = 258;
    tifIFD.TagList[2].DataType = SHORT;
    tifIFD.TagList[2].DataCount = 1;  // 3 if RGB
    tifIFD.TagList[2].DataOffset = 8; // If RGB this might need to point to the data instead of putting it here

    // Compression Tag
    tifIFD.TagList[3].TagId = 259;
    tifIFD.TagList[3].DataType = SHORT;
    tifIFD.TagList[3].DataCount = 1;
    tifIFD.TagList[3].DataOffset = UNCOMPRESSED;

    // Photometric Interpretation (colorspace) Tag
    tifIFD.TagList[4].TagId = 262;
    tifIFD.TagList[4].DataType = SHORT;
    tifIFD.TagList[4].DataCount = 1;
    tifIFD.TagList[4].DataOffset = BLACKISZERO; // RGB if RGB

    // Samples Per Pixel Tag
    tifIFD.TagList[5].TagId = 277;
    tifIFD.TagList[5].DataType = SHORT;
    tifIFD.TagList[5].DataCount = 1;
    tifIFD.TagList[5].DataOffset = 1;  // 3 for RGB

    // Tile Width
    tifIFD.TagList[6].TagId = 322;
    tifIFD.TagList[6].DataType = SHORT;
    tifIFD.TagList[6].DataCount = 1;
    tifIFD.TagList[6].DataOffset = tileSize;

    // Tile Length
    tifIFD.TagList[7].TagId = 323;
    tifIFD.TagList[7].DataType = SHORT;
    tifIFD.TagList[7].DataCount = 1;
    tifIFD.TagList[7].DataOffset = tileSize;

    // Tile Offsets, array of offsets to the data of each tile
    tifIFD.TagList[8].TagId = 324;
    tifIFD.TagList[8].DataType = LONG;
    tifIFD.TagList[8].DataCount = numTiles;
    // This is unusual, in the case that numTiles == 1 this is the offset to the tile image data. If
    // numTiles > 1 this is the offset to the array containing the offsets for each tile. Either way it points to the same place.
    tifIFD.TagList[8].DataOffset = sizeof(tifHead) + sizeof(tifIFD);

    // Tile Byte Counts, array of byte count for each tile
    tifIFD.TagList[9].TagId = 325;
    tifIFD.TagList[9].DataType = SHORT;
    tifIFD.TagList[9].DataCount = numTiles;
    if(numTiles > 1) { // If numTiles is 1 the DataOffset needs to contain the actual data rather pointing to it.
        tifIFD.TagList[9].DataOffset = sizeof(tifHead) + sizeof(tifIFD) + sizeof(tileOffsets);
    }
    else {
        tifIFD.TagList[9].DataOffset = tileSize * tileSize;
    }

    ofstream file(R"(C:\Users\Tristan\Desktop\mytif.tif)", ios::binary);
    if (file.fail()) {
        qCritical() << "Couldn't open file to write tif.";
        return;
    }

    file.write(reinterpret_cast<char*>(&tifHead), sizeof(tifHead));
    file.write(reinterpret_cast<char*>(&tifIFD), sizeof(tifIFD));
    if(numTiles > 1) { // If numTiles is one, this info goes into the tag directly
        file.write(reinterpret_cast<char*>(&tileOffsets), sizeof(tileOffsets));
        file.write(reinterpret_cast<char*>(&tileByteCount), sizeof(tileByteCount));
    }

    file.write(reinterpret_cast<char*>(tileData), sizeof(tileData)); // TODO: Write tiles properly

    file.close();
}
