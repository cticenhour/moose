//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SideIntegralPostprocessor.h"

/**
 * This postprocessor computes the area of a specified block.
 */
class AreaPostprocessor : public SideIntegralPostprocessor
{
public:
  static InputParameters validParams();

  AreaPostprocessor(const InputParameters & parameters);

  virtual void threadJoin(const UserObject & y) override;

protected:
  virtual Real computeQpIntegral() override;
};
