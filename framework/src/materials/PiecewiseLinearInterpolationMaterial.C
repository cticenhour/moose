//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PiecewiseLinearInterpolationMaterial.h"

// MOOSE includes
#include "MooseVariableFE.h"

registerMooseObject("MooseApp", PiecewiseLinearInterpolationMaterial);

InputParameters
PiecewiseLinearInterpolationMaterial::validParams()
{

  InputParameters params = Material::validParams();
  params.addClassDescription("Compute a property using a piecewise linear interpolation to define "
                             "its dependence on a variable");
  params.addRequiredParam<std::string>("property",
                                       "The name of the property this material will compute");
  params.addRequiredCoupledVar(
      "variable",
      "The name of the variable whose value is used as the abscissa in the interpolation");
  params.addParam<std::vector<Real>>("x", "The abscissa values");
  params.addParam<std::vector<Real>>("y", "The ordinate values");
  params.addParam<std::vector<Real>>("xy_data",
                                     "All function data, supplied in abscissa, ordinate pairs");
  params.addParam<Real>("scale_factor", 1.0, "Scale factor to be applied to the ordinate values");
  params.addParam<bool>(
      "extrapolation",
      false,
      "Use linear extrapolation to evaluate points that lie outside given data set domain. ");
  params.declareControllable("scale_factor");
  return params;
}

PiecewiseLinearInterpolationMaterial::PiecewiseLinearInterpolationMaterial(
    const InputParameters & parameters)
  : DerivativeMaterialInterface<Material>(parameters),
    _prop_name(getParam<std::string>("property")),
    _coupled_var(coupledValue("variable")),
    _scale_factor(getParam<Real>("scale_factor")),
    _extrap(getParam<bool>("extrapolation")),
    _property(declareProperty<Real>(_prop_name)),
    _dproperty(isCoupledConstant("variable")
                   ? nullptr
                   : &declarePropertyDerivative<Real>(_prop_name, coupledName("variable", 0)))
{
  std::vector<Real> x;
  std::vector<Real> y;

  if ((parameters.isParamValid("x")) || (parameters.isParamValid("y")))
  {
    if (!((parameters.isParamValid("x")) && (parameters.isParamValid("y"))))
      mooseError("In PiecewiseLinearInterpolationMaterial ",
                 _name,
                 ": Both 'x' and 'y' must be specified if either one is specified.");

    if (parameters.isParamValid("xy_data"))
      mooseError("In PiecewiseLinearInterpolationMaterial ",
                 _name,
                 ": Cannot specify 'x', 'y', and 'xy_data' together.");

    x = getParam<std::vector<Real>>("x");
    y = getParam<std::vector<Real>>("y");
  }
  else if (parameters.isParamValid("xy_data"))
  {
    std::vector<Real> xy = getParam<std::vector<Real>>("xy_data");
    unsigned int xy_size = xy.size();
    if (xy_size % 2 != 0)
      mooseError("In PiecewiseLinearInterpolationMaterial ",
                 _name,
                 ": Length of data provided in 'xy_data' must be a multiple of 2.");

    unsigned int x_size = xy_size / 2;
    x.reserve(x_size);
    y.reserve(x_size);
    for (unsigned int i = 0; i < xy_size / 2; ++i)
    {
      x.push_back(xy[i * 2]);
      y.push_back(xy[i * 2 + 1]);
    }
  }

  try
  {
    _linear_interp = std::make_unique<LinearInterpolation>(x, y, _extrap);
  }
  catch (std::domain_error & e)
  {
    mooseError("In PiecewiseLinearInterpolationMaterial ", _name, ": ", e.what());
  }
}

void
PiecewiseLinearInterpolationMaterial::computeQpProperties()
{
  _property[_qp] = _scale_factor * _linear_interp->sample(_coupled_var[_qp]);
  if (_dproperty)
    (*_dproperty)[_qp] = _scale_factor * _linear_interp->sampleDerivative(_coupled_var[_qp]);
}
