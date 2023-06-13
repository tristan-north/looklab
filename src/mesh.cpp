#include "mesh.h"
#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <string>
#include <QElapsedTimer>
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
    P3fArraySamplePtr positionsSPtr = schema.getValue().getPositions();
    const QVector3D *positions = reinterpret_cast<const QVector3D*>(positionsSPtr->getData());
    qDebug() << "-Positions-";
    for(int i=0; i<positionsSPtr->size(); i++) {
        qDebug() << positions[i];
    }

    // Get position indices
    Int32ArraySamplePtr posIndicesSPtr = schema.getValue().getFaceIndices();
    const uint *posIndices = reinterpret_cast<const unsigned int *>(posIndicesSPtr->getData());
    qDebug() << "-Pos indices-";
    for(int i=0; i<posIndicesSPtr->size(); i++) {
        qDebug() << posIndices[i];
    }

    // Get UVs
    IV2fGeomParam uvsParam = schema.getUVsParam();
    if(!uvsParam.valid()) {
        std::cout << "No uvs.";
        return;
    }

    IV2fGeomParam::Sample uvsSample = uvsParam.getIndexedValue();
    V2fArraySamplePtr uvSPtr = uvsSample.getVals();
    const QVector2D *uvs = reinterpret_cast<const QVector2D*>(uvSPtr->getData());
    qDebug() << "-UVs-";
    for(int i=0; i<uvSPtr->size(); i++) {
        qDebug() << uvs[i];
    }

    // Get UVs indices
    UInt32ArraySamplePtr indicesSPtr = uvsSample.getIndices();
    const unsigned int *indices = reinterpret_cast<const unsigned int *>(indicesSPtr->getData());
    qDebug() << "-UV indices-";
    for(int i=0; i<indicesSPtr->size(); i++) {
        qDebug() << indices[i];
    }


/*
    if (IPolyMeshSchema::matches(md2) || ISubDSchema::matches(md2)) {
        std::cout << "Found a mesh " << child2.getName() << "\n";

        IPolyMesh mesh(child1, child2.getName()); // First arg is parent object
        IPolyMeshSchema schema = mesh.getSchema();

        m_meshSampler = new IPolyMeshSchema::Sample;
        schema.get(*m_meshSampler);

        // Get positions
        const V3f *positions = m_meshSampler->getPositions()->get();
        m_positions = reinterpret_cast<const QVector3D*>(positions);

        // Get position indices
        m_numIndices = m_meshSampler->getFaceIndices()->size();
        std::cout << "Num triangles in mesh: " << m_numIndices / 3 << "\n";
        const int *indices = m_meshSampler->getFaceIndices()->get();
        const uint *posIndices = reinterpret_cast<const uint*>(indices);

        // Get UVs
        auto uvsParam = schema.getUVsParam();
        if (!uvsParam.valid()) {
            std::cerr << "Mesh has no UVs." << "\n";
            return;
        }
//        auto indexedUVs = new auto(uvsParam.getIndexedValue());
        Alembic::AbcGeom::v12::ITypedGeomParam<V2fTPTraits>::Sample *m_uvSampler = new auto(uvsParam.getIndexedValue());
        std::cout << "Num UVs in mesh: " << m_uvSampler->getVals()->size() << "\n";

        // Since assuming that there are more unique uv vals than positions
        // m_numPositions will actually be the number of uv vals, and then
        // we make a new positions array to conform to the uv indices.
        m_numPositions = m_uvSampler->getVals()->size();
        assert(m_numPositions >= m_meshSampler->getPositions()->size() &&
        "Assuming there are more UV values than Positions.");

        const unsigned int *uvIndices = m_uvSampler->getIndices()->get();
        size_t uvIndicesSize = m_uvSampler->getIndices()->size();

        QElapsedTimer timer;
        timer.start();
        // Need to create new position and position indices array since there may be vertices which
        // share the same position but have different UVs and OpenGL only supports one index for all
        // vertex attributes.
        auto newPositions = new std::vector<V3f>(m_uvSampler->getVals()->size());
        for(int i=0; i<uvIndicesSize; ++i) {
            QVector3D P = m_positions[posIndices[i]];
            (*newPositions)[uvIndices[i]] = P;
        }
        m_positions = (QVector3D*)newPositions->data();
        m_indices = uvIndices;

        QString elapsed = QString::number(timer.nsecsElapsed() / 1000000., 'f', 1);
        qInfo().noquote() << "Create new position array time: " <<  elapsed << " ms";

    }
    */
}

Mesh::~Mesh()
{
    delete m_meshSampler;
    delete m_archive;
    delete m_positions;
//    delete m_uvSampler; // This causes crash, double free?
}
