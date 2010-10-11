//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#include "all_atom_potential.hh"
#include <ost/mol/mol.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>
#include <boost/filesystem/convenience.hpp>
#include <ost/message.hh>
#include <ost/io/io_exception.hh>

/*
  Author: Marco Biasini, Pascal Benkert
 */
 
namespace ost { namespace qa {

namespace {

class AllAtomPotentialCalculator : public mol::EntityVisitor {
public:
  AllAtomPotentialCalculator(AllAtomPotential::AllAtomEnergies& energies,
                             AllAtomPotentialOpts& opts, mol::EntityView target_view):
    energies_(energies),
    options_(opts),
    energy_(0.0),
    target_view_(target_view),
    interaction_counts_(0)
  {
  }

  virtual bool VisitResidue(const mol::ResidueHandle& residue)
  {
    curr_aa_=ResidueToAminoAcid(residue);
    return curr_aa_!=Xxx;
  }
  
  virtual bool VisitAtom(const mol::AtomHandle& atom_handle)
  {
    //TODO: The handle contains all atoms of the original loded PDB file.
    //i.e. the selection on C-beta atoms for the residue-based potential
    //is lost. Therefore a first view contains all atoms of which the energy
    //should be alculated and an additional (target-)view is provided which
    //specifies the set of atoms against which the potential is calculated

    atom::ChemType type_a=GetAtomTypeByName(curr_aa_, atom_handle.GetName());
    if (type_a!=atom::UNKNOWN) {
      mol::AtomViewList nearby_atoms=target_view_.FindWithin(atom_handle.GetPos(),
                                                             options_.upper_cutoff);
      for (mol::AtomViewList::iterator i=nearby_atoms.begin(),
           e=nearby_atoms.end(); i!=e; ++i) {
        mol::AtomView a=*i;
        AminoAcid aa_b=ResidueToAminoAcid(a.GetHandle().GetResidue());
        if (aa_b==Xxx)
          continue;
        atom::ChemType type_b=GetAtomTypeByName(aa_b, a.GetName());
        if (type_b!=atom::UNKNOWN) {

          float d=geom::Distance(atom_handle.GetPos(),
                                  a.GetPos());

         // avoid rounding problems: distances very close to the cut-off may
         // be rounded up the to cut-off and lead to an overflow in container energies_
          d=d-0.0001; //used to prevent overflow if distance == upper_cutoff
          if (d<0) d=0.0;

          if (d>options_.lower_cutoff) {
            // GetHandle() necessary to retriev original index of the view (as compared to index in the view):
            if (abs(atom_handle.GetResidue().GetIndex()-a.GetResidue().GetHandle().GetIndex())<options_.sequence_sep) {
              continue;
            }
            energy_+=energies_.Get(type_a, type_b, d);
            interaction_counts_++;
            //std::cout << type_a << "(" << atom_handle.GetQualifiedName() << ")" << " " << type_b << "(" << a.GetQualifiedName() << ")" << " : " << d << "     " << energies_.Get(type_a, type_b, d) << std::endl;
          }
        }
// 	else {
// 	  std::cout << "Unknown type: " << type_b << std::endl;
// 	}
      }
    }
    return false;
  }
  float GetEnergy() const
  {
    return energy_;
  }

