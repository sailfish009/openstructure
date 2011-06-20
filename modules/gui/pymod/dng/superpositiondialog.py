#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2011 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------
#
# Authors: Stefan Bienert 
#
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from ost.mol.alg import Superpose
from ost import mol

class ChainComboBox(QComboBox):
  def __init__(self, ent, gfx, parent=None):
    # class variables
    self.all_chains = 'All'
    QComboBox.__init__(self, parent)
    self.entity = ent
    self.addItem(self.all_chains)
    for chain in self.entity.chains:
      self.addItem(chain.name)
    if self.count()>0:
      self.setCurrentIndex(0)
    if gfx:
      self.gfx = gfx
      QObject.connect(self,
                      SIGNAL('highlighted (const QString&)'),
                      self._HighlightChain)

  def focusOutEvent (self, event):
    if self.gfx:
      self.gfx.selection = None

  def SetItems(self, ent, gfx):
    self.clear()
    self.entity = ent
    self.addItem(self.all_chains)
    for chain in self.entity.chains:
      self.addItem(chain.name)
    if self.count()>0:
      self.setCurrentIndex(0)
    if gfx:
      self.gfx = gfx

  def _HighlightChain(self, chain):
    if str(chain) != 'All':
      self.gfx.SetSelection(self.entity.Select('cname=' + str(chain)))
    else:
      self.gfx.SetSelection(self.entity.Select(''))

  def _GetSelectedChain(self):
    if self.currentIndex() == -1:
      return mol.EntityHandle()
    elif self.currentText() == self.all_chains:
      return self.entity
    return self.entity.Select('cname=' + str(self.currentText()))

  def _SetSelectedChain(self, chain):
    if hasattr(chain, 'name'):
      name = chain.name
    else:
      name = str(chain)
    for i in range(self.count()):
      if self.itemText(i) == name:
        self.setCurrentIndex(i)
        break
  selected_chain = property(_GetSelectedChain, _SetSelectedChain)

