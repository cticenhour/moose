//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SolidMechanicsPlasticModel.h"
#include "SolidMechanicsHardeningModel.h"

/**
 * Class that limits the mean stress
 * Yield function = a*mean_stress - strength
 * mean_stress = (stress_xx + stress_yy + stress_zz)/3
 * a is a real constant, strength is a SolidMechanicsHardening object.
 * Associative
 */
class SolidMechanicsPlasticMeanCap : public SolidMechanicsPlasticModel
{
public:
  static InputParameters validParams();

  SolidMechanicsPlasticMeanCap(const InputParameters & parameters);

  virtual std::string modelName() const override;

protected:
  Real yieldFunction(const RankTwoTensor & stress, Real intnl) const override;

  RankTwoTensor dyieldFunction_dstress(const RankTwoTensor & stress, Real intnl) const override;

  Real dyieldFunction_dintnl(const RankTwoTensor & stress, Real intnl) const override;

  RankTwoTensor flowPotential(const RankTwoTensor & stress, Real intnl) const override;

  RankFourTensor dflowPotential_dstress(const RankTwoTensor & stress, Real intnl) const override;

  RankTwoTensor dflowPotential_dintnl(const RankTwoTensor & stress, Real intnl) const override;

  /// a/3
  Real _a_over_3;

  /// strength
  const SolidMechanicsHardeningModel & _strength;
};
