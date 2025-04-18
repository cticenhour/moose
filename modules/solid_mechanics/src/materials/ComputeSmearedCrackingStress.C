//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComputeSmearedCrackingStress.h"
#include "ElasticityTensorTools.h"
#include "StressUpdateBase.h"
#include "Conversion.h"

registerMooseObject("SolidMechanicsApp", ComputeSmearedCrackingStress);

InputParameters
ComputeSmearedCrackingStress::validParams()
{
  InputParameters params = ComputeMultipleInelasticStress::validParams();
  params.addClassDescription("Compute stress using a fixed smeared cracking model");
  params.addRequiredParam<std::vector<MaterialName>>(
      "softening_models",
      "The material objects used to compute softening behavior for loading a crack."
      "Either 1 or 3 models must be specified. If a single model is specified, it is"
      "used for all directions. If 3 models are specified, they will be used for the"
      "3 crack directions in sequence");
  params.addRequiredCoupledVar(
      "cracking_stress",
      "The stress threshold beyond which cracking occurs. Negative values prevent cracking.");
  MultiMooseEnum direction("x y z");
  params.addParam<MultiMooseEnum>(
      "prescribed_crack_directions", direction, "Prescribed directions of first cracks");
  params.addParam<unsigned int>(
      "max_cracks", 3, "The maximum number of cracks allowed at a material point.");
  params.addRangeCheckedParam<Real>("cracking_neg_fraction",
                                    0,
                                    "cracking_neg_fraction <= 1 & cracking_neg_fraction >= 0",
                                    "The fraction of the cracking strain at which "
                                    "a transition begins during decreasing "
                                    "strain to the original stiffness.");
  params.addParam<Real>(
      "max_stress_correction",
      1.0,
      "Maximum permitted correction to the predicted stress as a ratio of the "
      "stress change to the predicted stress from the previous step's damage level. "
      "Values less than 1 will improve robustness, but not be as accurate.");

  params.addRangeCheckedParam<Real>(
      "shear_retention_factor",
      0.0,
      "shear_retention_factor>=0 & shear_retention_factor<=1.0",
      "Fraction of original shear stiffness to be retained after cracking");
  params.set<std::vector<MaterialName>>("inelastic_models") = {};

  MooseEnum crackedElasticityType("DIAGONAL FULL", "DIAGONAL");
  crackedElasticityType.addDocumentation(
      "DIAGONAL", "Zero out terms coupling with directions orthogonal to a crack (legacy)");
  crackedElasticityType.addDocumentation(
      "FULL", "Consistently scale all entries based on damage (recommended)");
  params.addParam<MooseEnum>(
      "cracked_elasticity_type",
      crackedElasticityType,
      "Method to modify the local elasticity tensor to account for cracking");

  return params;
}

ComputeSmearedCrackingStress::ComputeSmearedCrackingStress(const InputParameters & parameters)
  : ComputeMultipleInelasticStress(parameters),
    _cracking_stress(coupledValue("cracking_stress")),
    _max_cracks(getParam<unsigned int>("max_cracks")),
    _cracking_neg_fraction(getParam<Real>("cracking_neg_fraction")),
    _shear_retention_factor(getParam<Real>("shear_retention_factor")),
    _max_stress_correction(getParam<Real>("max_stress_correction")),
    _cracked_elasticity_type(
        getParam<MooseEnum>("cracked_elasticity_type").getEnum<CrackedElasticityType>()),
    _crack_damage(declareProperty<RealVectorValue>(_base_name + "crack_damage")),
    _crack_damage_old(getMaterialPropertyOld<RealVectorValue>(_base_name + "crack_damage")),
    _crack_flags(declareProperty<RealVectorValue>(_base_name + "crack_flags")),
    _crack_rotation(declareProperty<RankTwoTensor>(_base_name + "crack_rotation")),
    _crack_rotation_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "crack_rotation")),
    _crack_initiation_strain(
        declareProperty<RealVectorValue>(_base_name + "crack_initiation_strain")),
    _crack_initiation_strain_old(
        getMaterialPropertyOld<RealVectorValue>(_base_name + "crack_initiation_strain")),
    _crack_max_strain(declareProperty<RealVectorValue>(_base_name + "crack_max_strain")),
    _crack_max_strain_old(getMaterialPropertyOld<RealVectorValue>(_base_name + "crack_max_strain"))
{
  MultiMooseEnum prescribed_crack_directions =
      getParam<MultiMooseEnum>("prescribed_crack_directions");
  if (prescribed_crack_directions.size() > 0)
  {
    if (prescribed_crack_directions.size() > 3)
      mooseError("A maximum of three crack directions may be specified");
    for (unsigned int i = 0; i < prescribed_crack_directions.size(); ++i)
    {
      for (unsigned int j = 0; j < i; ++j)
        if (prescribed_crack_directions[i] == prescribed_crack_directions[j])
          mooseError("Entries in 'prescribed_crack_directions' cannot be repeated");
      _prescribed_crack_directions.push_back(
          static_cast<unsigned int>(prescribed_crack_directions.get(i)));
    }

    // Fill in the last remaining direction if 2 are specified
    if (_prescribed_crack_directions.size() == 2)
    {
      std::set<unsigned int> available_dirs = {0, 1, 2};
      for (auto dir : _prescribed_crack_directions)
        if (available_dirs.erase(dir) != 1)
          mooseError("Invalid prescribed crack direction:" + Moose::stringify(dir));
      if (available_dirs.size() != 1)
        mooseError("Error in finding remaining available crack direction");
      _prescribed_crack_directions.push_back(*available_dirs.begin());
    }
  }
  if (!isParamSetByUser("cracked_elasticity_type"))
    paramWarning(
        "cracked_elasticity_type",
        "Defaulting to the legacy option of 'DIAGONAL', but the 'FULL' option is preferred");

  _local_elastic_vector.resize(9);
}

