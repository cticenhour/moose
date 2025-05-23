//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MortarFrictionalPressureVectorAux.h"
#include "SystemBase.h"

registerMooseObject("ContactApp", MortarFrictionalPressureVectorAux);

InputParameters
MortarFrictionalPressureVectorAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;

  params.addClassDescription("This class creates an auxiliary vector for outputting the mortar "
                             "frictional pressure vector.");
  params.addRequiredCoupledVar(
      "tangent_one",
      "First tangent vector Lagrange's multiplier for computing the mortar "
      "frictional pressure vector.");
  params.addRequiredCoupledVar(
      "tangent_two",
      "Second tangent vector Lagrange's multiplier for computing the mortar "
      "frictional pressure vector.");
  params.addRequiredParam<unsigned int>("component",
                                        "Cartesian component of frictional tangent vector");

  params.addRequiredParam<BoundaryName>("primary_boundary",
                                        "The name of the primary boundary sideset.");
  params.addRequiredParam<BoundaryName>("secondary_boundary",
                                        "The name of the secondary boundary sideset.");
  params.addParam<bool>(
      "use_displaced_mesh", true, "Whether to use the displaced mesh to get the mortar interface.");
  return params;
}

MortarFrictionalPressureVectorAux::MortarFrictionalPressureVectorAux(const InputParameters & params)
  : AuxKernel(params),
    _tangent_one(&coupledValueLower("tangent_one")),
    _tangent_two(&coupledValueLower("tangent_two")),
    _fe_problem(*params.get<FEProblemBase *>("_fe_problem_base")),
    _primary_id(_fe_problem.mesh().getBoundaryID(getParam<BoundaryName>("primary_boundary"))),
    _secondary_id(_fe_problem.mesh().getBoundaryID(getParam<BoundaryName>("secondary_boundary"))),
    _component(getParam<unsigned int>("component")),
    _use_displaced_mesh(getParam<bool>("use_displaced_mesh"))
{
  // Only consider nodal quantities
  if (!isNodal())
    mooseError(
        "MortarFrictionalPressureVector auxiliary kernel can only be used with nodal kernels.");

  if (!_use_displaced_mesh)
    paramError("use_displaced_mesh",
               "This auxiliary kernel requires the use of displaced meshes to compute the "
               "frictional pressure vector.");

  // Kernel need to be boundary restricted
  if (!this->_bnd)
    paramError("boundary",
               "MortarFrictionalPressureVector auxiliary kernel must be restricted to a boundary.");

  // Get mortar interfaces
  const auto & displaced_mortar_interfaces =
      _fe_problem.getMortarInterfaces(/*displaced=*/_use_displaced_mesh);

  if (displaced_mortar_interfaces.size() == 0)
    paramError("tangent_one",
               "No mortar interface could be identified in this problem. Make sure mortar contact "
               "is enabled.");

  const auto mortar_dimension = _fe_problem.mesh().dimension() - 1;
  if (mortar_dimension == 1)
    paramError("tangent_two",
               "MortarFrictionalPressureVector auxiliary kernel can only be used in "
               "three-dimensional problems");

  // Get automatic generation object for the boundary pair this auxiliary acts on.
  if (displaced_mortar_interfaces.count(std::make_pair(_primary_id, _secondary_id)) != 1)
    mooseError("primary_boundary",
               "The boundary pairs do not correspond to a single mortar contact boundary pair. "
               "Please revise your input file for proper mortar contact constraints and mortar "
               "frictional pressure vector auxiliary variable definition.");

  _mortar_generation_object =
      &libmesh_map_find(displaced_mortar_interfaces, std::make_pair(_primary_id, _secondary_id));
}

Real
MortarFrictionalPressureVectorAux::computeValue()
{

  // A node id may correspond to more than one lower-d elements on the secondary surface.
  // However, we are looping over nodes below, so we will locate the correct geometry
  const Elem * lower_dimensional_element =
      libmesh_map_find(_mortar_generation_object->nodesToSecondaryElem(), _current_node->id())[0];

  // Get nodal tangents for this element
  const auto & nodal_tangents =
      _mortar_generation_object->getNodalTangents(*lower_dimensional_element);

  Real tangent_one_component = 0;
  Real tangent_two_component = 0;

  for (const auto lowerd_node : make_range(lower_dimensional_element->n_nodes()))
  {
    if (_current_node->id() == lower_dimensional_element->node_id(lowerd_node))
    {
      tangent_one_component = nodal_tangents[0][lowerd_node](_component);
      tangent_two_component = nodal_tangents[1][lowerd_node](_component);
      break;
    }
  }

  // Compute nodal tangent vector component
  const Real tangent_vector_component =
      tangent_one_component * (*_tangent_one)[_qp] + tangent_two_component * (*_tangent_two)[_qp];

  return tangent_vector_component;
}
