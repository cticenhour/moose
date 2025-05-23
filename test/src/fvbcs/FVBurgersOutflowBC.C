//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FVBurgersOutflowBC.h"

registerMooseObject("MooseTestApp", FVBurgersOutflowBC);

InputParameters
FVBurgersOutflowBC::validParams()
{
  InputParameters params = FVFluxBC::validParams();
  return params;
}

FVBurgersOutflowBC::FVBurgersOutflowBC(const InputParameters & parameters) : FVFluxBC(parameters) {}

ADReal
FVBurgersOutflowBC::computeQpResidual()
{
  mooseAssert(_face_info->elem().dim() == 1, "FVBurgersOutflowBC works only in 1D");

  ADReal r = 0;
  // only add this on outflow faces
  if (_u[_qp] * _normal(0) > 0)
    r = 0.5 * _u[_qp] * _u[_qp] * _normal(0);
  return r;
}
