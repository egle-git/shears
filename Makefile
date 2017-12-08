# This Makefile builds things for all analysis code at once. It supports the
# following targets:
#   - all: Run `make` in every analysis folder
#   - Pruners: Run `make Pruners` in every analysis folder
# The Makefile will always try to compile the `pruner` executable in Bonzais/.
#
# WARNING: This Makefile won't fail if some analysis code is broken (ie doesn't
#          compile). It's not a bug, it's a feature.

# List of the analysis to compile
ANALYSIS = DYJets HZZ2l2nu TagAndProbe #WJets

# This rule builds everything
ALL = $(addsuffix .all, $(ANALYSIS))
.PHONY: all
all: $(ALL) Bonzais
%.all:
	$(MAKE) -C $(basename $@) || true

# This rule builds everything in the Bonzais/ directory
.PHONY: Bonzais
Bonzais:
	$(MAKE) -C Bonzais

# This rule build all pruners
PRUNERS = $(addsuffix .pruners, $(ANALYSIS))
.PHONY: Pruners
Pruners: $(PRUNERS)
%.pruners:
	$(MAKE) -C $(basename $@) Pruners || true
