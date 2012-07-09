if (NOT WIN32)
executable(NAME molck SOURCES main.cc
           DEPENDS_ON ost_io STATIC)
endif(WIN32)
