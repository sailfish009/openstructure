======
lDDT
======

------------
Where can I find the lDDT executable? 
------------

The lDDT executable can be found at <YOUR-OST-STAGE-DIR>/bin/

------------
Basic Usage 
------------

To calculate the lDDT for one PDB file (mdl1.pdb) against the reference structure stored in ref.pdb, use the following command; the local and global lDDT scores for each model are printed to the terminal:

.. code-block:: bash

    lddt mdl1.pdb ref.pdb

Similarly it is possible to calculate the lDDT for a list of PDB files:

.. code-block:: bash

    lddt mdl1.pdb mdl2.pdb mdl3.pdb ref.pdb

Or for multiple reference structures at the same time (please find at the bottom of the page an example of usage):  

.. code-block:: bash

    lddt mdl1.pdb ref1.pdb,ref2.pdb,ref3.pdb

------------
All Options 
------------

The lddt executable supports several other command line options, please find them following:

.. code-block:: bash 

    lddt [options] <mod1> [mod1 [mod2]] <re1>[,ref2,ref3]
   
    -s         selection performed on ref
    -c         use Calphas only
    -f         perform structural checks and filter input data
    -t         fault tolerant parsing
    -p <file>  use specified parmeter file. Mandatory
    -v <level> verbosity level (0=results only,1=problems reported, 2=full report)
    -b <value> tolerance in stddevs for bonds
    -a <value> tolerance in stddevs for angles
    -r <value> distance inclusion radius
    -i <value> sequence separation
    -e         print version
    -x         ignore residue name consistency checks


=================
lDDT - Use cases 
=================

--------------------
Model Quality Checks
--------------------

When the lddt executable is called with the -f option, the program performs some 
stereo-chemical and steric clashing checks before computing the lDDT scores. When 
using this option, the user must also provide a text file containing average bond 
lengths,angle widths and minimum clashing distances. lddt ships with a default 
parameter file based on Engh and Huber parameters, and on the atomic radii as 
defined in the Cambridge Structural Database. This file is human readable and can 
be modified with a text editor. The location of the file must be passed to the 
lddt executable using the -p parameter. 

For example:

.. code-block:: bash

    lddt -f -p stereo_chemical_params.txt mdl1.pdb ref.pdb

When the model quality checks are performed, the global and local lDDT scores are 
preceded in the output text by some information on the outcome of the tests. The 
number of serious stereo-chemical violations and steric-clashes in the structure 
is reported, along with some statistics on the global model quality. The local and
global lDDT scores are also adjusted according to the outcome of the tests.  When 
stereochemical violations or steric clashes happen in the side-chain of a residue, 
all distances involving atoms of that side-chain are automatically considered 
non-conserved. When the violations involve backbone atoms of a residue, all 
distances containing atoms of the residue are considered non-conserved. 


-----------------------
Custom Inclusion Radius
-----------------------

The lDDT score evaluates distances between atoms lying closer than a predermined 
inclusion radius. By default the radius is set to 15 Angstroms, but the user can 
override this value by passing a new one to the lddt executable through the -r 
parameter (in Angstroms):

For example:

.. code-block:: bash

    lddt -r 10.0 mdl1.pdb ref.pdb

------------------
Consistency Checks
------------------

When comparing structures, the lddt executable does not perform any chain name 
checks. It only processes the first chain in each structure, irrespective of the 
name. It does, however, perform a check on residue names, to make sure that the 
structures are correctly aligned. The lddt executable will stop with an error if 
the names of the residues being compared do not match. If the user needs for 
specific reasons to override this behavior and skip the check, the lddt executable 
can be called using the -x option. For example:

For example:

.. code-block:: bash

    lddt -x mdl1.pdb ref.pdb

-------------------------
Custom Quality Parameters
-------------------------

The lddt executable uses several thresholds to determine how serious stereo-
chemical violations and steric clashes are. For Bonds and Angles, the parameter 
file contains typical average lengths and widths, together with expected standard 
deviations for their measurements in protein structures. A violation is flagged as 
serious by lDDT when the measured value deviates from the expected one by more 
than a predefined number of standard deviations. By default this value is 12, but 
the user can override the default tolerance thresholds using the -b and -a flags, 
for bonds and angles respectively. 

For steric clashes, the lddt executable recovers atomic radii and clashing 
tolerance distances from the parameter file, depending on the atomic element under 
investigation. When an atomic element cannot be determined, the lddt executable 
uses a default atomic radius of 1.5 Angstrom. This value can be overriden using 
the -m value, passing a new radius (in Ansgstroms) to the program.

For example:

.. code-block:: bash

    lddt -f -p stereo_chemical_params.txt -b 8 -a 8 -m 1.0 mdl1.pdb ref.pdb


-----------------------------
Multiple Reference Structures
-----------------------------

lDDT allows the use of multiple reference structures at the same time (please see 
the manuscript referenced above for details). In order to use multiple references, 
simply add them to the first ref.pdb file, separated by a comma.  

For example:

.. code-block:: bash

    lddt mdl1.pdb ref1.pdb,ref2.pdb,ref3.pdb

----------------
Output Verbosity
----------------

By default the lddt executable only outputs the values of the global and local 
scores. However, users can tweak the verbosity of the output using the -v 
parameters. The devault verbosity level is 0 (scores only). Other available 
levels are: 1 (print information about non conserved distances and failed 
quality checks, if performed) and 2 (print information on all distances and 
all quality checks if performed)

For example:

.. code-block:: bash

    lddt -v 1 -f -p stereo_chemical_params.txt mdl1.pdb ref.pdb

WARNING: Verbosity levels 1 and 2 can generate a large amount of output text, 
especially with large structures and multiple models being evaluated. 