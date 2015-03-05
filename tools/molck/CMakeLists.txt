if (NOT WIN32)
executable(NAME molck SOURCES main.cc
           DEPENDS_ON ost_io STATIC)
endif(NOT WIN32)
