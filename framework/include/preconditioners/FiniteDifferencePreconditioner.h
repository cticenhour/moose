//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MoosePreconditioner.h"
#include "MooseEnum.h"

/**
 * Finite difference preconditioner.
 */
class FiniteDifferencePreconditioner : public MoosePreconditioner
{
public:
  static InputParameters validParams();

  FiniteDifferencePreconditioner(const InputParameters & params);
  MooseEnum & finiteDifferenceType() { return _finite_difference_type; }

private:
  MooseEnum _finite_difference_type;
};
