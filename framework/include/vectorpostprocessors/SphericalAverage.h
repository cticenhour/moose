//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SpatialAverageBase.h"

/**
 * Compute a spherical average of a variable as a function of radius throughout the
 * simulation domain.
 */
class SphericalAverage : public SpatialAverageBase
{
public:
  static InputParameters validParams();

  SphericalAverage(const InputParameters & parameters);

protected:
  /// compute the distance of the current quadrature point for binning
  virtual Real computeDistance() override;
};
