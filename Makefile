FILES = agora article database feed logger

LOGFILE = agora.log

RUNCMD = $(BINDIR)/agora whatif.atom test.sqlite

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

DEFINE = -DLOGFILE=\"$(LOGFILE)\"
INCLUDE = -I./$(INCDIR) -lpugixml -lsqlite3
FLAGS = $(DEFINE) $(INCLUDE) --std=c++11

SOURCES = $(FILES:%=$(SRCDIR)/%.cxx)
HEADERS = $(FILES:%=$(INCDIR)/%.hxx) $(addprefix $(INCDIR)/,logger.tcc)
OBJECTS = $(FILES:%=$(OBJDIR)/%.o)
DEPENDS = $(FILES:%=$(OBJDIR)/%.d)


$(BINDIR)/agora: $(OBJDIR) $(BINDIR) $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(FLAGS)

debug: FLAGS += -g -DDEBUG
debug: $(BINDIR)/agora

# Provide simple name to call
docs: $(DOCDIR)/html
$(DOCDIR)/html: $(DOCDIR)/Doxyfile $(SOURCES) $(HEADERS)
	doxygen $<

.PHONY: clean clean-docs
clean:
	-rm -f $(OBJDIR)/* $(LOGFILE) $(BINDIR)/agora
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
	g++ -o $@ -c $< -MMD $(FLAGS)

-include $(DEPENDS)
