..  Note on large code blocks: keep max. width to 120 or it will look bad
                               on webpage!
..  TODO: look at argparse directive to autogenerate --help output!

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
lDDT scores between two complexes from the command line with the
``ost compare-structures`` action.

In summary it performs the following steps:

- Read structures (PDB or mmCIF format, can be gzipped) and split into
  biological assemblies (all possible pairs are scored).
- Optional cleanup of structures with :func:`~ost.mol.alg.Molck`.
- Optional structural checks with :func:`~ost.mol.alg.CheckStructure`.
- Unless user-provided, find chain mapping between complexes (see
  :attr:`here <ost.mol.alg.qsscoring.QSscorer.chain_mapping>` for details)
- Perform sequence alignment of chain pairs (unless user asks for alignment
  based on residue numbers). Alignment can optionally checked for consistency
  if 100% sequence identity is expected.
- Compute scores requested by user (CA-RMSD of oligomer,
  :mod:`QS scores of oligomer <ost.mol.alg.qsscoring>`,
  :class:`single chain lDDT scores <ost.mol.alg.lDDTScorer>`,
  :attr:`weighted average of single chain lDDT scores <ost.mol.alg.qsscoring.OligoLDDTScorer.weighted_lddt>`,
  :attr:`lDDT score of oligomer <ost.mol.alg.qsscoring.OligoLDDTScorer.oligo_lddt>`).

.. note ::

  The action relies on OST's :mod:`~ost.mol.alg.qsscoring` module and has the
  same requirements on your OST installation (needs compound library, ClustalW,
  numpy and scipy).

Details on the usage (output of ``ost compare-structures --help``):