void
ComputeSmearedCrackingStress::initQpStatefulProperties()
{
  ComputeMultipleInelasticStress::initQpStatefulProperties();

  _crack_damage[_qp] = 0.0;

  _crack_initiation_strain[_qp] = 0.0;
  _crack_max_strain[_qp](0) = 0.0;

  switch (_prescribed_crack_directions.size())
  {
    case 0:
    {
      _crack_rotation[_qp] = RankTwoTensor::Identity();
      break;
    }
    case 1:
    {
      _crack_rotation[_qp].zero();
      switch (_prescribed_crack_directions[0])
      {
        case 0:
        {
          _crack_rotation[_qp](0, 0) = 1.0;
          _crack_rotation[_qp](1, 1) = 1.0;
          _crack_rotation[_qp](2, 2) = 1.0;
          break;
        }
        case 1:
        {
          _crack_rotation[_qp](1, 0) = 1.0;
          _crack_rotation[_qp](0, 1) = 1.0;
          _crack_rotation[_qp](2, 2) = 1.0;
          break;
        }
        case 2:
        {
          _crack_rotation[_qp](2, 0) = 1.0;
          _crack_rotation[_qp](0, 1) = 1.0;
          _crack_rotation[_qp](1, 2) = 1.0;
          break;
        }
      }
      break;
    }
    case 2:
    {
      mooseError("Number of prescribed crack directions cannot be 2");
      break;
    }
    case 3:
    {
      for (unsigned int i = 0; i < _prescribed_crack_directions.size(); ++i)
      {
        RealVectorValue crack_dir_vec;
        crack_dir_vec(_prescribed_crack_directions[i]) = 1.0;
        _crack_rotation[_qp].fillColumn(i, crack_dir_vec);
      }
    }
  }
}

void
ComputeSmearedCrackingStress::initialSetup()
{
  ComputeMultipleInelasticStress::initialSetup();

  if (!hasGuaranteedMaterialProperty(_elasticity_tensor_name, Guarantee::ISOTROPIC))
    mooseError("ComputeSmearedCrackingStress requires that the elasticity tensor be "
               "guaranteed isotropic");

  std::vector<MaterialName> soft_matls = getParam<std::vector<MaterialName>>("softening_models");
  for (auto soft_matl : soft_matls)
  {
    SmearedCrackSofteningBase * scsb =
        dynamic_cast<SmearedCrackSofteningBase *>(&getMaterialByName(soft_matl));
    if (scsb)
      _softening_models.push_back(scsb);
    else
      paramError("softening_models", "Model " + soft_matl + " is not a softening model");
  }
  if (_softening_models.size() == 1)
  {
    // Reuse the same model in all 3 directions
    _softening_models.push_back(_softening_models[0]);
    _softening_models.push_back(_softening_models[0]);
  }
  else if (_softening_models.size() != 3)
    paramError("softening_models", "Either 1 or 3 softening models must be specified");
}

