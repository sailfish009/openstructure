.. ost-actions:

OST Actions
================================================================================

A pure command line interface of OST is provided by actions.
You can execute ``ost -h`` for a list of possible actions and for every action,
you can type ``ost <ACTION> -h`` to get a description on its usage.

Here we list the most prominent actions with simple examples.

.. ost-compare-structures:

Comparing two structures
--------------------------------------------------------------------------------

You can compare two structures in terms of quaternary structure score and
lDDT scores between two complexes from the command line with:

.. code-block:: console

  $ ost compare-structures [-h] [-v VERBOSITY] -m MODEL -r REFERENCE
                           [-o OUTPUT] [-d] [-ds DUMP_SUFFIX]
                           [-rs REFERENCE_SELECTION] [-ms MODEL_SELECTION]
                           [-ca] [-ft] [-qs]
                           [-c CHAIN_MAPPING [CHAIN_MAPPING ...]] [-rna]
                           [-l] [-sc] [-p PARAMETER_FILE]
                           [-bt BOND_TOLERANCE] [-at ANGLE_TOLERANCE]
                           [-ir INCLUSION_RADIUS] [-ss SEQUENCE_SEPARATION]
                           [-cc] [-spr] [-ml] [-cl COMPOUND_LIBRARY]
                           [-rm REMOVE [REMOVE ...]] [-ce] [-mn]

By default the verbosity is set to 3 which will result in the informations
being shown in the console. The result can be (optionally) saved as JSON file
which is the preferred way of parsing it as the log output might change in the
future. Optionally, the local scores for lDDT can also be dumped to the output
file. Additionally, cleaned up structures can be saved to the disk.
The output file has following format:

.. code-block:: none

  {
      "result": {
          "<MODEL NAME>": { # Model name extracted from the file name
              "<REFERENCE NAME>": { # Reference name extracted from the file name
                  "info": {
                      "residue_names_consistent": <Are the residue numbers consistent? true or false>, 
                      "mapping": <Mapping of chains eg. {"A": "B", "B": "A"}>, 
                          "alignments": <list of chain-chain alignments in FASTA format>
                      }
                  }, 
                  "lddt": {  # calculated when --lddt (-l) option is selected
                      "oligo_lddt": {
                          "status": <SUCCESS or FAILURE>,
                          "error": <ERROR message if any>, 
                          "global_score": <calculated oligomeric lddt score>, 
                      }, 
                      "weighted_lddt": {
                          "status": <SUCCESS or FAILURE>,
                          "error": <ERROR message if any>, 
                          "global_score": <calculated weighted lddt score>, 
                      }, 
                      "single_chain_lddt": [ # a list of chain-chain lDDts
                          {
                              "status": <SUCCESS or FAILURE>,
                              "error": <ERROR message if any>, 
                              "reference_chain": <name of the chain in reference>, 
                              "model_chain": <name of the chain in model>
                              "global_score": <calculated single-chain lddt score>, 
                              "conserved_contacts": <number of conserved contacts between model and reference>, 
                              "total_contacts": <total number of contacts between model and reference>,
                              "per_residue_scores": [  # per-residue lDDT scores - calculated when --save-per-residue-scores (-spr) option is selected
                                  {
                                      "total_contacts": <total number of contacts between model and reference>, 
                                      "residue_name": <three letter code of the residue>, 
                                      "lddt": <residue lDDT score>, 
                                      "conserved_contacts": <number of conserved contacts between model and reference for given residue>, 
                                      "residue_number": <total number of contacts between model and reference for given residue>
                                  },
                                  .
                                  .
                                  .
                              ]
                          }
                      ]
                  },
                  "qs_score": {   # calculated when --qs-score (-q) option is selected
                    "status": <SUCCESS or FAILURE>,
                    "error": <ERROR message if any>, 
                    "global_score": <Global QS-score>, 
                    "best_score": <Best QS-score>, 
                }

              }
          }
      }, 
      "options": {}  # Options used to run the script
  }

The "result" filed is a dictionary mapping from model to reference as eg. in
mmCIF file there can be many entities and the script will compare all
combinations.

Example usage:

