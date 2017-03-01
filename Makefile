user = $(shell whoami)
ifeq ($(user),root)
INSTALL_DIR = /usr/lib/lv2
else
INSTALL_DIR = ~/.lv2
endif

BUNDLE = simple_filter.lv2
CFLAGS = -shared -fPIC -DPIC
BUNDLE_TTLS = simple_filter.ttl manifest.ttl

$(BUNDLE): $(BUNDLE_TTLS) simple_filter.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp $^ $(BUNDLE)

all: $(BUNDLE)

debug: CFLAGS += -DDEBUG -g -ggdb
debug: all

simple_filter.so: simple_filter.c
	gcc $< -o $@ -O2 $(CFLAGS)

clean:
	rm -rf $(BUNDLE) *.so

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(BUNDLE)

validate:
	lv2_validate $(BUNDLE_TTLS)

run: install
	jalv.gtk http://bado.so/plugins/simple_filter
