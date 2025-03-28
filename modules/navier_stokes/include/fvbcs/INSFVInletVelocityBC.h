//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FVFunctorDirichletBC.h"
#include "INSFVFlowBC.h"

/**
 * A class for velocity inlet boundary conditions
 */
class INSFVInletVelocityBC : public FVADFunctorDirichletBC, public INSFVFlowBC
{
public:
  static InputParameters validParams();
  INSFVInletVelocityBC(const InputParameters & params);
};
