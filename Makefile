FILES = agora feed article database logger

SRCDIR = src
INCDIR = $(SRCDIR)
OBJDIR = obj
BINDIR = bin

DEFINE = -DLOGFILE=\"agora.log\"
INCLUDE = -I./$(INCDIR) -lpugixml -lsqlite3
FLAGS = $(DEFINE) $(INCLUDE) -c

OBJECTS = $(FILES:%=$(OBJDIR)/%.o)


$(BINDIR)/agora: $(OBJDIR) $(BINDIR) $(OBJECTS)
	g++ -o $(BINDIR)/agora $(OBJECTS) $(INCLUDE)

debug: FLAGS += -g -DDEBUG
debug: feedarium

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BINDIR)/agora
	-rmdir $(BINDIR) $(OBJDIR)


$(INCDIR):
	mkdir -p $(INCDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/agora.o: $(addprefix $(INCDIR)/,database.hxx feed.hxx)
$(OBJDIR)/article.o: $(addprefix $(INCDIR)/,agora.hxx logger.hxx)
$(OBJDIR)/database.o: $(addprefix $(INCDIR)/,agora.hxx article.hxx feed.hxx logger.hxx)
$(OBJDIR)/feed.o: $(addprefix $(INCDIR)/,agora.hxx article.hxx logger.hxx)
#$(OBJDIR)/logger.o: $(addprefix $(INCDIR)/,)

$(OBJDIR)/%.o: $(SRCDIR)/%.cxx $(INCDIR)/%.hxx
	g++ $(FLAGS) -o $(<:$(SRCDIR)%.cxx=$(OBJDIR)%.o) $<
