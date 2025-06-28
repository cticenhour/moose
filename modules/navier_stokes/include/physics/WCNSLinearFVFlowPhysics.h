//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "WCNSFVFlowPhysicsBase.h"
#include "WCNSFVTurbulencePhysics.h"

/**
 * Creates all the objects needed to solve the Navier-Stokes equations with the SIMPLE algorithm
 * using the linear finite volume discretization
 * Currently does not implement:
 * - friction
 * - other momentum sources and sinks
 * - porous media
 * - transients
 */
class WCNSLinearFVFlowPhysics final : public WCNSFVFlowPhysicsBase
{
public:
  static InputParameters validParams();

  WCNSLinearFVFlowPhysics(const InputParameters & parameters);

protected:
  virtual void initializePhysicsAdditional() override;

private:
  virtual void addSolverVariables() override;
  virtual void addFVKernels() override;
  virtual void addUserObjects() override;

  /// Function adding kernels for the incompressible pressure correction equation
  void addPressureCorrectionKernels();

  /**
   * Functions adding kernels for the incompressible momentum equation
   * If the material properties are not constant, these can be used for
   * weakly-compressible simulations (except the Boussinesq kernel) as well.
   */
  void addMomentumTimeKernels() override;
  void addMomentumFluxKernels();
  virtual void addMomentumPressureKernels() override;
  virtual void addMomentumGravityKernels() override;
  virtual void addMomentumFrictionKernels() override;
  virtual void addMomentumBoussinesqKernels() override;

  virtual void addInletBC() override;
  virtual void addOutletBC() override;
  virtual void addWallsBC() override;
  virtual void addSeparatorBC() override {}

  virtual bool hasForchheimerFriction() const override { return false; };

  virtual void addRhieChowUserObjects() override;
  virtual void addFunctorMaterials() override;

  virtual MooseFunctorName getLinearFrictionCoefName() const override
  {
    mooseError("Not implemented");
  }

  UserObjectName rhieChowUOName() const override;

  unsigned short getNumberAlgebraicGhostingLayersNeeded() const override;

  /// Whether to use the correction term for non-orthogonality
  const bool _non_orthogonal_correction;
};
