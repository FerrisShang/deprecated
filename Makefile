app = bsc

CC     = mips-linux-gnu-gcc
STRIP  = mips-linux-gnu-strip
#CC     = gcc
#STRIP  = strip
srcExt = c
srcDir = src
objDir = obj
binDir = .
inc =  $(shell find $(srcDir) -exec dirname {} \; | uniq) \
	   ./c_code/linux/user/glib_startup/output/include/glib-2.0/ \
	   ./c_code/linux/user/glib_startup/output/lib/glib-2.0/include/

CFlags = -Wall -O2  -fdata-sections -ffunction-sections \
		-DG_DISABLE_CHECKS \
		-DG_MESSAGES_DEBUG=all \
		-g \
#		-DG_DISABLE_ASSERT \
		-DG_DISABLE_CHECKS \
		-fuse-ld=gold -fsanitize=address \
		-fuse-ld=gold -fsanitize=thread -ltsan \

LDFlags = -Wl,--gc-sections -g -lpthread -lrt \
#		  -fuse-ld=gold -fsanitize=address \
#		  -fuse-ld=gold -fsanitize=thread -ltsan \

libs = glib-2.0
libDir = ./c_code/linux/user/glib_startup/output/lib

#************************ DO NOT EDIT BELOW THIS LINE! ************************

inc := $(addprefix -I,$(inc))
libs := $(addprefix -l,$(libs))
libDir := $(addprefix -L,$(libDir))
CFlags += -c $(inc) $(libDir) $(libs)
LDFlags += $(libDir) $(libs)
sources := $(shell find $(srcDir) -name '*.$(srcExt)' -a -not -name '*test*.$(srcExt)')
srcDirs := $(shell find $(srcDir) -name '*.$(srcExt)' -a -not -name '*test*.$(srcExt)' -exec dirname {} \; | uniq)
objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources))

.phony: all clean distclean

all: $(binDir)/$(app)

$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) $(objects) $(LDFlags) -o $@
	@$(STRIP) $@

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
