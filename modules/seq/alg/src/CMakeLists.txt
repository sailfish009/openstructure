set(OST_SEQ_ALG_HEADERS
sequence_identity.hh
module_config.hh
ins_del.hh
subst_weight_matrix.hh
alignment_opts.hh
merge_pairwise_alignments.hh
conservation.hh
)

set(OST_SEQ_ALG_SOURCES
merge_pairwise_alignments.cc
sequence_identity.cc
ins_del.cc
subst_weight_matrix.cc
conservation.cc
)

module(NAME seq_alg HEADER_OUTPUT_DIR ost/seq/alg SOURCES ${OST_SEQ_ALG_SOURCES}
       HEADERS ${OST_SEQ_ALG_HEADERS} DEPENDS_ON ost_seq)


