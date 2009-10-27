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
    dump = cms.PSet(
        #inputFileNames = cms.vstring('../../Configuration/test/plotsZtoElecTau_all.root'),
        inputFileNames = cms.vstring(
            #'rfio:/castor/cern.ch/user/l/lusito/Plots/ZmumuForChristian/plotsZtoMuTau_Zmumu_part01.root',
            #'rfio:/castor/cern.ch/user/l/lusito/Plots/ZmumuForChristian/plotsZtoMuTau_Zmumu_part02.root',
            #'rfio:/castor/cern.ch/user/l/lusito/Plots/ZmumuForChristian/plotsZtoMuTau_Zmumu_part03.root'
            #'../../Configuration/test/plotsZtoElecTau_failedElecEcalIsolation.root'
            #'../../Configuration/test/plotsZtoMuTau_all.root'
            '../../Configuration/test/plotsWtoTauNu_all.root'
        ),
        dqmDirectory_store = cms.string('')
    )
)

process.dumpRunEventNumbers = cms.EDAnalyzer("DQMDumpRunEventNumbers",
    #dqmDirectories = cms.vstring('harvested/Zmumu/zMuTauAnalyzer/FilterStatistics/evtSelDiTauCandidateForMuTauPzetaDiff/events_passed_cumulative/'),
    #runEventNumberFileName = cms.string("selEvents_ZtoMuTau_Zmumu_passed.txt")
    dqmDirectories = cms.vstring('harvested/qcd_W/wTauNuAnalyzer/FilterStatistics/evtSelTauCharge/events_passed_cumulative/'),
    runEventNumberFileName = cms.string("selEvents_WtoTauNu_QCD_passed.txt")                                         
)
 
process.p = cms.Path( process.loadFilterStatistics
                     +process.dumpRunEventNumbers )
