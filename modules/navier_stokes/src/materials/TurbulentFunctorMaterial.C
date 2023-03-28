//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TurbulentFunctorMaterial.h"
#include "NS.h"

registerMooseObject("NavierStokesApp", TurbulentFunctorMaterial);

InputParameters
TurbulentFunctorMaterial::validParams()
{
  InputParameters params = FunctorMaterial::validParams();
  params.addClassDescription("Computes effective thermal conductivity due to turbulence");
  // params.addRequiredCoupledVar("mu_t", "Turbulent viscosity.");
  params.addRequiredParam<std::string>("k_eff", "The name of the effective thermal conductivity");
  params.addRequiredParam<MooseFunctorName>(NS::mu, "The viscosity");
  params.addRequiredParam<MooseFunctorName>(NS::k, "The value for the thermal conductivity");
  params.addRequiredParam<MooseFunctorName>(NS::cp, "The value for the specific heat capacity");
  params.addRequiredParam<MooseFunctorName>("Pr_t", "The value for the turbulent Prandtl number");
  params.addRequiredParam<MooseFunctorName>("mu_t", "the value for turb visc");
  return params;
}

TurbulentFunctorMaterial::TurbulentFunctorMaterial(const InputParameters & parameters)
  : FunctorMaterial(parameters),
    // _mu_t(*getVarHelper<MooseVariableFVReal>("mu_t", 0)),
    _mu_t(getFunctor<ADReal>("mu_t")),
    _mu(getFunctor<ADReal>(NS::mu)),
    _k(getFunctor<ADReal>(NS::k)),
    _cp(getFunctor<ADReal>(NS::cp)),
    _Pr_t(getFunctor<ADReal>("Pr_t")),
    _k_eff_name(getParam<std::string>("k_eff"))
{

  addFunctorProperty<ADReal>(_k_eff_name,

                             [this](const auto & r, const auto & t) -> ADReal
                             { return _k(r, t) + _cp(r, t) * _mu_t(r, t) / _Pr_t(r, t); });
}
