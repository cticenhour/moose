//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "DirectDirichletBC.h"

registerMooseObject("SolidMechanicsApp", DirectDirichletBC);

InputParameters
DirectDirichletBC::validParams()
{
  InputParameters params = DirectDirichletBCBase::validParams();
  params.addRequiredParam<Real>("value", "Value of the BC");
  params.declareControllable("value");
  params.addClassDescription("Imposes the essential boundary condition $u=g$, where $g$ "
                             "is a constant, controllable value.");
  return params;
}

DirectDirichletBC::DirectDirichletBC(const InputParameters & parameters)
  : DirectDirichletBCBase(parameters), _value(getParam<Real>("value"))
{
}

Real
DirectDirichletBC::computeQpValue()
{
  return _value;
}