void
ComputeSmearedCrackingStress::computeQpStress()
{
  bool force_elasticity_rotation = false;

  if (!previouslyCracked())
    computeQpStressIntermediateConfiguration();
  else
  {
    _elastic_strain[_qp] = _elastic_strain_old[_qp] + _strain_increment[_qp];

    // Propagate behavior from the (now inactive) inelastic models
    _inelastic_strain[_qp] = _inelastic_strain_old[_qp];
    for (auto model : _models)
    {
      model->setQp(_qp);
      model->propagateQpStatefulProperties();
    }

    // Since the other inelastic models are inactive, they will not limit the time step
    _material_timestep_limit[_qp] = std::numeric_limits<Real>::max();

    // update _local_elasticity_tensor based on cracking state in previous time step
    updateLocalElasticityTensor();

    // Calculate stress in intermediate configuration
    _stress[_qp] = _local_elasticity_tensor * _elastic_strain[_qp];

    _Jacobian_mult[_qp] = _local_elasticity_tensor;
    force_elasticity_rotation = true;
  }

  // compute crack status and adjust stress
  updateCrackingStateAndStress();

  if (_perform_finite_strain_rotations)
  {
    finiteStrainRotation(force_elasticity_rotation);
    _crack_rotation[_qp] = _rotation_increment[_qp] * _crack_rotation[_qp];
  }
}

