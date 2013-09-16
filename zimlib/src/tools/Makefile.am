AM_CPPFLAGS=-I$(top_builddir)/include
if MAKE_BENCHMARK
  ZIMBENCH = zimbench
endif
bin_PROGRAMS = zimdump zimsearch zimdiff zimpatch $(ZIMBENCH)
zimdump_SOURCES = zimDump.cpp
zimsearch_SOURCES = zimSearch.cpp
zimbench_SOURCES = zimBench.cpp
zimdiff_SOURCES = zimDiff.cpp
zimpatch_SOURCES = zimPatch.cpp
LDADD = $(top_builddir)/src/libzim.la
