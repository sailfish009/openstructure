// -----------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>

// Copyright (C) 2008-2011 by the OpenStructure authors

// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// -----------------------------------------------------------------------------

#include <ost/mol/mol.hh>
#include <ost/platform.hh>
#include <ost/conop/rule_based_builder.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>

#include <ost/log.hh>
using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::conop;
//using namespace ost::io;
using namespace ost::mol;
namespace {
} // anon ns

ResidueHandle make_cytosine(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();  
  ResidueHandle res=e.AppendResidue(chain, "DC");

  e.InsertAtom(res, "P",   geom::Vec3(21.412, 34.391, 37.142));
  e.InsertAtom(res, "OP1", geom::Vec3(22.938, 34.599, 36.988));
  e.InsertAtom(res, "OP2", geom::Vec3(20.690, 35.640, 37.689));
  e.InsertAtom(res, "O5'", geom::Vec3(21.215, 33.299, 38.339));
  e.InsertAtom(res, "C5'", geom::Vec3(20.524, 33.660, 39.548));
  e.InsertAtom(res, "C4'", geom::Vec3(19.064, 33.285, 39.452));
  e.InsertAtom(res, "O4'", geom::Vec3(18.246, 34.411, 39.034));
  e.InsertAtom(res, "C3'", geom::Vec3(18.778, 32.150, 38.469));
  e.InsertAtom(res, "O3'", geom::Vec3(17.930, 31.199, 39.081));
  e.InsertAtom(res, "C2'", geom::Vec3(18.046, 32.827, 37.330));
  e.InsertAtom(res, "C1'", geom::Vec3(17.326, 33.955, 38.048));
  e.InsertAtom(res, "N1",  geom::Vec3(16.891, 35.100, 37.196));
  e.InsertAtom(res, "C2",  geom::Vec3(15.709, 35.782, 37.543));
  e.InsertAtom(res, "O2",  geom::Vec3(15.137, 35.489, 38.610));
  e.InsertAtom(res, "N3",  geom::Vec3(15.226, 36.742, 36.710));
  e.InsertAtom(res, "C4",  geom::Vec3(15.878, 37.041, 35.582));
  e.InsertAtom(res, "N4",  geom::Vec3(15.324, 37.943, 34.762));
  e.InsertAtom(res, "C5",  geom::Vec3(17.116, 36.415, 35.238));
  e.InsertAtom(res, "C6",  geom::Vec3(17.584, 35.464, 36.067));

  return res;
}

ResidueHandle make_uracil1(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();
  ResidueHandle res = e.AppendResidue(chain, "U");

  e.InsertAtom(res, "P",   geom::Vec3(18.533, 30.238, 40.226));
  e.InsertAtom(res, "OP1", geom::Vec3(20.012, 30.098, 40.003));
  e.InsertAtom(res, "OP2", geom::Vec3(17.680, 29.017, 40.258));
  e.InsertAtom(res, "O5'", geom::Vec3(18.237, 31.056, 41.570));
  e.InsertAtom(res, "C5'", geom::Vec3(17.104, 31.923, 41.642));
  e.InsertAtom(res, "C4'", geom::Vec3(16.332, 31.729, 42.937));
  e.InsertAtom(res, "O4'", geom::Vec3(15.180, 32.616, 42.841));
  e.InsertAtom(res, "C3'", geom::Vec3(15.785, 30.319, 43.194));
  e.InsertAtom(res, "O3'", geom::Vec3(15.960, 29.834, 44.527));
  e.InsertAtom(res, "C2'", geom::Vec3(14.311, 30.371, 42.766));
  e.InsertAtom(res, "O2'", geom::Vec3(13.442, 29.708, 43.677));
  e.InsertAtom(res, "C1'", geom::Vec3(13.972, 31.863, 42.765));
  e.InsertAtom(res, "N1",  geom::Vec3(13.282, 32.212, 41.499));
  e.InsertAtom(res, "C2",  geom::Vec3(12.072, 32.896, 41.568));
  e.InsertAtom(res, "O2",  geom::Vec3(11.510, 33.142, 42.626));
  e.InsertAtom(res, "N3",  geom::Vec3(11.535, 33.257, 40.349));
  e.InsertAtom(res, "C4",  geom::Vec3(12.048, 32.976, 39.096));
  e.InsertAtom(res, "O4",  geom::Vec3(11.490, 33.447, 38.087));
  e.InsertAtom(res, "C5",  geom::Vec3(13.268, 32.207, 39.106));
  e.InsertAtom(res, "C6",  geom::Vec3(13.831, 31.872, 40.261));

  return res;
}

