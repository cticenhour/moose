//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralArrayVariablePostprocessor.h"

class ElementArrayL2Norm : public ElementIntegralArrayVariablePostprocessor
{
public:
  static InputParameters validParams();

  ElementArrayL2Norm(const InputParameters & parameters);

  virtual Real getValue() const override;

protected:
  virtual Real computeQpIntegral() override;
};
