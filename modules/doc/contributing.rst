Contributing to OpenStructure
================================================================================

.. note::

  This document describes how to develop features to be included in OpenStructure. If you are mainly planning to use the framework, from Python you probably shouldn't read this document and refer to the :doc:`API docs <index>` instead.

OpenStructure uses `git <http://git-scm.org>`_ as the revision control system. It allows for branch-and-merge driven workflows ideal for a scientific environment. 

The main repository can be browsed `here <http://dng.biozentrum.unibas.ch/git/>`_. 


Even though, technically, there is no such a thing as a central repository in git, we use a central "repository of truth" where everyone is pulling from. You can get the source code with ``git clone``:

.. code-block:: bash

  git clone https:/dng.biozentrum.unibas.ch/git/ost.git


For information on how to install OpenStructure from source, refer to :doc:`install`. 

Leaving aside all the short-lived features branches, the OpenStructure repository has two long-lived branches, one called master and one called develop. Master contains the latest stable changes, develop contains features currently under development that are  potentially not working. Once stable, the changes in develop will be merged into master and together will form the next release. A very detailed description of the branching model used for OpenStructure is available at `this site <http://nvie.com/posts/a-successful-git-branching-model/>`_.



Making Changes
--------------------------------------------------------------------------------

The common workflow of making changes to OpenStructure is depicted in the following section.

Even though it is not neccessary, it is advised to carry out your changes in a separate branch. The new branch can be created with:

.. code-block:: bash
  
  # checkout branch my_branch and create it if it doesn't exist yet.
  # Of course you are free to name the branch whatever you want.
  git checkout -b my_branch

From now on, all your work will be carried out in my_branch. Make your changes and once you are happy, commit them to your repository.

Synchronizing with upstream changes
--------------------------------------------------------------------------------

In the meanwhile, someone else might have committed changes to the develop branch. The best and cleanest way to incorporate these changes is to rebase your changes onto the latest develop branch:

.. code-block:: bash

  git checkout develop
  # pull the latest changes to make sure we are up to date
  git pull
  git checkout my_branch
  # rebase the changes
  git rebase develop


Sending Patches
--------------------------------------------------------------------------------

Changes always go into develop first before being integrated into master. Unless, you have commit access, the easiest way to share your changes is to create a patch and send it by email to the mailing list or one of the developers. To create a suitable diff, rebase your changes onto the latest develop branch and use the ``format-patch`` command:

.. code-block:: bash

  # this takes all the changes you introduced and writes them to 
  # changeset.diff
  git format-patch --stdout develop > changeset.diff

Applying Patches
--------------------------------------------------------------------------------

If you got a patch from someone else and would like to use apply it to your repository, use ``git am``.

.. code-block:: bash

  git am < changeset.diff