//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Action.h"

/**
 * Checks whether there are any Kernels or BoundaryConditions in the warehouses and if so adds a
 * default coupling functor to ensure correct sparsity
 */
class CouplingFunctorCheckAction : public Action
{
public:
  static InputParameters validParams();

  CouplingFunctorCheckAction(const InputParameters & parameters);

protected:
  void act() override;
};
