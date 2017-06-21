CXX       := g++
CPPFLAGS  := -std=c++11 -MMD
CXXFLAGS  := -O0 -g -Wall -Wextra -Wno-unused-parameter

TARGET    := resources

SOURCES   := $(wildcard *.cpp)
OBJECTS   := $(SOURCES:%.cpp=%.o)
DEPS      := $(SOURCES:%.cpp=%.d)

BOOST_INCLUDE_DIR := /usr/local/include

MATCHERS  := CA \
             IBA \
             IBBA \
             PFS1BA \
             PA \
             C+IBA \
             C+PFS1BA \
             C+PA \
             IB+IBBA \
             C+P+IBA \
             ALL
SCALES    := mini small #medium medplus large largest
GRAPHS    := $(foreach m, $(MATCHERS), $(foreach s, $(SCALES), $(m).$(s)))

#### Do not edit below here without confidence

CPPFLAGS += -I$(BOOST_INCLUDE_DIR)

.PHONY: clean clean-graph all everything

all: $(TARGET)

everything : all resource_gen test_resource_spec

graphs: $(GRAPHS)

$(TARGET): $(OBJECTS)
	$(LINK.cc) $(OUTPUT_OPTION) $^

$(GRAPHS): $(TARGET)
	mkdir -p graphs_dir/$(subst .,$(empty),$(suffix $@))
	mkdir -p graphs_dir/$(subst .,$(empty),$(suffix $@))/images
	$< --graph-scale=$(subst .,$(empty),$(suffix $@)) \
		--matcher=$(basename $@) \
		--output=graphs_dir/$(subst .,$(empty),$(suffix $@))/$@
	cd graphs_dir/$(subst .,$(empty),$(suffix $@)) && \
	dot -Tsvg $@.dot -o images/$@.svg

clean:
	rm -f $(OBJECTS) $(TARGETS) *~

veryclean: clean
	rm -f $(DEPS)

clean-graph:
	rm -f -r graphs_dir/*

-include $(DEPS)
