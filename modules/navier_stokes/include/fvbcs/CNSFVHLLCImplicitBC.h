//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "CNSFVHLLCBC.h"

/**
 * HLLC implicit boundary conditions
 */
class CNSFVHLLCImplicitBC : public CNSFVHLLCBC
{
public:
  CNSFVHLLCImplicitBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void preComputeWaveSpeed() override;

  virtual ADReal fluxBoundary() override;
  virtual ADReal hllcBoundary() override;
  virtual ADReal conservedVariableBoundary() override;
};
