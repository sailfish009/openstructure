mmCIF File Format
--------------------------------------------------------------------------------

The mmCIF file format is an alternate container for structural entities, also
provided by the PDB. Here we describe how to load those files and how to deal
with information provided above the common PDB format (:class:`MMCifInfo`,
:class:`MMCifInfoCitation`, :class:`MMCifInfoTransOp`,
:class:`MMCifInfoBioUnit`, :class:`MMCifInfoStructDetails`).


Loading mmCIF Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autofunction:: ost.io.LoadMMCIF


Categories Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following categories of a mmCIF file are considered by the reader:

* ``atom_site``: Used to build the :class:`entity <ost.mol.EntityHandle>`
* ``entity``: Involved in setting ChainTypes
* ``entity_poly``: Involved in setting ChainTypes
* ``citation``: Goes into :class:`MMCifInfoCitation`
* ``citation_author``: Goes into :class:`MMCifInfoCitation`
* ``exptl``: Goes into :class:`MMCifInfo` as :attr:`method <MMCifInfo.method>`.
* ``refine``: Goes into :class:`MMCifInfo` as
  :attr:`resolution <MMCifInfo.resolution>`.
* ``pdbx_struct_assembly``: Used for :class:`MMCifInfoBioUnit`.
* ``pdbx_struct_assembly_gen``: Used for :class:`MMCifInfoBioUnit`.
* ``pdbx_struct_oper_list``: Used for :class:`MMCifInfoBioUnit`.
* ``struct``: Details about a structure, stored in
  :class:`MMCifInfoStructDetails`.
* ``struct_conf``: Stores secondary structure information (practically helices)
  in the :class:`entity <ost.mol.EntityHandle>`
* ``struct_sheet_range``: Stores secondary structure information for sheets in
  the :class:`entity <ost.mol.EntityHandle>`
* ``pdbx_database_PDB_obs_spr``: Verbose information on obsoleted/ superseded
  entries, stored in :class:`MMCifInfoObsolete`.
* ``struct_ref`` stored in :class:`MMCifInfoStructRef`
* ``struct_ref_seq`` stored in :class:`MMCifInfoStructRefSeq`
* ``struct_ref_seq_dif`` stored in :class:`MMCifInfoStructRefDif`

Info Classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Information from mmCIF files which goes beyond structural data, is kept in a
special container, the :class:`MMCifInfo` class. Here is a detailed description
of the annotation available.

.. class:: MMCifInfo

  This is the container for all bits of non-molecular data pulled from a mmCIF
  file.

  .. attribute:: citations

    Stores a list of citations (:class:`MMCifInfoCitation`).

    Also available as :meth:`GetCitations`.

  .. attribute:: biounits

    Stores a list of biounits (:class:`MMCifInfoBioUnit`).

    Also available as :meth:`GetBioUnits`.

  .. attribute:: method

    Stores the experimental method used to create the structure.

    Also available as :meth:`GetMethod`. May also be modified by
    :meth:`SetMethod`.

  .. attribute:: resolution

    Stores the resolution of the crystal structure.

    Also available as :meth:`GetResolution`. May also be modified by
    :meth:`SetResolution`.

  .. attribute:: operations

    Stores the operations needed to transform a crystal structure into a
    bio unit.

    Also available as :meth:`GetOperations`. May also be modified by
    :meth:`AddOperation`.

  .. attribute:: struct_details

    Stores details about the structure in a :class:`MMCifInfoStructDetails`
    object.

    Also available as :meth:`GetStructDetails`. May also be modified by
    :meth:`SetStructDetails`.

  .. attribute:: struct_refs

    Lists all links to external databases in the mmCIF file.
  .. method:: AddCitation(citation)

    Add a citation to the citation list of an info object.

    :param citation: Citation to be added.
    :type citation: :class:`MMCifInfoCitation`

  .. method:: AddAuthorsToCitation(id, authors)

    Adds a list of authors to a specific citation.

    :param id: identifier of the citation
    :type id: :class:`str`
    :param authors: List of authors.
    :type authors: :class:`~ost.StringList`

  .. method:: GetCitations()

    See :attr:`citations`

  .. method:: AddBioUnit(biounit)

    Add a bio unit to the bio unit list of an info object.

    :param biounit: Bio unit to be added.
    :type biounit: :class:`MMCifInfoBioUnit`

  .. method:: GetBioUnits()

    See :attr:`biounits`

  .. method:: SetMethod(method)

    See :attr:`method`

  .. method:: GetMethod()

    See :attr:`method`

  .. method:: SetResolution(resolution)

    See :attr:`resolution`

  .. method:: GetResolution()

    See :attr:`resolution`

  .. method:: AddOperation(operation)

    See :attr:`operations`

  .. method:: GetOperations()

    See :attr:`operations`

  .. method:: SetStructDetails(details)

    See :attr:`struct_details`

  .. method:: GetStructDetails()

