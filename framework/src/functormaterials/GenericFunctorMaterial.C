//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "GenericFunctorMaterial.h"
#include "Function.h"
#include "MathUtils.h"

registerMooseObject("MooseApp", GenericFunctorMaterial);
registerMooseObject("MooseApp", ADGenericFunctorMaterial);
registerMooseObjectRenamed("MooseApp",
                           GenericConstantFunctorMaterial,
                           "06/30/2022 24:00",
                           GenericFunctorMaterial);
registerMooseObjectRenamed("MooseApp",
                           ADGenericConstantFunctorMaterial,
                           "06/30/2022 24:00",
                           ADGenericFunctorMaterial);
registerMooseObjectRenamed("MooseApp",
                           GenericFunctionFunctorMaterial,
                           "06/30/2022 24:00",
                           GenericFunctorMaterial);
registerMooseObjectRenamed("MooseApp",
                           ADGenericFunctionFunctorMaterial,
                           "06/30/2022 24:00",
                           ADGenericFunctorMaterial);

template <bool is_ad>
InputParameters
GenericFunctorMaterialTempl<is_ad>::validParams()
{
  InputParameters params = FunctorMaterial::validParams();
  params.set<ExecFlagEnum>("execute_on") = {EXEC_ALWAYS};
  params.addClassDescription(
      "FunctorMaterial object for declaring properties that are populated by evaluation of a "
      "Functor (a constant, variable, function or functor material property) objects.");
  params.addParam<std::vector<std::string>>("prop_names",
                                            "The names of the properties this material will have");
  params.addParam<std::vector<MooseFunctorName>>("prop_values",
                                                 "The corresponding names of the "
                                                 "functors that are going to provide "
                                                 "the values for the variables");
  params.addParam<bool>("define_dot_functors",
                        true,
                        "Whether to define additional functors for the time derivative and the "
                        "time derivative of the gradient");
  return params;
}

template <bool is_ad>
GenericFunctorMaterialTempl<is_ad>::GenericFunctorMaterialTempl(const InputParameters & parameters)
  : FunctorMaterial(parameters),
    _prop_names(getParam<std::vector<std::string>>("prop_names")),
    _prop_values(getParam<std::vector<MooseFunctorName>>("prop_values"))
{
  unsigned int num_names = _prop_names.size();
  unsigned int num_values = _prop_values.size();

  if (num_names != num_values)
    mooseError("Number of prop_names must match the number of prop_values for a "
               "GenericFunctorMaterial!");

  // Check that there is no name conflict, a common mistake with this object
  for (const auto i : make_range(num_names))
    for (const auto j : make_range(num_values))
      if (_prop_names[i] == _prop_values[j])
        paramError("prop_names",
                   "prop_names should not be the same as any of the prop_values. They"
                   " can both be functors, and functors may not have the same name.");

  _num_props = num_names;
  _functors.resize(num_names);

  for (const auto i : make_range(_num_props))
    _functors[i] = &getFunctor<GenericReal<is_ad>>(_prop_values[i]);

  const std::set<ExecFlagType> clearance_schedule(_execute_enum.begin(), _execute_enum.end());
  for (const auto i : make_range(_num_props))
  {
    addFunctorProperty<GenericReal<is_ad>>(
        _prop_names[i],
        [this, i](const auto & r, const auto & t) -> GenericReal<is_ad>
        { return (*_functors[i])(r, t); },
        clearance_schedule);
    if (getParam<bool>("define_dot_functors"))
    {
      addFunctorProperty<GenericReal<is_ad>>(
          MathUtils::timeDerivName(_prop_names[i]),
          [this, i](const auto & r, const auto & t) -> GenericReal<is_ad>
          { return _functors[i]->dot(r, t); },
          clearance_schedule);
      addFunctorProperty<GenericRealVectorValue<is_ad>>(
          MathUtils::gradName(MathUtils::timeDerivName(_prop_names[i])),
          [this, i](const auto & r, const auto & t) -> GenericRealVectorValue<is_ad>
          { return _functors[i]->gradDot(r, t); },
          clearance_schedule);
    }
  }
}

template class GenericFunctorMaterialTempl<false>;
template class GenericFunctorMaterialTempl<true>;
