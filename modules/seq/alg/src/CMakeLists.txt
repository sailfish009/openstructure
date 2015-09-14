set(OST_SEQ_ALG_HEADERS
sequence_identity.hh
sequence_similarity.hh
module_config.hh
ins_del.hh
subst_weight_matrix.hh
alignment_opts.hh
entropy.hh
merge_pairwise_alignments.hh
conservation.hh
local_align.hh
global_align.hh
semiglobal_align.hh
)

set(OST_SEQ_ALG_SOURCES
merge_pairwise_alignments.cc
local_align.cc
global_align.cc
semiglobal_align.cc
entropy.cc
sequence_identity.cc
sequence_similarity.cc
ins_del.cc
conservation.cc
subst_weight_matrix.cc
)

module(NAME seq_alg HEADER_OUTPUT_DIR ost/seq/alg SOURCES ${OST_SEQ_ALG_SOURCES}
       HEADERS ${OST_SEQ_ALG_HEADERS} DEPENDS_ON ost_seq)