.. class:: MMCifInfoCitation

  This stores citation information from an input file.

  .. attribute:: id

    Stores an internal identifier for a citation. If not provided, resembles an
    empty string.

    Also available as :meth:`GetID`. May also be modified by :meth:`SetID`.

  .. attribute:: cas

    Stores a Chemical Abstract Service identifier, if available. If not
    provided, resembles an empty string.

    Also available as :meth:`GetCAS`. May also be modified by :meth:`SetCas`.

  .. attribute:: isbn

    Stores the ISBN code, presumably for cited books.  If not
    provided, resembles an empty string.

    Also available as :meth:`GetISBN`. May also be modified by :meth:`SetISBN`.

  .. attribute:: published_in

    Stores the book or journal title of a publication. Should take the full
    title, no abbreviations. If not provided, resembles an empty string.

    Also available as :meth:`GetPublishedIn`. May also be modified by
    :meth:`SetPublishedIn`.

  .. attribute:: volume

    Supposed to store volume information for journals. Since the volume number
    is not always a simple integer, it is stored as a string. If not provided,
    resembles an empty string.

    Also available as :meth:`GetVolume`. May also be modified by
    :meth:`SetVolume`.

  .. attribute:: page_first

    Stores the first page of a publication. Since the page numbers are not
    always a simple integers, they are stored as strings. If not provided,
    resembles empty strings.

    Also available as :meth:`GetPageFirst`. May also be modified by
    :meth:`SetPageFirst`.

  .. attribute:: page_last

    Stores the last page of a publication. Since the page numbers are not
    always a simple integers, they are stored as strings. If not provided,
    resembles empty strings.

    Also available as :meth:`GetPageLast`. May also be modified by
    :meth:`SetPageLast`.

  .. attribute:: doi

    Stores the Document Object Identifier as used by doi.org for a cited
    document. If not provided, resembles a empty strings.

    Also available as :meth:`GetDOI`. May also be modified by :meth:`SetDOI`.

  .. attribute:: pubmed

    Stores the PubMed accession number. If not provided, is set to 0.

    Also available as :meth:`GetPubMed`. May also be modified by
    :meth:`SetPubmed`.

  .. attribute:: year

    Stores the publication year. If not provided, is set to 0.

    Also available as :meth:`GetYear`. May also be modified by :meth:`SetYear`.

  .. attribute:: title

    Stores a title. If not provided, is set to an empty string.

    Also available as :meth:`GetTitle`. May also be modified by
    :meth:`SetTitle`.

  .. attribute:: authors

    Stores a :class:`~ost.StringList` of authors.

    Also available as :meth:`GetAuthorList`. May also be modified by
    :meth:`SetAuthorList`.

  .. method:: GetCAS()
    
    See :attr:`cas`

  .. method:: SetCAS(cas)

    See :attr:`cas`

  .. method:: GetISBN()
    
    See :attr:`isbn`

  .. method:: SetISBN(isbn)

    See :attr:`isbn`

  .. method:: GetPublishedIn()
    
    See :attr:`published_in`

  .. method:: SetPublishedIn(title)

    See :attr:`published_in`

  .. method:: GetVolume()
    
    See :attr:`volume`

  .. method:: SetVolume(volume)

    See :attr:`volume`

  .. method:: GetPageFirst()
    
    See :attr:`page_first`

  .. method:: SetPageFirst(first)

    See :attr:`page_first`

  .. method:: GetPageLast()
    
    See :attr:`page_last`

  .. method:: SetPageLast(last)

    See :attr:`page_last`

  .. method:: GetDOI()
    
    See :attr:`doi`

  .. method:: SetDOI(doi)

    See :attr:`doi`

  .. method:: GetPubMed()
    
    See :attr:`pubmed`

  .. method:: SetPubMed(no)

    See :attr:`pubmed`

  .. method:: GetYear()
    
    See :attr:`year`

  .. method:: SetYear(year)

    See :attr:`year`

  .. method:: GetTitle()
    
    See :attr:`title`

  .. method:: SetTitle(title)

    See :attr:`title`

  .. method:: GetAuthorList()

    See :attr:`authors`

  .. method:: SetAuthorList(list)

    See :attr:`authors`


