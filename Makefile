INCLUDE = -lpugixml -lsqlite3
FLAGS = -c $(INCLUDE)

FILES = agora feed article database

SRCDIR = src
OBJDIR = obj
BINDIR = bin

OBJECTS = $(FILES:%=$(OBJDIR)/%.o)

$(BINDIR)/agora: $(OBJDIR) $(BINDIR) $(OBJECTS)
	g++ -o $(BINDIR)/agora $(OBJECTS) $(INCLUDE)

debug: FLAGS += -g -DDEBUG
debug: feedarium

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BINDIR)/agora
	-rmdir $(BINDIR) $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/agora.o: $(SRCDIR)/feed.hxx
$(OBJDIR)/article.o: $(SRCDIR)/database.hxx
#database.o: 
$(OBJDIR)/feed.o: $(SRCDIR)/article.hxx
$(OBJDIR)/%.o: $(SRCDIR)/%.cxx $(SRCDIR)/%.hxx
	g++ $(FLAGS) -o $(<:$(SRCDIR)%.cxx=$(OBJDIR)%.o) $<
