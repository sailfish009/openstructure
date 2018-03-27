.. ost-actions:

OST Actions
================================================================================

A pure command line interface of OST is provided by actions.
You can execute ``ost -h`` for a list of possible actions and for every action,
you can type ``ost <ACTION> -h`` to get a description on its usage.

Here we list the most prominent actions with simple examples.

.. ost-qs-score:

Calculating QS-Score
--------------------------------------------------------------------------------

You can calculate quaternary structure score between two complexes
from the command line with

.. code-block:: console

  $ ost qs-score [-h] [-v VERBOSITY] -m MODEL -r REFERENCE
                 [-c CHAIN_MAPPING [CHAIN_MAPPING ...]] [-o OUTPUT]

By default the verbosity is set to 3 which will result in the informations
being shown in the console. The result can be (optionally) saved as JSON file
which is the preferred way of parsing it as the log output might change in the
future. The output file has following format:

.. code-block:: python

  {
      "result": [
          {
              "status": <SUCCESS or FAILURE>,
              "reference_name": <Reference name extracted from the file name>, 
              "model_name": <Model name extracted from the file name>, 
              "error": <ERROR message if any>, 
              "best_score": <Best QS-score>, 
              "lddt_score": <lDDT score>
              "global_score": <Global QS-score>, 
              "chain_mapping": <Mapping of chains eg. {"A": "B", "B": "A"}>, 
          }
      ]
  }

The "result" filed is a list as eg. in mmCIF file there can be many entities and
the script will compare all combinations.

Example usage:

.. code-block:: console

  $ curl https://www.cameo3d.org/static/data/modeling/2018.03.03/5X7J_B/bu_target_01.pdb > reference.pdb
  $ curl https://www.cameo3d.org/static/data/modeling/2018.03.03/5X7J_B/servers/server11/oligo_model-1/superposed_oligo_model-1.pdb > model.pdb
  $ ost qs-score -r reference.pdb -m model.pdb -o qs.json

  Reading model from model.pdb
  imported 2 chains, 396 residues, 3106 atoms; with 0 helices and 0 strands
  Reading reference from reference.pdb
  imported 3 chains, 408 residues, 3011 atoms; with 0 helices and 0 strands
  #
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
  QSscore reference.pdb, model.pdb: best: 0.90, global: 0.86
  Computing lDDT score
  Reference reference.pdb has: 2 chains
  Model model.pdb has: 2 chains
  Reference reference.pdb has: 384 residues
  Model model.pdb has: 396 residues
  lDDT score: 0.924

This reads the model and reference file and calculates QS-score between them.
In the example above the output file looks as follows:

.. code-block:: python

  {
      "result": [
          {
              "status": "SUCCESS", 
              "chain_mapping": {
                  "A": "B", 
                  "B": "A"
              }, 
              "global_score": 0.8649009937090414, 
              "error": "", 
              "reference_name": "reference.pdb", 
              "best_score": 0.8979578363524581, 
              "model_name": "model.pdb", 
              "lddt_score": 0.924271821975708
          }
      ]
  }

