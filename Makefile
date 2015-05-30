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

$(OBJDIR)/agora.o: $(addprefix $(SRCDIR)/,feed.hxx)
$(OBJDIR)/article.o: $(addprefix $(SRCDIR)/,agora.hxx database.hxx)
#database.o: 
$(OBJDIR)/feed.o: $(addprefix $(SRCDIR)/,article.hxx)
$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.cxx %.hxx)
	g++ $(FLAGS) -o $(<:$(SRCDIR)%.cxx=$(OBJDIR)%.o) $<
