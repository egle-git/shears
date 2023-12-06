This folder contains the code used for Drell-Yan analyses. In addition, it has a
stub Higgs (to four leptons) analysis. A fair amount of documentation is
automatically extracted from the source code, and can be found
[here](http://homepage.iihe.ac.be/~lmoureau/shears/ci/).

Quick start
============

Clone the shears project to your working directory, and move to the analysis
folder:

    $ git clone ssh://git@gitlab.cern.ch:7999/shears/shears.git
    $ cd shears/DYJets

The code relies on recent versions of the C++ standard and ROOT libraries. They
are distributed by the [CERN LCG](http://lcginfo.cern.ch/) environment. A
supported version can be activated using:

    $ source lcg-env.sh

You're free to try any other environment.

The code relies on [CMake](https://cmake.org/) for building (building
out-of-source is *not* supported). The initial compilation is done as follows:

    $ cmake . -DCMAKE_BUILD_TYPE=RelWithDebInfo
    $ make -j$(nproc)

See `Documentation/Introduction.md` for more information about CMake options.

After the first compilation, the code can be rebuilt simply by running `make`
again:

    $ make -j$(nproc)

Usage
=====

Coming "soon", in the meantime take a look at the latest version of the
[Doxygen documentation](http://homepage.iihe.ac.be/~lmoureau/shears/ci/).

### eμ method
Edit the `dyjets16-emu-method.yml` file to your liking. Most notably, set the
input file locations. Two of them are needed and one is optional:
* `ll location` - directory containing dilepton files (either ee or μμ) - **mandatory**
* `emu location` - directory containing eμ files - **mandatory**
* `emu same sign location` - directory containing same sign eμ files - only needed if you want to estimate QCD in the eμ channel
If you do not have the same sign eμ files, you may just type in "none" in the `yml` file.

The procedure can be run as follows:
```bash
bin/dyjets-bkg-est emu-method -c dyjets16-emu-method.yml -o output_directory
```
You can find more possible options by using the `-h` flag:
```bash
bin/dyjets-bkg-est emu-method -h
```

### Systematic uncertainty estimation

To estimate the systematic uncertainty, the central value should be reproduced multiple times with different conditions. To reproduce in an efficient way, a dedicated analyzer is made (`Includes/dyjets-analyzer-syst.h` and `Sources/dyjets-analyzer-syst.cc`), by inheriting `dyjets-analyzer.h & cc`.

But at the user level, the syntax is quite similar to submit the job for the systematic variation.
To run interactively,

```bash
bin/dyjets-loop-syst --max-files 1 --max-events 10000 -c Config/dyjets18-ee.yml -s data
bin/dyjets-loop-syst --max-files 1 --max-events 10000 -c Config/dyjets18-ee.yml -s DYJets_M-50to100
```

You can use `dyjets-loop-syst` instead of `dyjets-loop`. 
It uses the same .yml file used for the central value. At the end of each .yml file, you can turn on or off the systematic variations for each uncertainty source. 
e.g. for the electron channel,

```yaml
uncertainties:
  PDF and scale: false
  pileup: false
  L1 prefiring: false
  # Muon Rochester correction: true
  Electron energy correction: true
  efficiency SF: false
  efficiency file: /pnfs/iihe/cms/store/user/kplee/Shears/Uncertainty/EffSF/v230730/LeptonEffMap_FullRun2.root
```

To submit the jobs to condor,

```bash
bin/dyjets-job-list --unc -c Config/dyjets18-ee.yml | bin/send-to-cluster ee_18
bin/dyjets-job-list --unc -c Config/dyjets18-mm.yml | bin/send-to-cluster mm_18
```

For the output, you can use `auto-hadd.h` as usual jobs.
