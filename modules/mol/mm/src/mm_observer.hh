#ifndef OST_MM_OBSERVER_HH
#define OST_MM_OBSERVER_HH


#include <OpenMM.h>

#include <boost/shared_ptr.hpp>

#include <ost/io/mol/io_profile.hh>
#include <ost/io/mol/pdb_writer.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/coord_frame.hh>
#include <ost/mol/coord_group.hh>
#include <ost/mol/mm/state_extractor.hh>
#include <ost/mol/mm/topology.hh>
#include <ost/mol/mm/mm_modeller.hh>


namespace ost { namespace mol{ namespace mm{

class MMObserver;
class TrajObserver;
class TrajWriter;
typedef boost::shared_ptr<MMObserver> MMObserverPtr;
typedef boost::shared_ptr<TrajObserver> TrajObserverPtr;
typedef boost::shared_ptr<TrajWriter> TrajWriterPtr;

class MMObserver{
public:

  virtual void Notify() = 0;

  virtual void Init(boost::shared_ptr<OpenMM::Context> c,
                    TopologyPtr top) = 0;

  virtual int Rythm() = 0;

};


class TrajObserver : public MMObserver{

public:  

  TrajObserver(int rythm): rythm_(rythm), registered_(false) { }

  void Init(boost::shared_ptr<OpenMM::Context> c, 
            TopologyPtr top);

  void Notify();

  int Rythm() { return rythm_; }

  CoordGroupHandle GetTraj() { return c_group_; }

private:

  ost::mol::CoordGroupHandle c_group_;
  boost::shared_ptr<OpenMM::Context> context_;
  int rythm_;
  bool registered_;
};

class TrajWriter : public MMObserver{

//Note, that this code is highly redundant to the code in io/mol/dcd_io.hh!!!

public:

  TrajWriter(int rythm, const String& pdb_filename, const String& dcd_filename): rythm_(rythm),
                                                                                 pdb_filename_(pdb_filename),
                                                                                 dcd_filename_(dcd_filename),
                                                                                 stream_(),
                                                                                 registered_(false),
                                                                                 frames_(0) { }

  void Init(boost::shared_ptr<OpenMM::Context> c, 
            TopologyPtr top);

  void Notify();

  int Rythm() { return rythm_; }

  void Finalize();

private:

  TrajWriter(const TrajWriter& writer) { } //output filestream is unique!


  boost::shared_ptr<OpenMM::Context> context_;
  int rythm_;
  String pdb_filename_;
  String dcd_filename_;
  std::ofstream stream_;
  bool registered_;
  uint32_t frames_;
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> z;
  OpenMM::Vec3 ucell_a;
  OpenMM::Vec3 ucell_b;
  OpenMM::Vec3 ucell_c;
};


}}}//ns

#endif
