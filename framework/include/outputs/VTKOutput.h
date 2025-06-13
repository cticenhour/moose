//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "SampledOutput.h"

/**
 *
 */
class VTKOutput : public SampledOutput
{
public:
  static InputParameters validParams();

  /**
   * Class constructor
   * @param parameters Object parameters
   */
  VTKOutput(const InputParameters & parameters);

  bool supportsMaterialPropertyOutput() const override { return true; }

protected:
  /**
   * Perform the output of VTKOutput
   */
  virtual void output() override;

  /**
   * Return the file name with the *.vtk extension
   */
  virtual std::string filename() override;

private:
  /// Flag for using binary compression
  bool _binary;
};
