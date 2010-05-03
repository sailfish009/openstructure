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
  taln=io.LoadAlignment(out)
  aln=seq.CreateAlignment()
  if (seq1.HasAttachedView() and seq2.HasAttachedView()):
    seq00=taln.GetSequence(0).Copy()
    seq00.AttachView(seq1.GetAttachedView().Copy())
    seq01=taln.GetSequence(1).Copy()
    seq01.AttachView(seq2.GetAttachedView().Copy())
    aln.AddSequence(seq00)
    aln.AddSequence(seq01)
  else:
    aln=taln.Copy()

  #~ temp_dir.Cleanup()
  
  return aln
  
