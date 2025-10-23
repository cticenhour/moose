//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "WCNSFVFluxBCBase.h"

class SinglePhaseFluidProperties;

/**
 * Flux boundary condition for the weakly compressible energy equation
 */
class WCNSFVEnergyFluxBC : public WCNSFVFluxBCBase
{
public:
  static InputParameters validParams();
  WCNSFVEnergyFluxBC(const InputParameters & params);

protected:
  ADReal computeQpResidual() override;

  /// override because energy_pp is not considered in base class
  virtual bool isInflow() const override;

  /// Postprocessor with the inlet temperature
  const PostprocessorValue * const _temperature_pp;

  /// Postprocessor with the inlet energy flow rate
  const PostprocessorValue * const _energy_pp;

  /// Fluid specific heat capacity functor
  const Moose::Functor<ADReal> * _cp;

  /// Fluid temperature functor
  const Moose::Functor<ADReal> & _temperature;

  /// Fluid pressure functor
  const Moose::Functor<ADReal> * _pressure;

  /// Pointer to the enthalpy functor
  const Moose::Functor<ADReal> * _h_fluid;

  /// Fluid properties object
  const SinglePhaseFluidProperties * const _fluid;

private:
  /// Computes the enthalpy using what the user has specified
  template <typename T>
  ADReal enthalpy(const T & loc_arg, const Moose::StateArg & state, const bool inflow) const;
};
