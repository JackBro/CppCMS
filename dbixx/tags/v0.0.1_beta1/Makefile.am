noinst_PROGRAMS = test
test_SOURCES = test.cpp
test_LDADD = libdbixx.la

lib_LTLIBRARIES     = libdbixx.la

libdbixx_la_SOURCES = row.cpp session.cpp result.cpp
libdbixx_la_LDFLAGS  = -version-info 0:0:0
libdbixx_la_CXXFLAGS = -Wall

nobase_pkginclude_HEADERS = dbixx.h
