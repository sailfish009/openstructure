:mod:`~ost.bindings.hhblits` - Search related sequences in databases
================================================================================

.. module:: ost.bindings.hhblits
  :synopsis: Search related sequences in databases

Introduction
--------------------------------------------------------------------------------

HHblits is a sequence search tool like BLAST, able to find more distant
homologs This is achieved by performing `profile-profile searches`. In BLAST, a
\query sequence is searched against a sequence database. That is a
`sequence-sequence search`. HHblits works on a profile database, usually that
one is provided, queried with a sequence profile. The latter one needs to be
calculated before the actual search. In very simple words, HHblits is using
per-sequence scoring functions to be more sensitive, in this particular case
Hidden Markov models. The software suite needed for HHblits can be found
`here <http://toolkit.tuebingen.mpg.de/hhblits>`_.


Examples
--------------------------------------------------------------------------------

A typical search: Get an instance of the bindings, build the search profile out
of the query sequence, run the search and iterate results. Since
:class:`~HHblits` works with a :class:`~ost.seq.SequenceHandle` or a FastA
file, both ways are shown.

First query by sequence:

.. code-block:: python

  from ost.bindings import hhblits

  # get a SequenceHandle
  query_seq = seq.CreateSequence('Query',
                                 'MRIILLGAPGAGKGTQAQFIMEKYGIPQISTGDMLRAAVKSGS'+
                                 'ELGKQAKDIMDAGKLVTDELVIALVKERIAQEDCRNGFLLDGF'+
                                 'PRTIPQADAMKEAGINVDYVLEFDVPDELIVDRIVGRRVHAPS'+
                                 'GRVYHVKFNPPKVEGKDDVTGEELTTRKDDQEETVRKRLVEYH'+
                                 'QMTAPLIGYYYYSKEAEAGNTKYAKVDGTKPVAEVRADLEKILG')

  # set up the search environment
  # $EBROOTHHMINSUITE points to the root of a HHsuite installation
  hh = hhblits.HHblits(query_seq, os.getenv('EBROOTHHMINSUITE'))

  # now create a search profile for the query sequence against the NR20 db
  # provided on the HHblits web page, nr20_12Aug11 is just the prefix common to
  # all db files, so `ls hhtools/nr20_12Aug11*` would list all of them
  a3m_file = hh.BuildQueryMSA(nrdb='hhtools/nr20_12Aug11')

  # search time! we just search against NR20 again, but every HHblits db is
  # working here, e.g. one build from all the sequences in PDB
  hit_file = hh.Search(a3m_file, 'hhtools/nr20_12Aug11')

  # lets have a look at the resuls
  with open(hit_file) as hit_fh:
      header, hits = hhblits.ParseHHblitsOutput(hit_fh)
  for hit in hits:
      print hit.aln

Very similar going by file:

.. code-block:: python

  from ost.bindings import hhblits

  # set up the search environment
  #  $EBROOTHHMINSUITE points to the root of a HHsuite installation
  hh = hhblits.HHblits('query.fas', os.getenv('EBROOTHHMINSUITE'))

  # now create a search profile for the query sequence against the NR20 db
  # provided on the HHblits web page, nr20_12Aug11 is just the prefix common to
  # all db files, so `ls hhtools/nr20_12Aug11*` would list all of them
  a3m_file = hh.BuildQueryMSA(nrdb='hhtools/nr20_12Aug11')

  # search time! we just search against NR20 again, but every HHblits db is
  # working here, e.g. one build from all the sequences in PDB
  hit_file = hh.Search(a3m_file, 'hhtools/nr20_12Aug11')

  # lets have a look at the resuls
  with open(hit_file) as hit_fh:
      header, hits = hhblits.ParseHHblitsOutput(hit_fh)
  for hit in hits:
      print hit.aln

The alignments produced by HHblits are sometimes slightly better than by BLAST,
so one may want to extract them:

.. code-block:: python

  from ost.bindings import hhblits

  # set up the search environment
  #  $EBROOTHHMINSUITE points to the root of a HHsuite installation
  hh = hhblits.HHblits('query.fas', os.getenv('EBROOTHHMINSUITE'))

  # now create a search profile for the query sequence against the NR20 db
  # provided on the HHblits web page, nr20_12Aug11 is just the prefix common to
  # all db files, so `ls hhtools/nr20_12Aug11*` would list all of them
  a3m_file = hh.BuildQueryMSA(nrdb='hhtools/nr20_12Aug11')

  # note that ParseA3M is not a class method but a module function
  output = hhblits.ParseA3M(open(a3m_file))

  print output['msa']


Binding API
--------------------------------------------------------------------------------

.. autoclass:: ost.bindings.hhblits.HHblits
   :members:

.. autoclass:: ost.bindings.hhblits.HHblitsHit

.. autoclass:: ost.bindings.hhblits.HHblitsHeader

.. autofunction:: ost.bindings.hhblits.ParseHHblitsOutput

.. autofunction:: ost.bindings.hhblits.ParseA3M

.. autofunction:: ost.bindings.hhblits.ParseHeaderLine

.. autofunction:: ost.bindings.hhblits.ParseHHM

.. autofunction:: ost.bindings.hhblits.EstimateMemConsumption
..  LocalWords:  HHblits homologs
