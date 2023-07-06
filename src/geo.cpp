#include "geo.h"
#include "ImathVec.h"
#include "perftimer.h"
#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>

using namespace Alembic::AbcGeom;
using namespace Imath;

namespace geo {

struct AbcGeoData {
    bool isLoaded;
    size_t numPositions;
    const V3f* positions;
    const V2f* UVs;
    const V3f* normals;

    size_t numIndices;
    const unsigned int* indices;

    Alembic::Abc::P3fArraySamplePtr posSharedPtr = nullptr;
    Alembic::Abc::Int32ArraySamplePtr posIndicesSharedPtr = nullptr;
    Alembic::Abc::V2fArraySamplePtr uvsSharedPtr = nullptr;
    Alembic::Abc::UInt32ArraySamplePtr indicesSharedPtr = nullptr;
};

AbcGeoData g_abcGeoData {};

void loadAlembic() {
    // Mesh mesh("../testGeo/testCube3.abc");
//         Mesh mesh("../testGeo/pika_kakashi.abc");
    //    Mesh mesh("../testGeo/plane_uvs.abc");
//    const char filepath[] = "../testGeo/buddha_light_autouvs.abc";
    const char filepath[] = "../testGeo/pika_kakashi.abc";
//    const char filepath[] = "../testGeo/testCube3.abc";

    IArchive archive(Alembic::AbcCoreOgawa::ReadArchive(), filepath);
    printf("Geo::Reading: %s\n", archive.getName().c_str());

    IObject rootObj = archive.getTop();
    size_t numChildren = rootObj.getNumChildren();
    printf("Geo::Root has %zu children.\n", numChildren);

    std::string childName1 = rootObj.getChildHeader(0).getName();
    printf("Geo::Child1: %s \n", childName1.c_str());
    IObject child1(rootObj, childName1);
    const MetaData& md = child1.getMetaData();
    printf("Geo::Child1 metadata: %s\n", md.serialize().c_str());

    std::string childName2 = child1.getChildHeader(0).getName();
    printf("Geo::Child2: %s \n", childName2.c_str());
    IObject child2(child1, childName2);
    const MetaData& md2 = child2.getMetaData();
    printf("Geo::Child2 metadata: %s\n", md2.serialize().c_str());

    if (!(IPolyMeshSchema::matches(md2) || ISubDSchema::matches(md2))) {
        printf("Geo::No poly or subd mesh found %s\n", child2.getName().c_str());
        return;
    }

    printf("Geo::Found mesh %s.\n", child2.getName().c_str());
    IPolyMesh mesh(child1, child2.getName()); // First arg is parent object

    // Get positions
    IPolyMeshSchema& schema = mesh.getSchema();
    g_abcGeoData.posSharedPtr = schema.getValue().getPositions();
    const V3f* positions = (V3f*)(g_abcGeoData.posSharedPtr->getData());

    // Get position indices
    g_abcGeoData.posIndicesSharedPtr = schema.getValue().getFaceIndices();
    const unsigned int* posIndices =
        reinterpret_cast<const unsigned int*>(g_abcGeoData.posIndicesSharedPtr->getData());
    //    qDebug() << "-Pos indices-";
    //    for(int i=0; i < m_posIndicesSharedPtr->size(); i++) {
    //        qDebug() << posIndices[i];
    //    }

    // Get UVs
    IV2fGeomParam uvsParam = schema.getUVsParam();
    if (!uvsParam.valid()) {
        printf("Geo::No uvs.\n");

        g_abcGeoData.numPositions = g_abcGeoData.posSharedPtr->size();
        g_abcGeoData.positions = positions;
        g_abcGeoData.indices = posIndices;
        g_abcGeoData.numIndices = g_abcGeoData.posIndicesSharedPtr->size();
        g_abcGeoData.isLoaded = true;
        return;
    }

    IV2fGeomParam::Sample uvsSample = uvsParam.getIndexedValue();
    g_abcGeoData.uvsSharedPtr = uvsSample.getVals();
    g_abcGeoData.UVs = reinterpret_cast<const V2f*>(g_abcGeoData.uvsSharedPtr->getData());

    // Get UVs indices (will also be pos indices)
    g_abcGeoData.indicesSharedPtr = uvsSample.getIndices();
    const unsigned int* indices =
        reinterpret_cast<const unsigned int*>(g_abcGeoData.indicesSharedPtr->getData());

    // Since assuming that there are more unique uv vals than positions
    // numPositions will actually be the number of uv vals, and then
    // we make a new positions array to conform to the uv indices.
    g_abcGeoData.numPositions = g_abcGeoData.uvsSharedPtr->size();
    assert(g_abcGeoData.numPositions >= g_abcGeoData.posSharedPtr->size() &&
           "Assuming there are more UV values than Positions.");

    // Need to create new position and position indices array since there may be vertices which
    // share the same position but have different UVs and OpenGL only supports one index for all
    // vertex attributes.
    auto newPositions = new std::vector<V3f>(g_abcGeoData.uvsSharedPtr->size());
    PerfTimer perfTimer;
    g_abcGeoData.numIndices = g_abcGeoData.indicesSharedPtr->size();
    for (int i = 0; i < g_abcGeoData.numIndices; ++i) {
        V3f P = positions[posIndices[i]];
        (*newPositions)[indices[i]] = P;
    }
    g_abcGeoData.positions = (V3f*)newPositions->data();
    g_abcGeoData.indices = indices;

    g_abcGeoData.isLoaded = true;

    perfTimer.printElapsedMSec("Geo::Time to split positions: ");
}

void calcNormals() {
    struct tri {
        unsigned int v1;
        unsigned int v2;
        unsigned int v3;
    };

    PerfTimer perfTimer;

    auto normals = (V3f*)calloc(getNumPoints(), sizeof(V3f));

    const tri* tris = reinterpret_cast<const tri*>(getIndices());

    for (size_t i = 0; i < getNumIndices()/3; ++i) {
        V3f v1 = getPositions()[tris[i].v1] - getPositions()[tris[i].v2];
        V3f v2 = getPositions()[tris[i].v3] - getPositions()[tris[i].v2];

        V3f normal = v1.cross(v2);
        normal.normalize();

        // add this normal to each vertex's normal
        normals[tris[i].v1] += normal;
        normals[tris[i].v2] += normal;
        normals[tris[i].v3] += normal;
    }

    // normalize the normals for each vertex
    for (size_t i = 0; i < getNumPoints(); ++i) {
        normals[i].normalize();
    }

    perfTimer.printElapsedMSec("Geo::Time to generate normals: ");

    g_abcGeoData.normals = normals;
}

void loadIfNotLoaded() {
    if(!g_abcGeoData.isLoaded)
        loadAlembic();
}

size_t getNumIndices() {
    loadIfNotLoaded();
    return g_abcGeoData.numIndices;
}

size_t getNumPoints() {
    loadIfNotLoaded();
    return g_abcGeoData.numPositions;
}

const unsigned int* getIndices() {
    loadIfNotLoaded();
    return g_abcGeoData.indices;
}

const Imath::V3f* getPositions() {
    loadIfNotLoaded();
    return g_abcGeoData.positions;
}

const Imath::V2f* getUVs() {
    loadIfNotLoaded();
    return g_abcGeoData.UVs;
}

const Imath::V3f* getNormals() {
    loadIfNotLoaded();
    if(g_abcGeoData.normals == nullptr)
        calcNormals();

    return g_abcGeoData.normals;
}

} // namespace geo