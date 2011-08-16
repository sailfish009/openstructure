//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#ifndef OST_MMCIF_INFO_HH
#define OST_MMCIF_INFO_HH

#include <vector>
#include <ost/string_ref.hh>
#include <ost/io/module_config.hh>

namespace ost { namespace io {

class DLLEXPORT_OST_IO MMCifInfoCitation {
public:
  /// \brief Create a citation.
  MMCifInfoCitation(): id_(""), where_(UNKNOWN), cas_(""), published_in_(""),
    volume_(""), page_first_(""), page_last_(""), doi_(""), pubmed_(0),
    year_(0), title_("") {};

  /// \brief Set ID
  ///
  /// \param id ID
  void SetID(String id) { id_ = id; }
  /// \brief Get ID
  ///
  /// \return ID
  String GetID() const { return id_; }

  /// \brief Set a CAS identifier
  ///
  /// \param id CAS identifier
  void SetCAS(String id) { cas_ = id; }
  /// \brief Get a CAS identifier
  ///
  /// \return CAS identifier
  String GetCAS() const { return cas_; }

  /// \brief Set an ISBN code
  ///
  /// \param code ISBN code
  void SetISBN(String code) { isbn_ = code; }

  /// \brief Get an ISBN code
  ///
  /// \return ISBN code
  String GetISBN() const { return isbn_; }

  /// \brief Set a book title or journal name
  ///
  /// \param title where published
  void SetPublishedIn(String title) { published_in_ = title; }

  /// \brief Get a book title or journal name
  ///
  /// \return title
  String GetPublishedIn() const { return published_in_; }

  /// \brief Set a journal volume
  ///
  /// \param volume
  void SetVolume(String volume) { volume_ = volume; }

  /// \brief Get a journal volume
  ///
  /// \return volume
  String GetVolume() const { return volume_; }

  /// \brief Set the start page for a publication
  ///
  /// \param start
  void SetPageFirst(String first) { page_first_ = first; }

  /// \brief Get the start page of a publication
  ///
  /// \return first page
  String GetPageFirst() const { return page_first_; }

  /// \brief Set the end page for a publication
  ///
  /// \param end
  void SetPageLast(String last) { page_last_ = last; }

  /// \brief Get the last page of a publication
  ///
  /// \return last page
  String GetPageLast() const { return page_last_; }

  /// \brief Set the DOI of a document
  ///
  /// \param doi
  void SetDOI(String doi) { doi_ = doi; }


  /// \brief Get the DOI of a document
  ///
  /// \return DOI
  String GetDOI() const { return doi_; }

  /// \brief Set the PubMed accession number
  ///
  /// \param no
  void SetPubMed(int no) { pubmed_ = no; }

  /// \brief Get the PubMed accession number
  ///
  /// \return PubMed accession
  int GetPubMed() const { return pubmed_; }

  /// \brief Set the year of a publication
  ///
  /// \param year
  void SetYear(int year) { year_ = year; }


  /// \brief Get the year of a publication
  ///
  /// \return year
  int GetYear() const { return year_; }


  /// \brief Set the title of a publication
  ///
  /// \param title
  void SetTitle(String title) { title_ = title; }

  /// \brief Get the title of a publication
  ///
  /// \return title
  String GetTitle() const { return title_; }

  /// \brief Set the list of authors
  ///
  /// \param list
  void SetAuthorList(std::vector<String> list) { authors_ = list; }

  /// \brief Get the list of authors
  ///
  /// \return list
  const std::vector<String>& GetAuthorList() const { return authors_; }

