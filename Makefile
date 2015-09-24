FILES   = agora article database feed logger
INCHEAD =
INCONLY = database.tcc logger.tcc

LIBRARIES = curl pugixml sqlite3

LOGFILE = agora.log

RUNCMD = $(BINDIR)/agora test.sqlite

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

FLAGS = --std=c++11
DEBUG = -g -DDEBUG


DEFINE = -DLOGFILE=\"$(LOGFILE)\"
INCLUDE = -I./$(INCDIR) $(LIBRARIES:%=-l%)
PARAMS = $(DEFINE) $(INCLUDE) $(FLAGS)

SOURCES = $(FILES:%=$(SRCDIR)/%.cxx)
HEADERS = $(FILES:%=$(INCDIR)/%.hxx) $(INCHEAD:%=$(INCDIR)/%.hxx) $(INCONLY:%=$(INCDIR)/%)
OBJECTS = $(FILES:%=$(OBJDIR)/%.o)
DEPENDS = $(FILES:%=$(OBJDIR)/%.d)


$(BINDIR)/agora: $(OBJDIR) $(BINDIR) $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(PARAMS)

debug: PARAMS += $(DEBUG)
debug: $(BINDIR)/agora

# Provide simple name to call
docs: $(DOCDIR)/html
$(DOCDIR)/html: $(DOCDIR)/Doxyfile $(SOURCES) $(HEADERS)
	doxygen $<

.PHONY: clean clean-docs
clean:
	-rm -fr $(OBJDIR)/* $(LOGFILE) $(BINDIR)/agora
	-rmdir $(BINDIR) $(OBJDIR)
clean-docs:
	find docs/* ! -iname 'Doxyfile' -print0 | xargs -0 rm -rf

.PHONY: run grind grind-full grind-all
run:
	$(RUNCMD)
grind:
	valgrind $(RUNCMD)
grind-full:
	valgrind --leak-check=full $(RUNCMD)
grind-all:
	valgrind --leak-check=full --show-leak-kinds=all $(RUNCMD)


$(OBJDIR):
	mkdir $(OBJDIR)
$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cxx
	g++ -o $@ -c $< -MMD $(PARAMS)

-include $(DEPENDS)
