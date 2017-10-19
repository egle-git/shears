Don't forget cmsenv!

usage:
############## First ##############
create a Bonzai for t&p:

cd ../shears/Bonzais/Pruner
cmsenv
make
./pruner -o test.root --selection TagAndProbePruner --subselection Mu -c Baobab-blabla.txt
####################################

############## Second ###############
skim Bonzai and create a t&p tree

cd ../shears/TagAndProbe
vi test.cc and change the rootfile path ( Bonzai for T&P)
make
./test
####################################


############# Last ##################
make some histograms:

cd ../TagAndProbe/makehisto
make
./makehisto
###############################
