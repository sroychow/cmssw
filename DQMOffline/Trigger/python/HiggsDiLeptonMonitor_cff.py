import FWCore.ParameterSet.Config as cms

from HiggsDiLeptonMonitor_cfi import *
#########same flavour monitor###########
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_muref = hlthiggsDiLeptonCrossTrigMonitoring.clone()
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_muref.FolderName = cms.string('HLT/Higgs/DiLepton/HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_muref/')
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_muref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v*") 
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_muref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Mu20_v*","HLT_TkMu20_v*",
	"HLT_IsoMu24_eta2p1_v*",
	"HLT_IsoMu24_v*",
	"HLT_IsoMu27_v*",
	"HLT_IsoMu20_v*",
	"HLT_IsoTkMu24_eta2p1_v*",
	"HLT_IsoTkMu24_v*",
	"HLT_IsoTkMu27_v*",
	"HLT_IsoTkMu20_v*"
)

mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_eleref = hlthiggsDiLeptonCrossTrigMonitoring.clone()
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_eleref.FolderName = cms.string('HLT/Higgs/DiLepton/HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_eleref/')
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_eleref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v*")
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_eleref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
        "HLT_Ele27_WPTight_Gsf_v*",
	"HLT_Ele35_WPTight_Gsf_v*"
)

#####HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v#####
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_muref = hlthiggsDiLeptonCrossTrigMonitoring.clone()
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_muref.FolderName = cms.string('HLT/Higgs/DiLepton/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_muref/')
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_muref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*") # 
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_muref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
	"HLT_Mu20_v*",
	"HLT_TkMu20_v*",
	"HLT_IsoMu24_eta2p1_v*",
	"HLT_IsoMu24_v*",
	"HLT_IsoMu27_v*",
	"HLT_IsoMu20_v*",
	"HLT_IsoTkMu24_eta2p1_v*",
	"HLT_IsoTkMu24_v*",
	"HLT_IsoTkMu27_v*",
	"HLT_IsoTkMu20_v*"
)

mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_eleref = hlthiggsDiLeptonCrossTrigMonitoring.clone()
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_eleref.FolderName = cms.string('HLT/Higgs/DiLepton/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_eleref/')
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_eleref.numGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*")
mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_eleref.denGenericTriggerEventPSet.hltPaths = cms.vstring(
        "HLT_Ele27_WPTight_Gsf_v*",
	"HLT_Ele35_WPTight_Gsf_v*"
)

