//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralPostprocessor.h"

template <bool>
class ElementIntegralMaterialPropertyTempl;
typedef ElementIntegralMaterialPropertyTempl<false> ElementIntegralMaterialProperty;
typedef ElementIntegralMaterialPropertyTempl<true> ADElementIntegralMaterialProperty;

template <bool is_ad>
class ElementIntegralMaterialPropertyTempl : public ElementIntegralPostprocessor
{
public:
  static InputParameters validParams();

  ElementIntegralMaterialPropertyTempl(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  const GenericMaterialProperty<Real, is_ad> & _scalar;
};
