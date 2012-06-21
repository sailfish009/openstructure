import unittest
from ost import *

class TestMMCifInfo(unittest.TestCase):
  def setUp(self):
    pass

  def test_mmcifinfo_citation(self):
    c = io.MMCifInfoCitation()
    # test ID setting/ getting
    c.SetID('ID')
    self.assertEquals(c.GetID(), 'ID')
    # test CAS setting/ getting
    c.SetCAS('FOO')
    self.assertEquals(c.GetCAS(), 'FOO')
    # test ISBN setting/ getting
    c.SetISBN('0-0-0-0-0-0')
    self.assertEquals(c.GetISBN(), '0-0-0-0-0-0')
    # test published_in setting/ getting
    c.SetPublishedIn('Best Book Ever')
    self.assertEquals(c.GetPublishedIn(), 'Best Book Ever')
    # test volume setting/ getting
    c.SetVolume('3')
    self.assertEquals(c.GetVolume(), '3')
    # test page setting/ getting
    c.SetPageFirst('1')
    self.assertEquals(c.GetPageFirst(), '1')
    c.SetPageLast('10')
    self.assertEquals(c.GetPageLast(), '10')
    # test doi setting/ getting
    c.SetDOI('HERE')
    self.assertEquals(c.GetDOI(), 'HERE')
    # test PubMed setting/ getting
    c.SetPubMed(815)
    self.assertEquals(c.GetPubMed(), 815)
    # test year setting/ getting
    c.SetYear(815)
    self.assertEquals(c.GetYear(), 815)
    # test title setting/ getting
    c.SetTitle('Foo')
    self.assertEquals(c.GetTitle(), 'Foo')
    # test auhtors setting/ getting
    s = ost.StringList()
    s.append('Foo')
    c.SetAuthorList(s)
    s2 = c.GetAuthorList()
    self.assertEquals(s2[0], 'Foo')

    i = io.MMCifInfo()
    i.SetMethod('Deep-Fry')
    i.SetResolution(2.0)
    i.AddCitation(c)
    s.append('Bar')
    i.AddAuthorsToCitation('ID', s)

    cl = i.GetCitations()
    self.assertEquals(len(cl), 1)
    al = cl[0].GetAuthorList()
    self.assertEquals(len(al), 2)
    self.assertEquals(al[0], 'Foo')
    self.assertEquals(al[1], 'Bar')

    self.assertEquals(i.GetMethod(), 'Deep-Fry')
    self.assertEquals(i.GetResolution(), 2.0)


  def test_mmcifinfo_biounit(self):
    b = io.MMCifInfoBioUnit()
    b.SetDetails('Details')
    self.assertEquals(b.GetDetails(), 'Details')
    b.AddChain('A')
    cl = b.GetChainList()
    self.assertEquals(cl[0], 'A')

    i = io.MMCifInfo()
    i.AddBioUnit(b)

    bl = i.GetBioUnits()
    self.assertEquals(len(bl), 1)


  def test_mmcifinfo_transoperation(self):
    o = io.MMCifInfoTransOp()
    o.SetID("1")
    self.assertEquals(o.GetID(), '1')
    o.SetType("identity operation")
    self.assertEquals(o.GetType(), 'identity operation')
    o.SetVector(1.0, 2.0, 3.0)
    self.assertEquals(o.GetVector().x, 1.0)
    self.assertEquals(o.GetVector().y, 2.0)
    self.assertEquals(o.GetVector().z, 3.0)
    o.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9)
    self.assertEquals(geom.Equal(o.GetMatrix(),
                                 geom.Mat3(1, 2, 3, 4, 5, 6, 7, 8, 9)), True)

    i = io.MMCifInfo()
    i.AddOperation(o)
    ol = i.GetOperations()
    self.assertEquals(ol[0].GetID(), '1')

    b = io.MMCifInfoBioUnit()
    b.AddOperations(ol)
    oll = b.GetOperations()
    self.assertEquals(oll[0][0].GetID(), '1')

  def test_mmcifinfo_biounit_pdbize(self):
    ent, seqres, info = io.LoadMMCIF("testfiles/mmcif/3T6C.cif.gz",
                                     seqres=True,
                                     info=True)
    pdb_ent = info.GetBioUnits()[0].PDBize(ent)
    pdb_seqres_ent = info.GetBioUnits()[0].PDBize(ent, seqres)

    # chains
    self.assertEquals(str(pdb_ent.GetChainList()[0]), 'A')
    self.assertEquals(str(pdb_ent.GetChainList()[1]), 'B')
    self.assertEquals(str(pdb_ent.GetChainList()[2]), '_')
    self.assertEquals(str(pdb_ent.GetChainList()[3]), '-')
    self.assertEquals(str(pdb_ent.GetChainList()[4]), 'C')
    self.assertEquals(str(pdb_ent.GetChainList()[5]), 'D')
    self.assertEquals(str(pdb_ent.GetChainList()[6]), 'E')
    self.assertEquals(str(pdb_ent.GetChainList()[7]), 'F')
    self.assertEquals(str(pdb_ent.GetChainList()[8]), 'G')
    self.assertEquals(str(pdb_ent.GetChainList()[9]), 'H')
    # size of chains
    self.assertEquals(len(pdb_ent.GetChainList()[0].GetResidueList()),  415)
    self.assertEquals(len(pdb_ent.GetChainList()[1].GetResidueList()),  414)
    self.assertEquals(len(pdb_ent.GetChainList()[2].GetResidueList()),   64)
    self.assertEquals(len(pdb_ent.GetChainList()[3].GetResidueList()), 3816)
    self.assertEquals(len(pdb_ent.GetChainList()[4].GetResidueList()),  415)
    self.assertEquals(len(pdb_ent.GetChainList()[5].GetResidueList()),  414)
    self.assertEquals(len(pdb_ent.GetChainList()[6].GetResidueList()),  415)
    self.assertEquals(len(pdb_ent.GetChainList()[7].GetResidueList()),  414)
    self.assertEquals(len(pdb_ent.GetChainList()[8].GetResidueList()),  415)
    self.assertEquals(len(pdb_ent.GetChainList()[9].GetResidueList()),  414)

    self.assertEquals(str(pdb_seqres_ent.GetChainList()[0]), 'A')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[1]), 'B')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[2]), '_')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[3]), '-')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[4]), 'C')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[5]), 'D')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[6]), 'E')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[7]), 'F')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[8]), 'G')
    self.assertEquals(str(pdb_seqres_ent.GetChainList()[9]), 'H')

    self.assertEquals(len(pdb_seqres_ent.GetChainList()[0].GetResidueList()),
                      415)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[1].GetResidueList()),
                      414)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[2].GetResidueList()),
                      64)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[3].GetResidueList()),
                      3816)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[4].GetResidueList()),
                      415)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[5].GetResidueList()),
                      414)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[6].GetResidueList()),
                      415)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[7].GetResidueList()),
                      414)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[8].GetResidueList()),
                      415)
    self.assertEquals(len(pdb_seqres_ent.GetChainList()[9].GetResidueList()),
                      414)

  def test_mmcifinfo_structdetails(self):
    d = io.MMCifInfoStructDetails()

    d.SetEntryID('1BAR')
    d.SetTitle('A Title')
    d.SetCASPFlag('N')
    d.SetDescriptor('FooBar')
    d.SetMass(1.0)
    d.SetMassMethod('Good Guess')
    d.SetModelDetails('Created with SwissModel')
    d.SetModelTypeDetails('Average')
    self.assertEquals(d.GetEntryID(), '1BAR')
    self.assertEquals(d.GetTitle(), 'A Title')
    self.assertEquals(d.GetCASPFlag(), 'N')
    self.assertEquals(d.GetDescriptor(), 'FooBar')
    self.assertEquals(d.GetMass(), 1.0)
    self.assertEquals(d.GetMassMethod(), 'Good Guess')
    self.assertEquals(d.GetModelDetails(), 'Created with SwissModel')  
    self.assertEquals(d.GetModelTypeDetails(), 'Average') 

    i = io.MMCifInfo()
    i.SetStructDetails(d)
    self.assertEquals(i.GetStructDetails().GetEntryID(), '1BAR')
    self.assertEquals(i.GetStructDetails().GetTitle(), 'A Title')
    self.assertEquals(i.GetStructDetails().GetCASPFlag(), 'N')
    self.assertEquals(i.GetStructDetails().GetDescriptor(), 'FooBar')
    self.assertEquals(i.GetStructDetails().GetMass(), 1.0)
    self.assertEquals(i.GetStructDetails().GetMassMethod(), 'Good Guess')
    self.assertEquals(i.GetStructDetails().GetModelDetails(),
                      'Created with SwissModel')
    self.assertEquals(i.GetStructDetails().GetModelTypeDetails(), 'Average')

  def test_mmcifinfo_obsolete(self):
    obs = io.MMCifInfoObsolete()
    obs.SetDate('2011-08-31')
    obs.SetID('SPRSDE')
    obs.SetPDBID('1FOO')
    obs.SetReplacedPDBID('2BAR')
    self.assertEquals(obs.GetDate(), '2011-08-31')
    self.assertEquals(obs.GetID(), 'Supersede')
    self.assertEquals(obs.GetPDBID(), '1FOO')
    self.assertEquals(obs.GetReplacedPDBID(), '2BAR')

    i = io.MMCifInfo()
    obs.id = 'OBSLTE'
    i.SetObsoleteInfo(obs)
    self.assertEquals(i.GetObsoleteInfo().GetDate(), '2011-08-31')
    self.assertEquals(i.GetObsoleteInfo().GetID(), 'Obsolete')
    self.assertEquals(i.GetObsoleteInfo().GetPDBID(), '1FOO')
    self.assertEquals(i.GetObsoleteInfo().GetReplacedPDBID(), '2BAR')

if __name__== '__main__':
  from ost import testutils
  testutils.RunTests()


