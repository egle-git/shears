#include <iostream>
#include <thread>
#include <TString.h>
#include <TChain.h>
#include "tpTree.h"


using namespace std;

int main(int argc, char **argv)
{

    tpTree mytree("../tpTree.root");
    mytree.Loop();
    return 0;

}
