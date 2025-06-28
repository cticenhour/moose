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

/**
 * A template class for finite volume dirichlet boundary conditions.
 * @tparam whether automatic differentiation is used or not
 */
template <bool is_ad>
class FVFunctorDirichletBCTempl : public FVDirichletBCBase
{
public:
  FVFunctorDirichletBCTempl(const InputParameters & parameters);

  static InputParameters validParams();

  ADReal boundaryValue(const FaceInfo & fi, const Moose::StateArg & state) const override;

private:
  /// The value for this BC
  const Moose::Functor<GenericReal<is_ad>> & _functor;

  /// Whether to evaluate the functor on the other side
  bool _use_other_side;
};

typedef FVFunctorDirichletBCTempl<false> FVFunctorDirichletBC;
typedef FVFunctorDirichletBCTempl<true> FVADFunctorDirichletBC;
