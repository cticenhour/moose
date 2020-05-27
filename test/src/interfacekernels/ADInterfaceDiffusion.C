//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADInterfaceDiffusion.h"

registerMooseObject("MooseTestApp", ADInterfaceDiffusion);

InputParameters
ADInterfaceDiffusion::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("D", "D", "The diffusion coefficient.");
  params.addParam<MaterialPropertyName>(
      "D_neighbor", "D_neighbor", "The neighboring diffusion coefficient.");
  return params;
}

ADInterfaceDiffusion::ADInterfaceDiffusion(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
    _D(getMaterialProperty<Real>("D")),
    _D_neighbor(getNeighborMaterialProperty<Real>("D_neighbor"))
{
}

ADReal
ADInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  ADReal r = 0;

  switch (type)
  {
    case Moose::Element:
      r = _test[_i][_qp] * -_D_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp];
      break;

    case Moose::Neighbor:
      r = _test_neighbor[_i][_qp] * _D[_qp] * _grad_u[_qp] * _normals[_qp];
      break;
  }

  return r;
}
