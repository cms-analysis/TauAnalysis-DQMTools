#include "TauAnalysis/DQMTools/interface/generalAuxFunctions.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>

std::string replace_string(const std::string& src, const std::string& keyword, const std::string& parameter, 
			   unsigned minReplacements, unsigned maxReplacements, int& errorFlag)
{
  std::string modSrc = src;
  unsigned numReplacements = 0;
  while ( modSrc.find(keyword) != std::string::npos ) {
    modSrc.replace(modSrc.find(keyword), keyword.length(), parameter);
    ++numReplacements;
  }
  if ( (numReplacements < minReplacements) ||
       (numReplacements > maxReplacements) ) {
    edm::LogError ("replace_string") << " Failed to replace parameter = " << parameter << " in src = " << src << ","
				     << " numReplacements = " << numReplacements 
				     << " (min = " << minReplacements << ", max = " << maxReplacements << ") !!";
    errorFlag = 1;
  }
  return modSrc;
}

//
//-----------------------------------------------------------------------------------------------------------------------
//

template <typename T>
std::string format_vT(const std::vector<T>& vT)
{
  std::ostringstream os;
  
  os << "{ ";

  unsigned numEntries = vT.size();
  for ( unsigned iEntry = 0; iEntry < numEntries; ++iEntry ) {
    os << vT[iEntry];
    if ( iEntry < (numEntries - 1) ) os << ", ";
  }

  os << " }";
  
  return os.str();
}

std::string format_vstring(const std::vector<std::string>& vs)
{
  return format_vT(vs);
}

std::string format_vdouble(const std::vector<double>& vd)
{
  return format_vT(vd);
}
