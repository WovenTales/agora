INCLUDE = -lpugixml -lsqlite3
FLAGS = -c $(INCLUDE)

TARGETS = feed.o entry.o


agora: $(TARGETS)
	g++ -o agora $(TARGETS) $(INCLUDE)

debug: FLAGS += -g -DDEBUG_TEST_FEED
debug: feedarium

.PHONY: clean
clean:
	-rm -f *.o agora

entry.o: feed.hxx
%.o: %.cxx %.hxx
	g++ $(FLAGS) $<
