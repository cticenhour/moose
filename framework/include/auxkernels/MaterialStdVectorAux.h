//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MaterialStdVectorAuxBase.h"

/**
 * AuxKernel for outputting a std::vector material-property component to an AuxVariable
 */
template <bool is_ad>
class MaterialStdVectorAuxTempl : public MaterialStdVectorAuxBaseTempl<Real, is_ad>
{
public:
  static InputParameters validParams();

  MaterialStdVectorAuxTempl(const InputParameters & parameters);

protected:
  virtual Real getRealValue() override;

  using MaterialStdVectorAuxBaseTempl<Real, is_ad>::_index;
  using MaterialStdVectorAuxBaseTempl<Real, is_ad>::getRealValue;
};

typedef MaterialStdVectorAuxTempl<false> MaterialStdVectorAux;
typedef MaterialStdVectorAuxTempl<true> ADMaterialStdVectorAux;
