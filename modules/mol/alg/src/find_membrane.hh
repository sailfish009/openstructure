#include <ost/mol/mol.hh>

#include <ost/geom/geom.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>

namespace ost { namespace mol{ namespace alg{

struct FindMemParam{
  FindMemParam() { }

  geom::Vec3 GetMembraneAxis() const;
  geom::Vec3 axis;
  geom::Vec3 tilt_axis;
  Real tilt;
  Real angle;
  Real width;
  Real pos;
  Real energy;
  ost::mol::EntityHandle membrane_representation;
};

FindMemParam FindMembrane(ost::mol::EntityHandle& ent, 
                          bool assign_membrane_representation,
                          bool fast);

FindMemParam FindMembrane(ost::mol::EntityView& ent, 
                          bool assign_membrane_representation,
                          bool fast);

}}} // ns
