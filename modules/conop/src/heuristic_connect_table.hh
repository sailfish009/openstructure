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
#ifndef OST_HEURISTIC_CONNECT_TABLE_HH
#define OST_HEURISTIC_CONNECT_TABLE_HH

#include <string>

namespace ost { namespace conop { namespace heuristic_connect {

struct CONN_DEF_ENTRY {
  char name[256];
  char abbrev[16],single;
  mol::ChemClass chem_class;
  char name_list[48][8];
  int name_count;
  int conn_list[48][2];
  int conn_count;
  struct CONN_TORSION_TMP {
    int n1,n2,n3,n4;
    String name;
  }tor_list[8];
  int tor_count;
  int flag_list[128];
  int flag_count;
};

CONN_DEF_ENTRY def_entry_table[]={
  // the first entry must be this generic one
  {"Generic","___",'_', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C"},3,
   {{-2,1}, {1,2}, {2,3}, {3,-3}},4,
   {
   },0,
   {0, 0, 0, 0, 0, 0},6
  },
  {"Alanine","ALA",'A', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","OXT"},6,
   {{-2,1}, {1,2}, {2,3}, {3,4}, {2,5}, {3,-3}, {6, 3}},7,
   {
   },0,
   {0, 0, 0, 0, 0, 0},6
  },
  {"Cystein","CYS",'C', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","SG","OXT"},7,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{3,-3},{8, 3}},8,
   {{1,2,5,6,"CHI1"}
   },1,
   {0, 0, 0, 0, 0, 0, 0},7
  },
  {"Aspartate","ASP",'D', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","OD1","OD2","OXT"},9,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{3,-3},{9, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0, 0, 0, 0, 0, 0, 0, 0, 0},9
  },
  {"Glutamate","GLU",'E', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD","OE1","OE2","OXT"},10,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{7,8},{7,9},{3,-3},{10, 3}},11,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"},{5,6,7,8,"CHI3"}
   },3,
   {0},1
  },
  {"Phenylalanine","PHE",'F', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD1","CD2","CE1","CE2","CZ","OXT"},12,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{7,9},{8,10},{9,11},{10,11},{3,-3},{12, 3}},14,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Glycin","GLY",'G', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","OXT"},5,
   {{-2,1},{1,2},{2,3},{3,4},{3,-3},{5, 3}},6,
   {
   },0,
   {0},1
  },
  {"Histidine","HIS",'H', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","ND1","CD2","CE1","NE2","OXT"},11,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{7,9},{8,10},{9,10},{3,-3},{11, 3}},13,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Isoleucine","ILE",'I', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG1","CG2","CD1","OXT"},9,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{5,7},{6,8},{3,-3},{9, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,8,"CHI2"}
   },2,
   {0},1
  },
  {"Lysin","LYS",'K', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD","CE","NZ","OXT"},10,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{7,8},{8,9},{3,-3},{10, 3}},11,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"},{5,6,7,8,"CHI3"},{6,7,8,9,"CHI4"}
   },4,
   {0},1
  },
  {"Leucin","LEU",'L', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD1","CD2","OXT"},9,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{3,-3},{9, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Methionine","MET",'M', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","SD","CE","OXT"},9,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{7,8},{3,-3},{10, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"},{5,6,7,8,"CHI3"}
   },3,
   {0},1
  },
  {"Asparagine","ASN",'N', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","OD1","ND2","OXT"},9,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{3,-3},{9, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Proline","PRO",'P', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD","OXT"},8,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{3,-3},{1,7},{8, 3}},10,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Glutamine","GLN",'Q', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD","OE1","NE2","OXT"},10,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{7,8},{7,9},{3,-3},{10, 3}},11,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"},{5,6,7,8,"CHI3"}
   },3,
   {0},1
  },
  {"Arginine","ARG",'R', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD","NE","CZ","NH1","NH2","OXT"},12,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{7,8},{8,9},{9,10},{9,11},{3,-3},{12, 3}},13,
   {
   {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"},{5,6,7,8,"CHI3"},{6,7,8,9,"CHI4"},{7,8,9,10,"CHI5"}
   },5,
   {0},1
  },
  {"Serine","SER",'S', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","OG","OXT"},7,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{3,-3},{7, 3}},8,
   {
    {1,2,5,6,"CHI1"}
   },1,
   {0},1
  },
  {"Threonine","THR",'T', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","OG1","CG2","OXT"},8,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{5,7},{3,-3},{8, 3}},9,
   {
    {1,2,5,6,"CHI1"}
   },1,
   {0},1
  },
  {"Valine","VAL",'V', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG1","CG2","OXT"},8,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{5,7},{3,-3},{8, 3}},9,
   {
    {1,2,5,6,"CHI1"}
   },1,
   {0},1
  },
  {"Tryptophan","TRP",'W', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD1","CD2","NE1","CE2","CE3","CZ2","CZ3","CH2","OXT"},15,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{7,9},{8,10},{9,10},{8,11},{10,12},{11,13},{12,14},{13,14},{3,-3},{15, 3}},18,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  {"Tyrosin","TYR",'Y', mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING),
   {"N","CA","C","O","CB","CG","CD1","CD2","CE1","CE2","CZ","OH","OXT"},13,
   {{-2,1},{1,2},{2,3},{3,4},{2,5},{5,6},{6,7},{6,8},{7,9},{8,10},{9,11},{10,11},{11,12},{3,-3},{13, 3}},15,
   {
    {1,2,5,6,"CHI1"},{2,5,6,7,"CHI2"}
   },2,
   {0},1
  },
  /* NUCLEIC ACIDS */
  {"Adenosin","A",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","N9","C8","N7","C5","C6","N6","N1","C2","N3","C4","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,22},{7,-3}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Cytosin","C",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,12},{7,-3}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Guanidin","G",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Thymidin","T",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{8,12},{1,11}},15,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Uracil","U",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Adenosin","ADE",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","N9","C8","N7","C5","C6","N6","N1","C2","N3","C4","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,22},{9,12},{7,-3}},15,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Cytosin","CYT",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,12},{7,-3}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Guanidin","GUA",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Thymidin","THY",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{8,12},{1,11}},15,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Uracil","URI",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Adenosin","DA",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","N9","C8","N7","C5","C6","N6","N1","C2","N3","C4","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,22},{9,12},{7,-3}},15,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Cytosin","DC",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{1,10},{1,11},{8,12},{7,-3}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Guanidin","DG",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Thymidin","DT",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{8,12},{1,11}},15,
   {{0,0,0,0,""}},1,
   {0},1
  },
  {"Uracil","DU",'?', mol::ChemClass(mol::ChemClass::DNA_LINKING),
   {"P","O5'","C5'","C4'","O4'","C3'","O3'","C2'","C1'","O1P","O2P","O2'"},12,
   {{-2,1},{1,2},{2,3},{3,4},{4,5},{4,6},{6,7},{6,8},{8,9},{5,9},{7,-3},{1,10},{1,11},{8,12}},14,
   {{0,0,0,0,""}},1,
   {0},1
  }
};

}}} // ns

#endif
