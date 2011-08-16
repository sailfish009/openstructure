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
#ifndef OST_MMCIF_PARSER_HH
#define OST_MMCIF_PARSER_HH

#include <map>

//#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/filesystem/fstream.hpp>

#include <ost/seq/sequence_list.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/chain_type.hh>
#include <ost/conop/compound_lib.hh>
#include <ost/io/mol/io_profile.hh>
#include <ost/io/io_exception.hh>
#include <ost/io/mol/star_parser.hh>
#include <ost/io/mol/mmcif_info.hh>

namespace ost { namespace io {

/// \brief reader for the mmcif file format
/// 
/// \section mmcif_format mmcif format description/ coverage
/// 
/// mmcif is an instance of the \link StarParser STAR format\endlink to store
/// entries of the PDB. The following data categories should be covered by this
/// parser:
/// 
/// \li atom_site
/// \li entity
/// \li entity_poly
/// \li citation
class DLLEXPORT_OST_IO MMCifParser : public StarParser  {
public:
  /// \brief create a MMCifParser
  ///
  /// \param stream input stream
  MMCifParser(std::istream& stream, mol::EntityHandle& ent_handle,
              const IOProfile& profile);

  /// \brief create a MMCifParser
  ///
  /// \param filename input file
  MMCifParser(const String& filename, mol::EntityHandle& ent_handle,
              const IOProfile& profile);

  /// \brief Initialise the parser.
  ///
  /// \param loc Location of the file
  void Init();

  /// \brief Set up a fresh instance
  void ClearState();

  /// \brief Set names of restricted chains for the parser.
  ///
  /// \param restrict_chains chain name
  void SetRestrictChains(const String& restrict_chains);

  /// \brief Toggle reading of canonical sequence residues
  ///        (entity_poly.pdbx_seq_one_letter_code_can instead of
  ///        entity_poly.pdbx_seq_one_letter_code). This flag is exclusive.
  ///
  /// \param flag True for reading canonical sequences.
  void SetReadCanonicalSeqRes(bool flag)
  {
    seqres_can_ = flag;
  }

  const String& GetRestrictChains() const
  {
    return restrict_chains_;
  }

  /// \brief check mmcif input to be read. Substitutional function for
  /// \link StarParser StarParser\endlink.
  ///
  /// \param data_name value of the data_ tag
  ///
  /// \return true, if the blockcode (PDB id) is valid, false otherwise
  virtual bool OnBeginData(const StringRef& data_name);

  /// \brief check if a current loop is to be parsed
  ///
  /// \param header categories of the upcoming loop block
  ///
  /// \return bool
  virtual bool OnBeginLoop(const StarLoopDesc& header); // tested

  /// \brief read a row of data
  ///
  /// \param header categories and items
  /// \param columns data
  virtual void OnDataRow(const StarLoopDesc& header, 
                         const std::vector<StringRef>& columns);

  /// \brief Finalise parsing.
  virtual void OnEndData();

  /// \brief Return sequences
  ///
  /// \return List of sequences
  seq::SequenceList GetSeqRes() const {
    return seqres_;
  }

  /// \brief Toggle reading of SEQRES
  ///
  /// \param flag True enables, False disables reading SEQRES
  void SetReadSeqRes(bool flag)
  {
    read_seqres_ = flag;
  }

  /// \brief Check if reading of SEQRES is enabled
  ///
  /// \return True if reading of SEQRES is enabled
  bool GetReadSeqRes() const
  {
    return read_seqres_;
  }

  /// \brief Get additional information of the MMCif file.
  ///
  /// \return MMCitfInfo object
  const MMCifInfo& GetInfo() { return info_; }

protected:
  /// \brief Store an item index from loop header in preparation for reading a 
  ///        row. Throws an exception if the item does not exist.
  ///
  /// \param mapping position the item index is stored at
  /// \param item exact item name to fetch
  /// \param header loop header to pull index from
  void TryStoreIdx(const int mapping,
                   const String& item,
                   const StarLoopDesc& header)
  {
    indices_[mapping] = header.GetIndex(item);
    
    if (indices_[mapping] == -1) {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                               "No item '" + item +
                                               "' found in '" +
                                               header.GetCategory()+
                                               "' header",
                                               this->GetCurrentLinenum()));
     }
  } // tested

