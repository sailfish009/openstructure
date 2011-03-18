from ost import settings
import subprocess
from xml.dom import minidom
from ost import io, seq
import ost
import re
import os
class AlignedPatch:
  def __init__(self, aln, bit_score, score, evalue):
    self.aln=aln
    self.bit_score=bit_score
    self.score=score
    self.evalue=evalue

class BlastHit:
  """
  A positive match found by BLAST.
  
  Each blast hit consists of one or more aligned patches. 
  
  """
  def __init__(self, identifier, aligned_patches):
    self.identifier=identifier
    self.aligned_patches=aligned_patches

def ParseBlastOutput(string):
  """
  Parses the blast output and returns a list of BlastHits
  """
  def _GetText(node):
    rc=''
    for child in node.childNodes:
        if child.nodeType==child.TEXT_NODE:
            rc+=child.data
    return rc
      
  def _GetValue(node, tag_name):
    tag=node.getElementsByTagName(tag_name)
    assert len(tag)==1
    return _GetText(tag[0])
    
  def _GetInt(node, tag_name):
    return int(_GetValue(node, tag_name))

  def _ParseHsp(query_id, hit_id, hsp):
    bit_score=float(_GetValue(hsp, 'Hsp_bit-score'))
    score=float(_GetValue(hsp, 'Hsp_score'))
    evalue=float(_GetValue(hsp, 'Hsp_evalue'))
    query_offset=_GetInt(hsp, 'Hsp_query-from')-1
    hit_offset=_GetInt(hsp, 'Hsp_hit-from')-1
    query_seq=seq.CreateSequence(str(query_id), str(_GetValue(hsp, 'Hsp_qseq')))
    query_seq.offset=query_offset
    hit_seq=seq.CreateSequence(str(hit_id), str(_GetValue(hsp, 'Hsp_hseq')))
    hit_seq.offset=hit_offset
    aln=seq.CreateAlignment(query_seq, hit_seq)
    return AlignedPatch(aln, bit_score, score, evalue)
  try:
    doc=minidom.parseString(string)
  except Exception, e:
    ost.LogError('Error while parsing BLAST output: %s' % str(e))
    return None
  hits=[]
  query_id=_GetValue(doc, 'BlastOutput_query-def')
  for hit in doc.getElementsByTagName('Hit'):
    hit_id=_GetValue(hit, 'Hit_def')
    hsps=hit.getElementsByTagName('Hsp')
    aligned_patches=[_ParseHsp(query_id, hit_id, hsp) for hsp in hsps]
    hits.append(BlastHit(hit_id, aligned_patches))
  return hits



class BlastError(RuntimeError):
  def __init__(self, brief, details):
    self.brief=brief
    self.details=details

  def __str__(self):
    if self.details:
      return '%s\n%s' % (self.brief, self.details)
    else:
      return self.brief

def Blast(query, database, gap_open=11, gap_ext=1, matrix='BLOSUM62',
         blast_location=None):
  """
  Runs a protein vs. protein blast search. The results are returned as a 
  list of :class:`BlastHit` instances.
  
  :param query: the query sequence
  :type query: :class:`seq.ConstSequenceHandle`
  
  :param database: The filename of the sequence database. Make sure that 
      formatdb has been run on the database and the <database>.pin file exists.
  :param matrix: The substitution matrix to be used. Must be one of 'BLOSUM45',
     'BLOSUM62', 'BLOSUM80', 'PAM30', 'PAM70'.
  :param gap_open: Gap opening penalty. Note that only a subset of gap opening 
     penalties is supported for each substitutition matrix. Consult the blast 
     docs for more information.
  :param gap_ext: Gap extension penalty. Only a subset of gap extension 
     penalties are supported for each of the substitution matrices. Consult the 
     blast docs for more information.
  """
  subst_mats=('BLOSUM45', 'BLOSUM62', 'BLOSUM80', 'PAM30', 'PAM70',)
  if matrix not in subst_mats:
    raise ValueError('matrix must be one of %s' % ', '.join(subst_mats))
  if not os.path.exists('%s.pin' % database):
    raise IOError("Database %s does not exist" % database)
  blastall_exe=settings.Locate('blastall', explicit_file_name=blast_location)
  args=[blastall_exe, '-d', database, '-p', 'blastp', 
        '-m', '7', '-M', matrix, '-G', str(gap_open), '-E', str(gap_ext)]
  ost.LogInfo('running BLAST (%s)' % ' '.join(args))
  blast_pipe=subprocess.Popen(args, stderr=subprocess.PIPE, 
                              stdout=subprocess.PIPE, stdin=subprocess.PIPE)
  stdout, stderr=blast_pipe.communicate(io.SequenceToString(query, 'fasta'))
  if len(stderr)>0:
     pattern=re.compile(r'^\[.*\]\s+ERROR:\s+(.*)')
     lines=stderr.split('\n')
     error_message=pattern.match(lines[0])
     if error_message:
       raise BlastError(error_message.group(1), '\n'.join(lines[1:]))
  return ParseBlastOutput(stdout)
