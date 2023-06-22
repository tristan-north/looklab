#include "mesh.h"
#include "perftimer.h"
#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <QDebug>
#include <QVector2D>

using namespace Alembic::AbcGeom;

Mesh::Mesh(const char* filepath)
{
    m_archive = new IArchive(Alembic::AbcCoreOgawa::ReadArchive(), filepath);
    std::cout << "Reading: " << m_archive->getName() << std::endl;

    IObject rootObj = m_archive->getTop();
    uint numChildren = rootObj.getNumChildren();
    std::cout << "Root has " << numChildren << " children.\n";

    std::string childName1 = rootObj.getChildHeader(0).getName();
    std::cout << "Child1: " << childName1 << "\n";
    IObject child1(rootObj, childName1);
    const MetaData& md = child1.getMetaData();
    std::cout << "Child1 metadata: " << md.serialize() << "\n";

    std::string childName2 = child1.getChildHeader(0).getName();
    std::cout << "Child2: " << childName2 << "\n";
    IObject child2(child1, childName2);
    const MetaData& md2 = child2.getMetaData();
    std::cout << "Child2 metadata: " << md2.serialize() << "\n";

    if( !(IPolyMeshSchema::matches(md2) || ISubDSchema::matches(md2)) ) {
        std::cout << "No poly or subd mesh found." << child2.getName() << "\n";
        return;
    }
    std::cout << "Found mesh " << child2.getName() << ".\n";

    IPolyMesh mesh(child1, child2.getName()); // First arg is parent object

    // Get positions
    IPolyMeshSchema& schema = mesh.getSchema();
    m_posSharedPtr = schema.getValue().getPositions();
    const QVector3D *positions = reinterpret_cast<const QVector3D*>(m_posSharedPtr->getData());
//    qDebug() << "-Positions-";
//    for(int i=0; i<m_posSharedPtr->size(); i++) {
//        qDebug() << positions[i];
//    }

    // Get position indices
    m_posIndicesSharedPtr = schema.getValue().getFaceIndices();
    const uint *posIndices = reinterpret_cast<const unsigned int *>(m_posIndicesSharedPtr->getData());
//    qDebug() << "-Pos indices-";
//    for(int i=0; i < m_posIndicesSharedPtr->size(); i++) {
//        qDebug() << posIndices[i];
//    }

    // Get UVs
    IV2fGeomParam uvsParam = schema.getUVsParam();
    if(!uvsParam.valid()) {
        std::cout << "No uvs.";
        return;
    }

    IV2fGeomParam::Sample uvsSample = uvsParam.getIndexedValue();
    m_uvsSharedPtr = uvsSample.getVals();
    m_UVs = reinterpret_cast<const QVector2D*>(m_uvsSharedPtr->getData());
//    qDebug() << "-UVs-";
//    for(int i=0; i < m_uvsSharedPtr->size(); i++) {
//        qDebug() << uvs[i];
//    }

    // Get UVs indices (will also be pos indices)
    m_indicesSharedPtr = uvsSample.getIndices();
    const unsigned int *indices = reinterpret_cast<const unsigned int *>(m_indicesSharedPtr->getData());
//    qDebug() << "-UV indices-";
//    for(int i=0; i<m_indicesSharedPtr->size(); i++) {
//        qDebug() << indices[i];
//    }


    // Since assuming that there are more unique uv vals than positions
    // m_numPositions will actually be the number of uv vals, and then
    // we make a new positions array to conform to the uv indices.
    m_numPositions = m_uvsSharedPtr->size();
    assert(m_numPositions >= m_posSharedPtr->size() &&
           "Assuming there are more UV values than Positions.");

    // Need to create new position and position indices array since there may be vertices which
    // share the same position but have different UVs and OpenGL only supports one index for all
    // vertex attributes.
    PerfTimer perfTimer;
    auto newPositions = new std::vector<V3f>(m_uvsSharedPtr->size());
    m_numIndices = m_indicesSharedPtr->size();
    for(int i=0; i<m_numIndices; ++i) {
        QVector3D P = positions[posIndices[i]];
        (*newPositions)[indices[i]] = P;
    }
    m_positions = (QVector3D*)newPositions->data();
    m_indices = indices;

    perfTimer.printElapsedMSec("Time to split positions: ");
}

Mesh::~Mesh()
{
    delete m_archive;
}
