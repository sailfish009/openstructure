set(OST_SEQ_ALG_HEADERS
sequence_identity.hh
module_config.hh
ins_del.hh
subst_weight_matrix.hh
local_align.hh
alignment_opts.hh
global_align.hh
)

set(OST_SEQ_ALG_SOURCES
global_align.cc
sequence_identity.cc
ins_del.cc
subst_weight_matrix.cc
local_align.cc
)

module(NAME seq_alg HEADER_OUTPUT_DIR ost/seq/alg SOURCES ${OST_SEQ_ALG_SOURCES}
       HEADERS ${OST_SEQ_ALG_HEADERS} DEPENDS_ON seq)


