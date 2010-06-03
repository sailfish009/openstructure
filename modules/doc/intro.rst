A gentle introduction to OpenStructure
================================================================================

In this tutorial you will be learning by example how to use the OpenStructure 
framework. 

We assume that you already have a version of OpenStructure installed. If not, 
please refer to :doc:`install`.


What will be covered in this tutorial?
--------------------------------------------------------------------------------

This tutorial is aimed at users that would like to get their hands dirty and 
execute commands in Python and write scripts rather clicking their way through a 
shiny user interface. The user interface of OpenStructure is in a very early 
state anyway that you probably won't go far by clicking you way through...

The first part of the tutorial is a walk-through of the basic functionality you 
will be using in your everyday work. You will learn how to load structure 
datasets, inspect, display them in the 3D window and save them. 


Getting ready to rumble
--------------------------------------------------------------------------------

The files we will be using in the tutorial are available in the examples folder 
that comes with OpenStructure. Depending on your platform, the examples are 
located at a different location:

 * on *MacOS X* the files are in /Applications/OpenStructure/Examples
 * on *Linux* and *Windows* PREFIX/share/openstructure/examples, where PREFIX is 
   the path to the directory containing OpenStructure.

Starting DNG
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The graphical user interface of OpenStructure is called DNG (Dino/DeepView Next 
Generation). To start it, 

 * on *MacOS X* double click DNG.app in /Applications/OpenStructure
 * on *Windows* double click dng.bat inside the PREFIX/bin directory
 * on *Linux* fire up a terminal change into the OpenStructure installation 
   directory and type 'bin/dng'. If you have the binary directory in the PATH, 
   typing dng is sufficient.

Interactive Python Shell
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now we will enter commands in the Python Shell (in the screenshot above, the 
python shell is located at the bottom of the main window). If you want to get 
more information on any object, function or class, the python help command may 
be useful. For example:

  .. code-block:: python

    # get list of methods of EntityView
    help(mol.EntityView)
    # get help for method Select
    help(mol.EntityView.Select)
    
Loading and inspecting a protein structure
--------------------------------------------------------------------------------

OpenStructure has a module that is dedicated to deal with input and output of 
data, including sequence alignment formats, protein structures and density data 
and images.  If you are reading this tutorial you most certainly have dealt with 
protein structures before and you are most certainly aware that they are usually 
stored in Brookhaven structure files (aka PDB files). The official name for 
molecules stored in a PDB file is an entity. You will hear this word all the 
time, but you can replace the word entity with molecule in your head.


To load a PDB file, type

  .. code-block:: python

     fragment=io.LoadPDB('/path/to/examples/entity/fragment.pdb')

This will load the fragment from the specified file 'fragment.pdb' and store the result in fragment. For more information on the LoadPDB function, type

  .. code-block:: python
  
     help(io.LoadPDB)
     
Now let's inspect what we just loaded:

  .. code-block:: python
  
     print fragment.chain_count
     print fragment.residue_count
     print fragment.atom_count

As you can see, our fragment consists of one peptide chain of 12 amino acids and 
has 81 atoms in total. Now let's examine our fragment in more detail. Enter the 
command

  .. code-block:: python
  
     for residue in fragment.residues:
       print residue  

This will print a list of all residues in the fragment. Similarly to get a list 
of atoms, use:

  .. code-block:: python
  
    for atom in fragment.atoms:
      print atom

Of course, we can also get a list of atoms grouped by residues:
    
  .. code-block:: python
  
    for residue in fragment.residues:
      print residue, 'has', residue.atom_count, 'atom(s).'
      for atom in residue.atoms:
        print ' ', atom.name, atom.pos

And, for completeness, we will first group them by chain, then by residues.

  .. code-block:: python
  
    for chain in fragments.chains:
      print 'chain', chain.name, 'has', chain.residue_count, 'residue(s)'
      for residue in chain.residues:
        print ' ', residue, 'has', residue.atom_count, 'atom(s).'
        for atom in residue.atoms:
          print '    ', atom.name, atom.pos

