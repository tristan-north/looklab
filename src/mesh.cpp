#include "mesh.h"
// #include <Alembic/AbcCoreAbstract/MetaData.h>
#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <string>

using namespace Alembic::AbcGeom;

Mesh::Mesh(const char* filepath)
{
    // std::string path = "../testGeo/testCube3.abc";
    m_archive = new IArchive(Alembic::AbcCoreOgawa::ReadArchive(), filepath);
    // IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), filepath );
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

    if (IPolyMeshSchema::matches(md2) || ISubDSchema::matches(md2)) {
        std::cout << "Found a mesh " << child2.getName() << "\n";

        IPolyMesh mesh(child1, child2.getName()); // First arg is parent object
        IPolyMeshSchema schema = mesh.getSchema();

        m_meshSampler = new IPolyMeshSchema::Sample;
        schema.get(*m_meshSampler);

        // Get positions
        m_numPositions = m_meshSampler->getPositions()->size();
        std::cout << "Num positions in mesh: " << m_numPositions << "\n";

        const V3f* positions = m_meshSampler->getPositions()->get();
        m_positions = reinterpret_cast<const QVector3D*>(positions);
        // for(size_t i=0; i<size; i++)
        //     std::cout << positions[i] << "\n";

        // Get indices
        m_numIndices = m_meshSampler->getFaceIndices()->size();
        std::cout << "Num triangles in mesh: " << m_numIndices / 3 << "\n";
        const int* indices = m_meshSampler->getFaceIndices()->get();
        m_indices = reinterpret_cast<const uint*>(indices);
        // for(size_t i=0; i<size; i++)
        //     std::cout << indices[i] << "\n";
    }
}

Mesh::~Mesh()
{
    delete m_meshSampler;
    delete m_archive;
}
