//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PiecewiseLinear.h"

registerMooseObject("MooseApp", PiecewiseLinear);
registerMooseObjectRenamed("MooseApp", ADPiecewiseLinear, "02/03/2024 00:00", PiecewiseLinear);

InputParameters
PiecewiseLinear::validParams()
{
  InputParameters params = PiecewiseLinearBase::validParams();
  params.addParam<bool>(
      "extrap", false, "If true, extrapolates when sample point is outside of abscissa range");
  params.addClassDescription("Linearly interpolates between pairs of x-y data");
  return params;
}

PiecewiseLinear::PiecewiseLinear(const InputParameters & parameters)
  : PiecewiseLinearBase(parameters)
{
  if (isRawDataLoaded())
  {
    this->buildInterpolation(this->template getParam<bool>("extrap"));
    _interpolation_created = true;
  }
}

void
PiecewiseLinear::initialSetup()
{
  PiecewiseTabularBase::initialSetup();
  if (isRawDataLoaded() && !_interpolation_created)
    this->buildInterpolation(this->template getParam<bool>("extrap"));
  else if (!isRawDataLoaded())
    mooseError("Data has still not been loaded at setup time. Something has gone wrong during "
               "Function initialization, contact a developer");
}
