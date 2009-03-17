#include "TauAnalysis/DQMTools/plugins/DQMHistScaler.h"

#include "TauAnalysis/DQMTools/interface/dqmAuxFunctions.h"
#include "TauAnalysis/DQMTools/interface/generalAuxFunctions.h"

// framework & common header files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//DQM services
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include <TH1.h>

#include <iostream>

const int verbosity = 0;

//
//-----------------------------------------------------------------------------------------------------------------------
//

DQMHistScaler::DQMHistScaler(const edm::ParameterSet& cfg)
{
  //std::cout << "<DQMHistScaler::DQMHistScaler>:" << std::endl;

  cfgError_ = 0;

  dqmDirectory_input_ = cfg.getParameter<std::string>("dqmDirectory_input");
  std::cout << " dqmDirectory_input = " << dqmDirectory_input_ << std::endl;
  if ( cfg.exists("dqmSubDirectories_input") ) dqmSubDirectories_input_ = cfg.getParameter<vstring>("dqmSubDirectories_input");
  std::cout << " dqmSubDirectories_input = " << format_vstring(dqmSubDirectories_input_) << std::endl;

  unsigned numScales = 0;

  if ( cfg.exists("scaleFactor") ) {
    cfgScaleFactor_ = cfg.getParameter<double>("scaleFactor");
    ++numScales;
  } else {
    cfgScaleFactor_ = -1.;
  }
  cfgScaleFactor_ = ( cfg.exists("scaleFactor") ) ? cfg.getParameter<double>("scaleFactor") : -1.;
  std::cout << " scaleFactor = " << cfgScaleFactor_ << std::endl;

  if ( cfg.exists("dqmDirectory_normalization") &&
       cfg.exists("meNameNumerator") && 
       cfg.exists("meNameDenominator") && 
       cfg.exists("meType") ) {
    dqmDirectory_normalization_ = cfg.getParameter<std::string>("dqmDirectory_normalization");
    meNameNumerator_ = cfg.getParameter<std::string>("meNameNumerator");
    meNameDenominator_ = cfg.getParameter<std::string>("meNameDenominator");
    meType_ = cfg.getParameter<std::string>("meType");
    ++numScales;
  } else {
    dqmDirectory_normalization_ = "";
    meNameNumerator_ = "";
    meNameDenominator_ = "";
    meType_ =  "";
  }
  std::cout << " dqmDirectory_normalization = " << dqmDirectory_normalization_ << std::endl;
  std::cout << " meNameNumerator = " << meNameNumerator_ << std::endl;
  std::cout << " meNameDenominator = " << meNameDenominator_ << std::endl;
  std::cout << " meType = " << meType_ << std::endl;
 
  dqmDirectory_output_ = cfg.getParameter<std::string>("dqmDirectory_output");
  std::cout << " dqmDirectory_output = " << dqmDirectory_output_ << std::endl;

//--- check that either:
//     o scaleFactor
//    or
//     o numerator + denominator
//    configuration parameters are defined 
  if ( numScales != 1 ) {
    edm::LogError("DQMHistScaler") << " Need to specify either Configuration parameter 'scaleFactor'"
				   << " or 'dqmDirectory_normalization', 'meNameNumerator', 'meNameDenominator' and 'meType' !!";
    cfgError_ = 1;
  } 

  //std::cout << "done." << std::endl;
}

DQMHistScaler::~DQMHistScaler() 
{
// nothing to be done yet...
}

void DQMHistScaler::analyze(const edm::Event&, const edm::EventSetup&)
{
// nothing to be done yet...
}

std::string dqmDirectoryName_full(const std::string& dqmDirectory, const std::string& dqmSubDirectory)
{
  std::string dqmDirectoryName_full = dqmDirectoryName(std::string(dqmRootDirectory)).append(dqmDirectory);
  dqmDirectoryName_full = dqmDirectoryName(dqmDirectoryName_full).append(dqmSubDirectory);
  return dqmDirectoryName_full;
}


