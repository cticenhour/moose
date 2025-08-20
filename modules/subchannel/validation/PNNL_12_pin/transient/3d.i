[Mesh]
  [subchannel]
    type = SCMDetailedQuadSubChannelMeshGenerator
    nx = 7
    ny = 3
    n_cells = 48
    pitch = 0.014605
    pin_diameter = 0.012065
    side_gap = 0.0015875
    heated_length = 1.2192
  []
[]

[AuxVariables]
  [mdot]
  []
  [SumWij]
  []
  [P]
  []
  [DP]
  []
  [h]
  []
  [T]
  []
  [rho]
  []
  [mu]
  []
  [S]
  []
  [w_perim]
  []
  [q_prime]
  []
[]

[Problem]
  type = NoSolveProblem
[]

[Outputs]
  exodus = true
[]

[Executioner]
  type = Transient
[]
