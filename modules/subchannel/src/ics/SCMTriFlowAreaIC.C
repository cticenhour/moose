//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SCMTriFlowAreaIC.h"
#include "TriSubChannelMesh.h"
#include "SCM.h"

registerMooseObject("SubChannelApp", SCMTriFlowAreaIC);
registerMooseObjectRenamed("SubChannelApp", TriFlowAreaIC, "06/30/2025 24:00", SCMTriFlowAreaIC);

InputParameters
SCMTriFlowAreaIC::validParams()
{
  InputParameters params = TriSubChannelBaseIC::validParams();
  params.addClassDescription(
      "Computes flow area of subchannels in a triangular lattice arrangement");
  return params;
}

SCMTriFlowAreaIC::SCMTriFlowAreaIC(const InputParameters & params)
  : TriSubChannelBaseIC(params), _subchannel_mesh(SCM::getConstMesh<SubChannelMesh>(_mesh))
{
}

Real
SCMTriFlowAreaIC::value(const Point & p)
{
  Real standard_area, rod_area, wire_area, additional_area;
  auto pitch = _mesh.getPitch();
  auto pin_diameter = _mesh.getPinDiameter();
  auto wire_diameter = _mesh.getWireDiameter();
  auto wire_lead_length = _mesh.getWireLeadLength();
  auto gap = _mesh.getDuctToPinGap();
  auto z_blockage = _mesh.getZBlockage();
  auto index_blockage = _mesh.getIndexBlockage();
  auto reduction_blockage = _mesh.getReductionBlockage();
  auto theta = std::acos(wire_lead_length /
                         std::sqrt(std::pow(wire_lead_length, 2) +
                                   std::pow(libMesh::pi * (pin_diameter + wire_diameter), 2)));
  auto i = _mesh.getSubchannelIndexFromPoint(p);
  // given the channel number, i, it computes the flow area of the subchannel
  // based on the subchannel type: CENTER, EDGE or CORNER.
  auto subch_type = _mesh.getSubchannelType(i);

  if (subch_type == EChannelType::CENTER)
  {
    standard_area = std::pow(pitch, 2) * std::sqrt(3.0) / 4.0;
    rod_area = libMesh::pi * std::pow(pin_diameter, 2.0) / 8.0;
    additional_area = 0.0;
    wire_area = libMesh::pi * std::pow(wire_diameter, 2.0) / 8.0 / std::cos(theta);
  }
  else if (subch_type == EChannelType::EDGE)
  {
    standard_area = pitch * (pin_diameter / 2.0 + gap);
    rod_area = libMesh::pi * std::pow(pin_diameter, 2.0) / 8.0;
    additional_area = 0.0;
    wire_area = libMesh::pi * std::pow(wire_diameter, 2.0) / 8.0 / std::cos(theta);
  }
  else
  {
    standard_area = 1.0 / std::sqrt(3.0) * std::pow((pin_diameter / 2.0 + gap), 2.0);
    rod_area = libMesh::pi * std::pow(pin_diameter, 2.0) / 24.0;
    additional_area = 0.0;
    wire_area = libMesh::pi * std::pow(wire_diameter, 2.0) / 24.0 / std::cos(theta);
  }

  /// Calculate subchannel area
  auto subchannel_area = standard_area + additional_area - rod_area - wire_area;

  /// Apply area reduction on subchannels affected by blockage
  auto index = 0;
  for (const auto & i_blockage : index_blockage)
  {
    if (i == i_blockage && (p(2) >= z_blockage.front() && p(2) <= z_blockage.back()))
    {
      return reduction_blockage[index] * subchannel_area;
    }
    index++;
  }

  return subchannel_area;
}