Aah, wait! A protein fragment would not be complete without bonds: Let's see 
what bonds we have in there:

  .. code-block:: python
  
    for bond in fragment.bonds:
      print bond
      
From these short code examples we already see how the entity is structured: On 
one hand we have a hierarchy of chains, residues and atoms. On the other hand, 
we have bonds that form a network overlayed on the hierarchy. This is 
illustrated in the picture on the left. An important feature of entities is that 
we can always assume that the hierarchy is intact. You will never find an atom 
without residues, no residue can exist without a parent chain and chains belong 
always to an entity. 

Let There Be Shiny Graphics
--------------------------------------------------------------------------------

For visually inspecting the fragment, we now create a graphical representation 
of the entity:

  .. code-block:: python
  
     go=gfx.Entity("Fragment", fragment)
     scene.Add(go)
     scene.CenterOn(go)

Now you will see the fragment in the 3D window (left):

![](docs/tut/sel.png)

Use the mouse to rotate, zoom in an shift the camera. Double clicking on an atom will center the camera on that atom. 

Introduction to Views
--------------------------------------------------------------------------------

Often during processing and visualisation of data, only parts of a protein 
structure are of interest. This realisation has had a major impact on the design 
of OpenStructure and is tied very deeply into the core of the framework. 
Subparts of structure are modeled as so-called :class:`EntityViews 
<mol.EntityView>`. You can think of them as a selection of chains, residues, 
atoms and bonds of an entity. A views has almost the same interface as the 
underlying entity, making it very easy to mix entity views with handles in 
Python due to the dynamic nature of the language. An algorithm that is written 
for entities will almost always (with some care) also work for 
:class:`EntityHandles <mol.EntityHandle>`. This is referred to as `duck-typing 
<http://en.wikipedia.org/wiki/Duck_typing>`_ (I don' t care if it is a duck as 
long as it looks like a duck), a concept used all over the place in Python. 
￼
A typical view can be seen in the image on the left. The view consists of one 
chain, one residue and two atoms. Again the same rule applies: No atom can be 
part of the view without it's residue. In this example, no bonds are included, 
since there is at most one atom per bond in the original structure.

To familiarize yourself with the concept of views, we will use the fragment in 
the 3D window.

We will use several ways to select parts of our fragment:
 * By using a dedicated query language
 * By manually constructing a view

The Query Language
--------------------------------------------------------------------------------