class SuperpositionDialog(QDialog):
  """
  Provides a graphical user interface to structurally superpose two entities.
  Uses function :func:`~ost.mol.alg.Superpose`. The RMSD of two superposed
  molecules will be stored in attribute ``rmsd``. An index for the selected
  reference molecule will be stored in attribute ``reference``.

  :param ent_one: The first entity
  :type ent_one: :class:`~ost.mol.EntityView`, :class:`~ost.mol.EntityHandle`
                 or :class:`~ost.gfx.Entity`
  :param ent_two: The second entity
  :type ent_two: :class:`~ost.mol.EntityView`, :class:`~ost.mol.EntityHandle`
                 or :class:`~ost.gfx.Entity`

  **Example Usage:**

  .. code-block:: python

    e1=io.LoadPDB('examples/code_fragments/entity/pdb1ake.ent')
    e2=io.LoadPDB('examples/code_fragments/entity/pdb4ake.ent')

    sd = superpositiondialog.SuperpositionDialog(e1, e2)

    g1=gfx.Entity('G1', e1)
    g2=gfx.Entity('G2', e2)
    scene.Add(g1)
    scene.Add(g2)

    if sd.reference == 0:
      scene.CenterOn(g1)
    else:
      scene.CenterOn(g2)

    LogScript('RMSD: %.3f'%sd.rmsd)
  """

  def __init__(self, ent_one, ent_two, parent=None):
    # class variables
    self.rmsd = None
    self._mmethod_dict = {'number': 'number',
                          'index': 'index',
                          'local alignment': 'local-aln',
                          'global alignment': 'global-aln'}
    self.gfx_one = None
    self.gfx_two = None
    self.gfx_select_one = None
    self.gfx_select_two = None
    QDialog.__init__(self, parent)
    self.setWindowTitle('Superpose structures')
    if not isinstance(ent_one, mol.EntityHandle) and \
       not isinstance(ent_one, mol.EntityView):
      n_one = ent_one.GetName()
      self.gfx_one = ent_one
      self.gfx_select_one = self.gfx_one.GetSelection()
      self.ent_one = ent_one.GetView()
    else:
      if isinstance(ent_one, mol.EntityHandle):
        n_one = ent_one.GetName()
      elif isinstance(ent_one, mol.EntityView):
        n_one = ent_one.GetHandle().GetName()
      self.ent_one = ent_one
    if len(n_one) == 0:
      n_one = '1'
    if not isinstance(ent_two, mol.EntityHandle) and \
       not isinstance(ent_two, mol.EntityView):
      n_two = ent_two.GetName()
      self.gfx_two = ent_two
      self.gfx_select_two = self.gfx_two.GetSelection()
      self.ent_two = ent_two.GetView()
    else:
      if isinstance(ent_two, mol.EntityHandle):
        n_two = ent_two.GetName()
      elif isinstance(ent_two, mol.EntityView):
        n_two = ent_two.GetHandle().GetName()
      self.ent_two = ent_two
    if len(n_two) == 0:
      n_two = '2'
    if n_one == n_two:
      n_one = n_one + ' 1'
      n_two = n_two + ' 2' 
    layout = QGridLayout(self)
    # select reference molecule
    self.reference = 0;
    self._reference = self._ReferenceSelection(n_one, n_two)
    grow = 0
    layout.addWidget(QLabel("reference"), grow, 0)
    layout.addWidget(self._reference, grow, 1)
    grow += 1
    # chains
    self._chain_one = ChainComboBox(self.ent_one, self.gfx_one, self)
    self._chain_two = ChainComboBox(self.ent_two, self.gfx_two, self)
    layout.addWidget(QLabel("reference chain"), grow, 0)
    layout.addWidget(self._chain_one, grow, 1)
    grow += 1
    layout.addWidget(QLabel("chain"), grow, 0)
    layout.addWidget(self._chain_two, grow, 1)
    grow += 1
    # link chain and reference selection
    QObject.connect(self._reference,
                    SIGNAL('currentIndexChanged(int)'),
                    self._ChangeChainSelection)
    # match methods
    self._methods = self._MatchMethods()
    layout.addWidget(QLabel('match residues by'), grow, 0)
    grow += 1
    layout.addWidget(self._methods)
    # atoms
    self._atoms = self._FetchAtoms(self._methods.size(),
                                   self.ent_one,
                                   self.ent_two)
    self._atmselectbx, self._atmselectgrp = self._AtomSelectionBox()
    layout.addWidget(self._atmselectbx, grow, 1)
    grow += 1
    # buttons
    ok_button = QPushButton("Superpose")
    QObject.connect(ok_button, SIGNAL('clicked()'), self.accept)
    cancel_button = QPushButton("Cancel")
    hbox_layout = QHBoxLayout()
    hbox_layout.addStretch(1)
    layout.addLayout(hbox_layout, grow, 0, 1, 2)
    grow += 1
    QObject.connect(cancel_button, SIGNAL('clicked()'), self.reject)
    QObject.connect(self, SIGNAL('accepted()'), self._Superpose)
    hbox_layout.addWidget(cancel_button, 0)
    hbox_layout.addWidget(ok_button, 0)
    ok_button.setDefault(True)
    self.exec_()
    # restore old selections
    if self.gfx_one:
      self.gfx_one.SetSelection(self.gfx_select_one)
    if self.gfx_two:
      self.gfx_two.SetSelection(self.gfx_select_two)

  def _Superpose(self):
    view_one = self._chain_one.selected_chain
    view_two = self._chain_two.selected_chain
    atoms = self._GetAtomSelection()
    sp = Superpose(view_one, view_two,
                   self._mmethod_dict[str(self._methods.currentText())],
                   atoms)
    self.rmsd = sp.rmsd

  def _toggle_atoms(self, checked):
    if checked:
      self._atoms.setEnabled(True)
    else:
      self._atoms.setEnabled(False)

  def _AtomSelectionBox(self):
    bt1 = QRadioButton('All')
    bt2 = QRadioButton('Backbone')
    bt3 = QRadioButton('CA')
    self.cstmbtntxt = 'Custom'
    custom_rbutton = QRadioButton(self.cstmbtntxt)
    group = QButtonGroup()
    group.addButton(bt1)
    group.addButton(bt2)
    group.addButton(bt3)
    group.addButton(custom_rbutton)
    bt1.setChecked(True)
    vbox_layout = QVBoxLayout()
    vbox_layout.addWidget(bt1)
    vbox_layout.addWidget(bt2)
    vbox_layout.addWidget(bt3)
    vbox_layout.addWidget(custom_rbutton)
    vbox_layout.addWidget(self._atoms)
    QObject.connect(custom_rbutton, SIGNAL('toggled(bool)'), self._toggle_atoms)
    box = QGroupBox("atom selection")
    box.setLayout(vbox_layout)
    return box, group

  def _GetAtomSelection(self):
    checkedbtn = self._atmselectgrp.checkedButton()
    if str(checkedbtn.text()) != self.cstmbtntxt:
      return str(checkedbtn.text())
    slctn_model = self._atoms.selectionModel()
    dt_model = slctn_model.model()
    atms = list()
    for idx in slctn_model.selectedRows():
      slctn = dt_model.data(idx, Qt.DisplayRole).toString()
      atms.append(str(slctn))
    return atms

  def _FetchAtoms(self, dim, ent_a, ent_b):
    # fetch list of atoms: only those which are in both entities are considered
    atm_dict = {}
    for atm in ent_a.GetAtomList():
      atm_dict[atm.name] = 0
    for atm in ent_b.GetAtomList():
      if atm.name in atm_dict:
        atm_dict[atm.name] = 1
    atmlst = QStringList()
    for atm in sorted(atm_dict.keys()):
      if atm_dict[atm]:
        atmlst.append(atm)
    elems = QStringListModel(atmlst)
    atoms = QListView(self)
    dim.setHeight(3*dim.height())
    atoms.setFixedSize(dim)
    atoms.setModel(elems)
    atoms.setSelectionMode(QAbstractItemView.MultiSelection)
    atoms.setEditTriggers(QAbstractItemView.NoEditTriggers)
    atoms.setEnabled(False)
    return atoms

  def _ReferenceSelection(self, name_a, name_b):
    cbox = QComboBox()
    cbox.addItem(name_a)
    cbox.addItem(name_b)
    if cbox.count() > 0:
      cbox.setCurrentIndex(0)
    return cbox

  def _ChangeChainSelection(self, index):
    if index == 0:
      self._chain_one.SetItems(self.ent_one, self.gfx_one)
      self._chain_two.SetItems(self.ent_two, self.gfx_two)
      self.reference = 0;
    elif index == 1:
      self._chain_one.SetItems(self.ent_two, self.gfx_two)
      self._chain_two.SetItems(self.ent_one, self.gfx_one)
      self.reference = 1;

  def _MatchMethods(self):
    methods=QComboBox(self)
    for method in sorted(self._mmethod_dict):
      methods.addItem(method)
    return methods
