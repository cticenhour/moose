//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

/*
 *Computes the mixing length for the mixing length turbulence model.
 */
class WallDistanceMixingLengthAux : public AuxKernel
{
public:
  static InputParameters validParams();

  WallDistanceMixingLengthAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const std::vector<BoundaryName> & _wall_boundary_names;

  const Moose::Functor<Real> & _von_karman_const;
  const Moose::Functor<Real> & _von_karman_const_0;
  const Moose::Functor<Real> & _delta;
};
