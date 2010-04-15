from ost.bindings import utils
from ost import settings, io, seq
import os
import subprocess

def ClustalW(seq1, seq2, clustalw=None):
  clustalw_path=settings.Locate(('clustalw', 'clustalw2'), 
                                explicit_file_name=clustalw)
  seq_list=seq.CreateSequenceList()
  seq_list.AddSequence(seq1)
  seq_list.AddSequence(seq2)  
  temp_dir=utils.TempDirWithFiles((seq_list,))
  out=os.path.join(temp_dir.dirname, 'out.fasta')
  command='%s -infile="%s" -output=fasta -outfile="%s"' % (clustalw_path,
                                                           temp_dir.files[0],
                                                           out)
  ps=subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
  ps.stdout.readlines()
  aln=io.LoadAlignment(out)
  temp_dir.Cleanup()
  return aln
  