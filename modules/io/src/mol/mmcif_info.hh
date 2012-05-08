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
#include <map>
#include <boost/shared_ptr.hpp>
#include <ost/geom/geom.hh>
#include <ost/string_ref.hh>
#include <ost/io/module_config.hh>

namespace ost { namespace io {

class DLLEXPORT_OST_IO MMCifInfoStructDetails {
public:
  /// \brief Create a details object.
  MMCifInfoStructDetails(): entry_id_(""), title_(""), casp_flag_('\0'),
    descriptor_(""), mass_(0.0), mass_method_(""), model_details_(""),
    model_type_details_("") {};

  /// \brief Set id.
  ///
  /// \param id id
  void SetEntryID(String id) { entry_id_ = id; }
  /// \brief Get id.
  ///
  /// \return id
  String GetEntryID() const { return entry_id_; }

  /// \brief Set CASP flag.
  ///
  /// \param flag flag
  void SetCASPFlag(char flag) { casp_flag_ = flag; }
  /// \brief Get CASP flag.
  ///
  /// \return flag
  char GetCASPFlag() const { return casp_flag_; }

  /// \brief Set descriptor.
  ///
  /// \param desc descriptor for an NDB structure or PDB COMPND record
  void SetDescriptor(String desc) { descriptor_ = desc; }
  /// \brief Get CASP flag.
  ///
  /// \return flag
  String GetDescriptor() const { return descriptor_; }

  /// \brief Set mass.
  ///
  /// \param mass molecular mass
  void SetMass(Real mass) { mass_ = mass; }
  /// \brief Get molecular weight.
  ///
  /// \return mass
  Real GetMass() const { return mass_; }

  /// \brief Set the method how the molecular weight was detected.
  ///
  /// \param method detection method
  void SetMassMethod(String method) { mass_method_ = method; }
  /// \brief Get the method how the molecular weight was determined.
  ///
  /// \return method
  String GetMassMethod() const { return mass_method_; }

  /// \brief Set the description about the production of this structure.
  ///
  /// \param desc explaination
  void SetModelDetails(String desc) { model_details_ = desc; }
  /// \brief Get the details how the structure was determined.
  ///
  /// \return details
  String GetModelDetails() const { return model_details_; }

  /// \brief Set a description for the type of the structure model.
  ///
  /// \param desc explaination
  void SetModelTypeDetails(String desc) { model_type_details_ = desc; }
  /// \brief Get the description for the type of the structure model.
  ///
  /// \return details
  String GetModelTypeDetails() const { return model_type_details_; }

  /// \brief Set a title for the data block.
  ///
  /// \param title title
  void SetTitle(String title) { title_ = title; }
  /// \brief Get the title of the structure model.
  ///
  /// \return title
  String GetTitle() const { return title_; }

  bool operator==(const MMCifInfoStructDetails& sd) const {
    if (this->entry_id_ != sd.entry_id_) {
      return false;
    }
    if (this->casp_flag_ != sd.casp_flag_) {
      return false;
    }
    if (this->descriptor_ != sd.descriptor_) {
      return false;
    }
    if (this->mass_ != sd.mass_) {
      return false;
    }
    if (this->mass_method_ != sd.mass_method_) {
      return false;
    }
    if (this->model_details_ != sd.model_details_) {
      return false;
    }
    if (this->model_type_details_ != sd.model_type_details_) {
      return false;
    }
    if (this->title_ != sd.title_) {
      return false;
    }

    return true;
  }

  bool operator!=(const MMCifInfoStructDetails& sd) const {
    return !this->operator == (sd);
  }

private:
  String entry_id_;
  String title_;
  char   casp_flag_;
  String descriptor_;
  Real   mass_;
  String mass_method_;
  String model_details_;
  String model_type_details_;
};

class DLLEXPORT_OST_IO MMCifInfoTransOp {
public:
  /// \brief Create an operation
  MMCifInfoTransOp(): id_(""), type_("")
  {
    translation_ = geom::Vec3();
  };

  /// \brief Set id
  ///
  /// \param id id
  void SetID(String id) { id_ = id; }
  /// \brief Get id
  ///
  /// \return id
  String GetID() const { return id_; }

  /// \brief Set type
  ///
  /// \param type
  void SetType(String type) { type_ = type; }
  /// \brief Get type
  ///
  /// \return type
  String GetType() const { return type_; }

