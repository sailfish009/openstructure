#ifndef OST_SEQ_HMM_HH
#define OST_SEQ_HMM_HH

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <ost/base.hh>

namespace ost { namespace seq { 
typedef enum {
  HMM_MATCH=0,
  HMM_INSERT=1,
  HMM_DELETE=2
} HMMState;


class HMMColumn {
 public:
  HMMColumn() : n_eff_(0.0), n_eff_ins_(0.0), n_eff_del_(0.0) { 
    memset(freq_, 0, sizeof(Real)*20);
    memset(trans_, 0, sizeof(Real)*9);
  }
  HMMColumn(const HMMColumn& rhs): olc_(rhs.olc_), n_eff_(rhs.n_eff_), 
    n_eff_ins_(rhs.n_eff_ins_), n_eff_del_(rhs.n_eff_del_) {
    memcpy(freq_, rhs.freq_, sizeof(Real)*20);
    memcpy(trans_, rhs.trans_, sizeof(Real)*9);
  }
  Real GetTransitionFreq(HMMState from, HMMState to) const {
    return trans_[from][to];
  }
  void SetTransitionFreq(HMMState from, HMMState to, Real freq){
    trans_[from][to] = freq;
  }
  void SetNEff(Real val) { n_eff_ = val; }
  void SetNEffIns(Real val) { n_eff_ins_ = val; }
  void SetNEffDel(Real val) { n_eff_del_ = val; }
  Real GetNEff() const { return n_eff_; }
  Real GetNEffIns() const { return n_eff_ins_; }
  Real GetNEffDel() const { return n_eff_del_; }

  Real GetFreq(char ch) const {
    return freq_[this->GetIndex(ch)];
  }

  void SetFreq(char ch, Real freq) {
    freq_[this->GetIndex(ch)]=freq;
  }

  bool operator==(const HMMColumn& rhs) const {
    return !memcmp(freq_, rhs.freq_, sizeof(freq_)) &&
           !memcmp(trans_, rhs.trans_, sizeof(trans_)) &&
           n_eff_ == rhs.n_eff_ &&
           n_eff_ins_ == rhs.n_eff_ins_ &&
           n_eff_del_ == rhs.n_eff_del_;
  }

  Real* freqs_begin() { return freq_; }
  Real* freqs_end() { return freq_+20; }
  const Real* freqs_begin() const { return freq_; }
  const Real* freqs_end() const { return freq_+20; }
  char GetOneLetterCode() const { return olc_; }
  void SetOneLetterCode(char olc) { olc_ = olc; }
  Real GetEntropy() const;

  static HMMColumn BLOSUMNullModel();
 private:
  int GetIndex(char ch) const;
  char olc_;
  Real freq_[20];
  Real trans_[3][3];
  Real n_eff_;
  Real n_eff_ins_;
  Real n_eff_del_;
};


class HMM;
typedef boost::shared_ptr<HMM> HMMPtr;
typedef std::vector<HMMColumn> HMMColumnList;
// Represents a HHsearch profile
// Minimalistic on Purpose. 
class HMM { 
 public:
  HMM() {}
  static HMMPtr Load(const std::string& filename);
  const std::vector<HMMColumn>& GetColumns() const { return columns_; }
  const HMMColumn& GetNullModel() const { return null_model_; } 
  void SetNullModel(const HMMColumn& null_model) { null_model_ = null_model; }


  //some functions to make it behave like a vector

  size_t size() const { return columns_.size(); }

  bool empty() const { return columns_.empty(); }

  void push_back(const HMMColumn& c) { columns_.push_back(c); }  

  HMMColumn& operator[](size_t index) { return columns_[index]; }

  const HMMColumn& operator[](size_t index) const { return columns_[index]; }

  HMMColumn& at(size_t index) { return columns_.at(index); }

  const HMMColumn& at(size_t index) const { return columns_.at(index); }

  bool operator==(const HMM& other) const { return columns_ == other.columns_ &&
                                                   null_model_ == other.null_model_; }
  
  bool operator!=(const HMM& other) const { return !(other == (*this)); }


  HMMColumnList::const_iterator columns_end() const { return columns_.end(); }
  HMMColumnList::iterator columns_end() { return columns_.end(); }
  HMMColumnList::const_iterator columns_begin() const { return columns_.begin(); }
  HMMColumnList::iterator columns_begin() { return columns_.begin(); }
  Real GetAverageEntropy() const;
 private:
   HMMColumn              null_model_; 
   HMMColumnList          columns_;
};

}}

#endif
