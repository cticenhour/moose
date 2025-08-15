//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "HDGKernel.h"
#include "DiffusionLHDGAssemblyHelper.h"

/**
 * Implements the diffusion equation for a hybridized discretization
 */
class DiffusionLHDGKernel : public HDGKernel, public DiffusionLHDGAssemblyHelper
{
public:
  static InputParameters validParams();

  DiffusionLHDGKernel(const InputParameters & params);
  virtual void computeResidual() override;
  /**
   * Compute this object's entire Jacobian, both on- and off-diagonal
   */
  virtual void computeJacobian() override;
  /**
   * Forwards to \p computeJacobian() the first time this is called for a given element
   */
  virtual void computeOffDiagJacobian(unsigned int jvar) override;
  virtual void computeResidualOnSide() override;
  virtual void computeJacobianOnSide() override;
  virtual void initialSetup() override;
  virtual void jacobianSetup() override;

  virtual std::set<std::string> additionalROVariables() override;

protected:
  /// optional source
  const Moose::Functor<Real> & _source;
};
