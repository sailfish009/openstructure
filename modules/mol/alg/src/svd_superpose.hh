//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------

/*
 * Author Juergen Haas
 */
#ifndef OST_SUPERPOSITION_HH
#define OST_SUPERPOSITION_HH

#include <ost/base.hh>
#include <ost/geom/geom.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/alg/module_config.hh>

namespace ost { namespace mol { 
  
class EntityHandle;

namespace alg {

/// \brief stores the number of cycles and the two final EntityViews
/// (in case IterativeSuperposition was applied),
/// the root mean square deviation of the two superposed objects and the 
/// transformation matrix (geom::Mat4) to superpose input1 onto input2.
struct DLLEXPORT_OST_MOL_ALG SuperpositionResult {
  SuperpositionResult()
    : ncycles(0), rmsd(0.0), transformation() {
  }
  int ncycles;
  Real rmsd;
  geom::Mat4 transformation;
  mol::EntityView entity_view1;
  mol::EntityView entity_view2;
};

class SuperposerSVDImpl;

/// \brief efficiently superpose a bunch of models with the same number of atoms
/// Choose either two EntityViews or two AtomViewLists.
class DLLEXPORT_OST_MOL_ALG SuperposerSVD {
public:
  SuperposerSVD(int natoms, bool alloc_atoms);
  ~SuperposerSVD();
  SuperpositionResult Run(const mol::EntityView& ev1,
                          const mol::EntityView& ev2);

  SuperpositionResult Run(const mol::AtomViewList& atoms1,
                          const mol::AtomViewList& atoms2);

  SuperpositionResult Run(const std::vector<geom::Vec3>& pl1,
                          const std::vector<geom::Vec3>& pl2);

private:
  SuperposerSVDImpl* impl_;
};

/// \brief takes the corresponding atoms and superposes them
SuperpositionResult DLLEXPORT_OST_MOL_ALG SuperposeAtoms(const mol::AtomViewList& atoms1,
                                                    const mol::AtomViewList& atoms2,
                                                    bool apply_transform);
/// \brief superposes two entity views
SuperpositionResult DLLEXPORT_OST_MOL_ALG SuperposeSVD(const mol::EntityView& ev1,
                                                    const mol::EntityView& ev2,
                                                    bool apply_transform);

/// \brief superposes two pointlists
SuperpositionResult DLLEXPORT_OST_MOL_ALG SuperposeSVD(const std::vector<geom::Vec3>& pl1,
                                                       const std::vector<geom::Vec3>& pl2);

/// \brief iterative superposition
SuperpositionResult DLLEXPORT_OST_MOL_ALG IterativeSuperposition(mol::EntityView& ev1,
                                                    mol::EntityView& ev2,
                                                    int ncycles,
                                                    Real distance_threshold,
                                                    bool apply_transform);

/// \brief calculates RMSD for two entity view 
Real DLLEXPORT_OST_MOL_ALG CalculateRMSD(const mol::EntityView& ev1,
                                          const mol::EntityView& ev2,
                                          const geom::Mat4& transformation);


/// \example superpose.py
/// 
/// Superpose 1AKE and 4AKE by a least squares fitting algorithm
/// \sa \ref superpose.py "Superposing entities"
}}}//ns
#endif
