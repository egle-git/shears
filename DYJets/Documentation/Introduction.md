# Introduction {#introduction}

## Quick start

The following commands assume that you have checked out a `CMSSW` release (8.0 or later) and cloned
`shears` in the `src` directory.

~~~{.sh}
cmsenv
cd shears/DYJets
make -j$(nproc) # This attempt fails
make -j$(nproc) # This attempt works
~~~

You're now ready to run the code. Programs relevant to the Higgs analysis are located in the `Main`
directory and start with `higgs-`. Use the `--help` option for usage information.

## Pointers to relevant documentation

- The \ref architecture "Architectural Overview" documents the code structure
- The \ref coding-style "Coding Style" page contains some information that any coder should know
- There is also a page about \ref options "Options Handling"
- It's always useful to have pointers to the lists of [namespaces](namespaces.html) and
  [classes](annotated.html).
