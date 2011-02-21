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
#ifndef OST_GENETRIC_PROPERTY_HH
#define OST_GENETRIC_PROPERTY_HH

/*
  usage:

  the impl is derived from GenericPropContainerImpl

  the handle is derived from GenericPropContainer, and then has the
  setter and getter methods for String, Real, int and bool mapping
*/

#include <exception>
#include <sstream>
#include <map>
#include <vector>
#include <boost/variant.hpp>

#include <ost/module_config.hh>
#include <ost/invalid_handle.hh>

namespace ost {

struct  TEMPLATE_EXPORT GenericPropError: public std::exception
{
  GenericPropError(const String& m):
    m_(m)
  {}
  virtual ~GenericPropError() throw() {}
  virtual const char* what() const throw() {
    return m_.c_str();
  }
  String m_;
};
  
typedef boost::variant<String, Real, int, bool> GenericPropValue;

///  \brief base class for the implementation
class  TEMPLATE_EXPORT GenericPropContainerImpl
{
  typedef std::map<String,GenericPropValue> PropertyMap;
  
public:
  GenericPropContainerImpl(): map_(NULL) {}
  ~GenericPropContainerImpl()
  {
    if (map_) {
      delete map_;
    }
  }
  GenericPropContainerImpl(const GenericPropContainerImpl& rhs):
    map_(rhs.map_ ? new PropertyMap(*rhs.map_) : NULL)
  { }
  
  GenericPropContainerImpl& operator=(const GenericPropContainerImpl& r)   
  {
    this->Assign(r);
    return *this;
  }
  
  GenericPropValue& GenericProp(const String& key) 
  {
    if (!map_) {
      map_=new PropertyMap;
    }
    return (*map_)[key];
  }
  
  const GenericPropValue& GenericProp(const String& key) const
  {
    if (!map_) {
      map_=new PropertyMap;
    }    
    return (*map_)[key];
  }
  
  bool HasProp(const String& key) const
  {
    return map_ && map_->find(key) != map_->end();
  }
  
  void ClearProps()
  {
    if (map_) {
      map_->clear();      
    }
  }
  
  void RemoveProp(const String& key)
  {
    if (map_) {
      map_->erase(key);
    }
  }

  void Assign(const GenericPropContainerImpl& impl)
  {
    if (impl.map_) {
      if (!map_) {
        map_=new PropertyMap(*impl.map_);
      } else {
        *map_=*impl.map_;
      }
    } else {
      this->ClearProps();
    }
  }
  
  PropertyMap GetPropMap() const
  {
    if (!map_) {
      map_=new PropertyMap;
    }    
    return *map_;
  }
      
  std::vector<String> GetPropList() const
  {
    std::vector<String> prop_list;
    if (map_) {
      PropertyMap::const_iterator i;
      for (i=map_->begin(); i!=map_->end(); ++i) {
        prop_list.push_back(i->first);
      }
    }
    return prop_list;
  }
  
private:
  mutable PropertyMap* map_;
};  

template <typename H>
class TEMPLATE_EXPORT ConstGenericPropContainer {
protected:
  
  template<typename T>
  T gp_get(const String& key) const {
    if(HasProp(key)) {
      return boost::get<T>(GetImpl()->GenericProp(key));
    } else {
      std::ostringstream m("");
      m << "unknown property " << key;
      throw GenericPropError(m.str());
    }
  }

  template<typename T>
  T gp_get(const String& key, const T& def) const {
    if(HasProp(key)) {
      return boost::get<T>(GetImpl()->GenericProp(key));
    }
    return def;
  }
  GenericPropContainerImpl* GetImpl() 
  {
    return static_cast<H*>(this)->GpImpl();
  }

  const GenericPropContainerImpl* GetImpl() const
  {
    return static_cast<const H*>(this)->GpImpl();
  }  
  
public:
  /// \brief checks existence of property
  bool HasProp(const String& key) const {
    CheckHandleValidity(*static_cast<const H*>(this));
    return this->GetImpl()->HasProp(key);
  }
    