  /// \brief Check a PDB id to be of length 4 and start with a digit
  ///
  /// \param pdbid putative PDB id
  ///
  /// \return true for a valid id, false otherwise
  bool IsValidPDBIdent(const StringRef& pdbid); // tested

  /// \brief fetch values identifying atoms
  ///
  /// \param[in]  columns data row
  /// \param[out] chain_name takes atom_site.label_asym_id or, if
  ///             auth_chain_id_ is set, atom_site.auth_asym_id as a chain name
  /// \param[out] res_name fetches atom_site.label_comp_id
  /// \param[out] resnum gets atom_site.label_seq_id if available, consecutive
  ///             numbers, otherwise
  /// \param[out] valid_res_num shows if we have a valid residue number or if
  ///             we have to invent our own
  /// \param[out] atom_name corresponds to label_atom_id
  /// \param[out] alt_loc gets first letter of atom_site.label_alt_id
  bool ParseAtomIdent(const std::vector<StringRef>& columns,
                      String& chain_name,
                      StringRef& res_name,
                      mol::ResNum& resnum,
                      bool& valid_res_num,
                      StringRef& atom_name,
                      char& alt_loc);

  /// \brief Fetch atom information and store it.
  ///
  /// \param columns data row
  void ParseAndAddAtom(const std::vector<StringRef>& columns);

  /// \brief Fetch MMCif entity information
  ///
  /// \param columns data row
  void ParseEntity(const std::vector<StringRef>& columns);

  /// \brief Fetch MMCif entity_poly information
  ///
  /// \param columns data row
  void ParseEntityPoly(const std::vector<StringRef>& columns);

  /// \brief Fetch MMCif citation information
  ///
  /// \param columns data row
  void ParseCitation(const std::vector<StringRef>& columns);

  /// \brief convert the seqres data item to canonical form. 
  /// 
  /// The seqres sequence lists non-standard residues in paranthesis. For 
  /// proper handling of our sequence classes, these need to be converted to 
  /// one-letter-codes. Ideally, we would use the canonical SEQRES. This is 
  /// not possible, however, since the PDB assigns multiple one letter codes 
  /// to some of the residues. To be consistent, we have to do the conversion on 
  /// our own.
  String ConvertSEQRES(const String& seqres, conop::CompoundLibPtr compound_lib);
  /// \brief Fetch MMCif citation_author information
  ///
  /// \param columns data row
  void ParseCitationAuthor(const std::vector<StringRef>& columns);

  /// \brief Fetch MMCif exptl information
  ///
  /// \param columns data row
  void ParseExptl(const std::vector<StringRef>& columns);

private:
  /// \enum magic numbers of this class
  typedef enum {
    PDBID_LEN=4,         ///< length of a PDB id
    MAX_ITEMS_IN_ROW=18, ///< count for possible items in a loop row
  } MMCifMagicNos;

  /// \enum items of the atom_site category
  typedef enum {
    AUTH_ASYM_ID,      ///< chain name by author as in PDB
    AS_ID,             ///< atom serial id
    LABEL_ALT_ID,      ///< AltLoc
    LABEL_ASYM_ID,     ///< chain name by PDB
    LABEL_ATOM_ID,
    LABEL_COMP_ID,
    LABEL_ENTITY_ID,   ///< link to category entity
    LABEL_SEQ_ID,      ///< residue no.
    AUTH_SEQ_ID,       ///< residue no. by author
    TYPE_SYMBOL,       ///< chemical element
    CARTN_X,           ///< Coordinates ||IMPORTANT: This 3 entries have to stay
    CARTN_Y,           ///< Coordinates ||together for the parser to work!
    CARTN_Z,           ///< Coordinates ||
    OCCUPANCY,
    B_ISO_OR_EQUIV,
    PDBX_PDB_INS_CODE,
    GROUP_PDB,         ///< record name
    PDBX_PDB_MODEL_NUM ///< model no. (especially NMR structures)
  } AtomSiteItems;

