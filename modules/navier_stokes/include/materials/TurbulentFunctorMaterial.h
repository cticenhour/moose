//* This file is part of the MOOSE framework
//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctorMaterial.h"

/*
 *Class to compute the effective thermal conductivity due to turbulence.
 */
class TurbulentFunctorMaterial : public FunctorMaterial
{
public:
  static InputParameters validParams();

  TurbulentFunctorMaterial(const InputParameters & parameters);

protected:
  /// Turbulent viscosity
  //  const MooseVariableFVReal & _mu_t;
  const Moose::Functor<ADReal> & _mu_t;
  /// viscosity
  const Moose::Functor<ADReal> & _mu;

  /// thermal conductivity
  const Moose::Functor<ADReal> & _k;

  /// specific heat capacity
  const Moose::Functor<ADReal> & _cp;

  /// turbulent Prandtl numer
  const Moose::Functor<ADReal> & _Pr_t;

  /// User-specified name for effective thermal conductivity
  const std::string _k_eff_name;
};
