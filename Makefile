app = bsc

#CC     = mips-linux-gnu-gcc
CC     = gcc
srcExt = c
srcDir = .
objDir = obj
binDir = .
inc = .  $(shell find -exec dirname {} \; | uniq)

CFlags = -Wall -O2 -fsanitize=address -fuse-ld=gold -g
LDFlags = -fsanitize=address -fuse-ld=gold -g -lpthread -lrt
libs =
libDir =

#************************ DO NOT EDIT BELOW THIS LINE! ************************

inc := $(addprefix -I,$(inc))
libs := $(addprefix -l,$(libs))
libDir := $(addprefix -L,$(libDir))
CFlags += -c $(inc) $(libDir) $(libs)
sources := $(shell find $(srcDir) -name '*.$(srcExt)' -a -not -name '*test*.$(srcExt)')
srcDirs := $(shell find $(srcDir) -name '*.$(srcExt)' -a -not -name '*test*.$(srcExt)' -exec dirname {} \; | uniq)
objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources))

.phony: all clean distclean

all: $(binDir)/$(app)

$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) $(objects) $(LDFlags) -o $@

sinclude $(sources:%.$(srcExt)=$(objDir)/%.d)
$(objDir)/%.o: %.$(srcExt)
#	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFlags) $< -o $@

clean:
	@$(RM) -r $(objDir)
	@$(RM) -r $(binDir)/$(app)

distclean: clean
	@$(RM) -r $(binDir)/$(app)

buildrepo:
	@$(call make-repo)

define make-repo
   for dir in $(srcDirs); \
   do \
	mkdir -p $(objDir)/$$dir; \
   done
endef


# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
        $(CFlags) \
        $1
endef
