LIBS=-lcppcms

all: clean hello-tmpl

hello-tmpl: hello-tmpl.cpp view.cpp content.h
	$(CXX) -Wall hello-tmpl.cpp view.cpp -o hello-tmpl.fcgi ${LIBS}

view.cpp: first.tmpl content.h
	cppcms_tmpl_cc first.tmpl -o view.cpp

clean:
	rm -fr *.fcgi *.exe *.so view.cpp cppcms_rundir

