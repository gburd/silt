noinst_LTLIBRARIES=		libsilt.la
libsilt_la_CPPFLAGS=		-I$(top_srcdir)/utils -D_FILE_OFFSET_BITS=64 \
				-I$(builddir)/gen-cpp -Icindex
libsilt_la_LIBADD=		$(top_builddir)/utils/libsiltkvutils.la

noinst_HEADERS= \
	bdb.h \
	configuration.h \
	file_io.h \
	file_store.h \
	global_limits.h \
	hash_functions.h \
	hash_table_cuckoo.h \
	hash_table_default.h \
	jsmin.h \
	rate_limiter.h \
	silt.h \
	silt_combi.h \
	silt_factory.h \
	silt_iterator.h \
	silt_monitor.h \
	silt_partition.h \
	silt_proxy.h \
	silt_sf.h \
	silt_sf_ordered_trie.h \
	silt_types.h \
	sorter.h \
	task.h \
	value.h

libsilt_la_SOURCES= \
	bdb.cc \
	cindex/bit_access.cc \
	cindex/bit_vector.cc \
	cindex/bucketing_index.cc \
	cindex/expected_size.cc \
	cindex/flat_absoff_bucketing.cc \
	cindex/semi_direct_16_absoff_bucketing.cc \
	cindex/semi_direct_16_reloff_bucketing.cc \
	cindex/twolevel_absoff_bucketing.cc \
	cindex/twolevel_reloff_bucketing.cc \
	configuration.cc \
	file_io.cc \
	file_store.cc \
	global_limits.cc \
	hash_functions.cc \
	hash_table_cuckoo.cc \
	hash_table_default.cc \
	jsmin.c \
	rate_limiter.cc \
	silt.cc \
	silt_combi.cc \
	silt_factory.cc \
	silt_iterator.cc \
	silt_monitor.cc \
	silt_partition.cc \
	silt_proxy.cc \
	silt_sf.cc \
	silt_sf_ordered_trie.cc \
	sorter.cc \
	task.cc

CLEANFILES=		$(BUILT_SOURCES)


# Note: (kaminsky) the ollowing line forces noinst_* libraries to build shared.
# This can help with development because a change to this library doesn't
# require re-building libs and programs that link against this library
#libsilt_la_LDFLAGS = -rpath `pwd`

noinst_PROGRAMS=		silt_bench
silt_bench_SOURCES=		silt_bench.cc
silt_bench_CPPFLAGS=		-I$(top_srcdir)/utils -I$(builddir)/gen-cpp
silt_bench_LDADD=		$(top_builddir)/utils/libsiltkvutils.la libsilt.la -lxerces-c
