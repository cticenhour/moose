//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MaxVarNDofsPerElemPP.h"
#include "NonlinearSystem.h"

registerMooseObject("MooseTestApp", MaxVarNDofsPerElemPP);

InputParameters
MaxVarNDofsPerElemPP::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  return params;
}

MaxVarNDofsPerElemPP::MaxVarNDofsPerElemPP(const InputParameters & parameters)
  : GeneralPostprocessor(parameters)
{
}

PostprocessorValue
MaxVarNDofsPerElemPP::getValue() const
{
  return _fe_problem.getNonlinearSystem(0).getMaxVarNDofsPerElem();
}
