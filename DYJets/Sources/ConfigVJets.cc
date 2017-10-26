#include <map>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <vector>
#include "stdlib.h"
#include "TObject.h"
#include <sstream>
#include "ConfigVJets.h"

bool ConfigVJets::read(const char* filename){
  bool rc = Config::read(filename);
  TString unfCfgFile = getS("unfConf");
  if(unfCfgFile.Length() > 0) rc &= unfCfg.read(unfCfgFile);
  return rc;
}

ConfigVJets::ConfigVJets(const char* filename): Config(filename){
  TString unfCfgFile = getS("unfConf");
  if(unfCfgFile.Length() > 0) unfCfg.read(unfCfgFile);
}
