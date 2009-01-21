#ifndef TauAnalysis_DQMTools_generalAuxFunctions_h
#define TauAnalysis_DQMTools_generalAuxFunctions_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>
#include <vector>

std::string replace_string(const std::string&, const std::string&, const std::string&, unsigned, unsigned, int&);

std::string format_vstring(const std::vector<std::string>& vs);

template <class T>
void readCfgParameter(const edm::ParameterSet& cfgParSet, std::map<std::string, T>& def)
{
  std::vector<std::string> cfgParNames = cfgParSet.getParameterNamesForType<edm::ParameterSet>();
  for ( std::vector<std::string>::const_iterator cfgParName = cfgParNames.begin(); 
	cfgParName != cfgParNames.end(); ++cfgParName ) {
    edm::ParameterSet cfgParDef = cfgParSet.getParameter<edm::ParameterSet>(*cfgParName);
    
    def.insert(std::pair<std::string, T>(*cfgParName, T(*cfgParName, cfgParDef)));
  }
}

#endif
