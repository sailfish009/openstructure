#include <ost/log.hh>

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

  mol::AtomHandleList within=ent_.FindWithin(ca_pos_one, 
                                             opts_.upper_cutoff-0.00001);
  for (mol::AtomHandleList::const_iterator 
       i=within.begin(), e=within.end(); i!=e; ++i) {
    mol::ResidueHandle res_two=i->GetResidue();
    if (res_two.GetIndex()-index<opts_.sequence_sep) {
      continue;
    }
    if (i->GetName()!="CA" || !res_two.IsPeptideLinking()) {
      continue;
    }


    AminoAcid aa_two=OneLetterCodeToAminoAcid(res_two.GetOneLetterCode());
    if (aa_two==Xxx) {
      continue;
    }
    geom::Vec3 ca_pos_two;
    geom::Vec3 cb_pos_two;
    if (!this->GetCAlphaCBetaPos(res_two, ca_pos_two, cb_pos_two)) {
      continue;
    }

    Real dist=geom::Length(ca_pos_one-ca_pos_two);
    if (dist<opts_.lower_cutoff) {
      continue;
    }
    Real angle=geom::Angle(cb_pos_one-ca_pos_one, cb_pos_two-ca_pos_two);

    this->OnInteraction(aa_one, aa_two, dist, angle);
  }
  return false;
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
  geom::Vec3 v1=geom::Normalize(ca.GetPos()-n.GetPos());
  geom::Vec3 v2=geom::Normalize(ca.GetPos()-c.GetPos());
  geom::Vec3 in_plane_v=geom::Normalize(v1+v2);
  geom::Plane p(ca.GetPos() ,n.GetPos(), c.GetPos());
  // rotate around vector perpendicular  to p and in_plane_v
  geom::Vec3 axis=geom::Normalize(geom::Cross(p.GetNormal(), in_plane_v));
  geom::Mat3 rot_mat=geom::AxisRotation(axis, (-54/180.0)*M_PI);
  cb_pos=ca.GetPos()+rot_mat*in_plane_v*bond_length;
  return true;
}

}}}

