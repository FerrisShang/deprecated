app     = msg_handle

CC      = gcc
AR      = ar
srcExt  = c
objDir  = obj
binDir  = .
inc     = . lib

CFlags  = -Wall -O2 -fdata-sections -ffunction-sections -fPIC
LDFlags = -Wl,--gc-sections
libs    = pthread
libDir  = lib

sources = \
	 main.c \
	 msg_handle.c \
	 mem_manage.c \

#************************ DO NOT EDIT BELOW THIS LINE! ************************

inc     := $(addprefix -I,$(inc))
libs    := $(addprefix -l,$(libs))
libDir  := $(addprefix -L,$(libDir))
CFlags  += -c $(inc) $(libDir) $(libs)
LDFlags += $(libs) $(libDir)
srcDirs := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | uniq)
objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources))

.phony: all clean distclean

all: $(binDir)/$(app)

$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Linking..."
	@$(CC) $(objects) $(LDFlags) -o $@

sinclude $(sources:%.$(srcExt)=$(objDir)/%.d)
$(objDir)/%.o: %.$(srcExt)
#	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFlags) $< -o $@

clean:
	@$(RM) -r $(objDir)
	@$(RM) -r $(app)

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
