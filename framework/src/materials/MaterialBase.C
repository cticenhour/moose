//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "MaterialBase.h"
#include "MaterialPropertyStorage.h"
#include "FEProblemBase.h"
#include "Assembly.h"
#include "Executioner.h"
#include "Transient.h"

#include "libmesh/quadrature.h"

InputParameters
MaterialBase::validParams()
{

  InputParameters params = MooseObject::validParams();
  params += BlockRestrictable::validParams();
  params += BoundaryRestrictable::validParams();
  params += TransientInterface::validParams();
  params += RandomInterface::validParams();
  params += ADFunctorInterface::validParams();

  params.addParam<bool>("use_displaced_mesh",
                        false,
                        "Whether or not this object should use the "
                        "displaced mesh for computation.  Note that "
                        "in the case this is true but no "
                        "displacements are provided in the Mesh block "
                        "the undisplaced mesh will still be used.");
  params.addParam<bool>("compute",
                        true,
                        "When false, MOOSE will not call compute methods on this material. "
                        "The user must call computeProperties() after retrieving the MaterialBase "
                        "via MaterialBasePropertyInterface::getMaterialBase(). "
                        "Non-computed MaterialBases are not sorted for dependencies.");

  params.addPrivateParam<bool>("_neighbor", false);
  params.addPrivateParam<bool>("_interface", false);

  // Forces the calling of initStatefulProperties() even when this material
  // does not declare any properties that are stateful. Right now,
  // this is only used for porous_flow... pretty please keep it that way?
  params.addPrivateParam<bool>("_force_stateful_init", false);

  // Outputs
  params += OutputInterface::validParams();
  params.set<std::vector<OutputName>>("outputs") = {"none"};
  params.addParam<std::vector<std::string>>(
      "output_properties",
      {},
      "List of material properties, from this material, to output (outputs "
      "must also be defined to an output type)");
  params.addParam<MaterialPropertyName>(
      "declare_suffix",
      "",
      "An optional suffix parameter that can be appended to any declared properties. The suffix "
      "will be prepended with a '_' character.");

  // Allow Material objects to be enabled/disabled by Control objects
  params.declareControllable("enable");

  params.addParamNamesToGroup("outputs output_properties", "Outputs");
  params.addParamNamesToGroup("use_displaced_mesh", "Advanced");
  params.registerBase("MaterialBase");
  return params;
}

MaterialBase::MaterialBase(const InputParameters & parameters)
  : MooseObject(parameters),
    BlockRestrictable(this),
    BoundaryRestrictable(this, blockIDs(), false), // false for being _not_ nodal
    SetupInterface(this),
    MooseVariableDependencyInterface(this),
    ScalarCoupleable(this),
    FunctionInterface(this),
    DistributionInterface(this),
    UserObjectInterface(this),
    TransientInterface(this),
    PostprocessorInterface(this),
    VectorPostprocessorInterface(this),
    DependencyResolverInterface(),
    Restartable(this, "MaterialBases"),
    MeshChangedInterface(parameters),
    // The false flag disables the automatic call buildOutputVariableHideList;
    // for MaterialBase objects the hide lists are handled by MaterialBaseOutputAction
    OutputInterface(parameters, false),
    RandomInterface(parameters,
                    *parameters.getCheckedPointerParam<FEProblemBase *>("_fe_problem_base"),
                    parameters.get<THREAD_ID>("_tid"),
                    false),
    ElementIDInterface(this),
    GeometricSearchInterface(this),
    ADFunctorInterface(this),
    SolutionInvalidInterface(this),
    _subproblem(*getCheckedPointerParam<SubProblem *>("_subproblem")),
    _fe_problem(*getCheckedPointerParam<FEProblemBase *>("_fe_problem_base")),
    _tid(parameters.get<THREAD_ID>("_tid")),
    _assembly(_subproblem.assembly(_tid, 0)),
    _qp(std::numeric_limits<unsigned int>::max()),
    _coord(_assembly.coordTransformation()),
    _normals(_assembly.normals()),
    _mesh(_subproblem.mesh()),
    _coord_sys(_assembly.coordSystem()),
    _compute(getParam<bool>("compute")),
    _has_stateful_property(false),
    _declare_suffix(getParam<MaterialPropertyName>("declare_suffix")),
    _force_stateful_init(getParam<bool>("_force_stateful_init"))
{
}

