TEST_DIR = ./test
NODIR = $(notdir $(wildcard $(TEST_DIR)/*.c))
SOME_O = $(patsubst %.c, $(TEST_DIR)/%.o, $(notdir $(NODIR)))
hello:
#	@echo $(CURDIR)
#	@echo $(wildcard $(TEST_DIR)/*.c)
#	@echo $(NODIR)
	@echo $(SOME_O)
	@echo $(basename $(notdir $(SOME_O)))
clean:
	@rm -f *.out *.o