.. code-block:: console

  $ curl https://www.cameo3d.org/static/data/modeling/2018.03.03/5X7J_B/bu_target_01.pdb > reference.pdb
  $ curl https://www.cameo3d.org/static/data/modeling/2018.03.03/5X7J_B/servers/server11/oligo_model-1/superposed_oligo_model-1.pdb > model.pdb
  $ $OST_ROOT/bin/ost compare-structures --model model.pdb --reference reference.pdb --output output.json --qs-score --residue-number-alignment --lddt --structural-checks --consistency-checks --inclusion-radius 15.0 --bond-tolerance 15.0 --angle-tolerance 15.0 --molck --remove oxt hyd unk --clean-element-column --map-nonstandard-residues

  ################################################################################
  Reading input files (fault_tolerant=False)
   --> reading model from model.pdb
  imported 2 chains, 396 residues, 3106 atoms; with 0 helices and 0 strands
   --> reading reference from reference.pdb
  imported 3 chains, 408 residues, 3011 atoms; with 0 helices and 0 strands
  ################################################################################
  Cleaning up input with Molck
  removing hydrogen atoms
   --> removed 0 hydrogen atoms
  removing OXT atoms
   --> removed 0 OXT atoms
  residue A.GLN54 is missing 4 atoms: 'CG', 'CD', 'OE1', 'NE2'
  residue A.GLU55 is missing 4 atoms: 'CG', 'CD', 'OE1', 'OE2'
  residue A.ARG139 is missing 6 atoms: 'CG', 'CD', 'NE', 'CZ', 'NH1', 'NH2'
  residue B.THR53 is missing 1 atom: 'CG2'
  residue B.GLN54 is missing 4 atoms: 'CG', 'CD', 'OE1', 'NE2'
  residue B.GLU55 is missing 4 atoms: 'CG', 'CD', 'OE1', 'OE2'
  residue B.GLU61 is missing 1 atom: 'OE2'
  residue B.GLU117 is missing 1 atom: 'O'
  residue B.ARG120 is missing 2 atoms: 'NH1', 'NH2'
  residue B.ARG142 is missing 2 atoms: 'NH1', 'NH2'
  residue B.GLU148 is missing 4 atoms: 'CG', 'CD', 'OE1', 'OE2'
  residue B.PRO198 is missing 1 atom: 'O'
  _.CL1 is not a standard amino acid
  _.CL2 is not a standard amino acid
  _.CL3 is not a standard amino acid
  _.CL4 is not a standard amino acid
  _.CA5 is not a standard amino acid
  _.CA6 is not a standard amino acid
  _.CA7 is not a standard amino acid
  _.CA8 is not a standard amino acid
  _.CA9 is not a standard amino acid
  _.CL10 is not a standard amino acid
  _.CL11 is not a standard amino acid
  _.CL12 is not a standard amino acid
  _.CL13 is not a standard amino acid
  _.CL14 is not a standard amino acid
  _.CL15 is not a standard amino acid
  _.CA16 is not a standard amino acid
  _.CA17 is not a standard amino acid
  _.CA18 is not a standard amino acid
  _.CA19 is not a standard amino acid
  _.CA20 is not a standard amino acid
  _.EDO21 is not a standard amino acid
  _.EDO22 is not a standard amino acid
  _.EDO23 is not a standard amino acid
  _.EDO24 is not a standard amino acid
  removing hydrogen atoms
   --> removed 0 hydrogen atoms
  removing OXT atoms
   --> removed 0 OXT atoms
  ################################################################################
  Performing structural checks
   --> for reference(s)
  Checking 
  Checking stereo-chemistry
  Average Z-Score for bond lengths: -nan
  Bonds outside of tolerance range: 0 out of 0
  Bond  Avg Length  Avg zscore  Num Bonds
  Average Z-Score angle widths: 0.00000
  Angles outside of tolerance range: 0 out of 1
  Filtering non-bonded clashes
  0 non-bonded short-range distances shorter than tolerance distance
  Distances shorter than tolerance are on average shorter by: 0.00000
   --> for model(s)
  Checking 
  Checking stereo-chemistry
  Average Z-Score for bond lengths: -nan
  Bonds outside of tolerance range: 0 out of 0
  Bond  Avg Length  Avg zscore  Num Bonds
  Average Z-Score angle widths: 0.00000
  Angles outside of tolerance range: 0 out of 1
  Filtering non-bonded clashes
  0 non-bonded short-range distances shorter than tolerance distance
  Distances shorter than tolerance are on average shorter by: 0.00000
  ################################################################################
  Comparing  to 
  Chains removed from : _
  Chains in : AB
  Chains in : AB
  Chemically equivalent chain-groups in pdb_1: [['B', 'A']]
  Chemically equivalent chain-groups in pdb_2: [['A', 'B']]
  Chemical chain-groups mapping: {('B', 'A'): ('A', 'B')}
  Identifying Symmetry Groups...
  Symmetry threshold 0.1 used for angles of pdb_1
  Symmetry threshold 0.1 used for axis of pdb_1
  Symmetry threshold 0.1 used for angles of pdb_2
  Symmetry threshold 0.1 used for axis of pdb_2
  Selecting Symmetry Groups...
  Symmetry-groups used in pdb_1: [('B',), ('A',)]
  Symmetry-groups used in pdb_2: [('A',), ('B',)]
  Closed Symmetry with strict parameters
  Mapping found: {'A': 'B', 'B': 'A'}
  --------------------------------------------------------------------------------
  Checking consistency between  and 
  Consistency check: OK
  --------------------------------------------------------------------------------
  Computing QS-score
  QSscore pdb_1, pdb_2: best: 0.90, global: 0.90
  --------------------------------------------------------------------------------
  Computing lDDT scores
  lDDT settings: 
  Inclusion Radius: 15
  Sequence separation: 0
  Cutoffs: 0.5, 1, 2, 4
  Residue properties label: lddt
  ===
   --> Computing lDDT between model chain B and reference chain A
  Coverage: 1 (187 out of 187 residues)
  Global LDDT score: 0.8257
  (877834 conserved distances out of 1063080 checked, over 4 thresholds)
   --> Computing lDDT between model chain A and reference chain B
  Coverage: 1 (197 out of 197 residues)
  Global LDDT score: 0.7854
  (904568 conserved distances out of 1151664 checked, over 4 thresholds)
   --> Computing oligomeric lDDT score
  Reference pdb_1 has: 2 chains
  Model pdb_2 has: 2 chains
  Coverage: 1 (384 out of 384 residues)
  Oligo lDDT score: 0.8025
   --> Computing weighted lDDT score
  Weighted lDDT score: 0.8048
  ################################################################################
  Saving output into output.json


