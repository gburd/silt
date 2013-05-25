noinst_LTLIBRARIES = libsilt.la
libsilt_la_CPPFLAGS = -I$(top_srcdir)/utils -D_FILE_OFFSET_BITS=64 \
			-I$(builddir)/gen-cpp -Icindex
libsilt_la_LIBADD = $(top_builddir)/utils/libsiltkvutils.la


noinst_HEADERS = 	DOMTreeErrorReporter.hpp	\
			DOMWriteErrorHandler.hpp	\
			configuration.h			\
			file_io.h			\
			task.h				\
			value.h				\
			rate_limiter.h			\
			global_limits.h         \
			silt_types.h			\
			silt_iterator.h		\
			silt.h			\
			silt_factory.h		\
			hash_functions.h		\
			hash_table_default.h		\
			hash_table_cuckoo.h		\
			file_store.h			\
			sorter.h			\
			silt_partition.h		\
			silt_combi.h			\
			silt_sf.h			\
			silt_sf_ordered_trie.h	\
			silt_proxy.h			\
			silt_monitor.h		\
			bdb.h

libsilt_la_SOURCES =  DOMTreeErrorReporter.cpp	\
			DOMWriteErrorHandler.cpp	\
			configuration.cc		\
			file_io.cc			\
			task.cc				\
			rate_limiter.cc			\
			global_limits.cc         \
			silt_iterator.cc		\
			silt.cc			\
			silt_factory.cc		\
			hash_functions.cc		\
			hash_table_default.cc		\
			hash_table_cuckoo.cc		\
			file_store.cc			\
			sorter.cc			\
			silt_partition.cc		\
			silt_combi.cc			\
			silt_sf.cc			\
			silt_sf_ordered_trie.cc			\
			silt_proxy.cc			\
			silt_monitor.cc			\
			bdb.cc				\
			cindex/bit_access.cpp			\
			cindex/bit_vector.cpp			\
			cindex/expected_size.cpp			\
			cindex/flat_absoff_bucketing.cpp		\
			cindex/twolevel_absoff_bucketing.cpp	\
			cindex/twolevel_reloff_bucketing.cpp	\
			cindex/semi_direct_16_absoff_bucketing.cpp	\
			cindex/semi_direct_16_reloff_bucketing.cpp	\
			cindex/bucketing_index.cpp

CLEANFILES = $(BUILT_SOURCES)


# kaminsky: following line forces noinst_* libraries to build
# shared.  This can help with development because a change to
# this library doesn't require re-building libs and programs
# that link against this library
#libsilt_la_LDFLAGS = -rpath `pwd`

noinst_PROGRAMS = silt_bench
silt_bench_SOURCES = silt_bench.cc
silt_bench_CPPFLAGS = -I$(top_srcdir)/utils -I$(builddir)/gen-cpp
silt_bench_LDADD = $(top_builddir)/utils/libsiltkvutils.la libsilt.la -lxerces-c
