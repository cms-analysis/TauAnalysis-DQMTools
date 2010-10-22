import FWCore.ParameterSet.Config as cms

#--------------------------------------------------------------------------------
# Write run + luminosity section + event numbers of events selected in FilterStatistics into ASCII file
# (format = three columns of numbers separated by white-space character (space or tab);
#           with the run#/luminosity section#/event# specified in the first/second/third column)
#--------------------------------------------------------------------------------

process = cms.Process('dumpRunLumiSectionEventNumbers')

process.DQMStore = cms.Service("DQMStore")

process.maxEvents = cms.untracked.PSet(            
    input = cms.untracked.int32(0)         
)

process.source = cms.Source("EmptySource")

process.loadFilterStatistics = cms.EDAnalyzer("DQMFileLoader",
    all = cms.PSet(
        inputFileNames = cms.vstring(
            '/data1/friis/Run10_v2/plots_all.root'
        ),
        dqmDirectory_store = cms.string('')
    )
)

process.dumpDQMStore = cms.EDAnalyzer("DQMStoreDump")

process.dumpRunLumiSectionEventNumbers = cms.EDAnalyzer("DQMDumpRunLumiSectionEventNumbers",
    dqmDirectories = cms.vstring(
        'harvested/data_Mu_145762_147454_Prompt/ahMuTauAnalyzer_woBtag/FilterStatistics/evtSelNonCentralJetEt20bTag/events_passed_cumulative/'),
    runLumiSectionEventNumberFileName = cms.string("selEvents_AHtoMuTau_woBtag_runs145762to144114.txt")
)
 
process.p = cms.Path(
    process.loadFilterStatistics
   + process.dumpDQMStore 
   + process.dumpRunLumiSectionEventNumbers
)
