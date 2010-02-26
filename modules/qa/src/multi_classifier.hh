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
#ifndef OST_QA_MULTI_CLASSIFIER_HH
#define OST_QA_MULTI_CLASSIFIER_HH

#include <ost/stdint.hh>
#include <vector>
#include <cassert>
#include <fstream>
#include <ost/message.hh>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "index.hh"
#include <ost/config.hh>

namespace ost { namespace qa {

namespace impl {
  // Determine length of a typelist, i.e. the number of template arguments
  // different from NullType. The length can be accessed over the Value
  // member.
  // usage example:
  //  int length=LengthOf<int, int>::Value;
  struct NullType {
    template <typename DS>
    void Serialize(DS&) {}
  };
  template <typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7>
  struct LengthOf;

  template <typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7>
  struct LengthOf {
    enum { Value = 1+LengthOf<T2, T3, T4, T5, T6, T7, NullType>::Value };
  };
  template <>
  struct LengthOf<NullType, NullType, NullType, NullType, NullType, NullType,
                  NullType>
  {
    enum { Value = 0 };
  };

  //! compile time if statement. If<C, T, F>::Type is set to T if C is true
  //  and set to F otherwise.
  template <bool C,
            typename T,
            typename F>
  struct If;

  template <typename T,
            typename F>
  struct If<true, T, F> {
    typedef T Type;
  };

  template <typename T,
            typename F>
  struct If<false, T, F> {
    typedef F Type;
  };

  //! Compile-time check for identity of two types. IsEqual<T1,T2>::Value
  //  is true if T1 and T2 are identical, and false otherwise.
  template <typename T1,
            typename T2>
  struct IsEqual;
  template <typename T1,
            typename T2>
  struct IsEqual {
    enum { Value = false };
  };
  template <typename T>
  struct IsEqual<T,T> {
    enum { Value = true };
  };

  // Compile-time selector statement to execute conditional statement based
  // on the identity of \c C. If C is identical to the NullType
  // then IfNull<...>::Type is a typedef of T, otherwise, IfNull<...>::Type is a
  // typedef for F.
  template <typename C, typename T, typename F>
  struct IfNull;

