#include <ost/log.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/alg/construct_cbeta.hh>
#include "reduced_impl.hh"


namespace ost { namespace qa { namespace impl {

bool ReducedPotentialImpl::VisitResidue(const mol::ResidueHandle& res)
{
  if (!res.IsPeptideLinking()) {
    return false;
  }
  AminoAcid aa_one=OneLetterCodeToAminoAcid(res.GetOneLetterCode());
  if (aa_one==Xxx) {
    return false;
  }
  geom::Vec3 ca_pos_one;
  geom::Vec3 cb_pos_one;
  uint index=res.GetIndex();
  if (!this->GetCAlphaCBetaPos(res, ca_pos_one, cb_pos_one)) {
    return false;
  }
  
  if (ent_) {
    // we got a full entity handle.
    mol::AtomHandleList within=ent_.FindWithin(ca_pos_one, 
                                               opts_.upper_cutoff-0.00001);
    for (mol::AtomHandleList::const_iterator 
         i=within.begin(), e=within.end(); i!=e; ++i) {
      if (i->GetName()=="CA") {
        this->HandleResidue(aa_one, ca_pos_one, cb_pos_one, index, *i);
      }

    }
    return false;
  }
  mol::AtomViewList within=view_.FindWithin(ca_pos_one, 
                                             opts_.upper_cutoff-0.00001);
  for (mol::AtomViewList::const_iterator 
       i=within.begin(), e=within.end(); i!=e; ++i) {
    if (i->GetName()=="CA") {
      this->HandleResidue(aa_one, ca_pos_one, cb_pos_one, index, i->GetHandle());
    }

  }
  return false;
}

void ReducedPotentialImpl::HandleResidue(AminoAcid aa_one, 
                                         const geom::Vec3& ca_pos_one, 
                                         const geom::Vec3& cb_pos_one, 
                                         uint index_one, 
                                         const mol::AtomHandle& ca_two)
{
  
  mol::ResidueHandle res_two=ca_two.GetResidue();
  if (res_two.GetIndex()-index_one<opts_.sequence_sep) {
    return;
  }
  if (!res_two.IsPeptideLinking()) {
    return;
  }


  AminoAcid aa_two=OneLetterCodeToAminoAcid(res_two.GetOneLetterCode());
  if (aa_two==Xxx) {
    return;
  }
  geom::Vec3 ca_pos_two;
  geom::Vec3 cb_pos_two;
  if (!this->GetCAlphaCBetaPos(res_two, ca_pos_two, cb_pos_two)) {
    return;
  }

  Real dist=geom::Length(ca_pos_one-ca_pos_two);
  if (dist<opts_.lower_cutoff) {
    return;
  }
  Real angle=geom::Angle(cb_pos_one-ca_pos_one, cb_pos_two-ca_pos_two);

  this->OnInteraction(aa_one, aa_two, dist, angle);
}
  
bool ReducedPotentialImpl::GetCAlphaCBetaPos(const mol::ResidueHandle& res, 
                                             geom::Vec3& ca_pos, 
                                             geom::Vec3& cb_pos)
{
  const static Real bond_length=1.5;
  mol::AtomHandle ca=res.FindAtom("CA");
  if (!ca.IsValid()) {
    return false;
  }
  ca_pos=ca.GetPos();
  mol::AtomHandle cb=res.FindAtom("CB");
  if (cb.IsValid()) {
    cb_pos=cb.GetPos();
    return true;
  }
  mol::AtomHandle n=res.FindAtom("N");
  mol::AtomHandle c=res.FindAtom("C");
  if (!(ca.IsValid() && c.IsValid() && n.IsValid())) {
    LOG_WARNING("residue " << res.GetQualifiedName() 
                << " doesn't have enough atoms to reconstruct Cbeta position");
    return false;
  }
  cb_pos=mol::alg::CBetaPosition(n.GetPos(), ca.GetPos(), c.GetPos());
  return true;
}

}}}

