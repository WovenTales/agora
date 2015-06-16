FILES = agora article database feed logger ncursesui ncursesfeedlist

LOGFILE = agora.log

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

DEFINE = -DLOGFILE=\"$(LOGFILE)\"
INCLUDE = -I./$(INCDIR) -lpugixml -lsqlite3 -lncurses
FLAGS = $(DEFINE) $(INCLUDE)

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
	-rm -f $(OBJECTS) $(DEPENDS) $(LOGFILE) $(BINDIR)/agora
	-rmdir $(BINDIR) $(OBJDIR)
clean-docs:
	find docs/* ! -iname 'Doxyfile' -print0 | xargs -0 rm -rf

.PHONY: run
run:
	bin/agora test.sqlite


$(OBJDIR):
	mkdir $(OBJDIR)
$(BINDIR):
	mkdir $(BINDIR)


$(OBJDIR)/%.o: $(SRCDIR)/%.cxx
	g++ -o $@ -c $< -MMD $(FLAGS)

-include $(DEPENDS)
