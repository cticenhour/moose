//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FVDirichletBCBase.h"

template <bool is_ad>
class FVUseFunctorSideForSsfDirichletBCTempl : public FVDirichletBCBase
{
public:
  FVUseFunctorSideForSsfDirichletBCTempl(const InputParameters & parameters);

  static InputParameters validParams();

  ADReal boundaryValue(const FaceInfo & fi, const Moose::StateArg & state) const override;

  bool hasFaceSide(const FaceInfo & fi, bool fi_elem_side) const override;

private:
  /// The value for this BC
  const Moose::Functor<GenericReal<is_ad>> & _functor;
};

typedef FVUseFunctorSideForSsfDirichletBCTempl<false> FVUseFunctorSideForSsfDirichletBC;
typedef FVUseFunctorSideForSsfDirichletBCTempl<true> FVADUseFunctorSideForSsfDirichletBC;
