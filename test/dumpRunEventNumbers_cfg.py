import FWCore.ParameterSet.Config as cms

#--------------------------------------------------------------------------------
# Write run + event numbers of events selected in FilterStatistics into ASCII file
# (format = two columns of numbers separated by white-space characted (space of tab);
#           with the run# (event#) specified in the first (second) column)
#--------------------------------------------------------------------------------

process = cms.Process('dumpRunEventNumbers')

process.DQMStore = cms.Service("DQMStore")

process.maxEvents = cms.untracked.PSet(            
  input = cms.untracked.int32(0)         
)

process.source = cms.Source("EmptySource")

process.loadFilterStatistics = cms.EDAnalyzer("DQMFileLoader",
  tmp = cms.PSet(
    inputFileNames = cms.vstring('../../Configuration/test/plotsZtoElecMu_Ztautau.root'),
    dqmDirectory_store = cms.string('tmp')
  )
)

process.dumpRunEventNumbers = cms.EDAnalyzer("DQMDumpRunEventNumbers",
  dqmDirectories = cms.vstring('tmp/zElecMuAnalyzer/FilterStatistics/diTauCandidateForElecMuZeroChargeCut/events_passed_cumulative/'),
  runEventNumberFileName = cms.string("selEvents_zElecMu.txt")
)
 
process.p = cms.Path( process.loadFilterStatistics
                     +process.dumpRunEventNumbers )
