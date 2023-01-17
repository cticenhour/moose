# Test for vector helmholtz equation with a spatial convergence study
# Manufactured solution: u_real = y^2 * x_hat - x^2 * y_hat
#                        u_imag = y^2 * x_hat - x^2 * y_hat

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmin = -1
    ymin = -1
    elem_type = QUAD9
  []
[]

[Functions]
  [source_real]
    type = ParsedVectorFunction
    expression_x = 'y*y - 2'
    expression_y = '2 - x*x'
  []
  [source_imag]
    type = ParsedVectorFunction
    expression_x = '2 - y*y'
    expression_y = 'x*x - 2'
  []
[]

[Variables]
  [u_real]
    family = NEDELEC_ONE
    order = FIRST
  []
  [u_imag]
    family = NEDELEC_ONE
    order = FIRST
  []
[]

[AuxVariables]
  [u_real_x_aux]
    family = MONOMIAL
    order = FIRST
  []
  [u_real_y_aux]
    family = MONOMIAL
    order = FIRST
  []
  [u_imag_x_aux]
    family = MONOMIAL
    order = FIRST
  []
  [u_imag_y_aux]
    family = MONOMIAL
    order = FIRST
  []
[]

[AuxKernels]
  [real_x]
    type = VectorVariableComponentAux
    vector_variable = u_real
    component = x
    variable = u_real_x_aux
  []
  [real_y]
    type = VectorVariableComponentAux
    vector_variable = u_real
    component = y
    variable = u_real_y_aux
  []
  [imag_x]
    type = VectorVariableComponentAux
    vector_variable = u_imag
    component = x
    variable = u_imag_x_aux
  []
  [imag_y]
    type = VectorVariableComponentAux
    vector_variable = u_imag
    component = y
    variable = u_imag_y_aux
  []
[]

[Kernels]
  [curl_curl_real]
    type = CurlCurlField
    variable = u_real
  []
  [coeff_real]
    type = VectorFunctionReaction
    variable = u_real
  []
  [current_real]
    type = VectorCurrentSource
    variable = u_real
    function_coefficient = -1.0
    source_real = source_real
    source_imag = source_imag
    component = real
  []
  [curl_curl_imag]
    type = CurlCurlField
    variable = u_imag
  []
  [coeff_imag]
    type = VectorFunctionReaction
    variable = u_imag
  []
  [current_imag]
    type = VectorCurrentSource
    variable = u_imag
    function_coefficient = -1.0
    source_real = source_real
    source_imag = source_imag
    component = imaginary
  []
[]

[BCs]
  [sides_real]
    type = VectorCurlPenaltyDirichletBC
    variable = u_real
    function_x = 'y*y'
    function_y = '-x*x'
    penalty = 1e8
    boundary = 'left right top bottom'
  []
  [sides_imag]
    type = VectorCurlPenaltyDirichletBC
    variable = u_imag
    function_x = 'y*y'
    function_y = '-x*x'
    penalty = 1e8
    boundary = 'left right top bottom'
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

[Adaptivity]
  marker = uniform
  steps = 0

  [Markers]
    [uniform]
      type = UniformMarker
      mark = REFINE
      outputs = none
    []
  []
[]

# Manufactured solution: u_real = y^2 * x_hat - x^2 * y_hat
#                        u_imag = y^2 * x_hat - x^2 * y_hat

[Postprocessors]
  active = ''
  [./dofs]
    type = NumDOFs
  [../]
  [./error_real]
    type = ElementVectorL2Error
    var_x = u_real_x_aux
    var_y = u_real_y_aux
    function_x = 'y*y'
    function_y = '-x*x'
  [../]
  [./error_imag]
    type = ElementVectorL2Error
    var_x = u_imag_x_aux
    var_y = u_imag_y_aux
    function_x = 'y*y'
    function_y = '-x*x'
  [../]
[]

[Outputs]
  execute_on = TIMESTEP_END
  exodus = true
  csv = true
[]
