import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rechitValidOT = DQMEDAnalyzer('Phase2TrackerValidateRecHit',
                            Verbosity = cms.bool(False),
                            TopFolderName = cms.string("Phase2TrackerRecHitV"),
                            ITPlotFillingFlag = cms.bool(False),
                            rechitsOT = cms.InputTag("siPhase2RecHits"),
                            rechitsIT = cms.InputTag("siPixelRecHits"),
                            clusters = cms.InputTag("siPhase2Clusters"),
                            OuterTrackerDigiSource = cms.InputTag("mix", "Tracker"),
                            OuterTrackerDigiSimLinkSource = cms.InputTag("simSiPixelDigis", "Tracker"),
                            InnerPixelDigiSource   = cms.InputTag("simSiPixelDigis","Pixel"),                          
                            InnerPixelDigiSimLinkSource = cms.InputTag("simSiPixelDigis", "Pixel"), 
                            PSimHitSource  = cms.VInputTag('g4SimHits:TrackerHitsPixelBarrelLowTof',
                                                           'g4SimHits:TrackerHitsPixelBarrelHighTof',
                                                           'g4SimHits:TrackerHitsPixelEndcapLowTof',
                                                           'g4SimHits:TrackerHitsPixelEndcapHighTof',
                                                           'g4SimHits:TrackerHitsTIBLowTof',
                                                           'g4SimHits:TrackerHitsTIBHighTof',
                                                           'g4SimHits:TrackerHitsTIDLowTof',
                                                           'g4SimHits:TrackerHitsTIDHighTof',
                                                           'g4SimHits:TrackerHitsTOBLowTof',
                                                           'g4SimHits:TrackerHitsTOBHighTof',
                                                           'g4SimHits:TrackerHitsTECLowTof',
                                                           'g4SimHits:TrackerHitsTECHighTof'),
                            SimTrackSource = cms.InputTag("g4SimHits"),
                            SimVertexSource = cms.InputTag("g4SimHits"),
                            usePhase2Tracker = cms.bool(True),#these are used by simHit assoc.
                            associatePixel = cms.bool(False),
                            associateRecoTracks = cms.bool(False),
                            associateStrip = cms.bool(True),
                            associateHitbySimTrack = cms.bool(True),
                            phase2TrackerSimLinkSrc = cms.InputTag("simSiPixelDigis", "Tracker"),
                            pixelSimLinkSrc = cms.InputTag("simSiPixelDigis","Pixel"),
                            ROUList  =  cms.vstring('g4SimHitsTrackerHitsPixelBarrelLowTof',
                                                    'g4SimHitsTrackerHitsPixelBarrelHighTof',
                                                    'g4SimHitsTrackerHitsTIBLowTof',
                                                    'g4SimHitsTrackerHitsTIBHighTof',
                                                    'g4SimHitsTrackerHitsTIDLowTof',
                                                    'g4SimHitsTrackerHitsTIDHighTof',
                                                    'g4SimHitsTrackerHitsTOBLowTof',
                                                    'g4SimHitsTrackerHitsTOBHighTof',
                                                    'g4SimHitsTrackerHitsPixelEndcapLowTof',
                                                    'g4SimHitsTrackerHitsPixelEndcapHighTof',
                                                    'g4SimHitsTrackerHitsTECLowTof',
                                                    'g4SimHitsTrackerHitsTECHighTof'),

) 


##For IT
rechitValidIT = rechitValidOT.clone()
rechitValidIT.ITPlotFillingFlag = cms.bool(True)
rechitValidIT.associatePixel = cms.bool(True)
rechitValidIT.associateStrip = cms.bool(False)

