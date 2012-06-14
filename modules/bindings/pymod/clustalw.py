from ost.bindings import utils
from ost import settings, io, seq, LogError
import os
import subprocess

def ClustalW(seq1, seq2=None, clustalw=None, keep_files=False, nopgap=False, 
             clustalw_option_string=False):
  clustalw_path=settings.Locate(('clustalw', 'clustalw2'), 
                                explicit_file_name=clustalw)
  
  if seq2!=None:
    if isinstance(seq1, seq.SequenceHandle) and isinstance(seq2, seq.SequenceHandle):
      seq_list=seq.CreateSequenceList()
      seq_list.AddSequence(seq1)
      seq_list.AddSequence(seq2)
    elif isinstance(seq1, str) and isinstance(seq2, str):
      seqh1=seq.CreateSequence("seq1", seq1)
      seqh2=seq.CreateSequence("seq2", seq2)
      seq_list=seq.CreateSequenceList()
      seq_list.AddSequence(seqh1)
      seq_list.AddSequence(seqh2)
    else:
      LogError("WARNING: Specify at least two Sequences")
      return
  elif isinstance(seq1, seq.SequenceList):
    seq_list=seq1
  else:
    LogError("WARNING: Specify either two SequenceHandles or one SequenceList")
    return

  new_list = seq.CreateSequenceList()
  for s in seq_list:
    ss = seq.CreateSequence( s.GetName(), s.GetString().replace('?','X') )
    new_list.AddSequence(ss)

  seq_list = new_list
  
  temp_dir=utils.TempDirWithFiles((seq_list,))
  out=os.path.join(temp_dir.dirname, 'out.fasta')
  command='%s -infile="%s" -output=fasta -outfile="%s"' % (clustalw_path,
                                                           temp_dir.files[0],
                                                           out)
  if nopgap:
    command+=" -nopgap"
  if clustalw_option_string!=False:
    command=command+" "+clustalw_option_string  #see useful flags: http://toolkit.tuebingen.mpg.de/clustalw/help_params

  ps=subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
  ps.stdout.readlines()
  aln=io.LoadAlignment(out)
  

  for sequence in seq_list:
    for seq_num,aln_seq in enumerate(aln.sequences):
      if aln_seq.GetName()==sequence.GetName():
        break
    aln.SetSequenceOffset(seq_num,sequence.offset)
    if sequence.HasAttachedView():
      aln.AttachView(seq_num,sequence.GetAttachedView().Copy())

  if not keep_files:
    temp_dir.Cleanup()
  
  return aln
