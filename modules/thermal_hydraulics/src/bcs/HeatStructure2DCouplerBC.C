//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HeatStructure2DCouplerBC.h"
#include "Function.h"

registerMooseObject("ThermalHydraulicsApp", HeatStructure2DCouplerBC);

InputParameters
HeatStructure2DCouplerBC::validParams()
{
  InputParameters params = HeatStructure2DCouplerBCBase::validParams();

  params.addRequiredParam<FunctionName>("heat_transfer_coefficient",
                                        "Heat transfer coefficient function");
  params.addRequiredParam<Real>("coupling_area_fraction", "Coupling area fraction");

  params.addClassDescription("Applies BC for HeatStructure2DCoupler for plate heat structure");

  return params;
}

HeatStructure2DCouplerBC::HeatStructure2DCouplerBC(const InputParameters & parameters)
  : HeatStructure2DCouplerBCBase(parameters),

    _htc(getFunction("heat_transfer_coefficient")),
    _coupling_area_fraction(getParam<Real>("coupling_area_fraction"))
{
}

ADReal
HeatStructure2DCouplerBC::computeQpResidual()
{
  const auto T_coupled = computeCoupledTemperature();
  return _htc.value(_t, _q_point[_qp]) * (_u[_qp] - T_coupled) * _coupling_area_fraction *
         _test[_i][_qp];
}
