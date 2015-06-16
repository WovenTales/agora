FILES = agora article database feed logger

LOGFILE = agora.log

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

DEFINE = -DLOGFILE=\"$(LOGFILE)\"
INCLUDE = -I./$(INCDIR) -lpugixml -lsqlite3
FLAGS = $(DEFINE) $(INCLUDE)

SOURCES = $(FILES:%=$(SRCDIR)/%.cxx)
HEADERS = $(FILES:%=$(INCDIR)/%.hxx) $(addprefix $(INCDIR)/,logger.tcc)
OBJECTS = $(FILES:%=$(OBJDIR)/%.o)


$(BINDIR)/agora: $(OBJDIR) $(BINDIR) $(OBJECTS)
	g++ -o $(BINDIR)/agora $(OBJECTS) $(FLAGS)

debug: FLAGS += -g -DDEBUG
debug: $(BINDIR)/agora

# Provide simple name to call
docs: $(DOCDIR)/html
$(DOCDIR)/html: $(DOCDIR)/Doxyfile $(SOURCES) $(HEADERS)
	doxygen $(DOCDIR)/Doxyfile

.PHONY: clean clean-docs
clean:
	-rm -f $(OBJECTS) $(LOGFILE) $(BINDIR)/agora
	-rmdir $(BINDIR) $(OBJDIR)
clean-docs:
	find docs/* ! -iname 'Doxyfile' -print0 | xargs -0 rm -rf

.PHONY: run
run:
	bin/agora whatif.atom test.sqlite


$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)

#TODO: Figure out how to separate .o and .hxx dependencies for proper cascading rebuilding
#        (eg. editing logger.tcc should rebuild everything, but would only rebuild logger.o)
$(OBJDIR)/agora.o:    $(addprefix $(INCDIR)/,article.hxx database.hxx feed.hxx)
$(OBJDIR)/article.o:  $(addprefix $(INCDIR)/,agora.hxx feed.hxx logger.hxx)
$(OBJDIR)/database.o: $(addprefix $(INCDIR)/,agora.hxx article.hxx feed.hxx logger.hxx)
$(OBJDIR)/feed.o:     $(addprefix $(INCDIR)/,agora.hxx logger.hxx)
$(OBJDIR)/logger.o:   $(addprefix $(INCDIR)/,logger.tcc)

$(OBJDIR)/%.o: $(SRCDIR)/%.cxx $(INCDIR)/%.hxx
	g++ $(FLAGS) -c -o $(<:$(SRCDIR)%.cxx=$(OBJDIR)%.o) $<