The first way to select parts of a structure is with a dedicated mini-language, 
called ["the query language”](docs/tut/query.html). In the Python Shell, type

  .. code-block:: python
  
    go.selection=fragment.Select('')
    
A green halo will be displayed around the selected parts (image in the middle).

As you can see the previous statement created a “full view”, containing all the 
chains, residues, atoms and bonds. To select lysine residues, type

  .. code-block:: python
  
    go.selection=fragment.Select('rname=LYS')
    

As you can see (image in the middle), the  only lysine residue is now 
highlighted in the 3D window, because it was the only one matching the predicate 
"residue name must be equal to LYS". Several such predicates can be combined 
with boolean operators such as *and* and *or*. To select residues with residue 
number 1 to 3, the following statement will do the job:

  .. code-block:: python
  
    go.selection=fragment.Select('rnum>=1 and rnum<=3')
    
but this is very cumbersome. That's why there is a shortcut to this statement. 
You can specify a range of values.

  .. code-block:: python
  
    go.selection=fragment.Select('rnum=1:3')

For a complete description of what you can do with the query language, have a 
look at the :doc:`../mol/base/query`.


Constructing Views Manually
--------------------------------------------------------------------------------

Sometimes the query language Is Not Enough (TM). For these cases the 
construction of manual entities becomes neccessary. This is pretty straight 
forward:

  .. code-block:: python
  
    view=fragment.CreateEmptyView()
    ca=fragment.FindAtom('A', mol.ResNum(1), 'CA')
    cb=fragment.FindAtom('A', mol.ResNum(1), 'CB')
    view.AddAtom(ca)
    view.AddAtom(cb)
    go.SetSelection(view)

The last step sets our constructed view as the current selection, displaying it 
in the 3D window. As you can see, C-alpha and C-beta of the first residue are 
not connected by bonds, even though both atoms are in the view. You have either 
to add the bond manually with

  .. code-block:: python
  
    ca_cb=ca.FindBondToAtom(cb)
    view.AddBond(ca_cb)
    
Or as a very convenient shortcut 'view.AddAllInclusiveBonds()' to add all bonds 
that have both bonding partners in the view.

Don't forget to call update the selection of the graphics object to see what 
view you have created.

Saving an Entity
--------------------------------------------------------------------------------

Saving an entity (or a view) is a breeze:

Type

  .. code-block:: python
  
     io.SavePDB(fragment, 'full.pdb')

to save the full view. To save only the backbone atoms, we can first select the 
backbone atoms and then save it:

  .. code-block:: python
  
     io.SavePDB(fragment.Select('aname=CA,C,N,O'), 'backbone.pdb')


Loading  images and density maps
--------------------------------------------------------------------------------

Openstructure features a :mod:`~ost.img` module that is dedicated to the 
manipulation of 
images/density maps. The images or density maps can either be one-, two- or 
three-dimensional. The most common formats used in x-ray and electron 
crystallography and atomic force microscope are supported in addition to several 
general purpose image formats. See `supported file formats` for details.
The :mod:`~ost.img` module was originally developed as part of the Image 
Processing Library & Toolbox IPLT. More documentation and examples can also be 
found on the `IPLT website <http://www.iplt.org>`_.

To load a density map, type

  .. code-block:: python

     map=io.LoadImage('/path/to/examples/map/1ppt.map')

This will load the fragment density map from the specified file 'fragment.map' 
and store the result in fragment_map.

Now let's inspect what we just loaded:

  .. code-block:: python
  
    print map.GetPixelSampling()
    
We can see that the sampling is set to 1.0 Angstroems in all three dimensions.

Manipulating  images and density maps
--------------------------------------------------------------------------------

The algorithms used for manipulation of an image are found in the 
:mod:`~ost.img` module. Therefore before using an algorithm we first have to 
import the :mod:`~ost.img` module.

  .. code-block:: python
  
    from ost import img


The :mod:`~ost.img` module provides a wide range of algorithm to manipulate 
image data. Here for the example we use a LowPassFilter to restrict the 
resolution of the density map.

  .. code-block:: python
  
     map_filtered=map.Apply(img.alg.LowPassFilter(3.0))

The filtered map is stored in a new variable called fragment\_map\_filtered.


Displaying images and density maps
--------------------------------------------------------------------------------

Now that we have a filtered map it's time to have a look at it. There are 
fundamentally two ways to visualize 3-dimensional density maps. One is by 
drawing isosurfaces. These are conceputally similar to contour lines used in 
cartography: every point on an isosurface has the same density value. 
Isosurfaces are easy to create in OpenStructure:

  .. code-block:: python
  
     go=gfx.MapIso("filtered", map_filtered,0.5)
     scene.Add(go)

The other way to visualize a 3-dimensional map is by showing one 2-dimensional 
density slice at a time, allowing the user to move through the slices. In 
OpenStructure this is achieved using a DataViewer docs/tut/imgdataviewer.html). 
A DataViewer showing the filtered map is created using the following command:

  .. code-block:: python
  
    gui.CreateDataViewer(map_filtered)

This command displays a panel showing one slice of the density map lying on a 
particular (x,y) plane in the coordinate reference system.
The 'z' and 'x' keys can be used to move to slices lying at a lower or higher 
coordinate along the 'z' axis, allowing the examination of
the full 3-dimensional volume.

A more detailed explanation of the :mod:`~ost.img` module can be found in the 
tutorial section for :mod:`~ost.img`.
