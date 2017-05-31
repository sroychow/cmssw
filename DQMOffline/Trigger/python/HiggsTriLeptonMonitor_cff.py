import FWCore.ParameterSet.Config as cms

from HiggsTriLeptonMonitor_cfi import *
#########same flavour monitor###########
higgsTrimumon = hlthiggsTripleMuonMonitoring.clone()
higgsTrimumon.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_TripleMu_12_10_5/')
higgsTrimumon.nmuons = cms.int32(3)
higgsTrimumon.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_TripleMu_12_10_5_v*") # 
higgsTrimumon.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v*","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v*")

##TripleElectron
higgsTrielemon = hlthiggsTripleElectronMonitoring.clone()
higgsTrielemon.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL/')
higgsTrielemon.nelectrons = cms.int32(3)
higgsTrielemon.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v*") # 
higgsTrielemon.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*")

#########cross-trigger monitor###########
####DiMuon+Single Ele Trigger
diMu9Ele9CaloIdLTrackIdL_muegref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
diMu9Ele9CaloIdLTrackIdL_muegref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_muegRef/')
diMu9Ele9CaloIdLTrackIdL_muegref.nelectrons = cms.int32(1)
diMu9Ele9CaloIdLTrackIdL_muegref.nmuons = cms.int32(2)
diMu9Ele9CaloIdLTrackIdL_muegref.leptonPDG = cms.int32(13)
diMu9Ele9CaloIdLTrackIdL_muegref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v*") # HLT_ZeroBias_v*
diMu9Ele9CaloIdLTrackIdL_muegref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v*", 
	"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v*",
	"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v*", 
        "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*"
)

diMu9Ele9CaloIdLTrackIdL_dimuref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
diMu9Ele9CaloIdLTrackIdL_dimuref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_dimuRef/')
diMu9Ele9CaloIdLTrackIdL_dimuref.nelectrons = cms.int32(1)
diMu9Ele9CaloIdLTrackIdL_dimuref.nmuons = cms.int32(2)
diMu9Ele9CaloIdLTrackIdL_dimuref.leptonPDG = cms.int32(13)
diMu9Ele9CaloIdLTrackIdL_dimuref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v*") # HLT_ZeroBias_v*
diMu9Ele9CaloIdLTrackIdL_dimuref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v*",
	"HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v*",
	"HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v*",
	"HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v*",
	"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v*",
	"HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v*"
)

####DiElectron+Single Muon Trigger
mu8diEle12CaloIdLTrackIdL_muegref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
mu8diEle12CaloIdLTrackIdL_muegref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_muegRef/')
mu8diEle12CaloIdLTrackIdL_muegref.nelectrons = cms.int32(2)
mu8diEle12CaloIdLTrackIdL_muegref.nmuons = cms.int32(1)
mu8diEle12CaloIdLTrackIdL_muegref.leptonPDG = cms.int32(11)
mu8diEle12CaloIdLTrackIdL_muegref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v*") # HLT_ZeroBias_v*
mu8diEle12CaloIdLTrackIdL_muegref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v*", 
	"HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v*",
	"HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v*", 
        "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*"
)

mu8diEle12CaloIdLTrackIdL_dieleref = hlthiggsTriLeptonCrossTrigMonitoring.clone()
mu8diEle12CaloIdLTrackIdL_dieleref.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_dieleRef/')
mu8diEle12CaloIdLTrackIdL_dieleref.nelectrons = cms.int32(2)
mu8diEle12CaloIdLTrackIdL_dieleref.nmuons = cms.int32(1)
mu8diEle12CaloIdLTrackIdL_dieleref.leptonPDG = cms.int32(11)
mu8diEle12CaloIdLTrackIdL_dieleref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v*") # HLT_ZeroBias_v*
mu8diEle12CaloIdLTrackIdL_dieleref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*"
)
