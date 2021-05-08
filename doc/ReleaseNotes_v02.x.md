# SNiPER v2.x Release Notes

This is the release notes for the SNiPER project.

## v2.0 (to be released)

- Changes
  - Move to CMake completely. If you still have to build a SNiPER-based project with CMT, please refer to [cmt4sniper](https://github.com/SNiPER-Framework/cmt4sniper).
  - API changes (details can be found in the README of [cmt4sniper](https://github.com/SNiPER-Framework/cmt4sniper)):
    - Move "DataBuffer/DataBuffer.h" to "SniperKernel/DataBuffer.h".
    - The property machanism is enhanced. However, we have to provide the `operator<<` and `operator>>` to a property variable if the operators are unavailable by default.
    - The return type of `DLElement::getParent()` and `DLElement::getRoot()` is changed from `Task*` to `IExecUnit*`.

- New features
  - SniperKernel can be used without boost and python now
  - Add an executable sniper.exe
  - Configure a job from a json file
