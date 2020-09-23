import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rechitValidOT = DQMEDAnalyzer('Phase2OTValidateRecHit',
                              Verbosity = cms.bool(False),
                              TopFolderName = cms.string("Phase2TrackerRecHitV/Phase2OT"),
                              ITPlotFillingFlag = cms.bool(False),
                              rechitsSrc = cms.InputTag("siPhase2RecHits"),
                              OuterTrackerDigiSource = cms.InputTag("mix", "Tracker"),
                              OuterTrackerDigiSimLinkSource = cms.InputTag("simSiPixelDigis", "Tracker"),
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
                              simTracksSrc = cms.InputTag("g4SimHits"),
                              SimTrackMinPt = cms.double(2.),
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
