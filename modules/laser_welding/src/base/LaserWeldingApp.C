#include "LaserWeldingApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

#include "NavierStokesApp.h"

template <>
InputParameters
validParams<LaserWeldingApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

registerKnownLabel("LaserWeldingApp");

LaserWeldingApp::LaserWeldingApp(InputParameters parameters) : MooseApp(parameters)
{
  LaserWeldingApp::registerAll(_factory, _action_factory, _syntax);
}

LaserWeldingApp::~LaserWeldingApp() {}

void
LaserWeldingApp::registerApps()
{
  registerApp(LaserWeldingApp);
}

void
LaserWeldingApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  NavierStokesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"LaserWeldingApp"});
  Registry::registerActionsTo(af, {"LaserWeldingApp"});
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
LaserWeldingApp__registerApps()
{
  LaserWeldingApp::registerApps();
}

extern "C" void
NavierStokesApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  LaserWeldingApp::registerAll(f, af, s);
}
