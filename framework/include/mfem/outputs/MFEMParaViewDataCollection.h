#ifdef MFEM_ENABLED

#pragma once
#include "MFEMDataCollection.h"

/**
 * Class for output information saved in MFEM ParaViewDataCollections
 */
class MFEMParaViewDataCollection : public MFEMDataCollection
{
public:
  static InputParameters validParams();

  MFEMParaViewDataCollection(const InputParameters & parameters);

  virtual mfem::DataCollection & getDataCollection() override { return _pv_dc; }

protected:
  mfem::ParaViewDataCollection _pv_dc;
  const bool _high_order_output;
  const unsigned int _refinements;
  const mfem::VTKFormat _vtk_format;
};

#endif
