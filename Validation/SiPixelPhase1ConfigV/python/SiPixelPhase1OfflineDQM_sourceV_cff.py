import FWCore.ParameterSet.Config as cms

# Pixel Digi Monitoring
from Validation.SiPixelPhase1DigisV.SiPixelPhase1DigisV_cfi import *
# Hits
from Validation.SiPixelPhase1HitsV.SiPixelPhase1HitsV_cfi import *
# RecHit (clusters)
from Validation.SiPixelPhase1RecHitsV.SiPixelPhase1RecHitsV_cfi import *
# Clusters ontrack/offtrack (also general tracks)
from Validation.SiPixelPhase1TrackClustersV.SiPixelPhase1TrackClustersV_cfi import *
# Tracking Truth MC
from Validation.SiPixelPhase1TrackingParticleV.SiPixelPhase1TrackingParticleV_cfi import *

PerModule.enabled = False

siPixelPhase1OfflineDQM_sourceV = cms.Sequence(SiPixelPhase1DigisAnalyzerV
                                            + SiPixelPhase1HitsAnalyzerV
                                            + SiPixelPhase1RecHitsAnalyzerV
                                            + SiPixelPhase1TrackClustersAnalyzerV
                                            + SiPixelPhase1TrackingParticleAnalyzerV
                                            )

###PixelTracking only configurations for gpu wf###
#digi
pixelOnlyDigisAnalyzerV = SiPixelPhase1DigisAnalyzerV.clone()
#cluster
pixelOnlyTrackClustersAnalyzerV = SiPixelPhase1TrackClustersAnalyzerV.clone()
pixelOnlyTrackClustersAnalyzerV.clusters = cms.InputTag('siPixelClustersPreSplitting')
pixelOnlyTrackClustersAnalyzerV.tracks = cms.InputTag('pixelTracks')

#rechit analyzer
pixelOnlyRecHitsAnalyzerV = SiPixelPhase1RecHitsAnalyzerV.clone()
pixelOnlyRecHitsAnalyzerV.src = cms.InputTag("siPixelRecHitsPreSplitting")
pixelOnlyRecHitsAnalyzerV.pixelSimLinkSrc = cms.InputTag("simSiPixelDigis")
pixelOnlyRecHitsAnalyzerV.ROUList = cms.vstring('TrackerHitsPixelBarrelLowTof',
                                                'TrackerHitsPixelBarrelHighTof',
                                                'TrackerHitsPixelEndcapLowTof',
                                                'TrackerHitsPixelEndcapHighTof')
#Hits
pixelOnlyHitsAnalyzerV = SiPixelPhase1HitsAnalyzerV.clone()
pixelOnlyHitsAnalyzerV.tracksTag = cms.InputTag("pixelTracks")

#TP
pixelOnlyTrackingParticleAnalyzerV = SiPixelPhase1TrackingParticleAnalyzerV.clone()

siPixelPhase1ValidationPixelTrackingOnly_sourceV = cms.Sequence(pixelOnlyDigisAnalyzerV 
                                                                + pixelOnlyTrackClustersAnalyzerV 
                                                                + pixelOnlyHitsAnalyzerV
                                                                + pixelOnlyRecHitsAnalyzerV
                                                                + pixelOnlyTrackingParticleAnalyzerV
)
