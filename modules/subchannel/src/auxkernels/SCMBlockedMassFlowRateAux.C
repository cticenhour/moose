//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SCMBlockedMassFlowRateAux.h"
#include "SCM.h"

registerMooseObject("SubChannelApp", SCMBlockedMassFlowRateAux);
registerMooseObjectRenamed("SubChannelApp",
                           BlockedMassFlowRateAux,
                           "06/30/2025 24:00",
                           SCMBlockedMassFlowRateAux);

InputParameters
SCMBlockedMassFlowRateAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes inlet mass flow rate BCs, from specified mass flux and "
                             "cross-sectional area and applies blocked inlet conditions");
  params.addRequiredCoupledVar("area", "Cross sectional area [m^2]");
  params.addRequiredParam<PostprocessorName>(
      "unblocked_mass_flux", "Specified mass flux for unblocked subchannels [kg/s-m^2]");
  params.addRequiredParam<PostprocessorName>(
      "blocked_mass_flux", "Specified mass flux for blocked subchannels [kg/s-m^2]]");
  params.addRequiredParam<std::vector<unsigned int>>("index_blockage",
                                                     "index of subchannels affected by blockage");
  return params;
}

SCMBlockedMassFlowRateAux::SCMBlockedMassFlowRateAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _subchannel_mesh(SCM::getConstMesh<SubChannelMesh>(_mesh)),
    _unblocked_mass_flux(getPostprocessorValue("unblocked_mass_flux")),
    _blocked_mass_flux(getPostprocessorValue("blocked_mass_flux")),
    _area(coupledValue("area")),
    _index_blockage(getParam<std::vector<unsigned int>>("index_blockage"))
{
}

Real
SCMBlockedMassFlowRateAux::computeValue()
{
  auto i = _subchannel_mesh.getSubchannelIndexFromPoint(*_current_node);

  for (const auto & index : _index_blockage)
  {
    if (i == index)
      return _blocked_mass_flux * _area[_qp];
  }

  return _unblocked_mass_flux * _area[_qp];
}