void
MaterialBase::initStatefulProperties(unsigned int n_points)
{
  for (_qp = 0; _qp < n_points; ++_qp)
    initQpStatefulProperties();

  // checking for statefulness of properties via this loop is necessary
  // because owned props might have been promoted to stateful by calls to
  // getMaterialProperty[Old/Older] from other objects.  In these cases, this
  // object won't otherwise know that it owns stateful properties.
  for (const auto id : _supplied_prop_ids)
    if (materialData().getMaterialPropertyStorage().getPropRecord(id).stateful() &&
        !_overrides_init_stateful_props)
      mooseWarning(std::string("Material \"") + name() +
                   "\" provides one or more stateful "
                   "properties but initQpStatefulProperties() "
                   "was not overridden in the derived class.");
}

void
MaterialBase::initQpStatefulProperties()
{
  _overrides_init_stateful_props = false;
}

void
MaterialBase::checkStatefulSanity() const
{
  for (const auto & [id, min_state] : _props_to_min_states)
    if (min_state > 0)
      mooseError("The stateful property '",
                 _fe_problem.getMaterialPropertyRegistry().getName(id),
                 "' is undefined");
}

void
MaterialBase::registerPropName(const std::string & prop_name, bool is_get, const unsigned int state)
{
  if (!is_get)
  {
    const auto property_id = materialData().getPropertyId(prop_name);

    _supplied_props.insert(prop_name);
    _supplied_prop_ids.insert(property_id);

    // Store the minimum state declared
    auto find_min_state = _props_to_min_states.find(property_id);
    if (find_min_state == _props_to_min_states.end())
      _props_to_min_states.emplace(property_id, state);
    else
      find_min_state->second = std::min(find_min_state->second, state);

    // Store material properties for block ids
    for (const auto & block_id : blockIDs())
      _fe_problem.storeSubdomainMatPropName(block_id, prop_name);

    // Store material properties for the boundary ids
    for (const auto & boundary_id : boundaryIDs())
      _fe_problem.storeBoundaryMatPropName(boundary_id, prop_name);
  }

  if (state > 0)
    _has_stateful_property = true;
}

void
MaterialBase::setActiveProperties(const std::unordered_set<unsigned int> & needed_props)
{
  _active_prop_ids.clear();
  for (const auto supplied_id : _supplied_prop_ids)
    if (needed_props.count(supplied_id))
      _active_prop_ids.insert(supplied_id);
}

std::set<OutputName>
MaterialBase::getOutputs()
{
  const std::vector<OutputName> & out = getParam<std::vector<OutputName>>("outputs");
  return std::set<OutputName>(out.begin(), out.end());
}

void
MaterialBase::subdomainSetup()
{
  mooseError("MaterialBase::subdomainSetup in Material'", name(), "' needs to be implemented");
}

void
MaterialBase::computeProperties()
{
  mooseError("MaterialBase::computeProperties in Material '", name(), "' needs to be implemented");
}

void
MaterialBase::computeQpProperties()
{
}

void
MaterialBase::resetProperties()
{
  for (_qp = 0; _qp < qRule().n_points(); ++_qp)
    resetQpProperties();
}

void
MaterialBase::resetQpProperties()
{
  if (!_compute)
    mooseDoOnce(mooseWarning("You disabled the computation of this (",
                             name(),
                             ") material by MOOSE, but have not overridden the 'resetQpProperties' "
                             "method, this can lead to unintended values being set for material "
                             "property values."));
}

void
MaterialBase::computePropertiesAtQp(unsigned int qp)
{
  _qp = qp;
  computeQpProperties();
}

void
MaterialBase::checkExecutionStage()
{
  if (_fe_problem.startedInitialSetup())
    mooseError("Material properties must be retrieved during material object construction to "
               "ensure correct dependency resolution.");
}

void
MaterialBase::markMatPropRequested(const std::string & name)
{
  _fe_problem.markMatPropRequested(name);
}

void
MaterialBase::storeSubdomainZeroMatProp(SubdomainID block_id, const MaterialPropertyName & name)
{
  _fe_problem.storeSubdomainZeroMatProp(block_id, name);
}

void
MaterialBase::storeBoundaryZeroMatProp(BoundaryID boundary_id, const MaterialPropertyName & name)
{
  _fe_problem.storeBoundaryZeroMatProp(boundary_id, name);
}

unsigned int
MaterialBase::getMaxQps() const
{
  return _fe_problem.getMaxQps();
}
