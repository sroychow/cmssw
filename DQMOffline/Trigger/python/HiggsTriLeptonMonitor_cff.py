import FWCore.ParameterSet.Config as cms

#########same flavour monitor###########
#from HiggsTriLeptonMonitor_cfi import hlthiggsTriLeptonMonitoring


#########cross-trigger monitor###########

from HiggsTriLeptonMonitor_cfi import hlthiggsTriLeptonCrossTrigMonitoring

higgscrossTriLeptonMonitoring_muegref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
higgscrossTriLeptonMonitoring_muegref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_muegRef/')
higgscrossTriLeptonMonitoring_muegref.nelectrons = cms.int32(1)
higgscrossTriLeptonMonitoring_muegref.nmuons = cms.int32(2)
higgscrossTriLeptonMonitoring_muegref.leptonPDG = cms.int32(13)
higgscrossTriLeptonMonitoring_muegref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_v*") # HLT_ZeroBias_v*
higgscrossTriLeptonMonitoring_muegref.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v*", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v*")

higgscrossTriLeptonMonitoring_dimuref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
higgscrossTriLeptonMonitoring_dimuref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_dimuRef/')
higgscrossTriLeptonMonitoring_dimuref.nelectrons = cms.int32(1)
higgscrossTriLeptonMonitoring_dimuref.nmuons = cms.int32(2)
higgscrossTriLeptonMonitoring_dimuref.leptonPDG = cms.int32(13)
higgscrossTriLeptonMonitoring_dimuref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_v*") # HLT_ZeroBias_v*
higgscrossTriLeptonMonitoring_dimuref.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v*")