  /// \brief Set the translational vector
  ///
  /// \param 
  void SetVector(Real x, Real y, Real z)
  {
    translation_.SetX(x);
    translation_.SetY(y);
    translation_.SetZ(z);
  }
  /// \brief Get the translational vector
  ///
  /// \return vector
  geom::Vec3 GetVector() const { return translation_; }

  /// \brief Set the rotational matrix
  ///
  /// \param 
  void SetMatrix(Real i00, Real i01, Real i02,
                 Real i10, Real i11, Real i12,
                 Real i20, Real i21, Real i22)
  {
    rotation_ = geom::Mat3(i00,i01,i02, i10,i11,i12, i20,i21,i22);
  }
  /// \brief Get the rotational matrix
  ///
  /// \return matrix
  geom::Mat3 GetMatrix() const { return rotation_; }

  bool operator==(const MMCifInfoTransOp& op) const {
    if (this->id_ != op.id_) {
      return false;
    }
    if (this->type_ != op.type_) {
      return false;
    }
    if (this->translation_ != op.translation_) {
      return false;
    }
    if (this->rotation_ != op.rotation_) {
      return false;
    }

    return true;
  }

  bool operator!=(const MMCifInfoTransOp& op) const {
    return !this->operator==(op);
  }

private:
  String id_;              ///< identifier
  String type_;            ///< type of operation
  geom::Vec3 translation_; ///< translational vector
  geom::Mat3 rotation_;    ///< rotational matrix
};
typedef boost::shared_ptr<MMCifInfoTransOp> MMCifInfoTransOpPtr;


class DLLEXPORT_OST_IO MMCifInfoBioUnit {
public:
  /// \brief Create a biounit.
  MMCifInfoBioUnit(): details_("") {};

  /// \brief Set details
  ///
  /// \param details details
  void SetDetails(String details) { details_ = details; }
  /// \brief Get details
  ///
  /// \return details
  String GetDetails() const { return details_; }

  /// \brief Add a chain name
  ///
  /// \param chain chain name
  void AddChain(String chain) { chains_.push_back(chain); }
  /// \brief Get vector of chain names
  ///
  /// \return chains
  const std::vector<String>& GetChainList() const { return chains_; }

  /// \brief Add a set of operations
  ///
  /// \param operations vector of operations to be added
  void AddOperations(std::vector<MMCifInfoTransOpPtr> operations)
  {
    operations_.push_back(operations);
  }
  /// \brief Get the list of operations
  ///
  /// \return vector of vectors of iterators.
  const std::vector<std::vector<MMCifInfoTransOpPtr> >& GetOperations()
  {
    return operations_;
  }

  bool operator==(const MMCifInfoBioUnit& bu) const {
    if (this->details_ != bu.details_) {
      return false;
    }
    if (this->chains_ != bu.chains_) {
      return false;
    }
    if (this->operations_.size() == bu.operations_.size()) {
      std::vector<std::vector<MMCifInfoTransOpPtr> >::const_iterator th_ops_it;
      std::vector<std::vector<MMCifInfoTransOpPtr> >::const_iterator bu_ops_it;
      std::vector<MMCifInfoTransOpPtr>::const_iterator th_op_it;
      std::vector<MMCifInfoTransOpPtr>::const_iterator bu_op_it;

      for (th_ops_it = this->operations_.begin(),
             bu_ops_it = bu.operations_.begin();
           th_ops_it != this->operations_.end();
           ++th_ops_it, ++bu_ops_it) {
        if (th_ops_it->size() == bu_ops_it->size()) {
          for (th_op_it = th_ops_it->begin(), bu_op_it = bu_ops_it->begin();
               th_op_it != th_ops_it->end();
               ++th_op_it, ++bu_op_it) {
            if (*th_op_it != *bu_op_it) {
              return false;
            }
          }
        } else {
          return false;
        }
      }
    } else {
      return false;
    }

    return true;
  }

  bool operator!=(const MMCifInfoBioUnit& bu) const {
    return !this->operator==(bu);
  }

private:
  String details_;             ///< pdbx_struct_assembly.details
  std::vector<String> chains_; ///< chains involved in this assembly
  std::vector<std::vector<MMCifInfoTransOpPtr> > operations_;
};

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

/// \brief container class for information on obsolete entries
/// 
class DLLEXPORT_OST_IO MMCifInfoObsolete {
public:
  /// \brief Create an object of information baout an obsolete entry.
  MMCifInfoObsolete(): date_(""), id_(UNKNOWN), pdb_id_(""),
    replaced_pdb_id_("") {};

