//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PorousFlowRelativePermeabilityFLAC.h"

registerMooseObject("PorousFlowApp", PorousFlowRelativePermeabilityFLAC);
registerMooseObject("PorousFlowApp", ADPorousFlowRelativePermeabilityFLAC);

template <bool is_ad>
InputParameters
PorousFlowRelativePermeabilityFLACTempl<is_ad>::validParams()
{
  InputParameters params = PorousFlowRelativePermeabilityBaseTempl<is_ad>::validParams();
  params.addRequiredRangeCheckedParam<Real>(
      "m", "m >= 0", "relperm = (1 + m)seff^m - m seff^(m+1)");
  params.addClassDescription(
      "This Material calculates relative permeability of a phase using a model inspired by FLAC");
  return params;
}

template <bool is_ad>
PorousFlowRelativePermeabilityFLACTempl<is_ad>::PorousFlowRelativePermeabilityFLACTempl(
    const InputParameters & parameters)
  : PorousFlowRelativePermeabilityBaseTempl<is_ad>(parameters),
    _m(this->template getParam<Real>("m"))
{
}

template <bool is_ad>
GenericReal<is_ad>
PorousFlowRelativePermeabilityFLACTempl<is_ad>::relativePermeability(GenericReal<is_ad> seff) const
{
  return PorousFlowFLACrelperm::relativePermeability(seff, _m);
}

template <bool is_ad>
Real
PorousFlowRelativePermeabilityFLACTempl<is_ad>::dRelativePermeability(Real seff) const
{
  return PorousFlowFLACrelperm::dRelativePermeability(seff, _m);
}

template class PorousFlowRelativePermeabilityFLACTempl<false>;
template class PorousFlowRelativePermeabilityFLACTempl<true>;
