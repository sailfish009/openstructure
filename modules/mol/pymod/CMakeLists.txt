set(OST_BASE_PYMOD_SOURCES
export_atom.cc
export_atom_view.cc
export_bond.cc
export_chain.cc
export_chain_view.cc
export_coord_group.cc
export_editors.cc
export_entity.cc
export_entity_view.cc
export_property_id.cc
export_query.cc
export_residue.cc
export_residue_view.cc
export_surface.cc
export_bounding_box.cc
export_torsion.cc
export_visitor.cc
wrap_mol.cc
)

ost_pymod(mol ost/mol "${OST_BASE_PYMOD_SOURCES}" "__init__.py")
