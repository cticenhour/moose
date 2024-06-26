# [Transfer System](syntax/Transfers/index.md)

A system to move data to and from the "parent application" and "sub-applications" of a MultiApp.

+New Feature+: Transfers can now send data between sub-applications. We refer to this capability as 'siblings' transfer.

!---

Transferred data typically is received by the Auxiliary and Postprocessor systems.

The data on the receiving application should couple to these values in the normal way and
each sub-application should be able to solve on its own.

!---

## Field Transfers

Fields can be transferred between applications using a variety of interpolation algorithms.

- The direction of the transfer is specified by giving the `from_multi_app` or `to_multi_app` parameter.
- The source field is evaluated at the destination points (generally nodes or element centroids, depending on the receiving variable type).
- The evaluations are then put into the receiving `AuxVariable` field specified by the `variable` parameter.
- `GeneralField` versions of each transfer are implemented using a different algorithm and may be preferred as they support more features.

!listing multiapp_mesh_function_transfer/exec_on_mismatch.i block=Transfers

!---

## UserObject Interpolation

- Many `UserObjects` compute spatially-varying data that is not associated directly with a mesh. For example, local pin-wise averages of a power variable.
- Any `UserObject` can override `Real spatialValue(Point &)` to provide a value given a point in space
- A `UserObjectTransfer` can sample this spatially-varying data from one app and put the values into an `AuxVariable` in another
- A `GeneralField` versions of this transfer is implemented using a different algorithm and may be preferred as it is more feature-complete.

!listing multiapp_userobject_transfer/3d_1d_parent.i block=Transfers

!---

## Scalar number Transfer

A Postprocessor or a scalar variable transfer allows a transfer of scalar values between applications

- When transferring to a `MultiApp`, the value can either be put into a `Postprocessor` value or can be put into a constant `AuxVariable` field
  - for example, the main app variable value at the position of the child application can be stored in a postprocessor
- When transferring from a `MultiApp` to the parent application, the `Postprocessor` values from all the sub-apps can be interpolated to form an auxiliary field
  on the parent application

!listing multiapp_postprocessor_transfer/parent.i block=Transfers