ResidueHandle make_uracil2(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();
  ResidueHandle res = e.AppendResidue(chain, "U");

  e.InsertAtom(res, "P",   geom::Vec3(16.249, 28.254, 44.759));
  e.InsertAtom(res, "OP1", geom::Vec3(16.654, 28.055, 46.181));
  e.InsertAtom(res, "OP2", geom::Vec3(17.115, 27.731, 43.656));
  e.InsertAtom(res, "O5'", geom::Vec3(14.826, 27.572, 44.574));
  e.InsertAtom(res, "C5'", geom::Vec3(14.711, 26.206, 44.216));
  e.InsertAtom(res, "C4'", geom::Vec3(13.279, 25.889, 43.887));
  e.InsertAtom(res, "O4'", geom::Vec3(12.832, 26.793, 42.838));
  e.InsertAtom(res, "C3'", geom::Vec3(13.155, 24.434, 43.329));
  e.InsertAtom(res, "O3'", geom::Vec3(12.269, 23.625, 44.098));
  e.InsertAtom(res, "C2'", geom::Vec3(12.871, 24.595, 41.875));
  e.InsertAtom(res, "O2'", geom::Vec3(11.811, 23.752, 41.462));
  e.InsertAtom(res, "C1'", geom::Vec3(12.424, 26.056, 41.694));
  e.InsertAtom(res, "N1",  geom::Vec3(13.030, 26.692, 40.497));
  e.InsertAtom(res, "C2",  geom::Vec3(12.517, 26.365, 39.228));
  e.InsertAtom(res, "O2",  geom::Vec3(11.579, 25.594, 39.068));
  e.InsertAtom(res, "N3",  geom::Vec3(13.141, 26.987, 38.161));
  e.InsertAtom(res, "C4",  geom::Vec3(14.197, 27.888, 38.210));
  e.InsertAtom(res, "O4",  geom::Vec3(14.627, 28.368, 37.156));
  e.InsertAtom(res, "C5",  geom::Vec3(14.671, 28.189, 39.542));
  e.InsertAtom(res, "C6",  geom::Vec3(14.087, 27.597, 40.612));

  return res;
}

ResidueHandle make_defective_uracil2(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();
  ResidueHandle res = e.AppendResidue(chain, "U");

  e.InsertAtom(res, "P",   geom::Vec3(16.249, 28.254, 44.759));
  e.InsertAtom(res, "OP1", geom::Vec3(16.654, 28.055, 46.181));
  e.InsertAtom(res, "OP2", geom::Vec3(17.115, 27.731, 43.656));
  e.InsertAtom(res, "O5'", geom::Vec3(14.826, 27.572, 44.574));
  e.InsertAtom(res, "C5'", geom::Vec3(14.711, 26.206, 44.216));
  e.InsertAtom(res, "C4'", geom::Vec3(13.279, 25.889, 43.887));
  e.InsertAtom(res, "O4'", geom::Vec3(12.832, 26.793, 42.838));
  e.InsertAtom(res, "C3'", geom::Vec3(13.155, 24.434, 43.329));
  e.InsertAtom(res, "O3'", geom::Vec3(12.269, 23.625, 44.098));
  e.InsertAtom(res, "C2'", geom::Vec3(12.871, 24.595, 41.875));
  e.InsertAtom(res, "O2'", geom::Vec3(11.811, 23.752, 41.462));
  e.InsertAtom(res, "C1'", geom::Vec3(12.424, 26.056, 41.694));
  e.InsertAtom(res, "N1",  geom::Vec3(13.030, 26.692, 40.497));
  e.InsertAtom(res, "C2",  geom::Vec3(12.517, 26.365, 39.228));
  e.InsertAtom(res, "O2",  geom::Vec3(11.579, 25.594, 39.068));
  e.InsertAtom(res, "N3",  geom::Vec3(13.141, 26.987, 38.161));
  e.InsertAtom(res, "C4",  geom::Vec3(14.197, 27.888, 38.210));
  e.InsertAtom(res, "O4",  geom::Vec3(14.627, 28.368, 37.156));
  e.InsertAtom(res, "C5",  geom::Vec3(14.671, 28.189, 39.542));
  e.InsertAtom(res, "C6",  geom::Vec3(14.087, 27.597, 80.612));

  return res;
}

