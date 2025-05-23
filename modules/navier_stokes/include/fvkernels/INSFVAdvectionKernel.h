//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FVFluxKernel.h"
#include "INSFVBCInterface.h"

/**
 * An advection kernel that implements interpolation schemes specific to Navier-Stokes flow
 * physics
 */
class INSFVAdvectionKernel : public FVFluxKernel, public INSFVBCInterface
{
public:
  static InputParameters validParams();
  INSFVAdvectionKernel(const InputParameters & params);
  void initialSetup() override;

protected:
  bool skipForBoundary(const FaceInfo & fi) const override;

  /**
   * @returns whether the equation this object adds to has a material time derivative. An instance
   * when this will be false is the mass equation for incompressible flows
   */
  virtual bool hasMaterialTimeDerivative() const = 0;

  /**
   * @returns the advecting velocity
   */
  ADRealVectorValue velocity() const;

  /// The interpolation method to use for the advected quantity
  Moose::FV::InterpMethod _advected_interp_method;

  /// The interpolation method to use for the velocity
  Moose::FV::InterpMethod _velocity_interp_method;

  /// The Rhie-Chow user object that provides us with the velocity
  const RhieChowInterpolatorBase & _rc_vel_provider;
};

inline ADRealVectorValue
INSFVAdvectionKernel::velocity() const
{
  return _rc_vel_provider.getVelocity(
      _velocity_interp_method, *_face_info, determineState(), _tid, hasMaterialTimeDerivative());
}
