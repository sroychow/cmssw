import FWCore.ParameterSet.Config as cms

from higgsDiLeptonCrossTriggerMonitoring_cfi import higgsDiLeptonCrossTriggerMonitoring
hlthiggsDiLeptonCrossTrigMonitoring = higgsDiLeptonCrossTriggerMonitoring.clone()
hlthiggsDiLeptonCrossTrigMonitoring.FolderName = cms.string('HLT/Higgs/DiLepton/')
hlthiggsDiLeptonCrossTrigMonitoring.histoPSet.lepPSet = cms.PSet(
  nbins = cms.int32 (  200  ),
  xmin  = cms.double(   -0.5),
  xmax  = cms.double(19999.5),
)
hlthiggsDiLeptonCrossTrigMonitoring.histoPSet.phiPSet = cms.PSet(
  nbins = cms.int32 (  64 ),
  xmin  = cms.double( -3.2),
  xmax  = cms.double( 3.2 ),
)
hlthiggsDiLeptonCrossTrigMonitoring.histoPSet.etaPSet = cms.PSet(
  nbins = cms.int32 (  60 ),
  xmin  = cms.double( -3.),
  xmax  = cms.double(  3. ),
)

hlthiggsDiLeptonCrossTrigMonitoring.electrons = cms.InputTag("gedGsfElectrons") 
hlthiggsDiLeptonCrossTrigMonitoring.nelectrons = cms.int32(1)
hlthiggsDiLeptonCrossTrigMonitoring.muons     = cms.InputTag("muons") 
hlthiggsDiLeptonCrossTrigMonitoring.nmuons = cms.int32(1)
hlthiggsDiLeptonCrossTrigMonitoring.vertices     = cms.InputTag("offlinePrimaryVertices")

hlthiggsDiLeptonCrossTrigMonitoring.leptondxycut = cms.double(0.5)
hlthiggsDiLeptonCrossTrigMonitoring.leptondzcut = cms.double(1.)

hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.andOr         = cms.bool( False )
#hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.dbLabel      = cms.string("HiggsDQMTrigger") # it does not exist yet
hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.andOrHlt      = cms.bool(True)# True:=OR; False:=AND
hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )

hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltPaths       = cms.vstring("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*") 
#hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltDBKey      = cms.string("Higgs_HLT_DiLepton")
hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.errorReplyHlt  = cms.bool( False )
hlthiggsDiLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.andOr          = cms.bool( False )
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.dcsInputTag    = cms.InputTag( "scalersRawToDigi" )
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.dcsPartitions  = cms.vint32 ( 24, 25, 26, 27, 28, 29 ) 
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.andOrDcs       = cms.bool( False )
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.errorReplyDcs  = cms.bool( True )
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)
hlthiggsDiLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_IsoMu24_v*")
