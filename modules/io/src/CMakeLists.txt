add_subdirectory(mol)
add_subdirectory(seq)
add_subdirectory(img)
add_subdirectory(msgpack)

set(OST_IO_HEADERS
binary_data_sink.hh		
io_manager.hh
binary_data_source.hh		
module_config.hh
container_serialization.hh	
swap_util.hh
io_utils.hh
io_exception.hh
convert.hh
converting_streams.hh

formatted_line.hh
)

set(OST_IO_SOURCES
io_manager.cc
convert.cc
io_utils.cc
)
if (WIN32)
 set(OST_IO_SOURCES ${OST_IO_SOURCES} zlib.cpp gzip.cpp)
endif(WIN32)

foreach(fname ${OST_IO_MOL_SOURCES})
  set(OST_IO_SOURCES ${OST_IO_SOURCES} mol/${fname})
endforeach(fname ${OST_IO_MOL_SOURCES})

foreach(fname ${OST_IO_SEQ_SOURCES})
  set(OST_IO_SOURCES ${OST_IO_SOURCES} seq/${fname})
endforeach(fname ${OST_IO_SEQ_SOURCES})

if (ENABLE_IMG)
  foreach(fname ${OST_IO_IMG_SOURCES})
    set(OST_IO_SOURCES ${OST_IO_SOURCES} img/${fname})
  endforeach(fname ${OST_IO_IMG_SOURCES})
endif()

####################################
# Add headers in subfolders (additionally) to OST_IO_HEADERS.
# In the end, we will have two copies (!) of each header in the subfolders.
# One copy in ost/io/<SUB> and one in ost/io.

# As it's silly to have duplicate headers, this behavior might be removed in the
# future. So please always include the headers in the subfolders.
# (e.g. use "#include <ost/io/mol/load_entity.hh>"
#       instead of #include <ost/io/load_entity.hh>)
# For compatibility reasons, we keep it for now (dates back to 2010).
foreach(fname ${OST_IO_MOL_HEADERS})
  set(OST_IO_HEADERS ${OST_IO_HEADERS} mol/${fname})
endforeach(fname ${OST_IO_MOL_HEADERS})

foreach(fname ${OST_IO_SEQ_HEADERS})
  set(OST_IO_HEADERS ${OST_IO_HEADERS} seq/${fname})
endforeach(fname ${OST_IO_SEQ_HEADERS})

if (ENABLE_IMG)
  foreach(fname ${OST_IO_IMG_HEADERS})
    set(OST_IO_HEADERS ${OST_IO_HEADERS} img/${fname})
  endforeach(fname ${OST_IO_IMG_HEADERS})
endif()
####################################

set(OST_IO_DEPENDENCIES ost_base;ost_conop;ost_seq)
if (ENABLE_IMG)
  set(OST_IO_DEPENDENCIES ${OST_IO_DEPENDENCIES};ost_img;ost_img_alg)
endif()
module(NAME io SOURCES "${OST_IO_SOURCES}" 
       HEADERS ${OST_IO_MOL_HEADERS} IN_DIR mol
               ${OST_IO_IMG_HEADERS} IN_DIR img
               ${OST_IO_SEQ_HEADERS} IN_DIR seq
               # Including msgpack by long lines of dirs looks ugly but is the
               # most simple way to do this.
               ${OST_IO_MSGPACK_HEADERS} IN_DIR msgpack
               ${OST_IO_MSGPACK_MSGPACK_HEADERS} IN_DIR msgpack/msgpack
               ${OST_IO_MSGPACK_MSGPACK_ADAPTOR_HEADERS} IN_DIR msgpack/msgpack/adaptor
               ${OST_IO_MSGPACK_MSGPACK_ADAPTOR_BOOST_HEADERS} IN_DIR msgpack/msgpack/adaptor/boost
               ${OST_IO_MSGPACK_MSGPACK_ADAPTOR_TR1_HEADERS} IN_DIR msgpack/msgpack/adaptor/tr1
               ${OST_IO_MSGPACK_MSGPACK_ADAPTOR_CPP11_HEADERS} IN_DIR msgpack/msgpack/adaptor/cpp11
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HEADERS} IN_DIR msgpack/msgpack/predef
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_LANGUAGE_HEADERS} IN_DIR msgpack/msgpack/predef/language
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_HEADERS} IN_DIR msgpack/msgpack/predef/architecture
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_X86_HEADERS} IN_DIR msgpack/msgpack/predef/architecture/x86
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_DETAIL_HEADERS} IN_DIR msgpack/msgpack/predef/detail
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_OS_HEADERS} IN_DIR msgpack/msgpack/predef/os
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_OS_BSD_HEADERS} IN_DIR msgpack/msgpack/predef/os/bsd
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_PLATFORM_HEADERS} IN_DIR msgpack/msgpack/predef/platform
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_HEADERS} IN_DIR msgpack/msgpack/predef/hardware
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_SIMD_HEADERS} IN_DIR msgpack/msgpack/predef/hardware/simd
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_SIMD_ARM_HEADERS} IN_DIR msgpack/msgpack/predef/hardware/simd/arm
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_SIMD_PPC_HEADERS} IN_DIR msgpack/msgpack/predef/hardware/simd/ppc
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_SIMD_X86_HEADERS} IN_DIR msgpack/msgpack/predef/hardware/simd/x86
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_HARDWARE_SIMD_X86_AMD_HEADERS} IN_DIR msgpack/msgpack/predef/hardware/simd/x86_amd
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_COMPILER_HEADERS} IN_DIR msgpack/msgpack/predef/compiler
               ${OST_IO_MSGPACK_MSGPACK_PREDEF_LIBRARY_HEADERS} IN_DIR msgpack/msgpack/predef/library
               ${OST_IO_MSGPACK_PREPROCESSOR_HEADERS} IN_DIR msgpack/msgpack/preprocessor
               ${OST_IO_MSGPACK_PREPROCESSOR_ARITHMETIC_HEADERS} IN_DIR msgpack/msgpack/preprocessor/arithmetic
               ${OST_IO_MSGPACK_PREPROCESSOR_ARITHMETIC_DETAIL_HEADERS} IN_DIR msgpack/msgpack/preprocessor/arithmetic/detail
               ${OST_IO_MSGPACK_PREPROCESSOR_ARRAY_HEADERS} IN_DIR msgpack/msgpack/preprocessor/array
               ${OST_IO_MSGPACK_PREPROCESSOR_ARRAY_DETAIL_HEADERS} IN_DIR msgpack/msgpack/preprocessor/array/detail
               ${OST_IO_MSGPACK_PREPROCESSOR_COMPARISON_HEADERS} IN_DIR msgpack/msgpack/preprocessor/comparison
               ${OST_IO_HEADERS}
       DEPENDS_ON ${OST_IO_DEPENDENCIES})
target_link_libraries(ost_io ${BOOST_IOSTREAM_LIBRARIES})
if (ENABLE_IMG)
  target_link_libraries(ost_io ${TIFF_LIBRARIES} ${PNG_LIBRARIES})
endif()
