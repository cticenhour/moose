//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#ifndef LASER_WELDINGAPP_H
#define LASER_WELDINGAPP_H

#include "MooseApp.h"

class LaserWeldingApp;

template <>
InputParameters validParams<LaserWeldingApp>();

class LaserWeldingApp : public MooseApp
{
public:
  LaserWeldingApp(InputParameters parameters);
  virtual ~LaserWeldingApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
};

#endif /* LASER_WELDINGAPP_H */
