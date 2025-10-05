SUBDIRS := libc blank weeshell
BUILDDIR := build

.PHONY: all $(SUBDIRS) clean

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ BUILDDIR=$(abspath $(BUILDDIR)/$@)

clean:
	rm -rf $(BUILDDIR)