.. class:: MMCifInfoTransOp

  This stores operations needed to transform an
  :class:`entity <ost.mol.EntityHandle>` into a bio unit.

  .. attribute:: id

    A unique identifier. If not provided, resembles an empty string.

    Also available as :meth:`GetID`. May also be modified by
    :meth:`SetID`.

  .. attribute:: type

    Describes the operation. If not provided, resembles an empty string.

    Also available as :meth:`GetType`. May also be modified by
    :meth:`SetType`.

  .. attribute:: translation

    The translational vector. Also available as :meth:`GetVector`. May also be

    modified by :meth:`SetVector`.

  .. attribute:: rotation

    The rotational matrix. Also available as :meth:`GetMatrix`. May also be

    modified by :meth:`SetMatrix`.

  .. method:: GetID()

    See :attr:`id`

  .. method:: SetID(id)

    See :attr:`id`

  .. method:: GetType()

    See :attr:`type`

  .. method:: SetType(type)

    See :attr:`type`

  .. method:: GetVector()

    See :attr:`translation`

  .. method:: SetVector(x, y, z)

    See :attr:`translation`

  .. method:: GetMatrix()

    See :attr:`rotation`

  .. method:: SetMatrix(i00,i01, i02, i10,i11, i12, i20,i21, i22)

    See :attr:`rotation`

.. class:: MMCifInfoBioUnit

  This stores information how a structure is to be assembled to form the
  bio unit.

  .. attribute:: id

    The id of a bio unit as given by the original mmCIF file.

    Also available as :meth:`GetID`. May also be modified by :meth:`SetID`.

    :type: :class:`str`

  .. attribute:: details

    Special aspects of the biological assembly. If not provided, resembles an
    empty string.

    Also available as :meth:`GetDetails`. May also be modified by
    :meth:`SetDetails`.

  .. attribute:: chains

    Chains involved in this bio unit. If not provided, resembles an empty list.

    Also available as :meth:`GetChainList`. May also be modified by
    :meth:`AddChain`.

  .. attribute:: operations

    Translations and rotations needed to create the bio unit. Filled with
    objects of class :class:`MMCifInfoTransOp`.

    Also available as :meth:`GetOperations`. May be modified by
    :meth:`AddOperations`

  .. method:: GetID()

    See :attr:`id`

  .. method:: SetID(id)

    See :attr:`id`

  .. method:: GetDetails()

    See :attr:`details`

  .. method:: SetDetails(details)

    See :attr:`details`

  .. method:: GetChainList()

    See :attr:`chains`

  .. method:: AddChain(chain name)

    See :attr:`chains`

  .. method:: GetOperations()

    See :attr:`operations`

  .. method:: AddOperations(list of operations)

    See :attr:`operations`

