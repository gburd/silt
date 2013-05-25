#hashdb_test code
noinst_PROGRAMS = testSilt testIterator testTrie testCuckoo testCombi testByYCSBWorkload benchCuckoo benchSemiRandomWrite benchStores preprocessTrace
testSilt_SOURCES = testSilt.cc
testSilt_CPPFLAGS = 				\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src			\
	-I$(top_builddir)/src/gen-cpp

testSilt_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

testIterator_SOURCES = testIterator.cc
testIterator_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

testIterator_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

testTrie_SOURCES = testTrie.cc
testTrie_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

testTrie_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

testCuckoo_SOURCES = testCuckoo.cc
testCuckoo_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

testCuckoo_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

testCombi_SOURCES = testCombi.cc
testCombi_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

testCombi_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)


testByYCSBWorkload_SOURCES= testByYCSBWorkload.cc
testByYCSBWorkload_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

testByYCSBWorkload_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

benchCuckoo_SOURCES = benchCuckoo.cc
benchCuckoo_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

benchCuckoo_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

benchSemiRandomWrite_SOURCES = benchSemiRandomWrite.cc
benchSemiRandomWrite_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

benchSemiRandomWrite_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)

benchStores_SOURCES = benchStores.cc
benchStores_CPPFLAGS = 			\
	-I$(top_srcdir)/utils 			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src		\
	-I$(top_builddir)/src/gen-cpp

benchStores_LDADD = 				\
	$(top_builddir)/src/libsilt.la 	\
	$(top_builddir)/utils/libsiltkvutils.la \
	$(THRIFT_LIBS)


preprocessTrace_SOURCES= preprocessTrace.cc

preprocessTrace_CPPFLAGS =

preprocessTrace_LDADD =
