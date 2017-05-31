import FWCore.ParameterSet.Config as cms
from DQMOffline.Trigger.HiggsTriLeptonMonitor_cff  import *
from DQMOffline.Trigger.HiggsDiLeptonMonitor_cff  import *

higgsMonitorHLT = cms.Sequence(
 higgsTrielemon
 + higgsTrimumon
 + mu8diEle12CaloIdLTrackIdL_muegref
 + mu8diEle12CaloIdLTrackIdL_dieleref
 + diMu9Ele9CaloIdLTrackIdL_dimuref
 + diMu9Ele9CaloIdLTrackIdL_muegref
 + mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_muref
 + mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_eleref
 + mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_muref
 + mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_eleref
)
