\mainpage Introduction

## Quick start

The following commands assume that you have checked out a `CMSSW` release (8.0 or later) and cloned
`shears` in the `src` directory.

~~~{.sh}
cmsenv
cd shears/DYJets
cmake3 . # Or "cmake ." if your cmake version is >= 3.6
make -j$(nproc)
~~~

You're now ready to run the code. Programs relevant to the Higgs analysis are located in the `Main`
directory and start with `higgs-`. Use the `--help` option for usage information.

## Build options

There are some options that change how things are built. They can be passed to `cmake` after the dot:

~~~{.sh}
cmake3 . -DOPTION=VALUE
~~~

Here is a list of the most important options:

| Option             | Contents
|:-------------------|:---------
| `DEBUG_PRINTOUT`   | Enable output for event-by-event debugging.
| `CMAKE_BUILD_TYPE` | See the [documentation](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html). `RelWithDebInfo` is the recommended setting.

## Pointers to relevant documentation

- The \ref architecture "Architectural Overview" documents the code structure
- The \ref coding-style "Coding Style" page contains some information that any coder should know
- There is also a page about \ref options "Options Handling"
- It's always useful to have pointers to the lists of [namespaces](namespaces.html) and
  [classes](annotated.html).
