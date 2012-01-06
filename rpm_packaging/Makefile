
# Makefile for generating RPM for deeptrim
#


NAME = $(shell cat pack-name)
VERSION = $(shell cat pack-version)
COUNT = $(shell ./docount.sh)

RPMBUILD = $(HOME)/rpmbuild
SOURCEDIR = $(RPMBUILD)/SOURCES

TAR_SRC = $(NAME)_$(VERSION).tar.gz
DT_SRC = $(NAME).desktop
SPEC = $(NAME).spec
CHANGES = $(NAME).changes

SRC = $(SOURCEDIR)/$(TAR_SRC) $(SOURCEDIR)/$(DT_SRC)

rpm: $(SRC) $(SPEC)
	rpmbuild -ba $(SPEC)

info:
	(  \
	  echo "name      " $(NAME) ; \
	  echo "spec file " $(SPEC) ; \
	  echo "source    " $(SRC) ; \
	  echo "version   " $(VERSION) ; \
	)

spec:	$(SPEC)

$(SPEC):  spec.in pack-version
	echo -n "%define sourceversion " > $(SPEC)
	echo $(VERSION) >> $(SPEC)
	echo -n "%define relcount " >> $(SPEC)
	echo $(COUNT) >> $(SPEC)
	cat spec.in >> $(SPEC)
	cat $(CHANGES) >> $(SPEC)

$(SOURCEDIR)/$(TAR_SRC): $(TAR_SRC)
	cp $(TAR_SRC) $(SOURCEDIR)/$(TAR_SRC)

$(SOURCEDIR)/$(DT_SRC): $(DT_SRC)
	cp $(DT_SRC) $(SOURCEDIR)/$(DT_SRC)