  /// \brief Set date of replacement.
  ///
  /// \param date
  void SetDate(String date) { date_ = date; }

  /// \brief Get the date string.
  ///
  /// \return date as string.
  String GetDate() { return date_; }

  /// \brief Set type of entry.
  ///
  /// \param type
  void SetID(StringRef type)
  {
    if (type == StringRef("OBSLTE", 6)) {
      id_ = OBSLTE;
    }
    else if (type == StringRef("SPRSDE", 6)) {
      id_ = SPRSDE;
    }
  }

  /// \brief Get type of entry.
  ///
  /// \return type as string, starting with an upper case letter.
  String GetID()
  {
    if (id_ == OBSLTE) {
      return "Obsolete";
    }
    if (id_ == SPRSDE) {
      return "Supersede";
    }
    return "Unknown";
  }

  /// \brief Set id of replacement.
  ///
  /// \param id
  void SetPDBID(String id) { pdb_id_ = id; }

  /// \brief Get id of replacement.
  ///
  /// \return id
  String GetPDBID() { return pdb_id_; }

  /// \brief Set id of replaced entry.
  ///
  /// \param id
  void SetReplacedPDBID(String id) { replaced_pdb_id_ = id; }

  /// \brief Get id of replaced entry.
  ///
  /// \return id
  String GetReplacedPDBID() { return replaced_pdb_id_; }

private:
  /// \enum types of obsolete entries
  typedef enum {
    OBSLTE,
    SPRSDE,
    UNKNOWN
  } MMCifObsoleteType;

  String date_;            ///< date of replacement
  MMCifObsoleteType id_;   ///< type of entry
  String pdb_id_;          ///< replacing entry
  String replaced_pdb_id_; ///< replaced entry
};

class MMCifInfoStructRef;
class MMCifInfoStructRefSeq;
class MMCifInfoStructRefSeqDif;


typedef boost::shared_ptr<MMCifInfoStructRef> MMCifInfoStructRefPtr;
typedef boost::shared_ptr<MMCifInfoStructRefSeq> MMCifInfoStructRefSeqPtr;
typedef boost::shared_ptr<MMCifInfoStructRefSeqDif> MMCifInfoStructRefSeqDifPtr;

typedef std::vector<MMCifInfoStructRefPtr> MMCifInfoStructRefs;
typedef std::vector<MMCifInfoStructRefSeqPtr> MMCifInfoStructRefSeqs;
typedef std::vector<MMCifInfoStructRefSeqDifPtr> MMCifInfoStructRefSeqDifs;
class DLLEXPORT_OST_IO MMCifInfoStructRef {
public:
  MMCifInfoStructRef(const String& id, const String& ent_id, 
  		               const String& db_name, 
  		               const String& db_ident, const String& db_access):
  	id_(id), ent_id_(ent_id), db_name_(db_name), db_ident_(db_ident), 
  	db_access_(db_access)
	{ }
  const String& GetID() const { return id_; }
  const String& GetDBName() const { return db_name_; }
  const String& GetDBID() const { return db_ident_; }
  const String& GetEntityID() const { return ent_id_; }
  const String& GetDBAccess() const { return db_access_; }
  MMCifInfoStructRefSeqPtr AddAlignedSeq(const String& align_id, 
  		                                   const String& chain_name, int seq_begin, 
  		                                   int seq_end, int db_begin, int db_end);
  MMCifInfoStructRefSeqPtr GetAlignedSeq(const String& align_id) const;
  MMCifInfoStructRefSeqs GetAlignedSeqs() const
	{
		MMCifInfoStructRefSeqs seqs;
		seqs.reserve(seqs_.size());
		for (std::map<String, MMCifInfoStructRefSeqPtr>::const_iterator
				 i=seqs_.begin(), e=seqs_.end(); i!=e; ++i) {
		  seqs.push_back(i->second);
		}
		return seqs;
	}
private:
	String  id_;
	String  ent_id_;
	String  db_name_;
	String  db_ident_;
	String  db_access_;
	std::map<String, MMCifInfoStructRefSeqPtr> seqs_;
};

class DLLEXPORT_OST_IO MMCifInfoStructRefSeq {
public:
  MMCifInfoStructRefSeq(const String& align_id, const String& chain_name, 
  		                  int seq_begin, int seq_end, 
  		                  int db_begin, int db_end):
  	id_(align_id), chain_name_(chain_name), 
  	seq_begin_(seq_begin), seq_end_(seq_end), db_begin_(db_begin), db_end_(db_end)
	{ }

