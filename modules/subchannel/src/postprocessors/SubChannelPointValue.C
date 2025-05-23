//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SubChannelPointValue.h"
#include "FEProblemBase.h"
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"
#include "libmesh/system.h"
#include "SCM.h"

registerMooseObject("SubChannelApp", SubChannelPointValue);

InputParameters
SubChannelPointValue::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<VariableName>("variable", "Variable you want the value of");
  params.addRequiredParam<Real>("height", "Axial location of point [m]");
  params.addRequiredParam<int>("index", "Index of subchannel");
  params.addClassDescription(
      "Prints out a user selected value of a specified subchannel at a user selected axial height");
  return params;
}

SubChannelPointValue::SubChannelPointValue(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _mesh(SCM::getConstMesh<SubChannelMesh>(_fe_problem.mesh())),
    _height(getParam<Real>("height")),
    _i_ch(getParam<int>("index")),
    _var_number(_subproblem
                    .getVariable(_tid,
                                 parameters.get<VariableName>("variable"),
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD)
                    .number()),
    _system(_subproblem.getSystem(getParam<VariableName>("variable"))),
    _value(0)
{
  _point =
      Point(_mesh._subchannel_position[_i_ch][0], _mesh._subchannel_position[_i_ch][1], _height);
}

void
SubChannelPointValue::execute()
{
  _value = _system.point_value(_var_number, _point, false);

  if (MooseUtils::absoluteFuzzyEqual(_value, 0.0))
  {
    auto pl = _subproblem.mesh().getPointLocator();
    pl->enable_out_of_mesh_mode();
    auto * elem = (*pl)(_point);
    auto elem_id = elem ? elem->id() : DofObject::invalid_id;
    gatherMin(elem_id);
    if (elem_id == DofObject::invalid_id)
      mooseError(name(), " height: No element located at given height.");
  }
}

Real
SubChannelPointValue::getValue() const
{
  return _value;
}
