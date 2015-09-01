''' Unit tests for the HHblits wrapper
'''

import unittest
import os
import sys
import tempfile
import shutil
import filecmp

import ost
from ost import seq
from ost.bindings import hhblits

class TestHHblitsBindings(unittest.TestCase):
    def setUp(self):
        self.hhroot = os.getenv('EBROOTHHMINSUITE')

    def tearDown(self):
        if hasattr(self, 'hh'):
            self.hh.Cleanup()
        if hasattr(self, 'tmpfile'):
            os.remove(self.tmpfile)

    def testOSTSequenceNoWDir(self):
        # OST sequence as input, no working dir should work
        query_seq = seq.CreateSequence('1AKE.B',
                                       'MRIILLGAPGAGKGTQAQFIMEKYGIPQISTGDMLRA'+
                                       'AVKSGSELGKQAKDIMDAGKLVTDELVIALVKERIAQ'+
                                       'EDCRNGFLLDGFPRTIPQADAMKEAGINVDYVLEFDV'+
                                       'PDELIVDRIVGRRVHAPSGRVYHVKFNPPKVEGKDDV'+
                                       'TGEELTTRKDDQEETVRKRLVEYHQMTAPLIGYYYYS'+
                                       'KEAEAGNTKYAKVDGTKPVAEVRADLEKILG')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        # this works only as long as utils.TempDirWithFiles() names the first
        # fasta file seq01.fasta
        self.assertEqual(self.hh.filename[-11:], 'seq01.fasta')

    def testOSTSequenceWDir(self):
        # OST sequence as input, with working dir should work
        query_seq = seq.CreateSequence('1AKE.B',
                                       'MRIILLGAPGAGKGTQAQFIMEKYGIPQISTGDMLRA'+
                                       'AVKSGSELGKQAKDIMDAGKLVTDELVIALVKERIAQ'+
                                       'EDCRNGFLLDGFPRTIPQADAMKEAGINVDYVLEFDV'+
                                       'PDELIVDRIVGRRVHAPSGRVYHVKFNPPKVEGKDDV'+
                                       'TGEELTTRKDDQEETVRKRLVEYHQMTAPLIGYYYYS'+
                                       'KEAEAGNTKYAKVDGTKPVAEVRADLEKILG')
        tmpdir = tempfile.mkdtemp()
        self.hh = hhblits.HHblits(query_seq, self.hhroot, working_dir=tmpdir)
        self.assertEqual(self.hh.filename, os.path.join(tmpdir,
                                                        'query_hhblits.fasta'))
        self.assertEqual(self.hh.working_dir, tmpdir)
        self.hh.needs_cleanup = True

    def testFileNoWDir(self):
        # we have a sequence but no working dir
        self.hh = hhblits.HHblits('testfiles/seq2.fasta', self.hhroot)
        self.assertEqual(self.hh.filename[-10:], 'seq2.fasta')

    def testFileWDir(self):
        # we have a sequence and a working dir
        tmpdir = tempfile.mkdtemp()
        self.hh = hhblits.HHblits('testfiles/seq2.fasta', self.hhroot,
                                  working_dir=tmpdir)
        self.assertEqual(self.hh.filename, os.path.join(tmpdir, 'seq2.fasta'))
        self.assertEqual(self.hh.working_dir, tmpdir)
        self.hh.needs_cleanup = True

    def testDoCleanup(self):
        # test that cleanup cleans up
        tmpdir = tempfile.mkdtemp()
        self.hh = hhblits.HHblits('testfiles/seq2.fasta', self.hhroot,
                                  working_dir=tmpdir)
        self.assertTrue(os.path.isdir(tmpdir))
        self.hh.needs_cleanup = True
        self.hh.Cleanup()
        self.assertFalse(os.path.isdir(tmpdir))

    def testDoNotCleanup(self):
        # test that cleanup cleans up
        tmpdir = tempfile.mkdtemp()
        self.hh = hhblits.HHblits('testfiles/seq2.fasta', self.hhroot,
                                  working_dir=tmpdir)
        self.assertTrue(os.path.isdir(tmpdir))
        self.hh.needs_cleanup = False
        self.hh.Cleanup()
        self.assertTrue(os.path.isdir(tmpdir))
        shutil.rmtree(tmpdir)
        self.assertFalse(os.path.isdir(tmpdir))

    def testCleanupFailed(self):
        # test that cleanup cleans up
        tmpdir = tempfile.mkdtemp()
        self.hh = hhblits.HHblits('testfiles/seq2.fasta', self.hhroot,
                                  working_dir=tmpdir)
        self.assertTrue(os.path.isdir(tmpdir))
        self.hh.needs_cleanup = False
        self.hh.CleanupFailed()
        self.assertFalse(os.path.isdir(tmpdir))

    def testFileNoExists(self):
        # test on calling with a non-existing fasta file
        with self.assertRaises(IOError) as ioe:
            hhblits.HHblits('notthere.fasta', self.hhroot)
        self.assertEqual(ioe.exception.errno, 2)
        self.assertEqual(ioe.exception.strerror, 'No such file or directory')

    def testProfile(self):
        # try to create a search profile
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        a3m = self.hh.BuildQueryMSA('testfiles/hhblitsdb/hhblitsdb')
        self.assertTrue(filecmp.cmp(a3m, "testfiles/testali.a3m"))

    def testA3mToProfileFileName(self):
        # test A3mToProfile to work with a given hhmake_file name
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        _, self.tmpfile = tempfile.mkstemp(suffix='.hmm')
        os.remove(self.tmpfile)
        hhfile = self.hh.A3MToProfile("testfiles/testali.a3m",
                                      hhm_file=self.tmpfile)
        tfh = open(hhfile)
        efh = open("testfiles/test.hmm")
        elst = efh.readlines()
        tlst = tfh.readlines()
        self.assertEqual(len(elst), len(tlst))
        for i in range(0, len(elst)):
            if not elst[i].startswith(('FILE', 'COM', 'DATE')):
                self.assertEqual(elst[i], tlst[i])

    def testA3mToProfileWithoutFileName(self):
        # test A3mToProfile to work without a given hhmake_file name
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        hhfile = self.hh.A3MToProfile("testfiles/testali.a3m")
        tfh = open(hhfile)
        efh = open("testfiles/test.hmm")
        elst = efh.readlines()
        tlst = tfh.readlines()
        self.assertEqual(len(elst), len(tlst))
        for i in range(0, len(elst)):
            if not elst[i].startswith(('FILE', 'COM', 'DATE')):
                self.assertEqual(elst[i], tlst[i])
        os.remove(hhfile)

    def testA3mToProfileWithExistingFile(self):
        # test A3mToProfile to work with an existing hmm file
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        hhfile = self.hh.A3MToProfile("testfiles/testali.a3m",
                                      hhm_file="testfiles/test.hmm")
        # when the hmm file already exists, its not touched, so files should be
        # completely equal.
        self.assertTrue(filecmp.cmp(hhfile, "testfiles/test.hmm"))

    def testA3mToProfileWithoutA3m(self):
        # test A3mToProfile to fail if we do not have an a3m file.
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        with self.assertRaises(IOError) as ioe:
            self.hh.A3MToProfile("doesnotexist.a3m")
        self.assertEqual(ioe.exception.errno, None)
        self.assertEqual(ioe.exception.message,
                         "could not convert a3m to hhm file")

    def testA3mToCSFileName(self):
        # test A3mToCS to work with a given hhmake_file name
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        _, self.tmpfile = tempfile.mkstemp(suffix='.seq219')
        os.remove(self.tmpfile)
        csfile = self.hh.A3MToCS("testfiles/testali.a3m",
                                 cs_file=self.tmpfile, options={'--alphabet' :
                                                os.path.join(self.hh.hhlib_dir,
                                                             'data',
                                                             'cs219.lib')})
        self.assertTrue(filecmp.cmp(csfile, 'testfiles/test.seq219'))

    def testA3mToCSNoFileName(self):
        # test A3mToCS to work with a given hhmake_file name
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        csfile = self.hh.A3MToCS("testfiles/testali.a3m",
                                 options={'--alphabet' :
                                          os.path.join(self.hh.hhlib_dir,
                                                       'data',
                                                       'cs219.lib')})
        self.assertTrue(filecmp.cmp(csfile, 'testfiles/test.seq219'))
        self.tmpfile = "testfiles/testali.seq219"

    def testA3mToCSFileExists(self):
        # test A3mToCS to work with a given hhmake_file name
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        csfile = self.hh.A3MToCS("testfiles/testali.a3m",
                                 cs_file='testfiles/test.seq219',
                                 options={'--alphabet' :
                                          os.path.join(self.hh.hhlib_dir,
                                                       'data',
                                                       'cs219.lib')})
        self.assertTrue(filecmp.cmp(csfile, 'testfiles/test.seq219'))

    def testSearchWorking(self):
        # successful search
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        search_file = self.hh.Search("testfiles/testali.a3m",
                                     'testfiles/hhblitsdb/hhblitsdb')
        tfh = open(search_file)
        efh = open("testfiles/test.hhr")
        elst = efh.readlines()
        tlst = tfh.readlines()
        self.assertEqual(len(elst), len(tlst))
        for i in range(0, len(elst)):
            if not elst[i].startswith(('Date', 'Command')):
                self.assertEqual(elst[i], tlst[i])

    def testSearchNotWorking(self):
        # successful search
        query_seq = seq.CreateSequence('Test', 'VLSPADKTNVKAAWGKVGAHAGEYGAEA'+
                                       'LERMFLSFPTTKTYFPHFDLSHGSAQVKGHGKKVAD'+
                                       'ALTNAVAHVDDMPNALSALSDLHAHKLRVDPVNFKL'+
                                       'LSHCLLVTLAAHLPAEFTPAVHASLDKFLASVSTVL'+
                                       'TSKYR')
        self.hh = hhblits.HHblits(query_seq, self.hhroot)
        search_file = self.hh.Search("doesnotexist.a3m",
                                     'testfiles/hhblitsdb/hhblitsdb')
        self.assertEqual(search_file, None)

# search:
# search with non-existing a3m

if __name__ == "__main__":
    hhsuite_root_dir =  os.getenv('EBROOTHHMINSUITE')
    if not hhsuite_root_dir:
        print "No environment variable 'EBROOTHHMINSUITE'. To enable the "+\
            "unit test, this needs to point to your HHsuite installation."
        sys.exit(0)
    from ost import testutils
    testutils.RunTests()
