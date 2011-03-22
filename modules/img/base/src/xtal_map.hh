#ifndef OST_IMG_XTAL_MAP_HH
#define OST_IMG_XTAL_MAP_HH

#include <boost/enable_shared_from_this.hpp>
#include <ost/dyn_cast.hh>
#include <ost/log.hh>
#include <ost/unit_cell.hh>

#include "module_config.hh"
#include "image_state.hh"
#include "isymop.hh"

namespace ost { namespace img {


class XtalMap;
typedef boost::shared_ptr<XtalMap>  XtalMapPtr;

class XtalExtentIterator {
  friend class XtalMap;
protected:
   //! Initialize with Extent
   explicit XtalExtentIterator(const Extent& b, const XtalMapPtr& map);


public:
    //! copy ctor
    XtalExtentIterator(const XtalExtentIterator &i): 
        start_(i.start_), end_(i.end_), current_(i.current_), 
        asu_pos_(i.asu_pos_), sym_(i.sym_), map_(i.map_)
   { }
   //! assignement op
   XtalExtentIterator& operator=(const XtalExtentIterator& p)
   {
     start_=p.start_;
     end_=p.end_;
     sym_=p.sym_;
     map_=p.map_;
     current_=p.current_;
     asu_pos_=p.asu_pos_;
     return *this;
   }

   //! Prefix increase operator
   XtalExtentIterator& operator++()
   {
     this->Inc();
     return *this;
   }

   //! Postfix increase operator
   XtalExtentIterator operator++(int)
   {
     XtalExtentIterator tmp(*this);
     this->Inc();
     return tmp;
   }
      
   //! Skip ahead to next point
   void Inc();

   //! Retrieve start point
   const Point &GetStart() const { return start_; }

   //! Retrieve end point
   const Point &GetEnd() const { return end_; }

   //! Returns true if iterator has reached end
   bool AtEnd() const { return current_==end_; }

   //! Returns true if iterator is at the start
   bool AtStart() const { return current_==start_; }

   //! element access
   int operator[](int i) const { return current_.operator[](i); }

   operator Point() const { return current_; }
   
   Real GetReal() const;
   void SetReal(Real value);
  private:
   Point start_, end_, current_;
   Point asu_pos_;
   int   sym_;
   XtalMapPtr map_;
};

/// \brief Represents a density map with apparent infinite extent
///
/// Internally, only one assymetric unit is stored and the other regions of the 
/// crystal are reconstructed from the symmetry operations
class DLLEXPORT_OST_IMG_BASE XtalMap : public boost::enable_shared_from_this<XtalMap> {
public:
  friend class XtalExtentIterator;
  
  XtalMap(const ImageHandle& map, const UnitCell& unit_cell,
          const Size& uc_size):
    map_(dyn_cast<RealSpatialImageState>(map.ImageStatePtr())), 
    unit_cell_(unit_cell), uc_ext_(map_->GetExtent().GetStart(), uc_size)
  {
    this->Init();
  }
  /// \brief GetReal Value
  /// This method requires a symmetry lookup and is therefore slow, especially 
  /// for high symmetries. If you want to iterate over a continuous region of 
  /// the crystal, consider using the XtalExtentIterator.
  Real GetReal(const Point& p) const
  {
    Point wp;
    int sym;
    this->FindSym(p, sym, wp);
    return map_->Value(wp);
  }
  /// \brief SetReal value
  ///
  /// This method requires a symmetry lookup and is therefore slow, especially 
  /// for high symmetries. If you want to iterate over a continuous region of 
  /// the crystal, consider using the XtalExtentIterator
  void SetReal(const Point& p, Real value)
  {
    Point wp;
    int sym;
    this->FindSym(p, sym, wp);
    map_->Value(wp)=value;
  }
  
  ImageHandle Extract(const Extent& extent);
  
  XtalExtentIterator Iter(const Extent& ext)
  {
    return XtalExtentIterator(ext, this->shared_from_this());
  }
  
  geom::Vec3 IndexToCoord(const Point& p) const
  {
    return map_->IndexToCoord(p);
  }
  const UnitCell& GetUnitCell() const { return unit_cell_; }
  
  const Size& GetUnitCellSize() const { return uc_ext_.GetSize(); }
  
  geom::Vec3 CoordToIndex(const Vec3& coord) const
  {
    return map_->CoordToIndex(coord);
  }
protected:
  void FindSym(const Point& p, int& sym, Point& wp) const;
  Point ToUnitCell(const Point& p) const
  {
    Point r=p-uc_ext_.GetStart();
    Size s=uc_ext_.GetSize();
    for (size_t i=0; i<3; ++i) {
      r[i]=r[i] % static_cast<int>(s[i]);
      if (r[i]<0) {
        r[i]+=s[i];
      }
    }
    return r+uc_ext_.GetStart();
  }
  struct SymDelta {
    SymDelta(const Point &u, const Point& v, const Point& w):
      du(u), dv(v), dw(w)
    { }
    Point du;
    Point dv;
    Point dw;
  };
  
  void Init();
  RealSpatialImageStatePtr    map_;
  UnitCell                    unit_cell_;
  Extent                      uc_ext_;  
  Extent                      asu_ext_;
  std::vector<ISymop>         symops_;
  std::vector<SymDelta>       symdeltas_;
  std::vector<unsigned char>  asu_;
};

}}

#endif
