set(OST_SEQ_ALG_HEADERS
sequence_identity.hh
module_config.hh
ins_del.hh
subst_weight_matrix.hh
alignment_opts.hh
merge_pairwise_alignments.hh
conservation.hh
local_align.hh
global_align.hh
)

set(OST_SEQ_ALG_SOURCES
merge_pairwise_alignments.cc
local_align.cc
global_align.cc
sequence_identity.cc
ins_del.cc
conservation.cc
)

if (ENABLE_INFO)
  list (APPEND OST_SEQ_ALG_SOURCES subst_weight_matrix.cc)
endif()

module(NAME seq_alg HEADER_OUTPUT_DIR ost/seq/alg SOURCES ${OST_SEQ_ALG_SOURCES}
       HEADERS ${OST_SEQ_ALG_HEADERS} DEPENDS_ON ost_seq)


