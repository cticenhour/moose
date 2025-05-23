//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "INSFVFluxBC.h"
#include "INSFVFreeSurfaceBC.h"

/**
 * A class that imparts a surface recoil force on the momentum equation due to liquid phase
 * evaporation
 */
class INSFVVaporRecoilPressureMomentumFluxBC : public INSFVFreeSurfaceBC
{
public:
  static InputParameters validParams();
  INSFVVaporRecoilPressureMomentumFluxBC(const InputParameters & params);

  using INSFVFluxBC::gatherRCData;
  void gatherRCData(const FaceInfo & fi) override;

protected:
  /// The recoil pressure
  const Moose::Functor<ADReal> & _rc_pressure;
};
