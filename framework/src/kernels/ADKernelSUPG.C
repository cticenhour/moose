//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADKernelSUPG.h"
#include "MathUtils.h"
#include "Assembly.h"

// libmesh includes
#include "libmesh/threads.h"

template <typename T>
InputParameters
ADKernelSUPGTempl<T>::validParams()
{
  InputParameters params = ADKernelStabilizedTempl<T>::validParams();
  params.addParam<MaterialPropertyName>(
      "tau_name", "tau", "The name of the stabilization parameter tau.");
  params.addCoupledVar("velocity", "The velocity variable.");
  params.addParam<MaterialPropertyName>("material_velocity",
                                        "A material property describing the velocity");
  return params;
}

template <typename T>
ADKernelSUPGTempl<T>::ADKernelSUPGTempl(const InputParameters & parameters)
  : ADKernelStabilizedTempl<T>(parameters),
    _tau(this->template getADMaterialProperty<Real>("tau_name")),
    _velocity(
        this->isParamValid("velocity")
            ? this->adCoupledVectorValue("velocity")
            : this->template getADMaterialProperty<RealVectorValue>("material_velocity").get())
{
}

template <typename T>
ADRealVectorValue
ADKernelSUPGTempl<T>::computeQpStabilization()
{
  return _velocity[_qp] * _tau[_qp];
}

template class ADKernelSUPGTempl<Real>;
template class ADKernelSUPGTempl<RealVectorValue>;