void
ComputeSmearedCrackingStress::updateLocalElasticityTensor()
{
  const Real youngs_modulus =
      ElasticityTensorTools::getIsotropicYoungsModulus(_elasticity_tensor[_qp]);

  bool cracking_locally_active = false;

  const Real cracking_stress = _cracking_stress[_qp];

  if (cracking_stress > 0)
  {
    RealVectorValue stiffness_ratio_local(1.0, 1.0, 1.0);
    const RankTwoTensor & R = _crack_rotation_old[_qp];
    RankTwoTensor ePrime(_elastic_strain_old[_qp]);
    ePrime.rotate(R.transpose());

    for (unsigned int i = 0; i < 3; ++i)
    {
      // Update elasticity tensor based on crack status of the end of last time step
      if (_crack_damage_old[_qp](i) > 0.0)
      {
        if (_cracking_neg_fraction == 0.0 && MooseUtils::absoluteFuzzyLessThan(ePrime(i, i), 0.0))
          stiffness_ratio_local(i) = 1.0;
        else if (_cracking_neg_fraction > 0.0 &&
                 ePrime(i, i) < _crack_initiation_strain_old[_qp](i) * _cracking_neg_fraction &&
                 ePrime(i, i) > -_crack_initiation_strain_old[_qp](i) * _cracking_neg_fraction)
        {
          const Real etr = _cracking_neg_fraction * _crack_initiation_strain_old[_qp](i);
          const Real Eo = cracking_stress / _crack_initiation_strain_old[_qp](i);
          const Real Ec = Eo * (1.0 - _crack_damage_old[_qp](i));
          const Real a = (Ec - Eo) / (4 * etr);
          const Real b = (Ec + Eo) / 2;
          // Compute the ratio of the current transition stiffness to the original stiffness
          stiffness_ratio_local(i) = (2.0 * a * etr + b) / Eo;
          cracking_locally_active = true;
        }
        else
        {
          stiffness_ratio_local(i) = (1.0 - _crack_damage_old[_qp](i));
          cracking_locally_active = true;
        }
      }
    }

    if (cracking_locally_active)
    {
      // Update the elasticity tensor in the crack coordinate system
      if (_cracked_elasticity_type == CrackedElasticityType::DIAGONAL)
      {
        const bool c0_coupled = MooseUtils::absoluteFuzzyEqual(stiffness_ratio_local(0), 1.0);
        const bool c1_coupled = MooseUtils::absoluteFuzzyEqual(stiffness_ratio_local(1), 1.0);
        const bool c2_coupled = MooseUtils::absoluteFuzzyEqual(stiffness_ratio_local(2), 1.0);

        const Real c01 = (c0_coupled && c1_coupled ? 1.0 : 0.0);
        const Real c02 = (c0_coupled && c2_coupled ? 1.0 : 0.0);
        const Real c12 = (c1_coupled && c2_coupled ? 1.0 : 0.0);

        const Real c01_shear_retention = (c0_coupled && c1_coupled ? 1.0 : _shear_retention_factor);
        const Real c02_shear_retention = (c0_coupled && c2_coupled ? 1.0 : _shear_retention_factor);
        const Real c12_shear_retention = (c1_coupled && c2_coupled ? 1.0 : _shear_retention_factor);

        _local_elastic_vector[0] = (c0_coupled ? _elasticity_tensor[_qp](0, 0, 0, 0)
                                               : stiffness_ratio_local(0) * youngs_modulus);
        _local_elastic_vector[1] = _elasticity_tensor[_qp](0, 0, 1, 1) * c01;
        _local_elastic_vector[2] = _elasticity_tensor[_qp](0, 0, 2, 2) * c02;
        _local_elastic_vector[3] = (c1_coupled ? _elasticity_tensor[_qp](1, 1, 1, 1)
                                               : stiffness_ratio_local(1) * youngs_modulus);
        _local_elastic_vector[4] = _elasticity_tensor[_qp](1, 1, 2, 2) * c12;
        _local_elastic_vector[5] = (c2_coupled ? _elasticity_tensor[_qp](2, 2, 2, 2)
                                               : stiffness_ratio_local(2) * youngs_modulus);
        _local_elastic_vector[6] = _elasticity_tensor[_qp](1, 2, 1, 2) * c12_shear_retention;
        _local_elastic_vector[7] = _elasticity_tensor[_qp](0, 2, 0, 2) * c02_shear_retention;
        _local_elastic_vector[8] = _elasticity_tensor[_qp](0, 1, 0, 1) * c01_shear_retention;
      }
      else // _cracked_elasticity_type == CrackedElasticityType::FULL
      {
        const Real & c0 = stiffness_ratio_local(0);
        const Real & c1 = stiffness_ratio_local(1);
        const Real & c2 = stiffness_ratio_local(2);

        const Real c01 = c0 * c1;
        const Real c02 = c0 * c2;
        const Real c12 = c1 * c2;

        const Real c01_shear_retention = std::max(c01, _shear_retention_factor);
        const Real c02_shear_retention = std::max(c02, _shear_retention_factor);
        const Real c12_shear_retention = std::max(c12, _shear_retention_factor);

        _local_elastic_vector[0] = _elasticity_tensor[_qp](0, 0, 0, 0) * c0;
        _local_elastic_vector[1] = _elasticity_tensor[_qp](0, 0, 1, 1) * c01;
        _local_elastic_vector[2] = _elasticity_tensor[_qp](0, 0, 2, 2) * c02;
        _local_elastic_vector[3] = _elasticity_tensor[_qp](1, 1, 1, 1) * c1;
        _local_elastic_vector[4] = _elasticity_tensor[_qp](1, 1, 2, 2) * c12;
        _local_elastic_vector[5] = _elasticity_tensor[_qp](2, 2, 2, 2) * c2;
        _local_elastic_vector[6] = _elasticity_tensor[_qp](1, 2, 1, 2) * c12_shear_retention;
        _local_elastic_vector[7] = _elasticity_tensor[_qp](0, 2, 0, 2) * c02_shear_retention;
        _local_elastic_vector[8] = _elasticity_tensor[_qp](0, 1, 0, 1) * c01_shear_retention;
      }

      // Filling with 9 components is sufficient because these are the only nonzero entries
      // for isotropic or orthotropic materials.
      _local_elasticity_tensor.fillFromInputVector(_local_elastic_vector,
                                                   RankFourTensor::symmetric9);

      // Rotate the modified elasticity tensor back into global coordinates
      _local_elasticity_tensor.rotate(R);
    }
  }
  if (!cracking_locally_active)
    _local_elasticity_tensor = _elasticity_tensor[_qp];
}