  template <typename C,
            typename T,
            typename F>
  struct IfNull {
    typedef typename If<IsEqual<NullType, C>::Value, T, F>::Type Type;
  };

}

//! Base class for classifiers.
class DLLEXPORT_OST_QA ClassifierBase {
public:
  ClassifierBase(uint32_t number_of_classes)
    : number_of_classes_(number_of_classes) {
  }
  ClassifierBase()
    : number_of_classes_(0) {}
  virtual ~ClassifierBase() {}
  uint32_t GetNumberOfClasses() const {
    return number_of_classes_;
  }
protected:
  uint32_t number_of_classes_;
};

//! Classifier for integral classes.
class DLLEXPORT_OST_QA IntegralClassifier : public ClassifierBase {
public:
  IntegralClassifier(uint32_t number_of_classes,
                     int lower_bound)
  : ClassifierBase(number_of_classes),
    lower_bound_(lower_bound) {
  }
  uint32_t GetIndexOf(int value) const {
    uint32_t idx=(value-lower_bound_);
    assert(this->GetNumberOfClasses()>idx);
    return idx;
  }
  IntegralClassifier()
    : ClassifierBase(0),
      lower_bound_(0) {
  }

  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & number_of_classes_;
    ds & lower_bound_;
  }
private:
  int32_t lower_bound_;
};

//! Classifier for real valued classes.
class DLLEXPORT_OST_QA ContinuousClassifier : public ClassifierBase {
public:
  ContinuousClassifier(uint32_t number_of_classes,
                       Real lower_bound,
                       Real upper_bound)
  : ClassifierBase(number_of_classes),
    lower_bound_(lower_bound),
    upper_bound_(upper_bound) {
  }
  uint32_t GetIndexOf(Real value) const {
    Real factor=(value-lower_bound_)/(upper_bound_-lower_bound_);
    uint32_t idx=uint32_t(floor(this->GetNumberOfClasses()*factor));
    assert(this->GetNumberOfClasses()>idx);
    return idx;
  }
  ContinuousClassifier()
    : ClassifierBase(1),
      lower_bound_(0), upper_bound_(1) {
  }
  template <typename DS>
  void Serialize(DS& ds)
  {
      ds & number_of_classes_;
      ds & lower_bound_;
      ds & upper_bound_;
  }
private:
  Real lower_bound_;
  Real upper_bound_;
};


template <typename T>
struct Classifier;

template <>
struct DLLEXPORT_OST_QA Classifier<int> {
  typedef IntegralClassifier           Type;
  typedef const IntegralClassifier&    ConstRefType;
  typedef IntegralClassifier&          RefType;
};
template <>
struct DLLEXPORT_OST_QA Classifier<Real> {
  typedef ContinuousClassifier         Type;
  typedef const ContinuousClassifier&  ConstRefType;
  typedef ContinuousClassifier&        RefType;
};
#if OST_DOUBLE_PRECISION
template <>
struct DLLEXPORT_OST_QA Classifier<float> {
  typedef ContinuousClassifier         Type;
  typedef const ContinuousClassifier&  ConstRefType;
  typedef ContinuousClassifier&        RefType;
};
#endif
template <>
struct DLLEXPORT_OST_QA Classifier<impl::NullType> {
  typedef impl::NullType               Type;
  typedef const impl::NullType&        ConstRefType;
  typedef impl::NullType&              RefType;
};

template <typename I>
struct DLLEXPORT_OST_QA NullFind {
  NullFind(const ClassifierBase&,uint32_t,const impl::NullType&,I&) {};
};
template <typename C, typename T, typename I>
struct IndexFind;

template <typename C, typename I>
struct DLLEXPORT_OST_QA IndexFind<C,impl::NullType,I> {
  IndexFind(const C&,
            uint32_t,
            const impl::NullType&, I&) {
  }
};

template <typename C, typename T, typename I>
struct DLLEXPORT_OST_QA IndexFind {
  IndexFind(const C& classifier, uint32_t i, const T& value, I& index) {
    index[i]=classifier.GetIndexOf(value);
  }
};
template <typename T>
struct NumberOfClasses;


template <>
struct DLLEXPORT_OST_QA NumberOfClasses<impl::NullType> {
  uint32_t operator ()(const impl::NullType& t) {
    return 1;
  }
};

template <typename T>
struct DLLEXPORT_OST_QA NumberOfClasses {
  uint32_t operator ()(const T& t) {
    return t.GetNumberOfClasses();
  }
};

//! \brief generic n-dimensional classifier
template <typename V, typename T1,
          typename T2=impl::NullType,
          typename T3=impl::NullType,
          typename T4=impl::NullType,
          typename T5=impl::NullType,
          typename T6=impl::NullType,
          typename T7=impl::NullType>
class DLLEXPORT_OST_QA MultiClassifier {
public:
  enum { Dimension = impl::LengthOf<T1, T2, T3, T4, T5, T6, T7>::Value };
  typedef V                                    ValueType;
  typedef Index<MultiClassifier::Dimension>    IndexType;
  typedef IndexIterator<Dimension>             Iterator;
  typedef Classifier<T1>                       C1;
  typedef Classifier<T2>                       C2;
  typedef Classifier<T3>                       C3;
  typedef Classifier<T4>                       C4;
  typedef Classifier<T5>                       C5;
  typedef Classifier<T6>                       C6;
  typedef Classifier<T7>                       C7;
#if WIN32
  MultiClassifier(const V& initial_value,
                  typename C1::ConstRefType c1,
                  typename C2::ConstRefType c2=C2::Type(),
                  typename C3::ConstRefType c3=C3::Type(),
                  typename C4::ConstRefType c4=C4::Type(),
                  typename C5::ConstRefType c5=C5::Type(),
                  typename C6::ConstRefType c6=C6::Type(),
                  typename C7::ConstRefType c7=C7::Type())
#else
  MultiClassifier(const V& initial_value,
                  typename C1::ConstRefType c1,
                  typename C2::ConstRefType c2=typename C2::Type(),
                  typename C3::ConstRefType c3=typename C3::Type(),
                  typename C4::ConstRefType c4=typename C4::Type(),
                  typename C5::ConstRefType c5=typename C5::Type(),
                  typename C6::ConstRefType c6=typename C6::Type(),
                  typename C7::ConstRefType c7=typename C7::Type())
#endif
    : classifier1_(c1), classifier2_(c2), classifier3_(c3),
      classifier4_(c4), classifier5_(c5), classifier6_(c6),
      classifier7_(c7) {
    this->ExtractNumberOfClasses();
    // allocate enough memory for all the buckets
    uint32_t total=this->CalculateNumberOfBuckets();
    buckets_.resize(total, initial_value);
  }

  MultiClassifier()
  {
    memset(number_of_classes_, 0, sizeof(number_of_classes_));
  }

  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & classifier1_;
    ds & classifier2_;
    ds & classifier3_;
    ds & classifier4_;
    ds & classifier5_;
    ds & classifier6_;
    ds & classifier7_;
    if (ds.IsSource()) {
      this->ExtractNumberOfClasses();
    }
    ds & buckets_;
  }

