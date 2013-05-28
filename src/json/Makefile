YACC=		bison
LEX=		flex
CXX=		g++

CXXFLAGS=	-DDEBUG_LOG

TARGET=		json
LEX_SRC=	$(wildcard *.l)
YACC_SRC=	$(wildcard *.y)
CPP_SRC=	$(filter-out %.lex.cc %.tab.cc,$(wildcard *.cc))
SRC=		$(patsubst %.y,%.tab.cc,$(YACC_SRC)) \
		$(patsubst %.l,%.lex.cc,$(LEX_SRC)) $(CPP_SRC)
OBJ=		$(patsubst %.cc,%.o,$(SRC))

all: $(OBJ)
	$(CXX) -o test $(OBJ) -lFl

clean:
	rm $(OBJ) \
	$(patsubst %.y,%.tab.cc,$(YACC_SRC)) \
	$(patsubst %.l,%.lex.cc,$(LEX_SRC))


$(TARGET): $(OBJ)
	$(CXX)  -o $* $(OBJ)

.PRECIOUS: %.tab.cc

%.tab.cc: %.y
	$(YACC) -o $@ -d $<

.PRECIOUS: %.lex.cc

%.lex.cc: %.l
	$(LEX) -t $< > $@
