#include <iostream>
#include <thread>
#include <TString.h>
#include <TChain.h>
#include "Includes/EventTree.h"
#include "Includes/ObjectSelection.h"

using namespace std;

int main(int argc, char **argv)
{

EventTree mytree("../Bonzais/Pruner/test.root");
mytree.Loop();
return 0;

}
