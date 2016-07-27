app = mem_share

CC = mips-linux-gnu-gcc
AR = mips-linux-gnu-ar
#CC = gcc
#AR = ar
srcExt = c
objDir = obj
binDir = .
inc = $(shell find . -name '*.h' -exec dirname {} \; | uniq)

CFlags = -Wall -O2 -fdata-sections -ffunction-sections
LDFlags = -Wl,--gc-sections
libs = rt pthread
libDir = .
main = src/main.c
sources = \
		src/ipc_sem.c \
		src/ipc_shm.c \
		src/ipc_cmd.c \
		src/proc_comm.c \
		src/wi_bus.c \
		src/common/wi_sem.c \
		src/common/wi_mutex.c \
		src/common/queue.c \
		src/common/log.c \
		src/common/mem_manage.c \

#************************ DO NOT EDIT BELOW THIS LINE! ************************

inc := $(addprefix -I,$(inc))
libs := $(addprefix -l,$(libs))
libDir := $(addprefix -L,$(libDir))
CFlags += $(inc) $(libDir) $(libs)
LDFlags += $(libs)
srcDirs := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | uniq)
objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources))

.phony: all clean distclean

all: $(binDir)/$(app)
$(binDir)/$(app): buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) $(main) -l$@ $(CFlags) -o $@

so: $(binDir)/lib$(app).so
$(binDir)/lib$(app).so: buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Create $@"
	@$(CC) $(sources) $(CFlags) -shared -fPIC -o $@

a: $(binDir)/lib$(app).a
$(binDir)/lib$(app).a: buildrepo $(objects)
	@mkdir -p `dirname $@`
	@echo "Create $@"
	@$(AR) -cr $@ $(objects)

sinclude $(sources:%.$(srcExt)=$(objDir)/%.d)
$(objDir)/%.o: %.$(srcExt)
#	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) -c $(CFlags) $< -o $@

clean:
	@$(RM) -r $(objDir)

distclean: clean
	@$(RM) -r $(binDir)/$(app)
	@$(RM) -r $(binDir)/lib$(app).a
	@$(RM) -r $(binDir)/lib$(app).so

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
        -c $(CFlags) \
        $1
endef