  MultiClassifier(const MultiClassifier& rhs)
    : classifier1_(rhs.classifier1_), classifier2_(rhs.classifier2_),
      classifier3_(rhs.classifier3_), classifier4_(rhs.classifier4_),
      classifier5_(rhs.classifier5_), classifier6_(rhs.classifier6_),
      classifier7_(rhs.classifier7_) {
        this->ExtractNumberOfClasses();
      uint32_t total=this->CalculateNumberOfBuckets();
      buckets_.resize(total);
      memcpy(&buckets_.front(), &rhs.buckets_.front(), sizeof(V)*total);
  }

  uint32_t GetBucketCount() const {
    return static_cast<uint32_t>(buckets_.size());
  }

  void Add(const ValueType& value,
           T1 x1=T1(), T2 x2=T2(),
           T3 x3=T3(), T4 x4=T4(),
           T5 x5=T5(), T6 x6=T6(),
           T7 x7=T7()) {
    IndexType index=this->FindBucket(x1, x2, x3, x4, x5, x6, x7);
    uint32_t linear_index=this->LinearizeBucketIndex(index);
    buckets_[linear_index]+=value;
  }

  const ValueType& Get(T1 x1=T1(), T2 x2=T2(),
                       T3 x3=T3(), T4 x4=T4(),
                       T5 x5=T5(), T6 x6=T6(), T7 x7=T7()) const {
    IndexType index=this->FindBucket(x1, x2, x3, x4, x5, x6, x7);
    uint32_t linear_index=this->LinearizeBucketIndex(index);
    return buckets_[linear_index];
  }

  const ValueType& Get(const IndexType& index) const
  {
    return buckets_[this->LinearizeBucketIndex(index)];
  }

  void Set(const IndexType& index, const ValueType& value)
  {
    buckets_[this->LinearizeBucketIndex(index)]=value;
  }
  //TODO Make sure that FindBucket is called with the correct number of
  //     arguments. Can be done over a wrapper type around T1..T6 that only
  //     provides a default c'tor when T is equal to NullType.
  IndexType FindBucket(T1 x1=T1(), T2 x2=T2(), T3 x3=T3(),
                       T4 x4=T4(), T5 x5=T5(), T6 x6=T6(),
                       T7 x7=T7()) const {
    // determine indices for parameters whose type is not equal to
    // NullType
    IndexType index;
    IndexFind<typename C1::Type, T1,
              IndexType> find_index_1(classifier1_, 0, x1, index);
    IndexFind<typename C2::Type, T2,
              IndexType> find_index_2(classifier2_, 1, x2, index);
    IndexFind<typename C3::Type, T3,
              IndexType> find_index_3(classifier3_, 2, x3, index);
    IndexFind<typename C4::Type, T4,
              IndexType> find_index_4(classifier4_, 3, x4, index);
    IndexFind<typename C5::Type, T5,
              IndexType> find_index_5(classifier5_, 4, x5, index);
    IndexFind<typename C6::Type, T6,
              IndexType> find_index_6(classifier6_, 5, x6, index);
    IndexFind<typename C7::Type, T7,
              IndexType> find_index_7(classifier7_, 6, x7, index);
    return index;
  }

  void Add(const ValueType& value, const IndexType& index)
  {
    buckets_[this->LinearizeBucketIndex(index)]+=value;
  }
private:
  void ExtractNumberOfClasses()
  {
    number_of_classes_[0]=NumberOfClasses<typename C1::Type>()(classifier1_);
    number_of_classes_[1]=NumberOfClasses<typename C2::Type>()(classifier2_);
    number_of_classes_[2]=NumberOfClasses<typename C3::Type>()(classifier3_);
    number_of_classes_[3]=NumberOfClasses<typename C4::Type>()(classifier4_);
    number_of_classes_[4]=NumberOfClasses<typename C5::Type>()(classifier5_);
    number_of_classes_[5]=NumberOfClasses<typename C6::Type>()(classifier6_);
    number_of_classes_[6]=NumberOfClasses<typename C7::Type>()(classifier7_);
  }

  uint32_t LinearizeBucketIndex(const IndexType& index) const
  {
    uint32_t factor=1;
    uint32_t linear_index=0;
    for (uint32_t i=0; i<MultiClassifier::Dimension; ++i) {
      linear_index+=factor*index[i];
      factor*=number_of_classes_[i];
    }
    return linear_index;
  }

  uint32_t CalculateNumberOfBuckets() const
  {
    uint32_t total=1;
    for (uint32_t i=0; i<MultiClassifier::Dimension; ++i) {
      total*=number_of_classes_[i];
    }
    return total;
  }
  typename C1::Type         classifier1_;
  typename C2::Type         classifier2_;
  typename C3::Type         classifier3_;
  typename C4::Type         classifier4_;
  typename C5::Type         classifier5_;
  typename C6::Type         classifier6_;
  typename C7::Type         classifier7_;
  uint32_t                  number_of_classes_[7];
  std::vector<ValueType>    buckets_;
};

}}

#endif
