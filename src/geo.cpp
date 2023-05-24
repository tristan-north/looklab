#include "geo.h"
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

using namespace Alembic::AbcGeom;

void Example1_MeshIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "/home/tristan/Downloads/testCube2.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    IGeomBaseObject geomBase( IObject( archive, kTop ), "meshy" );

    IPolyMesh meshyObj( IObject( archive, kTop ), "meshy" );
    IPolyMeshSchema &mesh = meshyObj.getSchema();
    IN3fGeomParam N = mesh.getNormalsParam();
    IV2fGeomParam uv = mesh.getUVsParam();

    IPolyMeshSchema::Sample mesh_samp;
    mesh.get( mesh_samp );
    IGeomBase::Sample baseSamp;
    geomBase.getSchema().get( baseSamp );

    ICompoundProperty arbattrs = mesh.getArbGeomParams();

    // getExpandedValue() takes an optional ISampleSelector;
    // getVals() returns a TypedArraySamplePtr
    N3fArraySamplePtr nsp = N.getExpandedValue().getVals();

    N3f n0 = (*nsp)[0];

    for ( size_t i = 0 ; i < nsp->size() ; ++i )
    {
        std::cout << i << "th normal: " << (*nsp)[i] << std::endl;
    }

    std::cout << "0th normal: " << n0 << std::endl;

    IV2fGeomParam::Sample uvsamp;
    uvsamp = uv.getIndexedValue();

    Imath::V2f uv2 = (*(uvsamp.getVals()))[2];
    std::cout << "2th UV: " << uv2 << std::endl;

    std::cout << "Mesh num vertices: "
              << mesh_samp.getPositions()->size() << std::endl;

    std::cout << "0th vertex from the mesh sample: "
              << (*(mesh_samp.getPositions()))[0] << std::endl;

    std::cout << "0th vertex from the mesh sample with get method: "
              << mesh_samp.getPositions()->get()[0] << std::endl;
}