  const String& GetID() const { return id_; }
  const String& GetChainName() const { return chain_name_; }
  int GetSeqBegin() const { return seq_begin_; }
  int GetSeqEnd() const { return seq_end_; }
  int GetDBBegin() const { return db_begin_; }
  int GetDBEnd() const { return db_end_; }
  MMCifInfoStructRefSeqDifPtr AddDif(int seq_num, const String& details);
  const std::vector<MMCifInfoStructRefSeqDifPtr>& GetDifs() const { return difs_; }
private:
	String   id_;
	String   chain_name_;
	int      seq_begin_;
	int      seq_end_;
	int      db_begin_;
	int      db_end_;
	std::vector<MMCifInfoStructRefSeqDifPtr> difs_;
};

class DLLEXPORT_OST_IO MMCifInfoStructRefSeqDif {
public:
	MMCifInfoStructRefSeqDif(int rnum, const String& details): 
		rnum_(rnum), details_(details) {}
	int GetRNum() const { return rnum_;}
	const String& GetDetails() const { return details_; }
private:
	int    rnum_;
	String details_;
};

/// \brief container class for additional information from MMCif files
/// 
/// \section mmcif annotation information
///
/// MMCif files contain loads of additional information beside coordinates.
/// This class is set up to capture some of it. In detail, we have:
/// 
/// \li citations
/// \li biounits
/// \li transformation information from asym. unit to biounit
/// \li structure information
/// \li resolution
/// \li method
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

  /// \brief Add a biounit
  ///
  /// \param bu biounit to be added
  void AddBioUnit(MMCifInfoBioUnit bu) // unit test
  {
    biounits_.push_back(bu);
  }

  /// \brief Get the list of biounits stored in an info object.
  ///
  /// \return vector of MMCifInfoBioUnit objects
  const std::vector<MMCifInfoBioUnit>& GetBioUnits() const
  {
    return biounits_;
  }

  /// \brief Add a operation
  ///
  /// \param op operation to be added
  void AddOperation(MMCifInfoTransOpPtr op) // unit test
  {
    transops_.push_back(op);
  }

  /// \brief Get the list of operations stored in an info object.
  ///
  /// \return vector of MMCifInfoTransOp objects
  const std::vector<MMCifInfoTransOpPtr>& GetOperations() const
  {
    return transops_;
  }

  /// \brief Add a set of structure details
  ///
  /// \param details info block to be added
  void SetStructDetails(MMCifInfoStructDetails details)
  {
    struct_details_ = details;
  }

  /// \brief Get the list of details about structures.
  ///
  /// \return vector of MMCifInfoStructDetails objects
  const MMCifInfoStructDetails GetStructDetails() const
  {
    return struct_details_;
  }

  /// \brief Add a block of information on obsolete entries
  ///
  /// \param obsolete
  void SetObsoleteInfo(MMCifInfoObsolete obsolete)
  {
    obsolete_ = obsolete;
  }

  /// \brief Get information on an obsolete entries
  ///
  /// \return MMCifInfoObsolete object
  MMCifInfoObsolete GetObsoleteInfo() const
  {
    return obsolete_;
  }
  const MMCifInfoStructRefs& GetStructRefs() const { return struct_refs_; }
  void SetStructRefs(const MMCifInfoStructRefs& sr) { struct_refs_=sr; }
//protected:

private:
  // members
  String exptl_method_;
  Real resolution_;
  MMCifInfoStructDetails struct_details_;     ///< mmCIF struct category
  MMCifInfoObsolete obsolete_;                ///< obsolete/ superseded entry
  std::vector<MMCifInfoCitation> citations_;  ///< list of citations
  std::vector<MMCifInfoBioUnit>  biounits_;   ///< list of biounits
  std::vector<MMCifInfoTransOpPtr> transops_;
	MMCifInfoStructRefs            struct_refs_;
};


}} // ns

#endif
