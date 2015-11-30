#ifndef OST_SEQ_HMM_HH
#define OST_SEQ_HMM_HH

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <ost/base.hh>

namespace ost { namespace seq { 

class HMM;
class HMMColumn;
class HMMDB;
typedef boost::shared_ptr<HMM> HMMPtr;
typedef boost::shared_ptr<HMMDB> HMMDBPtr;
typedef std::vector<HMMColumn> HMMColumnList;

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

  Real GetFreq(char ch) const;

  void SetFreq(char ch, Real freq);

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
  static int GetIndex(char ch);

  static HMMColumn BLOSUMNullModel();

  //functions to feed streams with limited accuracy of internal data
  //not intended for python export

  friend std::ofstream& operator<<(std::ofstream& os, HMMColumn& col){

    char data[61];
    char* data_ptr = &data[0];

    //transform aa_freq
    for(uint i = 0; i < 20; ++i){
      *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.freq_[i]*10000);
      data_ptr+=2;
    }
    //transform transition freq
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[0][0]*10000); //M-M
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[0][1]*10000); //M-I
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[0][2]*10000); //M-D
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[1][0]*10000); //I-M
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[1][1]*10000); //I-I
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[2][0]*10000); //D-M
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.trans_[2][2]*10000); //D-D
    data_ptr+=2;

    //transform neff values
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.n_eff_*1000); 
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.n_eff_ins_*1000); 
    data_ptr+=2;
    *(reinterpret_cast<int16_t*>(data_ptr)) = static_cast<int16_t>(col.n_eff_del_*1000);
    data_ptr+=2;

    //finally write one letter code
    *data_ptr = col.olc_;

    os.write(data,61);
    return os;
  }

  friend std::ifstream& operator>>(std::ifstream& is, HMMColumn& col){

    char data[61];
    char* data_ptr = &data[0];
    is.read(data,61);

    //transform aa_freq
    for(uint i = 0; i < 20; ++i){
      col.freq_[i] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001;
      data_ptr+=2;
    }
    //transform transition freq
    col.trans_[0][0] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //M-M
    data_ptr+=2;
    col.trans_[0][1] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //M-I
    data_ptr+=2;
    col.trans_[0][2] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //M-D
    data_ptr+=2;
    col.trans_[1][0] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //I-M
    data_ptr+=2;
    col.trans_[1][1] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //I-I
    data_ptr+=2;
    col.trans_[2][0] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //D-M
    data_ptr+=2;
    col.trans_[2][2] = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.0001; //D-D
    data_ptr+=2;

    //transform neff values
    col.n_eff_ = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.001; 
    data_ptr+=2;
    col.n_eff_ins_ = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.001; 
    data_ptr+=2;
    col.n_eff_del_ = *(reinterpret_cast<int16_t*>(data_ptr)) * 0.001;
    data_ptr+=2;

    //finally read one letter code
    col.olc_ = *data_ptr;

    return is;
  }

 private:
  char olc_;
  Real freq_[20];
  Real trans_[3][3];
  Real n_eff_;
  Real n_eff_ins_;
  Real n_eff_del_;
};


class HMM { 
 public:
  HMM() {}

  static HMMPtr Load(const std::string& filename);

  const std::vector<HMMColumn>& GetColumns() const { return columns_; }

  const HMMColumn& GetNullModel() const { return null_model_; } 

  void SetNullModel(const HMMColumn& null_model) { null_model_ = null_model; }

  String GetSequence() const;

  HMMPtr Extract(uint from, uint to);

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

  friend std::ofstream& operator<<(std::ofstream& os, HMM& hmm){

    os << hmm.null_model_;

    uint32_t size = hmm.size();
    os.write(reinterpret_cast<char*>(&size),4);

    for(uint i = 0; i < size; ++i){
      os << hmm.columns_[i];
    }

    return os;
  }

  friend std::ifstream& operator>>(std::ifstream& is, HMM& hmm){

    is >> hmm.null_model_;

    uint32_t size;
    is.read(reinterpret_cast<char*>(&size),4);

    hmm.columns_.resize(size);

    for(uint i = 0; i < size; ++i){
      is >> hmm.columns_[i];
    }

    return is;
  }

 private:
   HMMColumn              null_model_; 
   HMMColumnList          columns_;
};


class HMMDB{

public:

  void Save(const String& filename) const;

  static HMMDBPtr Load(const String& filename);

  void AddHMM(const String& name, HMMPtr hmm);

  HMMPtr GetHMM(const String& name) const;

  size_t size() const { return data_.size(); }

  std::vector<String> GetNames() const;

private:

  std::map<String,HMMPtr> data_;
};

}}

#endif
