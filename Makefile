# Makefile 
COMMON_CPP = CRubanCombination.cpp CRubanHistory.cpp CRubanState.cpp RubanSignal.cpp SRubanOp.cpp
VIEWER_CPP = Rubest_view.cpp CGlRubanLabel.cpp CGlKubRub.cpp
TARGETS = Rubest Rubest_view

# You can pass MODE=debug/release to force specified mode. Default is release
KNOWN_MODES=debug release
ifndef MODE
  MODE=debug
endif
ifeq ($(filter $(MODE),$(KNOWN_MODES)),)
  $(error Unknown mode '$(MODE)'. Known modes are: $(foreach m,$(KNOWN_MODES),'$m'))
endif

# Output all build files here
DST=$(MODE)

CC = g++
CFLAGS_debug = -O0 -g
CFLAGS_release = -O3
CFLAGS=$(CFLAGS_$(MODE))

SOURCES=$(COMMON_CPP) $(VIEWER_CPP) Rubest.cpp
COMMON_OBJ = $(addprefix $(DST)/,$(subst .cpp,.o,$(COMMON_CPP)))
VIEWER_OBJ = $(addprefix $(DST)/,$(subst .cpp,.o,$(VIEWER_CPP)))
TARGETS_BIN = $(addprefix $(DST)/,$(TARGETS))

#GL_FLAGS=-L/usr/X11R6/lib -lGL -lGLU -lglut 
GL_FLAGS=-lGL -lGLU -lglut 

.PHONY: all clean doc
all: $(TARGETS_BIN)

clean:
	rm -rf $(DST)/*
	rm -rf DoxyDoc/*

doc:
	doxygen

$(DST)/%.o: %.cpp $(DST)/%.d
	$(CC) -c $(CFLAGS) -o $@ $<

$(DST)/Rubest: $(COMMON_OBJ) $(DST)/Rubest.o
	$(CC) $(CFLAGS) -o $@ $^

$(DST)/Rubest_view: $(COMMON_OBJ) $(VIEWER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(GL_FLAGS)

ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),doc)
    # minus before include means 'ignore non-existent of the first pass'
    -include $(addprefix $(DST)/,$(subst .cpp,.d,$(SOURCES)))
  endif
endif

#Auto-depend making target
$(DST)/%.d: %.cpp
	@echo "[DEPENDS]" $< 
	@$(CC) -M $(CFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g'  > $@;