.. function:: PDBize(asu, seqres=None, min_polymer_size=10)

    Returns the biological assembly (bio unit) for an entity. The new entity
    created is well suited to be saved as a PDB file. Therefore the function
    tries to meet the requirements of single-character chain names. The
    following measures are taken.
  
      - All ligands are put into one chain (_)
      - Water is put into one chain (-)
      - Each polymer gets its own chain, named A-Z 0-9 a-z.
      - The description of non-polymer chains will be put into a generic string
        property called description on the residue level.
      - ligands which resemble a polymer but have less than min_polymer_size
        residues are assigned the same numeric residue number. The residues are
        distinguished by insertion code.

    Since this function is at the moment mainly used to create biounits from
    mmCIF files to be saved as PDBs, the function assumes that the
    :ref:`ChainType` properties are set correctly. :func:`ost.conop.ConnectAll`
    is used to derive connectivity.

    :param asu:  Asymmetric unit to work on. Should be created from a mmCIF
                 file.
    :type asu: :class:`~ost.mol.EntityHandle>`
    :param seqres: If set to a valid sequence list, the length of the seqres 
      records will be used to determine if a certain chain has the minimally 
      required length.
    :type seqres: :class:'~ost.seq.SequenceList'
    :param min_polymer_size:  The minimal number of residues a polymer needs to 
      get its own chain. Everything below that number will be sorted into the 
      ligand chain.
    :type min_polymer_size: int

.. class:: MMCifInfoStructDetails

  Holds details about the structure.

  .. attribute:: entry_id

    Identifier for a curtain data block. If not provided, resembles an empty
    string.

    Also available as :meth:`GetEntryID`. May also be modified by
    :meth:`SetEntryID`.

  .. attribute:: title

    Set a title for the structure.

    Also available as :meth:`GetTitle`. May also be modified by
    :meth:`SetTitle`.

  .. attribute:: casp_flag

    Tells whether this structure was target in some competition.

    Also available as :meth:`GetCASPFlag`. May also be modified by
    :meth:`SetCASPFlag`.

  .. attribute:: descriptor

    Descriptor for an NDB structure or the unstructured content of a PDB COMPND
    record.

    Also available as :meth:`GetDescriptor`. May also be modified by
    :meth:`SetDescriptor`.

  .. attribute:: mass

    Molecular mass of a molecule.

    Also available as :meth:`GetMass`. May also be modified by
    :meth:`SetMass`.

  .. attribute:: mass_method

    Method used to determine the molecular weight.

    Also available as :meth:`GetMassMethod`. May also be modified by
    :meth:`SetMassMethod`.

  .. attribute:: model_details

    Details about how the structure was determined.

    Also available as :meth:`GetModelDetails`. May also be modified by
    :meth:`SetModelDetails`.

  .. attribute:: model_type_details

    Details about how the type of the structure.

    Also available as :meth:`GetModelTypeDetails`. May also be modified by
    :meth:`SetModelTypeDetails`.

  .. method:: GetEntryID()

    See :attr:`entry_id`

  .. method:: SetEntryID(id)

    See :attr:`entry_id`

  .. method:: GetTitle()

    See :attr:`title`

  .. method:: SetTitle(title)

    See :attr:`title`

  .. method:: GetCASPFlag()

    See :attr:`casp_flag`

  .. method:: SetCASPFlag(flag)

    See :attr:`casp_flag`

  .. method:: GetDescriptor()

    See :attr:`descriptor`

  .. method:: SetDescriptor(descriptor)

    See :attr:`descriptor`

  .. method:: GetMass()

    See :attr:`mass`

  .. method:: SetMass(mass)

    See :attr:`mass`

  .. method:: GetMassMethod()

    See :attr:`mass_method`

  .. method:: SetMassMethod(method)

    See :attr:`mass_method`

  .. method:: GetModelDetails()

    See :attr:`model_details`

  .. method:: SetModelDetails(details)

    See :attr:`model_details`

  .. method:: GetModelTypeDetails()

    See :attr:`model_type_details`

  .. method:: SetModelTypeDetails(details)

    See :attr:`model_type_details`