void verify_nucleotide_connectivity(const ResidueHandle& res) 
{
  BOOST_CHECK(BondExists(res.FindAtom("P"),
                         res.FindAtom("OP1")));
  BOOST_CHECK(BondExists(res.FindAtom("P"),
                         res.FindAtom("OP2")));
  BOOST_CHECK(BondExists(res.FindAtom("P"),
                         res.FindAtom("O5'")));
  BOOST_CHECK(BondExists(res.FindAtom("O5'"),
                         res.FindAtom("C5'")));
  BOOST_CHECK(BondExists(res.FindAtom("C5'"),
                         res.FindAtom("C4'")));
  BOOST_CHECK(BondExists(res.FindAtom("C4'"),
                         res.FindAtom("O4'")));
  BOOST_CHECK(BondExists(res.FindAtom("C4'"),
                         res.FindAtom("C3'")));
  BOOST_CHECK(BondExists(res.FindAtom("C3'"),
                         res.FindAtom("O3'")));
  BOOST_CHECK(BondExists(res.FindAtom("C3'"),
                         res.FindAtom("C2'")));
  BOOST_CHECK(BondExists(res.FindAtom("C2'"),
                         res.FindAtom("C1'")));
  BOOST_CHECK(BondExists(res.FindAtom("C1'"),
                         res.FindAtom("O4'")));

  if (res.GetKey()=="DC") {
    BOOST_CHECK(BondExists(res.FindAtom("C1'"),
                           res.FindAtom("N1")));
    BOOST_CHECK(BondExists(res.FindAtom("N1"),
                           res.FindAtom("C2")));
    BOOST_CHECK(BondExists(res.FindAtom("C2"),
                           res.FindAtom("O2")));
    BOOST_CHECK(BondExists(res.FindAtom("C2"),
                           res.FindAtom("N3")));
    BOOST_CHECK(BondExists(res.FindAtom("N3"),
                           res.FindAtom("C4")));
    BOOST_CHECK(BondExists(res.FindAtom("C4"),
                           res.FindAtom("N4")));
    BOOST_CHECK(BondExists(res.FindAtom("C4"),
                           res.FindAtom("C5")));
    BOOST_CHECK(BondExists(res.FindAtom("C5"),
                           res.FindAtom("C6")));
    BOOST_CHECK(BondExists(res.FindAtom("C6"),
                           res.FindAtom("N1")));
    // TODO: Check that no other atoms are connected!
  }

  if (res.GetKey()=="U") {
    BOOST_CHECK(BondExists(res.FindAtom("O2'"),
                           res.FindAtom("C2'")));
    BOOST_CHECK(BondExists(res.FindAtom("C1'"),
                           res.FindAtom("N1")));
    BOOST_CHECK(BondExists(res.FindAtom("N1"),
                           res.FindAtom("C2")));
    BOOST_CHECK(BondExists(res.FindAtom("C2"),
                           res.FindAtom("O2")));
    BOOST_CHECK(BondExists(res.FindAtom("C2"),
                           res.FindAtom("N3")));
    BOOST_CHECK(BondExists(res.FindAtom("N3"),
                           res.FindAtom("C4")));
    BOOST_CHECK(BondExists(res.FindAtom("C4"),
                           res.FindAtom("O4")));
    BOOST_CHECK(BondExists(res.FindAtom("C4"),
                           res.FindAtom("C5")));
    BOOST_CHECK(BondExists(res.FindAtom("C5"),
                           res.FindAtom("C6")));
    BOOST_CHECK(BondExists(res.FindAtom("C6"),
                           res.FindAtom("N1")));
    // TODO: Check that no other atoms are connected!
  }
}
void verify_nucleotide_link(const ResidueHandle& p3, const ResidueHandle& p5) 
{
  BOOST_CHECK(BondExists(p3.FindAtom("O3'"),
                         p5.FindAtom("P")));
}

