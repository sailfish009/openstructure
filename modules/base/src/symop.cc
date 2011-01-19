#include <ost/string_ref.hh>
#include <ost/geom/geom.hh>
#include "unit_cell.hh"
#include "symop.hh"
namespace ost {


Symop Symop::FromString(const String& symop_string)
{
  StringRef symop_str(symop_string.data(),symop_string.length());
  
  std::vector<StringRef> rows=symop_str.split(',');
  if (rows.size()!=3) {
    throw SymopParseError("string must contain two ','", symop_string);
  }
  std::vector<StringRef> cols;  
  geom::Mat3 rot(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  geom::Vec3 trans;
  for (size_t i=0; i<rows.size(); ++i) {
    StringRef row=rows[i];

    cols.clear(); cols.push_back(StringRef(&row[0], 0));
    for (size_t j=0; j<row.size(); ++j) {
      if (row[j]=='-' || row[j]=='+') {
        cols.push_back(StringRef(&row[j], 0));
      }
      cols.back()=StringRef(&cols.back()[0], cols.back().size()+1);
    }
    for (size_t j=0; j<cols.size(); ++j) {
      int axis=-1; // x=0, y=1, z=2, unknown=-1
      StringRef col=cols[j];
      StringRef num(&col[0], 0);
      for (size_t k=0; k<col.size(); ++k) {
        if (col[k]=='X' || col[k]=='x') {
          axis=0;
          continue;
        }
        if (col[k]=='Y' || col[k]=='y') {
          axis=1;
          continue;
        }
        if (col[k]=='Z' || col[k]=='z') {
          axis=2;
          continue;
        }
        if (col[k]=='-' || col[k]=='/' || isdigit(col[k])) {
          if (col[k]=='-') {
            num=StringRef(&col[k], 1);
          } else {
            num=StringRef(&num[0], num.size()+1);            
          }
          continue;
        }
        if (col[k]=='+') {
          num=StringRef(&col[k+1], 0);
          continue;
        }
        std::stringstream ss;
        ss << "unknown character '" << col[k] << "'";
        throw SymopParseError(ss.str(), symop_string);
      }
      if (axis==-1) {
        // ok we have a translational component
        Real trans_val=0.0;
        if (!num.empty()) {
          std::pair<bool,float> r=num.parse_rational();
          if (!r.first) {
            throw SymopParseError("can't parse rational number", symop_string);
          }
          trans_val=r.second;
        }
        trans[i]=trans_val;
      } else {
        // we have a rotational component
        Real factor=1.0;
        if (num.empty()) {
          factor=1.0;
        } else if (num==StringRef("-", 1)) {
          factor=-1.0;
        } else {
          std::pair<bool, float> r=num.parse_rational();
          if (!r.first) {
            throw SymopParseError("can't parse rational number", symop_string);
          }
          factor=r.second;
        }
        rot(i, axis)=factor;
      }
    }
  }
  return Symop(rot, trans);
} 

FractionalTransform FractionalTransform::GetOrthoTF(const UnitCell& cell) const
{
  return FractionalTransform(FractionalTransform(cell.GetOrthoTF())*(*this)*FractionalTransform(cell.GetFractionalTF()));
}

}
