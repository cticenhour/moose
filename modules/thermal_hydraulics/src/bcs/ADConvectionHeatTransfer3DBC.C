//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADConvectionHeatTransfer3DBC.h"
#include "ADHeatTransferFromHeatStructure3D1PhaseUserObject.h"
#include "THMIndicesVACE.h"

registerMooseObject("ThermalHydraulicsApp", ADConvectionHeatTransfer3DBC);

InputParameters
ADConvectionHeatTransfer3DBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredParam<UserObjectName>(
      "ht_uo", "The name of the user object that computes the heat flux");
  params.addClassDescription("Adds a convective heat flux boundary condition between the local "
                             "component heat structure and a 3D heat structure");
  return params;
}

ADConvectionHeatTransfer3DBC::ADConvectionHeatTransfer3DBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _ht_uo(getUserObject<ADHeatTransferFromHeatStructure3D1PhaseUserObject>("ht_uo"))
{
}

ADReal
ADConvectionHeatTransfer3DBC::computeQpResidual()
{
  const std::vector<ADReal> & T_fluid = _ht_uo.getTfluid(_current_elem->id());
  const std::vector<ADReal> & htc = _ht_uo.getHeatTransferCoeff(_current_elem->id());
  return htc[_qp] * (_u[_qp] - T_fluid[_qp]) * _test[_i][_qp];
}