  bool operator==(const MMCifInfoCitation& cit) const {
    if (this->year_ != cit.year_) {
      return false;
    }
    if (this->pubmed_ != cit.pubmed_) {
      return false;
    }
    if (this->where_ != cit.where_) {
      return false;
    }
    if (StringRef(this->id_.c_str(), this->id_.length()) !=
        StringRef(cit.id_.c_str(), cit.id_.length())) {
      return false;
    }
    if (StringRef(this->cas_.c_str(), this->cas_.length()) !=
        StringRef(cit.cas_.c_str(), cit.cas_.length())) {
      return false;
    }
    if (StringRef(this->isbn_.c_str(), this->isbn_.length()) !=
        StringRef(cit.isbn_.c_str(), cit.isbn_.length())) {
      return false;
    }
    if (StringRef(this->published_in_.c_str(), this->published_in_.length()) !=
        StringRef(cit.published_in_.c_str(), cit.published_in_.length())) {
      return false;
    }
    if (StringRef(this->volume_.c_str(), this->volume_.length()) !=
        StringRef(cit.volume_.c_str(), cit.volume_.length())) {
      return false;
    }
    if (StringRef(this->page_first_.c_str(), this->page_first_.length()) !=
        StringRef(cit.page_first_.c_str(), cit.page_first_.length())) {
      return false;
    }
    if (StringRef(this->page_last_.c_str(), this->page_last_.length()) !=
        StringRef(cit.page_last_.c_str(), cit.page_last_.length())) {
      return false;
    }
    if (StringRef(this->doi_.c_str(), this->doi_.length()) !=
        StringRef(cit.doi_.c_str(), cit.doi_.length())) {
      return false;
    }
    if (StringRef(this->title_.c_str(), this->title_.length()) !=
        StringRef(cit.title_.c_str(), cit.title_.length())) {
      return false;
    }
    if (this->authors_ != cit.authors_) {
      return false;
    }

    return true;
  }

  bool operator!=(const MMCifInfoCitation& cit) const {
    return !this->operator==(cit);
  }

private:
  /// \enum types of citations
  typedef enum {
    JOURNAL,
    BOOK,
    UNKNOWN
  } MMCifInfoCType;

  //CITATION_ID
  String              id_;           ///< internal identifier
  MMCifInfoCType      where_;        ///< journal or book?
  String              cas_;          ///< CAS identifier
  String              isbn_;         ///< ISBN no. of medium
  String              published_in_; ///< book title or full journal name
  String              volume_;       ///< journal volume
  String              page_first_;   ///< first page
  String              page_last_;    ///< last page
  String              doi_;          ///< DOI identifier
  int                 pubmed_;       ///< accession no.
  int                 year_;         ///< year of publication
  String              title_;        ///< title of the publication
  std::vector<String> authors_;       ///< author information
};

/// \brief container class for additional information from MMCif files
/// 
/// \section mmcif annotation information
///
/// MMCif files contain loads of additional information beside coordinates.
/// This class is set up to capture some of it. In detail, we have:
/// 
/// \li citations
class DLLEXPORT_OST_IO MMCifInfo {
public:
  /// \brief Create an info object.
  MMCifInfo(): exptl_method_(""), resolution_(0.0f) {};

  /// \brief Add an item to the list of citations
  ///
  /// \param citation to be added
  void AddCitation(MMCifInfoCitation citation) // unit test
  {
    citations_.push_back(citation);
  }

  /// \brief Add a list of authors to a specific citation.
  ///
  /// \param id identifier of the citation to be modified.
  /// \param list list of authors to be added.
  void AddAuthorsToCitation(StringRef id, std::vector<String> list); //unit test

  /// \brief Get the list of citations stored in an info object.
  ///
  /// \return vector of MMCifInfoCitation objects
  const std::vector<MMCifInfoCitation>& GetCitations() const
  {
    return citations_;
  }

  /// \brief Set an experimental method.
  ///
  /// \param method Method description
  void SetMethod(String method) { exptl_method_ = method; }

  /// \brief Get an experimental method.
  ///
  /// \return Method description
  const StringRef GetMethod() const
  { 
    return StringRef(exptl_method_.c_str(), exptl_method_.length());
  }

  /// \brief Set resolution.
  ///
  /// \param res experiment resolution
  void SetResolution(Real res) { resolution_ = res; }

  /// \brief Get resolution.
  ///
  /// \return experiment resolution
  Real GetResolution() const { return resolution_; }

//protected:

private:
  // members
  String exptl_method_;
  Real resolution_;
  std::vector<MMCifInfoCitation> citations_; ///< list of citations
};

}} // ns

#endif
