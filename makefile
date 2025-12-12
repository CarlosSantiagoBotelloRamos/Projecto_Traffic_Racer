# Wrapper makefile to delegate to proyecto-252

SUBDIR := proyecto-252

.PHONY: all clean run rebuild help check

all:
	@$(MAKE) -C $(SUBDIR) all

clean:
	@$(MAKE) -C $(SUBDIR) clean

run:
	@$(MAKE) -C $(SUBDIR) run

rebuild:
	@$(MAKE) -C $(SUBDIR) rebuild

help:
	@$(MAKE) -C $(SUBDIR) help

check:
	@$(MAKE) -C $(SUBDIR) check