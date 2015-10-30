CC := g++ -std=c++11
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET := predictors
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g # -Wall
LIB := -L lib # plus any other libs IF I HAD THEM
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

dist:
	@echo " Creating submission tar...";
	@mkdir tfoxhal1
	@cp -r include tfoxhal1
	@cp -r src tfoxhal1
	@cp makefile tfoxhal1
	@cp README tfoxhal1
	@cp -r doc/report.pdf tfoxhal1
	@tar -czvf tfoxhal1.tar.gz tfoxhal1/
	@rm -r tfoxhal1

.PHONY: clean
