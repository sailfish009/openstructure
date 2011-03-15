Crystallographic datasets
================================================================================

The image handles themselves are only partially suited to deal with X-ray crystallographic data. While they support Non-orthogonal coordinate systems, they do not have a notion of 

The XtalMap class
--------------------------------------------------------------------------------

The XtalMap class is used to represent a crystal. It only stores one assymetric unit (ASU) and reconstructs the symmetry related copies of the crystal based on symmetry operators. 