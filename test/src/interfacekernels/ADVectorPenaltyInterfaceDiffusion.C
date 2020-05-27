//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADVectorPenaltyInterfaceDiffusion.h"

registerMooseObject("MooseTestApp", ADVectorPenaltyInterfaceDiffusion);

InputParameters
ADVectorPenaltyInterfaceDiffusion::validParams()
{
  InputParameters params = ADVectorInterfaceKernel::validParams();
  params.addClassDescription("A test ADVectorInterfaceKernel that imposes the condition: $\\vec{u} - "
                             "\\vec{v} = 0$ across an interface.");
  params.addRequiredParam<Real>(
      "penalty", "The penalty that penalizes jump between master and neighbor variables.");
  return params;
}

ADVectorPenaltyInterfaceDiffusion::ADVectorPenaltyInterfaceDiffusion(const InputParameters & parameters)
  : ADVectorInterfaceKernel(parameters), _penalty(getParam<Real>("penalty"))
{
}

ADReal
ADVectorPenaltyInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  ADReal res = 0;

  switch (type)
  {
    case Moose::Element:
      res = _test[_i][_qp] * _penalty * (_u[_qp] - _neighbor_value[_qp]);
      break;

    case Moose::Neighbor:
      res = _test_neighbor[_i][_qp] * -_penalty * (_u[_qp] - _neighbor_value[_qp]);
      break;
  }

  return res;
}