  int GetEnergyCounts() const
  {
    return interaction_counts_;
  }

private:
  AllAtomPotential::AllAtomEnergies& energies_;
  AllAtomPotentialOpts                options_;
  AminoAcid                           curr_aa_;
  float                               energy_;
  mol::EntityView                 target_view_;
  int                      interaction_counts_;
};

}

AllAtomPotentialPtr AllAtomPotential::Create(const InteractionStatisticsPtr& s,
                                             const AllAtomPotentialOpts  & o)
{

  AllAtomPotentialPtr p(new AllAtomPotential);
  p->options_=o;
  int num=int((o.upper_cutoff-o.lower_cutoff)/o.distance_bucket_size);
  p->energies_=AllAtomEnergies(0.0, IntegralClassifier(atom::UNKNOWN, 0),
                               IntegralClassifier(atom::UNKNOWN, 0),
                               ContinuousClassifier(num, o.lower_cutoff, 
                                                    o.upper_cutoff));
  p->Fill(s);
  return p;
}


void AllAtomPotential::SaveToFile(const String& filename)
{
  std::ofstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);  
  ds << *this;
}

AllAtomPotentialPtr AllAtomPotential::LoadFromFile(const String& filename)
{
  if(!boost::filesystem::exists(filename))
  throw io::IOException("Could not open interaction potential data file.\nFile does not exist at: "+filename);

  std::ifstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSource ds(stream);  
  AllAtomPotentialPtr all_atom_p(new AllAtomPotential);
  ds >> *all_atom_p.get();
  return all_atom_p;
}


float AllAtomPotential::GetTotalEnergy(mol::EntityView view, mol::EntityView target_view)
{
  AllAtomPotentialCalculator c(energies_, options_, target_view);
  mol::EntityHandle e=view.GetHandle();
  view.Apply(c);
  interaction_counts_member_=c.GetEnergyCounts();
  return c.GetEnergy();
}

float AllAtomPotential::GetEnergy(atom::ChemType type_a, atom::ChemType type_b, float distance)
{
  return energies_.Get(type_a, type_b, distance);
}

float AllAtomPotential::GetTotalEnergy(mol::EntityView view)
{
  AllAtomPotentialCalculator c(energies_, options_, view);
  mol::EntityHandle e=view.GetHandle();
  view.Apply(c);
  interaction_counts_member_=c.GetEnergyCounts();
  return c.GetEnergy();
}


int AllAtomPotential::GetEnergyCounts() const {
  return interaction_counts_member_;
}


void AllAtomPotential::SetSequenceSeparation(int seq_sep) {
  options_.sequence_sep=seq_sep;
}


AllAtomPotentialOpts::AllAtomPotentialOpts():
  sigma(0.02), lower_cutoff(3.0), upper_cutoff(20.0), distance_bucket_size(1.0),
  sequence_sep(8)
{
}

AllAtomPotentialOpts::AllAtomPotentialOpts(float lc, float uc,
                                           float dbs, int sp, float s):
  sigma(s), lower_cutoff(lc), upper_cutoff(uc), distance_bucket_size(dbs),
  sequence_sep(sp)
{

}

template <typename DS>
void AllAtomPotential::Serialize(DS& ds)
{
  ds & options_;
  ds & energies_;
}

template <typename DS>
void AllAtomPotentialOpts::Serialize(DS& ds)
{
  ds & lower_cutoff;
  ds & upper_cutoff;
  ds & sigma;
  ds & sequence_sep;
  ds & distance_bucket_size;
} 



void AllAtomPotential::Fill(const InteractionStatisticsPtr& stats)
{
  typedef AllAtomPotential::AllAtomEnergies::IndexType Index;
  int num=int((options_.upper_cutoff-
               options_.lower_cutoff)/options_.distance_bucket_size);  

  uint64_t total_counts = 0;
  for (int k=0; k<num; ++k) {
    total_counts = total_counts+stats->GetCount(options_.distance_bucket_size*k+
                                 options_.lower_cutoff+
                                 options_.distance_bucket_size*0.5);
  }
//   std::cout << "total_counts: " << total_counts << std::endl; 

  for (int i=0; i<atom::UNKNOWN; ++i) {
    for (int j=0; j<atom::UNKNOWN; ++j) {
      uint64_t t2=stats->GetCount(atom::ChemType(i), atom::ChemType(j));

      for (int k=0; k<num; ++k) {
        uint64_t t3=stats->GetCount(atom::ChemType(i), atom::ChemType(j),
                                  options_.distance_bucket_size*k+
                                  options_.lower_cutoff+
                                  Real(options_.distance_bucket_size*0.5));
        // could be moved outside of the loop and cached...
        uint64_t t4=stats->GetCount(options_.distance_bucket_size*k+
                                 options_.lower_cutoff+
                                 options_.distance_bucket_size*0.5);
        float f1=t2 > 0 ?(float(t3)/float(t2)) : 0.0;
        float f2=total_counts > 0 ? (float(t4)/float(total_counts)) : 0.0;
        float d = f2>0.000000001 ? f1/f2: 0.0;
        // prop = (Nd_xy / Nxy) / (Nd / Ntot)
        float e=0.582*log(1+options_.sigma*t2)-0.582*log(1+options_.sigma*t2*d);
        energies_.Set(Index(i, j, k), e);

        // potential is only calculated for Cbeta atoms:
        // copy the potential for Calpha (Calphas are selected if 
        // Cbetas are not present)
        if (stats->isCbetaStaistics() == true) {
          //check if Cbeta (has counts) and not Glycin-Calpha
          if (t3 != 0) {
             if (i==3) {
              energies_.Set(Index(i, j-1, k), e);
             }
             else if (j==3) {
              energies_.Set(Index(i-1, j, k), e);
              }
             else {
              energies_.Set(Index(i-1, j, k), e);
              energies_.Set(Index(i, j-1, k), e);
              energies_.Set(Index(i-1, j-1, k), e);
             }
          }
        }
      }
    }
  }
}

}}
