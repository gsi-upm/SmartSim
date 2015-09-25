#include "vhcl.h"
#include "SBPython.h"
#include <sbm/sbm_deformable_mesh.h>

#ifndef NO_PYTHON
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#endif

#ifndef NO_PYTHON

namespace SmartBody
{
	void pythonFuncsMesh()
	{

		boost::python::class_<DeformableMesh, boost::python::bases<SBObject> >("SBMesh")
			.def(boost::python::init<>())
			.def("isSkinnedMesh", &DeformableMesh::isSkinnedMesh, "Whether the current mesh is static or skinned/deformable")
			.def("saveToSmb", &DeformableMesh::saveToSmb, "Save the static mesh into a binary file with extension .smb")
			.def("saveToDmb", &DeformableMesh::saveToDmb, "Save the dynamic mesh into a binary file with extension .dmb")
			.def("scale", &DeformableMesh::scale, "Scale mesh")
			.def("translate", &DeformableMesh::translate, "Translate mesh")
			.def("rotate", &DeformableMesh::rotate, "Rotate mesh")
			.def("centralize", &DeformableMesh::centralize, "Make the center of the mesh (0,0,0)")
			.def("computeNormals", &DeformableMesh::computeNormals, "Recompute normals.")
			.def("copySkinWeights", &DeformableMesh::copySkinWeights, "Copies the skin weights from the input mesh to this mesh.")

			;
	}
}


#endif