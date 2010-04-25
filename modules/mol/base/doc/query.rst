Queries
================================================================================

.. class:: Query

  Blabla

It is often convenient to highlight or focus certain parts of the structure. 
OpenStructure includes a powerful query system that allows you to perform custom 
selections in a convenient way.

Please refer to the tutorial on the query language for an introduction.

The query language reference
--------------------------------------------------------------------------------

The query consists of one or more predicates combined with boolean 
operators. A predicate takes the form *prop op value*. Operators are
one of `` =, !=, >, >=, <=`` and ``<``. A predicate has the following form:
*PROPERTY - OPERATOR - ARGUMENT*.

The following properties may be used in predicates. The supported type of the
value is given for each property.

 *      ``rname`` residue name. type: string
 *      ``rnum`` residue number. currently only numeric part is honored. 
        type: int
 *      ``rtype`` residue type as given by the DSSP code, i.e. H for helix, E
        for extended. type: string
 *      ``aname`` atom name. type: string
 *      ``ele`` element of atom. type: string
 *      ``cname`` chain name. type: string
 *      ``occ`` occupancy, between 0 and 1. type: float
 *      ``abfac`` B (temperature) factor of atom. type: float
 *      ``rbfac`` average B (temperature) factor of residue. type: float. 
        Note that this does not work for views as expected. When running a 
        second selection statement on a view, all atoms of the residue handle 
        and not the view are considered in the calculation
 *      ``ishetatm`` whether the atom is a hetorogenous atom. type: bool or int
        (0 for false, 1 for true)
 *      ``peptide`` whether the residue is peptide linking. type: bool or int
        (0 for false, 1 for true)        
 *      ``x``  X coordinate of atom. type: float
 *      ``y``  Y coordinate of atom. type: float
 *      ``z``  Z coordinate of atom. type: float
 *      ``rindex`` index of residue in chain. This index is the same for views 
        and handles
 *      For :ref:`generic properties in queries <genprop-in-queries>` see below.

Distance-based selections within a given atom position are supported by
the within statement. To select atoms within 5 Angstrom of the point
``{1, 2, 3}``, one would write:

``5 <> {1,2,3}``

Negation of this expression is possible with the not operator, i.e.

``not 5 <> {1,2,3}``
will select atoms that are further than five Angstrom apart from ``{1, 2 , 3}``

Abbreviations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Two abbreviations exist for convenience:

Instead of writing 

``aname=CA or aname=C or aname=O or aname=N``, 

one can write 

``aname=CA,N,C,O``. 

For integral value ranges, one can use the colon syntax: instead of  

``rnum>=10 and rnum<=20`` 

one can write 

``rnum=10:20``

.. _genprop-in-queries:

Generic Properties in Queries
--------------------------------------------------------------------------------

The query language can also be used for numeric generic properties (i.e.
float and int), but the syntax is slightly different. To access any generic
properties, it needs to be specified that they are generic and at which level 
they are defined. Therefore, all generic properties start with a ``g``, followed by an ``a``, ``r`` or ``c`` for atom, residue or chain level respectively. 

.. code-block:: python
   
   # set generic properties for atom, residue, chain
   atom_handle.SetFloatProp("testpropatom", 5.2)
   resid_handle.SetFloatProp("testpropres", 1.1)
   chain_handle.SetIntProp("testpropchain", 10)
   
   # query statements
   sel_a=e.Select("gatestpropatom<=10.0")
   sel_r=e.Select("grtestpropres=1.0")
   sel_c=e.Select("gctestpropchain>5")

Since generic properties do not need to be defined for all parts of an entity
(e.g. it could be specified for one single :class:`AtomHandle`), the query
statement will throw an error unless you specify a default value in the query
statement which can be done using a ':' character:

.. code-block:: python

  # if one or more atoms have no generic properties
  
  sel=e.Select("gatestprop=5")
  # this will throw an error
  
  # you can specify a default value:
  sel=e.Select("gatestprop:1.0=5")
  # this will run through smoothly and use 1.0 as
  # the default value for all atoms that do not
  # have the generic property 'testprop'

Using this method, you will be warned if a generic property is not set for all
atoms, residues or chains unless you specify a default value. So, be careful
when you do.
