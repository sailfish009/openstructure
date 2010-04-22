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

  the impl is derived from GenericPropertyContainerImpl

  the handle is derived from GenericPropertyContainer, and then has the
  setter and getter methods for String, Real, int and bool mapping
*/

#include <exception>
#include <sstream>
#include <map>
#include <boost/variant.hpp>

#include <ost/module_config.hh>

namespace ost {

struct  TEMPLATE_EXPORT GenericPropertyError: public std::exception
{
  GenericPropertyError(const String& m):
    m_(m)
  {}
  virtual ~GenericPropertyError() throw() {}
  virtual const char* what() const throw() {
    return m_.c_str();
  }
  String m_;
};
  
typedef boost::variant<String, Real, int, bool> GenericPropertyValue;

///  \brief base class for the implementation
class  TEMPLATE_EXPORT GenericPropertyContainerImpl
{
  typedef std::map<String,GenericPropertyValue> PropertyMap;
  
public:
  GenericPropertyContainerImpl(): map_(NULL) {}
  ~GenericPropertyContainerImpl()
  {
    if (map_) {
      delete map_;
    }
  }
  GenericPropertyContainerImpl(const GenericPropertyContainerImpl& rhs):
    map_(rhs.map_ ? new PropertyMap(*rhs.map_) : NULL)
  { }
  
  GenericPropertyContainerImpl& operator=(const GenericPropertyContainerImpl& r)   
  {
    this->Assign(r);
    return *this;
  }
  
  GenericPropertyValue& GenericProperty(const String& key) 
  {
    if (!map_) {
      map_=new PropertyMap;
    }
    return (*map_)[key];
  }
  
  const GenericPropertyValue& GenericProperty(const String& key) const
  {
    if (!map_) {
      map_=new PropertyMap;
    }    
    return (*map_)[key];
  }
  
  bool HasGenericProperty(const String& key) const
  {
    return map_ && map_->find(key) != map_->end();
  }
  
  void ClearGenericProperties()
  {
    if (map_) {
      map_->clear();      
    }
  }
  
  void Assign(const GenericPropertyContainerImpl& impl)
  {
    if (impl.map_) {
      if (!map_) {
        map_=new PropertyMap(*impl.map_);
      } else {
        *map_=*impl.map_;
      }
    } else {
      this->ClearGenericProperties();
    }
  }
  
  PropertyMap GetGenericPropertyMap() const
  {
    if (!map_) {
      map_=new PropertyMap;
    }    
    return *map_;
  }
      
  
private:
  mutable PropertyMap* map_;
};  

template <typename H>
class TEMPLATE_EXPORT ConstGenericPropertyContainer {
protected:
  
  template<typename T>
  T gp_get(const String& key) const {
    if(HasGenericProperty(key)) {
      return boost::get<T>(GetImpl()->GenericProperty(key));
    } else {
      std::ostringstream m("");
      m << "unknown property " << key;
      throw GenericPropertyError(m.str());
    }
  }

  template<typename T>
  T gp_get(const String& key, const T& def) const {
    if(HasGenericProperty(key)) {
      return boost::get<T>(GetImpl()->GenericProperty(key));
    }
    return def;
  }
  GenericPropertyContainerImpl* GetImpl() 
  {
    return static_cast<H*>(this)->GpImpl();
  }

  const GenericPropertyContainerImpl* GetImpl() const
  {
    return static_cast<const H*>(this)->GpImpl();
  }  
public:
  /// \brief checks existence of property
  bool HasGenericProperty(const String& key) const {
    return this->GetImpl()->HasGenericProperty(key);
  }
    
  /// \brief returns a String representation of stored value
  /// 
  /// returns the String representation of this property, or the empty String if 
  /// the property addressed by key does not exist. Note that this is not the 
  /// same as trying to get a generic float/int/bool property as a String type; 
  /// the latter will result in a boost:get exception. Use this method to obtain 
  /// a representation suitable for output.
  String GetGenericPropertyStringRepresentation(const String& key) const
  {
    if(!HasGenericProperty(key)) return "";
    std::ostringstream rep("");
    rep << this->GetImpl()->GenericProperty(key);
    return rep.str();
  }  
    /// \brief returns String property, raises an exception if it does not exist
    String GetGenericStringProperty(const String& key) const
    {
      return this->gp_get<String>(key);
    }

  /// \brief returns floating point property, raises an exception if it does 
  ///     not exist
  Real GetGenericFloatProperty(const String& key) const
  {
    if(HasGenericProperty(key)) {
      GenericPropertyValue value=this->GetImpl()->GenericProperty(key);
      if (value.which()==1) {
        return boost::get<Real>(this->GetImpl()->GenericProperty(key));
      } else if (value.which()==2) {
        return boost::get<int>(this->GetImpl()->GenericProperty(key));
      }
      std::ostringstream m("");
      m << "property '" << key << "' is not numeric";
      throw GenericPropertyError(m.str());
    } else {
      std::ostringstream m("");
      m << "unknown property " << key;
      throw GenericPropertyError(m.str());
    }
  }


  /// \brief returns integer property, raises an exception if it does not exist
  int GetGenericIntProperty(const String& key) const
  {
    return this->gp_get<int>(key);
  }

  /// \brief returns boolean property, raises an exception if it does not exist
  bool GetGenericBoolProperty(const String& key) const
  {
    return this->gp_get<bool>(key);
  }

  /// \brief returns String property, or the given default if it does not exist
  String GetGenericStringProperty(const String& key, const String& def) const
  {
    return this->gp_get<String>(key,def);
  }

  /// \brief returns floating point property, or the given default if it does
  ///     not exist
  Real GetGenericFloatProperty(const String& key, Real def) const
  {
    if(this->HasGenericProperty(key)) {
      GenericPropertyValue value=GetImpl()->GenericProperty(key);
      if (value.which()==1) {
        return boost::get<Real>(GetImpl()->GenericProperty(key));
      } else if (value.which()==2) {
        return boost::get<int>(GetImpl()->GenericProperty(key));
      }
      std::ostringstream m("");
      m << "property '" << key << "' is not numeric";
      throw GenericPropertyError(m.str());
    } else {
      return def;
    }
  }

  /// \brief returns integer property, or the given default if it does not exist
  int GetGenericIntProperty(const String& key, int def) const
  {
    return this->gp_get<int>(key, def);
  }

  /// \brief returns boolean property, or the given default if it does not exist
  bool GetGenericBoolProperty(const String& key, bool def) const
  {
    return this->gp_get<bool>(key, def);
  }

  std::map<String,GenericPropertyValue> GetGenericPropertyMap() const
  {
    return this->GetImpl()->GetGenericPropertyMap();
  }  
};

/// \brief base class for the handler classes
template <typename H>
class  TEMPLATE_EXPORT GenericPropertyContainer : 
   public ConstGenericPropertyContainer<H>
{
public:
  void ClearGenericProperties()
  {
    this->GetImpl()->ClearGenericProperties();
  }
  
  /// \brief sets String property
  void SetGenericStringProperty(const String& key, const String& value)
  {
    this->GetImpl()->GenericProperty(key)=value;
  }

  /// \brief sets floating point property
  void SetGenericFloatProperty(const String& key, Real value)
  {
    this->GetImpl()->GenericProperty(key)=value;
  }

  /// \brief sets integer property
  void SetGenericIntProperty(const String& key, int value)
  {
    this->GetImpl()->GenericProperty(key)=value;
  }

  /// \ brief sets boolean property
  void SetGenericBoolProperty(const String& key, bool value)
  {
    this->GetImpl()->GenericProperty(key)=value;
  }
};


} // ns

#endif