.. code-block:: console

  usage: ost compare-structures [-h] -m MODEL -r REFERENCE [-v VERBOSITY]
                                [-o OUTPUT] [-d] [-ds DUMP_SUFFIX]
                                [-rs REFERENCE_SELECTION] [-ms MODEL_SELECTION]
                                [-ca] [-ft] [-cl COMPOUND_LIBRARY] [-ml]
                                [-rm REMOVE [REMOVE ...]] [-ce] [-mn] [-sc]
                                [-p PARAMETER_FILE] [-bt BOND_TOLERANCE]
                                [-at ANGLE_TOLERANCE]
                                [-c CHAIN_MAPPING [CHAIN_MAPPING ...]]
                                [--qs-max-mappings-extensive QS_MAX_MAPPINGS_EXTENSIVE]
                                [-cc] [-rna] [-qs] [--qs-rmsd] [-l]
                                [-ir INCLUSION_RADIUS] [-ss SEQUENCE_SEPARATION]
                                [-spr]

  Evaluate model structure against reference.

  eg.

    ost compare-structures \
        --model <MODEL> \
        --reference <REF> \
        --output output.json \
        --lddt \
        --structural-checks \
        --consistency-checks \
        --molck \
        --remove oxt hyd \
        --map-nonstandard-residues

  Here we describe how the parameters can be set to mimick a CAMEO evaluation
  (as of August 2018).

  CAMEO calls the lddt binary as follows:

    lddt \
        -p <PARAMETER FILE> \
        -f \
        -a 15 \
        -b 15 \
        -r 15 \
        <MODEL> \
        <REF>

  Only model structures are "Molck-ed" in CAMEO. The call to molck is as follows:

    molck \
        --complib=<COMPOUND LIB> \
        --rm=hyd,oxt,unk \
        --fix-ele \
        --map-nonstd \
        --out=<OUTPUT> \
        <FILEPATH>

  To be as much compatible with with CAMEO as possible one should call
  compare-structures as follows:

    ost compare-structures \
        --model <MODEL> \
        --reference <REF> \
        --output output.json \
        --molck \
        --remove oxt hyd unk \
        --clean-element-column \
        --map-nonstandard-residues \
        --structural-checks \
        --bond-tolerance 15.0 \
        --angle-tolerance 15.0 \
        --residue-number-alignment \
        --consistency-checks \
        --qs-score \
        --lddt \
        --inclusion-radius 15.0

  optional arguments:
    -h, --help            show this help message and exit

  required arguments:
    -m MODEL, --model MODEL
                          Path to the model file.
    -r REFERENCE, --reference REFERENCE
                          Path to the reference file.

  general arguments:
    -v VERBOSITY, --verbosity VERBOSITY
                          Set verbosity level. Defaults to 3.
    -o OUTPUT, --output OUTPUT
                          Output file name. The output will be saved as a JSON file.
    -d, --dump-structures
                          Dump cleaned structures used to calculate all the scores as
                          PDB files using specified suffix. Files will be dumped to the
                          same location as original files.
    -ds DUMP_SUFFIX, --dump-suffix DUMP_SUFFIX
                          Use this suffix to dump structures.
                          Defaults to .compare.structures.pdb.
    -rs REFERENCE_SELECTION, --reference-selection REFERENCE_SELECTION
                          Selection performed on reference structures.
    -ms MODEL_SELECTION, --model-selection MODEL_SELECTION
                          Selection performed on model structures.
    -ca, --c-alpha-only   Use C-alpha atoms only. Equivalent of calling the action with
                          '--model-selection="aname=CA" --reference-selection="aname=CA"'
                          options.
    -ft, --fault-tolerant
                          Fault tolerant parsing.
    -cl COMPOUND_LIBRARY, --compound-library COMPOUND_LIBRARY
                          Location of the compound library file (compounds.chemlib).
                          If not provided, the following locations are searched in this
                          order: 1. Working directory, 2. OpenStructure standard library
                          location.

  molecular check arguments:
    -ml, --molck          Run molecular checker to clean up input.
    -rm REMOVE [REMOVE ...], --remove REMOVE [REMOVE ...]
                          Remove atoms and residues matching some criteria:
                           * zeroocc - Remove atoms with zero occupancy
                           * hyd - remove hydrogen atoms
                           * oxt - remove terminal oxygens
                           * nonstd - remove all residues not one of the 20
                           * standard amino acids
                           * unk - Remove unknown and atoms not following the
                                   nomenclature
                          Defaults to hyd.
    -ce, --clean-element-column
                          Clean up element column
    -mn, --map-nonstandard-residues
                          Map modified residues back to the parent amino acid, for
                          example MSE -> MET, SEP -> SER.

  structural check arguments:
    -sc, --structural-checks
                          Perform structural checks and filter input data.
    -p PARAMETER_FILE, --parameter-file PARAMETER_FILE
                          Location of the stereochemical parameter file
                          (stereo_chemical_props.txt).
                          If not provided, the following locations are searched in this
                          order: 1. Working directory, 2. OpenStructure standard library
                          location.
    -bt BOND_TOLERANCE, --bond-tolerance BOND_TOLERANCE
                          Tolerance in STD for bonds. Defaults to 12.
    -at ANGLE_TOLERANCE, --angle-tolerance ANGLE_TOLERANCE
                          Tolerance in STD for angles. Defaults to 12.

  chain mapping arguments:
    -c CHAIN_MAPPING [CHAIN_MAPPING ...], --chain-mapping CHAIN_MAPPING [CHAIN_MAPPING ...]
                          Mapping of chains between the reference and the model.
                          Each separate mapping consist of key:value pairs where key
                          is the chain name in reference and value is the chain name in
                          model.
    --qs-max-mappings-extensive QS_MAX_MAPPINGS_EXTENSIVE
                          Maximal number of chain mappings to test for 'extensive'
                          chain mapping scheme which is used as a last resort if
                          other schemes failed. The extensive chain mapping search
                          must in the worst case check O(N!) possible mappings for
                          complexes with N chains. Two octamers without symmetry
                          would require 322560 mappings to be checked. To limit
                          computations, no scores are computed if we try more than
                          the maximal number of chain mappings. Defaults to 1000000.

  sequence alignment arguments:
    -cc, --consistency-checks
                          Take consistency checks into account. By default residue name
                          consistency between a model-reference pair would be checked
                          but only a warning message will be displayed and the script
                          will continue to calculate scores. If this flag is ON, checks
                          will not be ignored and if the pair does not pass the test
                          all the scores for that pair will be marked as a FAILURE.
    -rna, --residue-number-alignment
                          Make alignment based on residue number instead of using
                          a global BLOSUM62-based alignment.

  QS score arguments:
    -qs, --qs-score       Calculate QS-score.
    --qs-rmsd             Calculate CA RMSD between shared CA atoms of mapped chains.
                          This uses a superposition using all mapped chains which
                          minimizes the CA RMSD.

  lDDT score arguments:
    -l, --lddt            Calculate lDDT.
    -ir INCLUSION_RADIUS, --inclusion-radius INCLUSION_RADIUS
                          Distance inclusion radius for lDDT. Defaults to 15 A.
    -ss SEQUENCE_SEPARATION, --sequence-separation SEQUENCE_SEPARATION
                          Sequence separation. Only distances between residues whose
                          separation is higher than the provided parameter are
                          considered when computing the score. Defaults to 0.
    -spr, --save-per-residue-scores


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
                      "mapping": {
                          "chain_mapping": <Mapping of chains eg. {"A": "B", "B": "A"}>,
                          "chain_mapping_scheme": <Scheme used to get mapping, check mapping manually
                                                   if "permissive" or "extensive">,
                          "alignments": <list of chain-chain alignments in FASTA format>
                      }
                  }, 
                  "lddt": {
                      # calculated when --lddt (-l) option is selected
                      "oligo_lddt": {
                          "status": <SUCCESS or FAILURE>,
                          "error": <ERROR message if any>, 
                          "global_score": <calculated oligomeric lDDT score>
                      }, 
                      "weighted_lddt": {
                          "status": <SUCCESS or FAILURE>,
                          "error": <ERROR message if any>, 
                          "global_score": <calculated weighted lDDT score>
                      }, 
                      "single_chain_lddt": [
                          # a list of chain-chain lDDTs
                          {
                              "status": <SUCCESS or FAILURE>,
                              "error": <ERROR message if any>, 
                              "reference_chain": <name of the chain in reference>, 
                              "model_chain": <name of the chain in model>
                              "global_score": <calculated single-chain lDDT score>, 
                              "conserved_contacts": <number of conserved contacts between model and reference>,
                              "total_contacts": <total number of contacts in reference>,
                              "per_residue_scores": [
                                  # per-residue lDDT scores
                                  # only calculated when --save-per-residue-scores (-spr) option is selected
                                  {
                                      "residue_name": <three letter code of the residue in reference chain>,
                                      "residue_number": <residue number in reference chain>,
                                      "lddt": <residue lDDT score>,
                                      "conserved_contacts": <conserved_contacts for given residue>,
                                      "total_contacts": <total_contacts for given residue>
                                  },
                                  .
                                  .
                                  .
                              ]
                          }
                      ]
                  },
                  "qs_score": {
                    # calculated when --qs-score (-q) option is selected
                    "status": <SUCCESS or FAILURE>,
                    "error": <ERROR message if any>,
                    "global_score": <Global QS-score>,
                    "best_score": <Best QS-score>
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

  $ CAMEO_TARGET_URL=https://www.cameo3d.org/static/data/modeling/2018.03.03/5X7J_B
  $ curl $CAMEO_TARGET_URL/bu_target_01.pdb > reference.pdb
  $ curl $CAMEO_TARGET_URL/servers/server11/oligo_model-1/superposed_oligo_model-1.pdb > model.pdb
  $ $OST_ROOT/bin/ost compare-structures \
        --model model.pdb --reference reference.pdb --output output.json \
        --qs-score --residue-number-alignment --lddt --structural-checks \
        --consistency-checks --inclusion-radius 15.0 --bond-tolerance 15.0 \
        --angle-tolerance 15.0 --molck --remove oxt hyd unk \
        --clean-element-column --map-nonstandard-residues

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
  Checking reference.pdb
  Checking stereo-chemistry
  Average Z-Score for bond lengths: 0.13694
  Bonds outside of tolerance range: 0 out of 2654
  Bond  Avg Length  Avg zscore  Num Bonds
  C-C 1.50876     0.09299     1501
  C-N 1.42978     0.17690     635
  C-O 1.25079     0.21528     518
  Average Z-Score angle widths: 0.07562
  Angles outside of tolerance range: 0 out of 2941
  Filtering non-bonded clashes
  0 non-bonded short-range distances shorter than tolerance distance
  Distances shorter than tolerance are on average shorter by: 0.00000
   --> for model(s)
  Checking model.pdb
  Checking stereo-chemistry
  Average Z-Score for bond lengths: -0.22524
  Bonds outside of tolerance range: 0 out of 2774
  Bond  Avg Length  Avg zscore  Num Bonds
  C-C 1.50225     -0.20158    1558
  C-N 1.42294     -0.12261    666
  C-O 1.24232     -0.42115    546
  C-S 1.80215     0.20858     4
  Average Z-Score angle widths: -0.06767
  Angles outside of tolerance range: 0 out of 3079
  Filtering non-bonded clashes
  0 non-bonded short-range distances shorter than tolerance distance
  Distances shorter than tolerance are on average shorter by: 0.00000
  ################################################################################
  Comparing model.pdb to reference.pdb
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
  Reference reference.pdb has: 2 chains
  Model model.pdb has: 2 chains
  Coverage: 1 (384 out of 384 residues)
  Oligo lDDT score: 0.8025
   --> Computing weighted lDDT score
  Weighted lDDT score: 0.8048
  ################################################################################
  Saving output into output.json

This reads the model and reference file and calculates QS- and lDDT-scores
between them. In the example above the output file looks as follows (FASTA
alignments were cut in display here for readability):

..  code snippet to fix output.json generated above
  import json
  json_data = json.load(open("output.json"))
  mapping = json_data["result"]["model.pdb"]["reference.pdb"]["info"]["mapping"]
  new_alns = list()
  for aln in mapping["alignments"]:
    aln_lines = aln.splitlines()
    aln_lines[1] = aln_lines[1][:20] + "..."
    aln_lines[3] = aln_lines[3][:20] + "..."
    new_alns.append("\n".join(aln_lines))
  mapping["alignments"] = new_alns
  json_data["options"]["parameter_file"] = "Path to stage/share/openstructure/stereo_chemical_props.txt"
  json_data["options"]["compound_library"] = "Path to stage/share/openstructure/compounds.chemlib"
  with open("output_fixed.json", "w") as outfile:
    json.dump(json_data, outfile, indent=4, sort_keys=True)

.. code-block:: json

  {
      "options": {
          "angle_tolerance": 15.0, 
          "bond_tolerance": 15.0, 
          "c_alpha_only": false, 
          "chain_mapping": null, 
          "clean_element_column": true, 
          "compound_library": "Path to stage/share/openstructure/compounds.chemlib", 
          "consistency_checks": true, 
          "cwd": "/home/taurielg/GT/Code/ost/build", 
          "dump_structures": false, 
          "dump_suffix": ".compare.structures.pdb", 
          "fault_tolerant": false, 
          "inclusion_radius": 15.0, 
          "lddt": true, 
          "map_nonstandard_residues": true, 
          "model": "model.pdb", 
          "model_selection": "", 
          "molck": true, 
          "output": "output.json", 
          "parameter_file": "Path to stage/share/openstructure/stereo_chemical_props.txt", 
          "qs_max_mappings_extensive": 1000000, 
          "qs_rmsd": false, 
          "qs_score": true, 
          "reference": "reference.pdb", 
          "reference_selection": "", 
          "remove": [
              "oxt", 
              "hyd", 
              "unk"
          ], 
          "residue_number_alignment": true, 
          "save_per_residue_scores": false, 
          "sequence_separation": 0, 
          "structural_checks": true, 
          "verbosity": 3
      }, 
      "result": {
          "model.pdb": {
              "reference.pdb": {
                  "info": {
                      "mapping": {
                          "alignments": [
                              ">reference:A\n-PGLFLTLEGLDGSGKTTQA...\n>model:B\nMPGLFLTLEGLDGSGKTTQA...", 
                              ">reference:B\n-PGLFLTLEGLDGSGKTTQA...\n>model:A\nMPGLFLTLEGLDGSGKTTQA..."
                          ], 
                          "chain_mapping": {
                              "A": "B", 
                              "B": "A"
                          }, 
                          "chain_mapping_scheme": "strict"
                      }, 
                      "residue_names_consistent": true
                  }, 
                  "lddt": {
                      "oligo_lddt": {
                          "error": "", 
                          "global_score": 0.8025223275721413, 
                          "status": "SUCCESS"
                      }, 
                      "single_chain_lddt": [
                          {
                              "conserved_contacts": 877834, 
                              "error": "", 
                              "global_score": 0.8257459402084351, 
                              "model_chain": "B", 
                              "reference_chain": "A", 
                              "status": "SUCCESS", 
                              "total_contacts": 1063080
                          }, 
                          {
                              "conserved_contacts": 904568, 
                              "error": "", 
                              "global_score": 0.7854443788528442, 
                              "model_chain": "A", 
                              "reference_chain": "B", 
                              "status": "SUCCESS", 
                              "total_contacts": 1151664
                          }
                      ], 
                      "weighted_lddt": {
                          "error": "", 
                          "global_score": 0.804789180710712, 
                          "status": "SUCCESS"
                      }
                  }, 
                  "qs_score": {
                      "best_score": 0.9022811630070536, 
                      "error": "", 
                      "global_score": 0.8974384796108209, 
                      "status": "SUCCESS"
                  }
              }
          }
      }
  }

If all the structures are clean and have matching residue numbers, one can omit
all the checking steps and calculate scores directly as here:

.. code:: console

  $ $OST_ROOT/bin/ost compare-structures \
        --model model.pdb --reference reference.pdb --output output_qs.json \
        --qs-score --residue-number-alignment

  ################################################################################
  Reading input files (fault_tolerant=False)
   --> reading model from model.pdb
  imported 2 chains, 396 residues, 3106 atoms; with 0 helices and 0 strands
   --> reading reference from reference.pdb
  imported 3 chains, 408 residues, 3011 atoms; with 0 helices and 0 strands
  ################################################################################
  Comparing model.pdb to reference.pdb
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
