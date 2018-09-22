all:
	$(MAKE) -C src all

debug:
	$(MAKE) -C src debug

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C src uninstall