This reads the model and reference file and calculates QS-score between them.
In the example above the output file looks as follows:

.. code-block:: python

  {
      "result": {
          "": {
              "": {
                  "info": {
                      "residue_names_consistent": true, 
                      "mapping": {
                          "chain_mapping": {
                              "A": "B", 
                              "B": "A"
                          }, 
                          "alignments": [
                              ">reference:A\n-PGLFLTLEGLDGSGKTTQARRLAAFLEAQGRPVLLTREPGGGLPEVRSL---QELSPEAEYLLFSADRAEHVRKVILPGLAAGKVVISDRYLDSSLAYQGYGRGLPLPWLREVAREATRGLKPRLTFLLDLPPEAALRRVR-------LGLEFFRRVREGYLALARAEPGRFVVLDATLPEEEIARAIQAHLRPLLP\n>model:B\nMPGLFLTLEGLDGSGKTTQARRLAAFLEAQGRPVLLTREPGGGLPEVRSLLLTQELSPEAEYLLFSADRAEHVRKVILPGLAAGKVVISDRYLDSSLAYQGYGRGLPLPWLREVAREATRGLKPRLTFLLDLPPEAALRRVRRPDRLEGLGLEFFRRVREGYLALARAEPGRFVVLDATLPEEEIARAIQAHLRPLLP", 
                              ">reference:B\n-PGLFLTLEGLDGSGKTTQARRLAAFLEAQGRPVLLTREPGGGLPEVRSLLLTQELSPEAEYLLFSADRAEHVRKVILPGLAAGKVVISDRYLDSSLAYQGYGRGLPLPWLREVAREATRGLKPRLTFLLDLPPEAALRRVRRPDRLEGLGLEFFRRVREGYLALARAEPGRFVVLDATLPEEEIARAIQAHLRPLLP\n>model:A\nMPGLFLTLEGLDGSGKTTQARRLAAFLEAQGRPVLLTREPGGGLPEVRSLLLTQELSPEAEYLLFSADRAEHVRKVILPGLAAGKVVISDRYLDSSLAYQGYGRGLPLPWLREVAREATRGLKPRLTFLLDLPPEAALRRVRRPDRLEGLGLEFFRRVREGYLALARAEPGRFVVLDATLPEEEIARAIQAHLRPLLP"
                          ]
                      }
                  }, 
                  "lddt": {
                      "oligo_lddt": {
                          "status": "SUCCESS", 
                          "global_score": 0.8025223016738892, 
                          "error": ""
                      }, 
                      "weighted_lddt": {
                          "status": "SUCCESS", 
                          "global_score": 0.804789180710712, 
                          "error": ""
                      }, 
                      "single_chain_lddt": [
                          {
                              "status": "SUCCESS", 
                              "global_score": 0.8257459402084351, 
                              "conserved_contacts": 877834, 
                              "reference_chain": "A", 
                              "total_contacts": 1063080, 
                              "error": "", 
                              "model_chain": "B"
                          }, 
                          {
                              "status": "SUCCESS", 
                              "global_score": 0.7854443788528442, 
                              "conserved_contacts": 904568, 
                              "reference_chain": "B", 
                              "total_contacts": 1151664, 
                              "error": "", 
                              "model_chain": "A"
                          }
                      ]
                  }, 
                  "qs_score": {
                      "status": "SUCCESS", 
                      "global_score": 0.8974384796108209, 
                      "best_score": 0.9022811630070536, 
                      "error": ""
                  }
              }
          }
      }, 
      "options": {
          "reference": "reference.pdb", 
          "structural_checks": true, 
          "chain_mapping": null, 
          "bond_tolerance": 15.0, 
          "parameter_file": "Path to stage/share/openstructure/stereo_chemical_props.txt", 
          "consistency_checks": true, 
          "qs_score": true, 
          "map_nonstandard_residues": true, 
          "save_per_residue_scores": false, 
          "fault_tolerant": false, 
          "reference_selection": "", 
          "cwd": "CWD", 
          "inclusion_radius": 15.0, 
          "angle_tolerance": 15.0, 
          "c_alpha_only": false, 
          "clean_element_column": true, 
          "dump_suffix": ".compare.structures.pdb", 
          "compound_library": "Path to stage/share/openstructure/compounds.chemlib", 
          "dump_structures": false, 
          "residue_number_alignment": true, 
          "verbosity": 3, 
          "remove": [
              "oxt", 
              "hyd", 
              "unk"
          ], 
          "molck": true, 
          "sequence_separation": 0, 
          "output": "output.json", 
          "model": "model.pdb", 
          "lddt": true, 
          "model_selection": ""
      }
  }

