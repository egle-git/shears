TreeAnalysis
============

Clone this file to your working directory:

    git clone git@github.com:iihe-cms-sw/TreeAnalysis.git

Once you have cloned the repository, you just have to build the RooUnfold
that is provided. This RooUnfold is a slightly modified version of the one 
you can download at CERN. That is why it is provided here.

To build it, nothing hard:

    $ cd DYJets/RooUnfold
    $ make clean
    $ make 

Bravo ! You have compiled the RooUnfold classes and you are ready to use it.

A Makefile is provided for the rest of the framework. So just do this:

    $ cd DYJets
    $ make clean
    $ make

Now you should have everything setup and you can try to run the executables.
But for your information, you need to know that the source code is contained
in the Sources directory and some other directly in the DYJets. The code in
Sources will only be compiled into .o files while the code in DYJets, meanly
some runBlaBla.cc code are dedicated to user executables and therefore contain
the int main(int argc, char **argv) function. For example, after
compilation you have access to

    $ ./runZJets

The executables come with some options. You can use the command line option -help
to know more about what it is possible to do.

Next you need to have the data to run on. For this, if you are running on m-machine, 
a simple symbolic link will be enough. You can also opy the full directory.

Go to the racine directory TreeAnalysis2012
    
    $ ln -s /THIS_is_wehre_I_store_data/Data_Z_5311 Data_Z_5311
    $ ln -s /THIS_is_wehre_I_store_data/DataTTbarEMu DataTTbarEMu
    $ ln -s /THIS_is_wehre_I_store_data/DataW DataW

#############################
################  quick fix for lxplus from Darin
#############################

# Set up CMSSW ( for root ) 
    cmsrel CMSSW_5_3_0
    cd CMSSW_5_3_0/src
    cmsenv

# Get Git Repository
    git clone git@github.com:iihe-cms-sw/TreeAnalysis.git TreeAnalysis2012

# Setup W+Jets directory
    cd TreeAnalysis2012
    mkdir WJets
    mkdir WJets/DataW
    cp -r DYJets/* WJets/
    cd WJets

--- Get example file
    cmsStage /store/group/phys_smp/WPlusJets/NtuplesTomislav/SMu_8TeV_T_s_channel_dR_5311.root DataW/SMu_8TeV_T_s_channel_dR_5311.root

--- Output directories
    mkdir HistoFiles PNGFiles

---- Compile RooUnfold
    cd RooUnfold-1.1.1
    make
    cd -

--- Quick hacks for making LHAPDF work
    sed -i 's#/user/aleonard/LHAPDF/lib/libLHAPDF.so#/afs/cern.ch/cms/slc5_amd64_gcc434/external/lhapdf/5.8.5/lib/libLHAPDF.so#g' runDYJets.cc
    sed -i 's#/user/aleonard/lhapdf-5.9.1/include/#/afs/cern.ch/cms/slc5_amd64_gcc434/external/lhapdf/5.8.5/include/#g' rootlogon.C
    sed -i 's#NNPDF23_nlo_as_0118#NNPDF20_as_0118_100#g' ComputePDFUncertainties.cc
    sed -i 's#NNPDF23_nlo_as_0118#NNPDF20_as_0118_100#g' runDYJets.cc

--- Run the root file maker
    root -b runDYJets.cc