  /// \brief returns a String representation of stored value
  /// 
  /// returns the String representation of this property, or the empty String if 
  /// the property addressed by key does not exist. Note that this is not the 
  /// same as trying to get a generic float/int/bool property as a String type; 
  /// the latter will result in a boost:get exception. Use this method to obtain 
  /// a representation suitable for output.
  String GetPropAsString(const String& key) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    if(!HasProp(key)) return "";
    std::ostringstream rep("");
    rep << this->GetImpl()->GenericProp(key);
    return rep.str();
  }  

  /// \brief returns String property, raises an exception if it does not exist
  String GetStringProp(const String& key) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));
    return this->gp_get<String>(key);
  }

  /// \brief returns floating point property, raises an exception if it does 
  ///     not exist
  Real GetFloatProp(const String& key) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    if(HasProp(key)) {
      GenericPropValue value=this->GetImpl()->GenericProp(key);
      if (value.which()==1) {
        return boost::get<Real>(this->GetImpl()->GenericProp(key));
      } else if (value.which()==2) {
        return boost::get<int>(this->GetImpl()->GenericProp(key));
      }
      std::ostringstream m("");
      m << "property '" << key << "' is not numeric";
      throw GenericPropError(m.str());
    } else {
      std::ostringstream m("");
      m << "unknown property " << key;
      throw GenericPropError(m.str());
    }
  }


  /// \brief returns integer property, raises an exception if it does not exist
  int GetIntProp(const String& key) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->gp_get<int>(key);
  }

  /// \brief returns boolean property, raises an exception if it does not exist
  bool GetBoolProp(const String& key) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->gp_get<bool>(key);
  }

  /// \brief returns String property, or the given default if it does not exist
  String GetStringProp(const String& key, const String& def) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->gp_get<String>(key,def);
  }

  /// \brief returns floating point property, or the given default if it does
  ///     not exist
  Real GetFloatProp(const String& key, Real def) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    if(this->HasProp(key)) {
      GenericPropValue value=GetImpl()->GenericProp(key);
      if (value.which()==1) {
        return boost::get<Real>(GetImpl()->GenericProp(key));
      } else if (value.which()==2) {
        return boost::get<int>(GetImpl()->GenericProp(key));
      }
      std::ostringstream m("");
      m << "property '" << key << "' is not numeric";
      throw GenericPropError(m.str());
    } else {
      return def;
    }
  }

  /// \brief returns integer property, or the given default if it does not exist
  int GetIntProp(const String& key, int def) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->gp_get<int>(key, def);
  }

  /// \brief returns boolean property, or the given default if it does not exist
  bool GetBoolProp(const String& key, bool def) const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->gp_get<bool>(key, def);
  }

  std::map<String,GenericPropValue> GetPropMap() const
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    return this->GetImpl()->GetPropMap();
  }  

  std::vector<String> GetPropList() const
  {
    CheckHandleValidity(*static_cast<const H*>(this));
    return this->GetImpl()->GetPropList();
  }
};

/// \brief base class for the handler classes
template <typename H>
class  TEMPLATE_EXPORT GenericPropContainer : 
   public ConstGenericPropContainer<H>
{
public:
  void ClearProps()
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    this->GetImpl()->ClearProps();
  }
  
  /// \brief sets String property
  void SetStringProp(const String& key, const String& value)
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    this->GetImpl()->GenericProp(key)=value;
  }

  /// \brief sets floating point property
  void SetFloatProp(const String& key, Real value)
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    this->GetImpl()->GenericProp(key)=value;
  }

  /// \brief sets integer property
  void SetIntProp(const String& key, int value)
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    this->GetImpl()->GenericProp(key)=value;
  }

  /// \ brief sets boolean property
  void SetBoolProp(const String& key, bool value)
  {
    CheckHandleValidity(*static_cast<const H*>(this));    
    this->GetImpl()->GenericProp(key)=value;
  }
  
  void RemoveProp(const String& key)
  {
    CheckHandleValidity(*static_cast<const H*>(this));
    this->GetImpl()->RemoveProp(key);
  }
};


} // ns

#endif