double getMonitorElementNorm(DQMStore& dqmStore, const std::string& dqmDirectory, 
			     const std::string& meName, const std::string& meType, int& errorFlag)
{
  std::string meName_full = dqmDirectoryName(dqmDirectory).append(meName);
  //std::cout << " meName_full = " <<  meName_full << std::endl;

  dqmStore.setCurrentFolder(dqmDirectory);
  MonitorElement* me = dqmStore.get(meName_full);

  if ( meType == "real" ) {
    return me->getFloatValue();
  } else if ( meType == "int" ) {
    return me->getIntValue();
  } else if ( meType == "hist" ) {
    if ( me->getTH1() ) {
      return me->getTH1()->Integral();
    } else {
      edm::LogError("getMonitorElementNorm") << " No histogram associated to Monitor Element = " << meName << " !!";
      errorFlag = 1;
      return -1.;
    } 
  } else {
    edm::LogError("getMonitorElementNorm") << " Undefined type = " << meType << " !!";
    errorFlag = 1;
    return -1.;
  }
}

void DQMHistScaler::endJob()
{
  std::cout << "<DQMHistScaler::endJob>:" << std::endl;

//--- check that configuration parameters contain no errors
  if ( cfgError_ ) {
    edm::LogError ("endJob") << " Error in Configuration ParameterSet --> histograms will NOT be scaled !!";
    return;
  }

//--- check that DQMStore service is available
  if ( !edm::Service<DQMStore>().isAvailable() ) {
    edm::LogError ("endJob") << " Failed to access dqmStore --> histograms will NOT be scaled !!";
    return;
  }

  DQMStore& dqmStore = (*edm::Service<DQMStore>());
  
//--- compute scale factor
  double scaleFactor = 0.;
  if ( cfgScaleFactor_ != -1. ) {
    scaleFactor = cfgScaleFactor_;
  } else {
    int errorFlag = 0;
    double normNumerator = getMonitorElementNorm(dqmStore, dqmDirectory_normalization_, meNameNumerator_, meType_, errorFlag);
    double normDenominator = getMonitorElementNorm(dqmStore, dqmDirectory_normalization_, meNameDenominator_, meType_, errorFlag);
    if ( errorFlag ) {
      edm::LogError ("endJob") << " Failed to access numerator and denominator Monitor Elements"
			       << " --> histograms will NOT be scaled !!";
      return;
    }

    scaleFactor = ( normDenominator > 0. ) ? normNumerator/normDenominator : 0.;
  }
  std::cout << " scaleFactor = " << scaleFactor << std::endl;

//--- scale histograms;
//    in case specific subdirectories have been specified in configuration parameters, 
//    copy the specified subdirectories only, else copy all subdirectories 
//    from dqmDirectory_input to dqmDirectory_output
  //std::cout << "--> scaling histograms..." << std::endl;
  if ( dqmSubDirectories_input_.size() > 0 ) {
    for ( vstring::const_iterator dqmSubDirectory = dqmSubDirectories_input_.begin();
	  dqmSubDirectory != dqmSubDirectories_input_.end(); ++dqmSubDirectory ) {

      std::string inputDirectory = dqmDirectoryName_full(dqmDirectory_input_, *dqmSubDirectory);
      //std::cout << " inputDirectory = " << inputDirectory << std::endl;
      std::string outputDirectory = dqmDirectoryName_full(dqmDirectory_output_, *dqmSubDirectory);
      //std::cout << " outputDirectory = " << outputDirectory << std::endl;

      dqmCopyRecursively(dqmStore, inputDirectory, outputDirectory, scaleFactor, 1, false);
    }
  } else {
    std::string inputDirectory = dqmDirectoryName(std::string(dqmRootDirectory));
    //std::cout << " inputDirectory = " << inputDirectory << std::endl;
    std::string outputDirectory = dqmDirectoryName(std::string(dqmRootDirectory));
    //std::cout << " outputDirectory = " << outputDirectory << std::endl;
      
    dqmCopyRecursively(dqmStore, inputDirectory, outputDirectory, scaleFactor, 1, false);
  }

  std::cout << "done." << std::endl; 
  if ( verbosity ) dqmStore.showDirStructure();
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(DQMHistScaler);