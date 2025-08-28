//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADKernelGrad.h"

/**
 * SUPG stabilization term for a forcing function.
 */
class PhaseFieldForcingFunctionSUPG : public ADKernelGrad
{
public:
  static InputParameters validParams();

  PhaseFieldForcingFunctionSUPG(const InputParameters & parameters);

protected:
  virtual ADRealVectorValue precomputeQpResidual() override;

  /// Function value
  const Function & _function;

  /// Velocity vector variable
  const ADVectorVariableValue & _velocity;
};
