//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "LaserWeldingTestApp.h"
#include "LaserWeldingApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<LaserWeldingTestApp>()
{
  InputParameters params = validParams<LaserWeldingApp>();
  return params;
}

LaserWeldingTestApp::LaserWeldingTestApp(InputParameters parameters) : MooseApp(parameters)
{
  LaserWeldingTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

LaserWeldingTestApp::~LaserWeldingTestApp() {}

void
LaserWeldingTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  LaserWeldingApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"LaserWeldingTestApp"});
    Registry::registerActionsTo(af, {"LaserWeldingTestApp"});
  }
}

void
LaserWeldingTestApp::registerApps()
{
  registerApp(LaserWeldingApp);
  registerApp(LaserWeldingTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
LaserWeldingTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  LaserWeldingTestApp::registerAll(f, af, s);
}
extern "C" void
LaserWeldingTestApp__registerApps()
{
  LaserWeldingTestApp::registerApps();
}
