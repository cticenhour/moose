//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PINSFEMaterial.h"

registerMooseObject("NavierStokesApp", PINSFEMaterial);
registerMooseObjectRenamed("NavierStokesApp",
                           GenericPorousMediumMaterial,
                           "02/01/2024 00:00",
                           PINSFEMaterial);

InputParameters
PINSFEMaterial::validParams()
{
  InputParameters params = INSFEMaterial::validParams();

  params.addClassDescription("Computes generic material properties related to simulation of fluid "
                             "flow in a porous medium");
  params.addRequiredParam<Real>("alpha", "Inertia drag coefficient");
  params.addRequiredParam<Real>("beta", "Viscous drag coefficient");
  params.addParam<Real>("pm_htc", 0, "Fluid-to-solid heat transfer coefficient in porous medium");
  params.addParam<Real>("pm_aw", 0, "Fluid-to-solid heat transfer area density in porous medium");

  return params;
}

PINSFEMaterial::PINSFEMaterial(const InputParameters & parameters)
  : INSFEMaterial(parameters),
    _alpha(getParam<Real>("alpha")),
    _beta(getParam<Real>("beta")),
    _pm_htc_const(getParam<Real>("pm_htc")),
    _pm_aw_const(getParam<Real>("pm_aw")),
    _pm_htc(declareProperty<Real>("pm_heat_transfer_coefficient")),
    _pm_aw(declareProperty<Real>("pm_heat_transfer_area_density"))
{
}

void
PINSFEMaterial::computeQpProperties()
{
  INSFEMaterial::computeQpProperties();

  _inertia_resistance_coeff[_qp](0, 0) = _alpha;
  _inertia_resistance_coeff[_qp](1, 1) = _alpha;
  _inertia_resistance_coeff[_qp](2, 2) = _alpha;

  _viscous_resistance_coeff[_qp](0, 0) = _beta;
  _viscous_resistance_coeff[_qp](1, 1) = _beta;
  _viscous_resistance_coeff[_qp](2, 2) = _beta;

  _pm_htc[_qp] = _pm_htc_const;
  _pm_aw[_qp] = _pm_aw_const;
}
