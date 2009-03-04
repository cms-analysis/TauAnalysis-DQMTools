#ifndef TauAnalysis_DQMTools_DQMDumpRunEventNumbers_h
#define TauAnalysis_DQMTools_DQMDumpRunEventNumbers_h

/** \class DQMDumpRunEventNumbers
 *  
 *  Class to write run + event numbers of events selected in FilterStatistics into ASCII file
 *  (format = two columns of numbers separated by white-space characted (space of tab);
 *            with the run# (event#) specified in the first (second) column)
 *
 *  $Date: 2009/01/21 17:34:57 $
 *  $Revision: 1.1 $
 *  \author Christian Veelken, UC Davis
 */

// framework & common header files
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>
#include <vector>

class DQMDumpRunEventNumbers : public edm::EDAnalyzer
{
 public:
  explicit DQMDumpRunEventNumbers(const edm::ParameterSet&);
  virtual ~DQMDumpRunEventNumbers();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();  

private:
  typedef std::vector<std::string> vstring;
  vstring dqmDirectories_;

  std::string runEventNumberFileName_;

  int cfgError_;
};

#endif


