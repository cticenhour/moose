//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "NodalKernel.h"

/**
 * Represents the rate in a simple ODE of du/dt = rate
 */
class ConstantRate : public NodalKernel
{
public:
  /**
   * Constructor initializes the rate
   */
  static InputParameters validParams();

  ConstantRate(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  /// The rate
  const Real & _rate;
};
