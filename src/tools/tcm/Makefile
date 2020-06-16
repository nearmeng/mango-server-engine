SUBDIRS = xreplacer tcm_so
CLEAN_MODULES = $(addprefix _clean_,$(SUBDIRS))

.PHONY: all clean $(SUBDIRS)

all: tcm_so
clean: $(CLEAN_MODULES)

tcm_so: xreplacer
	$(MAKE) -C $@ all

xreplacer:
	$(MAKE) -C $@ all

$(CLEAN_MODULES):
	$(MAKE) -C $(patsubst _clean_%,%,$@) clean