void verify_nucleotide_nolink(const ResidueHandle& p3, const ResidueHandle& p5) 
{
  BOOST_CHECK(!BondExists(p3.FindAtom("O3'"),
                          p5.FindAtom("P")));
}

BOOST_AUTO_TEST_SUITE( conop );


BOOST_AUTO_TEST_CASE(nucleotide_based_connect) 
{
  SetPrefixPath(getenv("OST_ROOT"));
  String lib_path=GetSharedDataPath()+"/compounds.chemlib";
  CompoundLibPtr compound_lib=CompoundLib::Load(lib_path);  
  if (!compound_lib) {
    std::cout << "WARNING: skipping NUCLEOTIDE_BASED connect unit test. " 
              << "Rule-based builder is required" << std::endl;
    return;    
  }
  RuleBasedBuilder rb_builder = RuleBasedBuilder(compound_lib);

  RuleBasedBuilder drb_builder = RuleBasedBuilder(compound_lib);
  drb_builder.SetBondFeasibilityCheck(false);
  
  EntityHandle e=CreateEntity();
  ChainHandle c=e.EditXCS().InsertChain("A");
  ResidueHandle c0=make_cytosine(c);
  ResidueHandle u1=make_uracil1(c);
  ResidueHandle u2=make_uracil2(c);
  
  EntityHandle de=CreateEntity();  
  ChainHandle dc=de.EditXCS().InsertChain("A");
  ResidueHandle du2=make_defective_uracil2(dc);


  for (AtomHandleIter i=e.AtomsBegin(),x=e.AtomsEnd(); i!=x; ++i) {
    rb_builder.FillAtomProps(*i);
  }

  for (AtomHandleIter i=de.AtomsBegin(),x=de.AtomsEnd(); i!=x; ++i) {
    drb_builder.FillAtomProps(*i);
  }
  
  // running positive test
  BOOST_MESSAGE("running distance based checks on cytosine");
  rb_builder.ConnectAtomsOfResidue(c0);
  verify_nucleotide_connectivity(c0);
  BOOST_MESSAGE("running distance based checks on first uracil");
  rb_builder.ConnectAtomsOfResidue(u1);
  verify_nucleotide_connectivity(u1);
  BOOST_MESSAGE("running distance based checks on second uracil");
  rb_builder.ConnectAtomsOfResidue(u2);
  verify_nucleotide_connectivity(u2);
  BOOST_MESSAGE("connecting cytosine to first uracil");
  rb_builder.ConnectResidueToNext(c0, u1);
  verify_nucleotide_link(c0, u1);
  BOOST_MESSAGE("connecting first uracil to second uracil");
  rb_builder.ConnectResidueToNext(u1, u2);
  verify_nucleotide_link(u1, u2);
  // one negative test
  BOOST_MESSAGE("connecting cytosine to second uracil");
  rb_builder.ConnectResidueToNext(c0, u2);
  verify_nucleotide_nolink(c0, u2);
  
  // running positive test
  BOOST_MESSAGE("running distance based checks on defective uracil");
  drb_builder.ConnectAtomsOfResidue(du2);
  verify_nucleotide_connectivity(du2);
  
}

BOOST_AUTO_TEST_SUITE_END( );
