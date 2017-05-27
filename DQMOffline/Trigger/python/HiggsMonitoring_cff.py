import FWCore.ParameterSet.Config as cms
from DQMOffline.Trigger.HiggsTriLeptonMonitor_cff  import *

higgsMonitorHLT = cms.Sequence(
 higgscrossTriLeptonMonitoring_muegref 
 + higgscrossTriLeptonMonitoring_dimuref
)
