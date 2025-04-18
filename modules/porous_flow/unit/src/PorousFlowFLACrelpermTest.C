//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "gtest/gtest.h"

#include "PorousFlowFLACrelperm.h"

const double eps = 1.0E-8;

TEST(PorousFlowFLACrelpermTest, relperm)
{
  EXPECT_NEAR(1.0, PorousFlowFLACrelperm::relativePermeability(1.0E30, 2.7), 1.0E-5);
  EXPECT_NEAR(0.0, PorousFlowFLACrelperm::relativePermeability(-1.0, 2.7), 1.0E-5);
  EXPECT_NEAR(0.111976072427008, PorousFlowFLACrelperm::relativePermeability(0.3, 2.7), 1.0E-5);
  EXPECT_NEAR(0.208087549965399, PorousFlowFLACrelperm::relativePermeability(0.8, 12.7), 1.0E-5);
}

TEST(PorousFlowFLACrelpermTest, drelperm)
{
  Real fd;
  EXPECT_NEAR(0.0, PorousFlowFLACrelperm::dRelativePermeability(1.0E30, 2.7), 1.0E-5);
  EXPECT_NEAR(0.0, PorousFlowFLACrelperm::dRelativePermeability(-1.0, 2.7), 1.0E-5);
  fd = (PorousFlowFLACrelperm::relativePermeability(0.3 + eps, 2.7) -
        PorousFlowFLACrelperm::relativePermeability(0.3, 2.7)) /
       eps;
  EXPECT_NEAR(fd, PorousFlowFLACrelperm::dRelativePermeability(0.3, 2.7), 1.0E-5);
  fd = (PorousFlowFLACrelperm::relativePermeability(0.8 + eps, 0.65) -
        PorousFlowFLACrelperm::relativePermeability(0.8, 0.65)) /
       eps;
  EXPECT_NEAR(fd, PorousFlowFLACrelperm::dRelativePermeability(0.8, 0.65), 1.0E-5);
}

TEST(PorousFlowFLACrelpermTest, d2relperm)
{
  Real fd;
  EXPECT_NEAR(0.0, PorousFlowFLACrelperm::d2RelativePermeability(1.0E30, 2.7), 1.0E-5);
  EXPECT_NEAR(0.0, PorousFlowFLACrelperm::d2RelativePermeability(-1.0, 2.7), 1.0E-5);
  fd = (PorousFlowFLACrelperm::dRelativePermeability(0.3 + eps, 2.7) -
        PorousFlowFLACrelperm::dRelativePermeability(0.3, 2.7)) /
       eps;
  EXPECT_NEAR(fd, PorousFlowFLACrelperm::d2RelativePermeability(0.3, 2.7), 1.0E-5);
  fd = (PorousFlowFLACrelperm::dRelativePermeability(0.8 + eps, 0.65) -
        PorousFlowFLACrelperm::dRelativePermeability(0.8, 0.65)) /
       eps;
  EXPECT_NEAR(fd, PorousFlowFLACrelperm::d2RelativePermeability(0.8, 0.65), 1.0E-5);
}

TEST(PorousFlowFLACrelpermTest, adrelperm)
{
  ADReal sat = 0.3;
  sat.derivatives() = {};
  Moose::derivInsert(sat.derivatives(), 0, 1.0);

  const auto adrelperm = PorousFlowFLACrelperm::relativePermeability(sat, 2.7);

  const auto relperm = PorousFlowFLACrelperm::relativePermeability(sat.value(), 2.7);
  const auto drelperm = PorousFlowFLACrelperm::dRelativePermeability(sat.value(), 2.7);

  EXPECT_NEAR(adrelperm.value(), relperm, 1.0E-5);
  EXPECT_NEAR(adrelperm.derivatives()[0], drelperm, 1.0E-5);
}