If only all the structures are clean one can omit all the checking steps and
calculate eg. QS-score directly:

.. code:: console

  $OST_ROOT/bin/ost compare-structures --model model.pdb --reference reference.pdb --output output_qs.json --qs-score --residue-number-alignment
  ################################################################################
  Reading input files (fault_tolerant=False)
   --> reading model from model.pdb
  imported 2 chains, 396 residues, 3106 atoms; with 0 helices and 0 strands
   --> reading reference from reference.pdb
  imported 3 chains, 408 residues, 3011 atoms; with 0 helices and 0 strands
  ################################################################################
  Comparing model.pdb to reference.pdb
  Chains removed from reference.pdb: _
  Chains in reference.pdb: AB
  Chains in model.pdb: AB
  Chemically equivalent chain-groups in reference.pdb: [['B', 'A']]
  Chemically equivalent chain-groups in model.pdb: [['A', 'B']]
  Chemical chain-groups mapping: {('B', 'A'): ('A', 'B')}
  Identifying Symmetry Groups...
  Symmetry threshold 0.1 used for angles of reference.pdb
  Symmetry threshold 0.1 used for axis of reference.pdb
  Symmetry threshold 0.1 used for angles of model.pdb
  Symmetry threshold 0.1 used for axis of model.pdb
  Selecting Symmetry Groups...
  Symmetry-groups used in reference.pdb: [('B',), ('A',)]
  Symmetry-groups used in model.pdb: [('A',), ('B',)]
  Closed Symmetry with strict parameters
  Mapping found: {'A': 'B', 'B': 'A'}
  --------------------------------------------------------------------------------
  Checking consistency between model.pdb and reference.pdb
  Consistency check: OK
  --------------------------------------------------------------------------------
  Computing QS-score
  QSscore reference.pdb, model.pdb: best: 0.90, global: 0.90
  ################################################################################
  Saving output into output_qs.json