void
ComputeSmearedCrackingStress::updateCrackingStateAndStress()
{
  const Real youngs_modulus =
      ElasticityTensorTools::getIsotropicYoungsModulus(_elasticity_tensor[_qp]);

  Real cracking_stress = _cracking_stress[_qp];

  if (cracking_stress > 0)
  {
    // Initializing crack states
    _crack_rotation[_qp] = _crack_rotation_old[_qp];

    for (unsigned i = 0; i < 3; ++i)
    {
      _crack_max_strain[_qp](i) = _crack_max_strain_old[_qp](i);
      _crack_initiation_strain[_qp](i) = _crack_initiation_strain_old[_qp](i);
      _crack_damage[_qp](i) = _crack_damage_old[_qp](i);
    }

    // Compute crack orientations: updated _crack_rotation[_qp] based on current strain
    RealVectorValue strain_in_crack_dir;
    computeCrackStrainAndOrientation(strain_in_crack_dir);

    for (unsigned i = 0; i < 3; ++i)
    {
      if (strain_in_crack_dir(i) > _crack_max_strain[_qp](i))
        _crack_max_strain[_qp](i) = strain_in_crack_dir(i);
    }

    // Check for new cracks.
    // Rotate stress to cracked orientation.
    const RankTwoTensor & R = _crack_rotation[_qp];
    RankTwoTensor sigmaPrime(_stress[_qp]);
    sigmaPrime.rotate(R.transpose()); // stress in crack coordinates

    unsigned int num_cracks = 0;
    for (unsigned int i = 0; i < 3; ++i)
    {
      if (_crack_damage_old[_qp](i) > 0.0)
        ++num_cracks;
    }

    bool cracked(false);
    RealVectorValue sigma;
    mooseAssert(_softening_models.size() == 3, "Must have 3 softening models");
    for (unsigned int i = 0; i < 3; ++i)
    {
      sigma(i) = sigmaPrime(i, i);

      Real stiffness_ratio = 1.0 - _crack_damage[_qp](i);

      const bool pre_existing_crack = (_crack_damage_old[_qp](i) > 0.0);
      const bool met_stress_criterion = (sigma(i) > cracking_stress);
      const bool loading_existing_crack = (strain_in_crack_dir(i) >= _crack_max_strain[_qp](i));
      const bool allowed_to_crack = (pre_existing_crack || num_cracks < _max_cracks);
      bool new_crack = false;

      cracked |= pre_existing_crack;

      // Adjustments for newly created cracks
      if (met_stress_criterion && !pre_existing_crack && allowed_to_crack)
      {
        new_crack = true;
        ++num_cracks;

        // Assume Poisson's ratio drops to zero for this direction.  Stiffness is then Young's
        // modulus.
        _crack_initiation_strain[_qp](i) = cracking_stress / youngs_modulus;

        if (_crack_max_strain[_qp](i) < _crack_initiation_strain[_qp](i))
          _crack_max_strain[_qp](i) = _crack_initiation_strain[_qp](i);
      }

      // Update stress and stiffness ratio according to specified crack release model
      if (new_crack || (pre_existing_crack && loading_existing_crack))
      {
        cracked = true;
        _softening_models[i]->computeCrackingRelease(sigma(i),
                                                     stiffness_ratio,
                                                     strain_in_crack_dir(i),
                                                     _crack_initiation_strain[_qp](i),
                                                     _crack_max_strain[_qp](i),
                                                     cracking_stress,
                                                     youngs_modulus);
        _crack_damage[_qp](i) = 1.0 - stiffness_ratio;
      }

      else if (cracked && _cracking_neg_fraction > 0 &&
               _crack_initiation_strain[_qp](i) * _cracking_neg_fraction > strain_in_crack_dir(i) &&
               -_crack_initiation_strain[_qp](i) * _cracking_neg_fraction < strain_in_crack_dir(i))
      {
        const Real etr = _cracking_neg_fraction * _crack_initiation_strain[_qp](i);
        const Real Eo = cracking_stress / _crack_initiation_strain[_qp](i);
        const Real Ec = Eo * (1.0 - _crack_damage_old[_qp](i));
        const Real a = (Ec - Eo) / (4.0 * etr);
        const Real b = 0.5 * (Ec + Eo);
        const Real c = 0.25 * (Ec - Eo) * etr;
        sigma(i) = (a * strain_in_crack_dir(i) + b) * strain_in_crack_dir(i) + c;
      }
    }

    if (cracked)
      updateStressTensorForCracking(_stress[_qp], sigma);
  }

  _crack_flags[_qp](0) = 1.0 - _crack_damage[_qp](2);
  _crack_flags[_qp](1) = 1.0 - _crack_damage[_qp](1);
  _crack_flags[_qp](2) = 1.0 - _crack_damage[_qp](0);
}

