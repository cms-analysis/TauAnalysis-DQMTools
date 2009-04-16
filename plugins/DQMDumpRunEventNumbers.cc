#include "TauAnalysis/DQMTools/plugins/DQMDumpRunEventNumbers.h"

#include "TauAnalysis/DQMTools/interface/dqmAuxFunctions.h"
#include "TauAnalysis/DQMTools/interface/generalAuxFunctions.h"

// framework & common header files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//DQM services
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <TPRegexp.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

#include <iostream>
#include <fstream>
#include <map>

DQMDumpRunEventNumbers::DQMDumpRunEventNumbers(const edm::ParameterSet& cfg)
{
  //std::cout << "<DQMDumpRunEventNumbers::DQMDumpRunEventNumbers>:" << std::endl;

  cfgError_ = 0;

  dqmDirectories_ = cfg.getParameter<vstring>("dqmDirectories");
  if ( dqmDirectories_.size() == 0 ) {
    edm::LogError("DQMDumpRunEventNumbers") << " Configuration Parameter dqmDirectories = " << format_vstring(dqmDirectories_)
					    << " contains no Entries --> skipping !!";
    cfgError_ = 1;
  }
  
  runEventNumberFileName_ = cfg.getParameter<std::string>("runEventNumberFileName");
  if ( runEventNumberFileName_ == "" ) {
    edm::LogError("DQMDumpRunEventNumbers") << " Configuration Parameter runEventNumberFileName_ = " << runEventNumberFileName_ 
					    << " invalid --> skipping !!";
    cfgError_ = 1;
  }
}

DQMDumpRunEventNumbers::~DQMDumpRunEventNumbers() 
{
//--- nothing to be done yet
}

void DQMDumpRunEventNumbers::analyze(const edm::Event&, const edm::EventSetup&)
{
//--- nothing to be done yet
}

void DQMDumpRunEventNumbers::endJob()
{
  //std::cout << "<DQMDumpRunEventNumbers::endJob>:" << std::endl;

//--- check that configuration parameters contain no errors
  if ( cfgError_ ) {
    edm::LogError ("endjob") << " Error in Configuration ParameterSet --> run + event Numbers will NOT be written !!";
    return;
  }

//--- check that DQMStore service is available
  if ( !edm::Service<DQMStore>().isAvailable() ) {
    edm::LogError ("endJob") << " Failed to access dqmStore --> run + event Numbers will NOT be written !!";
    return;
  }

  TPRegexp regexpParser_triplet("r[[:digit:]]+ev[[:digit:]]+ls[[:digit:]]+");
  TPRegexp regexpParser_number("r([[:digit:]]+)ev([[:digit:]]+)ls([[:digit:]]+)");

  typedef std::set<edm::EventNumber_t> lumiSectionNumberSet;
  typedef std::map<edm::EventNumber_t, lumiSectionNumberSet> eventLumiSectionNumberMap;
  std::map<edm::RunNumber_t, eventLumiSectionNumberMap> runEventLumiSectionNumbers;
  
//--- decode run + event numbers from names of MonitorElements 
  DQMStore& dqmStore = (*edm::Service<DQMStore>());

  for ( vstring::const_iterator dqmDirectory = dqmDirectories_.begin();
	dqmDirectory != dqmDirectories_.end(); ++dqmDirectory ) {
    //std::cout << "dqmDirectory = " << (*dqmDirectory) << std::endl;

    dqmStore.setCurrentFolder(*dqmDirectory);
    std::vector<std::string> meNames = dqmStore.getMEs();
    for ( std::vector<std::string>::const_iterator meName = meNames.begin();
	  meName != meNames.end(); ++meName ) {
      //std::cout << "meName = " << (*meName) << std::endl;

      bool parseError = false;

      TString meName_tstring = meName->data();
//--- check if name of MonitorElement matches run + event + luminosity section number format
      if ( regexpParser_triplet.Match(meName_tstring) == 1 ) {

//--- match individual run, event and luminosity section numbers;
//    require four matches (first match refers to entire line)
	TObjArray* subStrings = regexpParser_number.MatchS(meName_tstring);
	int numSubStrings = subStrings->GetEntries();
	if ( numSubStrings == 4 ) {
	  //std::cout << ((TObjString*)subStrings->At(1))->GetString() << std::endl;
	  edm::RunNumber_t runNumber = ((TObjString*)subStrings->At(1))->GetString().Atoll();
	  //std::cout << ((TObjString*)subStrings->At(2))->GetString() << std::endl;
	  edm::EventNumber_t eventNumber = ((TObjString*)subStrings->At(2))->GetString().Atoll();
	  //std::cout << ((TObjString*)subStrings->At(3))->GetString() << std::endl;
	  edm::LuminosityBlockNumber_t lumiSectionNumber = ((TObjString*)subStrings->At(3))->GetString().Atoll();
	  
	  std::cout << "--> adding Run# = " << runNumber << "," 
		    << " Event# " << eventNumber << "," 
		    << " Luminosity Section# " << lumiSectionNumber << std::endl;
	  runEventLumiSectionNumbers[runNumber][eventNumber].insert(lumiSectionNumber);
	} else {
	  parseError = true;
	}      
      } else { 
	parseError = true;
      }

      if ( parseError ) {
	edm::LogError ("endJob") << " Failed to decode run + event Number from meName = " << (*meName) << " --> skipping !!";
      }
    }
  }

//--- write run + event numbers decoded from names of MonitorElements into ASCII file
  ofstream runEventNumbersFile(runEventNumberFileName_.data(), std::ios::out);
  int numRunEventNumbersWritten = 0;
  for ( std::map<edm::RunNumber_t, eventLumiSectionNumberMap>::const_iterator run = runEventLumiSectionNumbers.begin();
	run != runEventLumiSectionNumbers.end(); ++run ) {
    for ( eventLumiSectionNumberMap::const_iterator event = run->second.begin();
	  event != run->second.end(); ++event ) {
      for ( lumiSectionNumberSet::const_iterator lumiSection = event->second.begin();
	    lumiSection != event->second.end(); ++lumiSection ) {
	runEventNumbersFile << std::setw(12) << std::setiosflags(std::ios::left) 
			    << run->first << " " << event->first << " " << (*lumiSection) << std::endl;
	++numRunEventNumbersWritten;
      }
    }
  }

  edm::LogInfo ("endjob") << " Number of Events selected = " << numRunEventNumbersWritten << ".";
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(DQMDumpRunEventNumbers);
