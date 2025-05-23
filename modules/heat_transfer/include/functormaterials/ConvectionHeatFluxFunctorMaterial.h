//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctorMaterial.h"

/**
 * Computes a convection heat flux from a solid surface to a fluid.
 */
template <bool is_ad>
class ConvectionHeatFluxFunctorMaterialTempl : public FunctorMaterial
{
public:
  static InputParameters validParams();

  ConvectionHeatFluxFunctorMaterialTempl(const InputParameters & parameters);

protected:
  /// Heat transfer coefficient
  const Moose::Functor<GenericReal<is_ad>> & _htc;
  /// Solid temperature
  const Moose::Functor<GenericReal<is_ad>> & _T_solid;
  /// Fluid temperature
  const Moose::Functor<GenericReal<is_ad>> & _T_fluid;
};

typedef ConvectionHeatFluxFunctorMaterialTempl<false> ConvectionHeatFluxFunctorMaterial;
typedef ConvectionHeatFluxFunctorMaterialTempl<true> ADConvectionHeatFluxFunctorMaterial;
