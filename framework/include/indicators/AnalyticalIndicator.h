//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralIndicator.h"

class AnalyticalIndicator : public ElementIntegralIndicator
{
public:
  static InputParameters validParams();

  AnalyticalIndicator(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  const Function & _func;
};
