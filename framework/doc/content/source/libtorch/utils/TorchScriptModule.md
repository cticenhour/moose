# TorchScriptModule

## Overview

This object can be used to create a module which is read from a torch-script file which can be
exported using pytorch in python. For instruction on how to save a neural network using the torch-script format
using pytorch, click [here](https://pytorch.org/tutorials/advanced/cpp_export.html)!

## Example usage

Within MOOSE a torchscript-based neural network can be created using the name of the torch-script file:

!listing test/src/libtorch/vectorpostprocessors/TorchScriptModuleTest.C start=We read the neural net end=TorchScriptModule include-end=true
