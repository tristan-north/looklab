#pragma once

#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcGeom/IPoints.h>
#include <Alembic/AbcGeom/IPolyMesh.h>
#include <QVector3D>
#include <QList>

namespace Alembic {
    namespace Abc {
        namespace v12 {
            class IArchive;
        }
    }
    namespace AbcGeom {
        namespace v12 {
            class IPolyMeshSchema;
        }
    }
}

class Mesh
{
public:
    Mesh(const char* filepath);
    ~Mesh();

    uint m_numPositions = {};
    const QVector3D* m_positions;

    const QVector2D* m_UVs;

    uint m_numIndices = {};
    const uint* m_indices = nullptr;

private:
    Alembic::Abc::v12::IArchive* m_archive = 0;
    Alembic::AbcGeom::v12::IPolyMeshSchema::Sample* m_meshSampler;
    Alembic::AbcGeom::v12::ITypedGeomParam<Alembic::Abc::V2fTPTraits>::Sample* m_uvSampler;
};