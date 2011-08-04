import unittest
from ost import *
from ost import settings
from ost import seq
from ost import io

class TestAlignToSeqRes(unittest.TestCase):
  def testAlignWorking(self):
    ent, seqres = io.LoadMMCIF("testfiles/align_to_seqres.mmcif")
    chain =  ent.FindChain("A")
    sequence = seqres.FindSequence(chain.GetName());
    seqres_aln = seq.alg.AlignToSEQRES(chain, sequence, try_resnum_first=False)
    self.assertEqual(str(sequence), "MYTNSDFVVIKALEDGVNVIGLTRGADTRFHHSEKLDKGEV"+
                     "LIAQFTEHTSAIKVRGKAYIQTRHGVIESEGKK")
    self.assertEqual(str(seqres_aln.sequences[1]), "----SDFVVIKALEDGVNVIGLTR--"+
                     "-TRFHHSEKLDKGEVLIAQFTEHTSAIKVRGKAYIQTRHGVIESEGK-")

    seqres_aln = seq.alg.AlignToSEQRES(chain, sequence, try_resnum_first=True)
    self.assertEqual(str(seqres_aln.sequences[1]), "----SDFVVIKALEDGVNVIGLTR--"+
                     "-TRFHHSEKLDKGEVLIAQFTEHTSAIKVRGKAYIQTRHGVIESEGK-")

  def testAlignFail(self):
    ent, seqres = io.LoadMMCIF("testfiles/align_to_seqres_valueerror.mmcif")
    chain =  ent.FindChain("A")
    sequence = seqres.FindSequence(chain.GetName());
    ost.PushVerbosityLevel(0)
    self.assertRaises(ValueError, seq.alg.AlignToSEQRES, chain, sequence, True)
    ost.PopVerbosityLevel()

if __name__ == "__main__":
  try:
    unittest.main()
  except Exception, e:
    print e
