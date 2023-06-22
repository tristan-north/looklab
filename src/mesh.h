#pragma once

#include <Alembic/AbcGeom/IPolyMesh.h>
#include <QVector3D>

class Mesh
{
public:
    Mesh(const char* filepath);
    ~Mesh();

    int m_numPositions = {};
    const QVector3D* m_positions;
    const QVector2D* m_UVs;

    int m_numIndices = {};
    const uint* m_indices = nullptr;

private:
    Alembic::Abc::v12::IArchive* m_archive = nullptr;
    Alembic::Abc::P3fArraySamplePtr m_posSharedPtr;
    Alembic::Abc::Int32ArraySamplePtr m_posIndicesSharedPtr;
    Alembic::Abc::V2fArraySamplePtr m_uvsSharedPtr;
    Alembic::Abc::UInt32ArraySamplePtr m_indicesSharedPtr;
};