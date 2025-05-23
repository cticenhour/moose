//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADNodalKernel.h"
#include "SubProblem.h"
#include "SystemBase.h"
#include "MooseVariableFE.h"
#include "Assembly.h"

#include "metaphysicl/raw_type.h"

InputParameters
ADNodalKernel::validParams()
{
  auto params = NodalKernelBase::validParams();
  params += ADFunctorInterface::validParams();
  return params;
}

ADNodalKernel::ADNodalKernel(const InputParameters & parameters)
  : NodalKernelBase(parameters), ADFunctorInterface(this), _u(_var.adDofValues())
{
  if (getParam<std::vector<AuxVariableName>>("save_in").size())
    paramError("save_in",
               "ADNodalKernels do not support save_in. Please use the tagging system instead.");
  if (getParam<std::vector<AuxVariableName>>("diag_save_in").size())
    paramError(
        "diag_save_in",
        "ADNodalKernels do not support diag_save_in. Please use the tagging system instead.");
}

void
ADNodalKernel::computeResidual()
{
  if (_var.isNodalDefined())
  {
    const auto dof_idx = _var.nodalDofIndex();
    _qp = 0;
    auto res = MetaPhysicL::raw_value(computeQpResidual());
    addResiduals(_assembly,
                 std::array<Real, 1>{{res}},
                 std::array<dof_id_type, 1>{{dof_idx}},
                 _var.scalingFactor());
  }
}

void
ADNodalKernel::computeJacobian()
{
  if (_var.isNodalDefined())
  {
    const auto dof_idx = _var.nodalDofIndex();
    _qp = 0;
    const auto res = computeQpResidual();
    addJacobian(_assembly,
                std::array<ADReal, 1>{{res}},
                std::array<dof_id_type, 1>{{dof_idx}},
                _var.scalingFactor());
  }
}

void
ADNodalKernel::computeOffDiagJacobian(const unsigned int jvar)
{
  if (jvar == _var.number())
    computeJacobian();
}
