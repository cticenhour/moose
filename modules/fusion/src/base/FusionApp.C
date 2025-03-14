#include "FusionApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ThermalHydraulicsApp.h"

InputParameters
FusionApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;

  return params;
}

FusionApp::FusionApp(InputParameters parameters) : MooseApp(parameters)
{
  FusionApp::registerAll(_factory, _action_factory, _syntax);
}

FusionApp::~FusionApp() {}

void
FusionApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  Registry::registerObjectsTo(f, {"FusionApp"});
  Registry::registerActionsTo(af, {"FusionApp"});

  ThermalHydraulicsApp::registerAll(f, af, s);

  /* register custom execute flags, action syntax, etc. here */
}

void
FusionApp::registerApps()
{
  registerApp(FusionApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
FusionApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  FusionApp::registerAll(f, af, s);
}
extern "C" void
FusionApp__registerApps()
{
  FusionApp::registerApps();
}
