app = ancs_test

CC = mips-linux-gnu-gcc
srcExt = c
objDir = obj
binDir = .
inc = . lib tools/ParseDate tools/AncsParser/

CFlags = -Wall -O2 -fdata-sections -ffunction-sections
LDFlags = -Wl,--gc-sections
libs = rt pthread
libDir =

sources=src/main.c \
	 src/ancs.c \
	 src/gatt-client.c \
	 src/gatt-helpers.c \
	 src/mainloop.c \
	 src/gatt-db.c \
	 src/gatt-server.c \
	 src/att.c \
	 src/queue.c \
	 src/util.c \
	 src/crypto.c \
	 src/io-mainloop.c \
	 src/timeout-mainloop.c \
	 src/mem_manage.c \
	 lib/bluetooth.c \
	 lib/uuid.c \
	 lib/hci.c \
	 tools/AncsParser/AncsParser.c \

#************************ DO NOT EDIT BELOW THIS LINE! ************************

inc := $(addprefix -I,$(inc))
libs := $(addprefix -l,$(libs))
libDir := $(addprefix -L,$(libDir))
CFlags += -c $(inc) $(libDir) $(libs)
LDFlags += $(libs)
srcDirs := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | uniq)
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
