[GlobalParams]
  nx = 3
  ny = 3
  n_cells = 2
  pitch = 1
  side_gap = 0.1
  heated_length = 1
  pin_diameter = 0.5
[]

[Mesh]
  [subchannel]
    type = SCMDetailedQuadSubChannelMeshGenerator
  []
  [pins]
    type = SCMDetailedQuadPinMeshGenerator
    input = subchannel
  []
[]