.. class:: MMCifInfoObsolete

  Holds details on obsolete/ superseded structures.

  .. attribute:: date

    When was the entry replaced?

    Also available as :meth:`GetDate`. May also be modified by
    :meth:`SetDate`.

  .. attribute:: id

    Type of change. Either *Obsolete* or *Supersede*. Returns a string starting
    upper case. Has to be set via ``OBSLTE`` or ``SPRSDE``.

    Also available as :meth:`GetID`. May also be modified by
    :meth:`SetID`.

  .. attribute:: pdb_id

    ID of the replacing entry.

    Also available as :meth:`GetPDBID`. May also be modified by
    :meth:`SetPDBID`.

  .. attribute:: replace_pdb_id

    ID of the replaced entry.

    Also available as :meth:`GetReplacedPDBID`. May also be modified by
    :meth:`SetReplacedPDBID`.

  .. method:: GetDate()

    See :attr:`date`

  .. method:: SetDate(date)

    See :attr:`date`

  .. method:: GetID()

    See :attr:`id`

  .. method:: SetID(id)

    See :attr:`id`

  .. method:: GetPDBID()

    See :attr:`pdb_id`

  .. method:: SetPDBID(flag)

    See :attr:`pdb_id`

  .. method:: GetReplacedPDBID()

    See :attr:`replace_pdb_id`

  .. method:: SetReplacedPDBID(descriptor)

    See :attr:`replace_pdb_id`

.. class:: MMCifInfoStructRef

  Holds the information of the struct_ref category. The category describes the 
  link of polymers in the mmCIF file to sequences stored in external databases 
  such as uniprot. The related categories ``struct_ref_seq`` and 
  ``struct_ref_seq_dif`` also list differences between the sequences of the 
  deposited structure and the sequences in the database. A prominent example of 
  such differences include point mutations and/or expression tags.

  .. attribute:: db_name

    
    Name of the external database, for example UNP for uniprot.

    :type: :class:`str`


  .. attribute:: db_id
    
    Name of the reference sequence in the database pointed to by :attr:`db_name`.

    :type: :class:`str`
  
  .. attribute:: db_access
    
    Alternative accession code for the sequence in the database pointed to by 
    :attr:`db_name`.

    :type: :class:`str`

  .. method:: GetAlignedSeq(name)

    Returns the aligned sequence for the given name, None if the sequence does 
    not exist.
  
  .. attribute:: aligned_seqs

    List of aligned sequences (all entries of the struct_ref_seq category 
    mapping to this struct_ref).

.. class:: MMCifInfoStructRefSeq

  An aligned range of residues between a sequence in a reference database and the 
  deposited sequence.

  .. attribute:: align_id
    
    Uniquely identifies every struct_ref_seq item in the mmCIF file.
    :type: :class:`str`

  .. attribute:: seq_begin
                 seq_end
    The starting point (1-based) and end point of the aligned range in the 
    deposited sequence, respectively.

   :type: :class:`int`
   
  .. attribute:: db_begin
                 db_end
    The starting point (1-based) and end point of the aligned range in the 
    database sequence, respectively.

   :type: :class:`int`

  .. attribute:: difs

    List of differences between the deposited sequence and the sequence in the 
    database.

 .. attribute:: chain_name
   
   Chain name of the polymer in the mmCIF file.

.. class:: MMCifInfoStructRefSeqDif

  A particular difference between the deposited sequence and the sequence in 
  the database.

  .. attribute:: rnum

    The residue number (1-based) of the residue in the deposited sequence
   
    :type: :class:`int`

  .. attribute:: details

    A textual description of the difference, e.g. point mutation, 
    expression tag, purification artifact.

    :type: :class:`str`

..  LocalWords:  cas isbn pubmed asu seqres conop ConnectAll casp COMPND OBSLTE
..  LocalWords:  SPRSDE pdb func autofunction exptl attr pdbx oper conf spr dif
..  LocalWords:  biounits biounit uniprot UNP seqs
