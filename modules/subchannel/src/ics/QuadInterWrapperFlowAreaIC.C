//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "QuadInterWrapperFlowAreaIC.h"
#include "QuadInterWrapperMesh.h"

registerMooseObject("SubChannelApp", QuadInterWrapperFlowAreaIC);

InputParameters
QuadInterWrapperFlowAreaIC::validParams()
{
  InputParameters params = QuadInterWrapperBaseIC::validParams();
  params.addClassDescription("Computes cross-sectional area for inter-wrapper cells in a square "
                             "lattice subchannel arrangement");
  return params;
}

QuadInterWrapperFlowAreaIC::QuadInterWrapperFlowAreaIC(const InputParameters & params)
  : QuadInterWrapperBaseIC(params)
{
}

Real
QuadInterWrapperFlowAreaIC::value(const Point & p)
{
  auto assembly_pitch = _mesh.getPitch();
  auto side_x = _mesh.getSideX();
  auto side_y = _mesh.getSideY();
  auto gap = _mesh.getGap();

  // Compute the flow area for a standard inter-wrapper channel (one that touches 4 assemblies).
  auto assembly_area = side_x * side_y;
  auto standard_area = assembly_pitch * assembly_pitch - assembly_area;

  auto i = _mesh.getSubchannelIndexFromPoint(p);
  auto subch_type = _mesh.getSubchannelType(i);
  if (subch_type == EChannelType::CORNER)
  {
    // Need to modify later to include the specific side
    auto side_avg = (side_x + side_y) / 2.0;
    return side_avg * gap;
  }
  else if (subch_type == EChannelType::EDGE)
  {
    // Need to modify later to include the specific side
    auto side_avg = (side_x + side_y) / 2.0;
    return standard_area * 0.25 + side_avg * gap;
  }
  else
    return standard_area;
}
