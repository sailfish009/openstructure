MMCif File Format
--------------------------------------------------------------------------------

The MMCif file format is an alternate container for structural entities, also
provided by the PDB. Here we describe how to load those files and how to deal
with information provided above the common PDB format.


Loading MMCif Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autofunction:: ost.io.LoadMMCIF


Categories Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Info Classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Information from MMCif files which goes beyond structural data, is kept in a
special container, the :class:`MMCifInfo` class. Here is a detailed description
of the annotation available.

.. class:: MMCifInfo

  This is the container for all bits of non-molecular data pulled from a MMCif
  file.

  .. attribute:: citations

    Stores a list of citations (:class:`MMCifInfoCitation`).

    Also available as :meth:`GetCitations`.

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