  /// \enum items of the entity category
  typedef enum {
    E_ID,              ///< unique identifier
    E_TYPE,            ///< polymer, non-polymer or water
    PDBX_DESCRIPTION   ///< special aspects of the entity
  } EntityItems;

  /// \enum items of the entity_poly category
  typedef enum {
    ENTITY_ID,                    ///< pointer to entity.id
    EP_TYPE,                      ///< type of polymer
    PDBX_SEQ_ONE_LETTER_CODE,     ///< sequence, 1-letter code
    PDBX_SEQ_ONE_LETTER_CODE_CAN  ///< canonical sequence, 1-letter code
  } EntityPolyItems;

  /// \enum items of the citation category
  typedef enum {
    CITATION_ID,                  ///< unique identifier
    ABSTRACT_ID_CAS,              ///< CAS identifier
    BOOK_ID_ISBN,                 ///< ISBN code assigned, if book cited
    BOOK_TITLE,                   ///< title of book storing the citation
    JOURNAL_FULL,                 ///< full journal title for articles
    JOURNAL_VOLUME,               ///< volume of cited journal
    PAGE_FIRST,                   ///< first page of citation
    PAGE_LAST,                    ///< last page of citation
    PDBX_DATABASE_ID_DOI,         ///< Document Object Identifier of doi.org
    PDBX_DATABASE_ID_PUBMED,      ///< Ascession number of PubMed
    YEAR,                         ///< year of the citation
    TITLE                         ///< title of the citation
  } CitationItems;

  /// \enum items of the citation_author category
  typedef enum {
    AUTHOR_CITATION_ID,           ///< link to CITATION_ID
    AUTHOR_NAME,                  ///< name of an author
    ORDINAL                       ///< position in author list
  } CitationAuthorItems;

  /// \enum items of the exptl category
  typedef enum {
    EXPTL_ENTRY_ID,               ///< identifier
    METHOD                        ///< method of the experiment
  } ExptlItems;

  /// \enum categories of the mmcif format
  typedef enum {
    ATOM_SITE,
    ENTITY,
    ENTITY_POLY,
    CITATION,
    CITATION_AUTHOR,
    EXPTL,
    DONT_KNOW
  } MMCifCategory;

  /// \struct keeping track of entity information
  typedef struct {
    mol::ChainType type; ///< characterise entity
    String details;      ///< description of this entity
    String seqres;       ///< chain of monomers
  } MMCifEntityDesc;

  typedef std::map<String, MMCifEntityDesc> MMCifEntityDescMap;
  //typedef std::map<String, std::pair<std::vector<int>, std::vector<String> > >
  typedef std::map<String, std::pair<std::vector<int>, std::vector<String> > >
    MMCifCitationAuthorMap;

  // members
  MMCifCategory category_;
  int category_counts_[DONT_KNOW+1]; ///< overall no. of atom_site loops
  int indices_[MAX_ITEMS_IN_ROW]; ///< map items to values in loops
  const IOProfile& profile_;
  mol::EntityHandle& ent_handle_;
  String restrict_chains_;
  bool auth_chain_id_;       ///< use chain IDs given by authors rather than pdb
  bool seqres_can_;          ///< read canonical 1-letter residues?
  mol::ChainHandle curr_chain_;
  mol::ResidueHandle curr_residue_;
  int chain_count_;
  int residue_count_;
  int atom_count_;
  bool warned_name_mismatch_;
  bool warned_rule_based_;
  String subst_res_id_; ///< work around for missing label_seq_id's
  bool has_model_;      ///< keep track of models through different atom_sites
  int curr_model_;      ///< if we have pdbx_PDB_model_num, store no.
  std::vector<std::pair<mol::ChainHandle, String> > chain_id_pairs_;
  ///< chain and label_entity_id
  MMCifEntityDescMap entity_desc_map_; ///< stores entity items
  seq::SequenceList seqres_;
  bool read_seqres_;
  MMCifInfo info_;      ///< info container
  MMCifCitationAuthorMap authors_map_;
};

}}

#endif