void
ComputeSmearedCrackingStress::computeCrackStrainAndOrientation(
    RealVectorValue & strain_in_crack_dir)
{
  // The rotation tensor is ordered such that directions for pre-existing cracks appear first
  // in the list of columns.  For example, if there is one existing crack, its direction is in the
  // first column in the rotation tensor.
  const unsigned int num_known_dirs = getNumKnownCrackDirs();

  if (num_known_dirs == 0)
  {
    std::vector<Real> eigval(3, 0.0);
    RankTwoTensor eigvec;

    _elastic_strain[_qp].symmetricEigenvaluesEigenvectors(eigval, eigvec);

    // If the elastic strain is beyond the cracking strain, save the eigen vectors as
    // the rotation tensor. Reverse their order so that the third principal strain
    // (most tensile) will correspond to the first crack.
    _crack_rotation[_qp].fillColumn(0, eigvec.column(2));
    _crack_rotation[_qp].fillColumn(1, eigvec.column(1));
    _crack_rotation[_qp].fillColumn(2, eigvec.column(0));

    strain_in_crack_dir(0) = eigval[2];
    strain_in_crack_dir(1) = eigval[1];
    strain_in_crack_dir(2) = eigval[0];
  }
  else if (num_known_dirs == 1)
  {
    // This is easily the most complicated case.
    // 1.  Rotate the elastic strain to the orientation associated with the known
    //     crack.
    // 2.  Extract the lower 2x2 block into a separate tensor.
    // 3.  Run the eigen solver on the result.
    // 4.  Update the rotation tensor to reflect the effect of the 2 eigenvectors.

    // 1.
    const RankTwoTensor & R = _crack_rotation[_qp];
    RankTwoTensor ePrime(_elastic_strain[_qp]);
    ePrime.rotate(R.transpose()); // elastic strain in crack coordinates

    // 2.
    ColumnMajorMatrix e2x2(2, 2);
    e2x2(0, 0) = ePrime(1, 1);
    e2x2(1, 0) = ePrime(2, 1);
    e2x2(0, 1) = ePrime(1, 2);
    e2x2(1, 1) = ePrime(2, 2);

    // 3.
    ColumnMajorMatrix e_val2x1(2, 1);
    ColumnMajorMatrix e_vec2x2(2, 2);
    e2x2.eigen(e_val2x1, e_vec2x2);

    // 4.
    RankTwoTensor eigvec(
        1.0, 0.0, 0.0, 0.0, e_vec2x2(0, 1), e_vec2x2(1, 1), 0.0, e_vec2x2(0, 0), e_vec2x2(1, 0));

    _crack_rotation[_qp] = _crack_rotation_old[_qp] * eigvec; // Roe implementation

    strain_in_crack_dir(0) = ePrime(0, 0);
    strain_in_crack_dir(1) = e_val2x1(1, 0);
    strain_in_crack_dir(2) = e_val2x1(0, 0);
  }
  else if (num_known_dirs == 2 || num_known_dirs == 3)
  {
    // Rotate to cracked orientation and pick off the strains in the rotated
    // coordinate directions.
    const RankTwoTensor & R = _crack_rotation[_qp];
    RankTwoTensor ePrime(_elastic_strain[_qp]);
    ePrime.rotate(R.transpose()); // elastic strain in crack coordinates

    strain_in_crack_dir(0) = ePrime(0, 0);
    strain_in_crack_dir(1) = ePrime(1, 1);
    strain_in_crack_dir(2) = ePrime(2, 2);
  }
  else
    mooseError("Invalid number of known crack directions");
}

unsigned int
ComputeSmearedCrackingStress::getNumKnownCrackDirs() const
{
  unsigned int num_known_dirs = 0;
  for (unsigned int i = 0; i < 3; ++i)
  {
    if (_crack_damage_old[_qp](i) > 0.0 || _prescribed_crack_directions.size() >= i + 1)
      ++num_known_dirs;
  }
  return num_known_dirs;
}

void
ComputeSmearedCrackingStress::updateStressTensorForCracking(RankTwoTensor & tensor,
                                                            const RealVectorValue & sigma)
{
  // Get transformation matrix
  const RankTwoTensor & R = _crack_rotation[_qp];
  // Rotate to crack frame
  tensor.rotate(R.transpose());

  // Reset stress if cracked
  for (unsigned int i = 0; i < 3; ++i)
    if (_crack_damage[_qp](i) > 0.0)
    {
      const Real stress_correction_ratio = (tensor(i, i) - sigma(i)) / tensor(i, i);
      if (stress_correction_ratio > _max_stress_correction)
        tensor(i, i) *= (1.0 - _max_stress_correction);
      else if (stress_correction_ratio < -_max_stress_correction)
        tensor(i, i) *= (1.0 + _max_stress_correction);
      else
        tensor(i, i) = sigma(i);
    }

  // Rotate back to global frame
  tensor.rotate(R);
}

bool
ComputeSmearedCrackingStress::previouslyCracked()
{
  for (unsigned int i = 0; i < 3; ++i)
    if (_crack_damage_old[_qp](i) > 0.0)
      return true;
  return false;
}
