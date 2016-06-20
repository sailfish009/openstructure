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
pair_subst_weight_matrix.hh
contact_weight_matrix.hh
contact_prediction_score.hh
data/default_pair_subst_weight_matrix.hh
data/default_contact_weight_matrix.hh
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
contact_weight_matrix.cc
subst_weight_matrix.cc
pair_subst_weight_matrix.cc
contact_prediction_score.cc
)

module(NAME seq_alg HEADER_OUTPUT_DIR ost/seq/alg SOURCES ${OST_SEQ_ALG_SOURCES}
       HEADERS ${OST_SEQ_ALG_HEADERS} DEPENDS_ON ost_seq)


