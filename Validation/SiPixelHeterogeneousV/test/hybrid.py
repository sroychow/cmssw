import FWCore.ParameterSet.Config as cms
from HeterogeneousCore.CUDACore.SwitchProducerCUDA import SwitchProducerCUDA
from EventFilter.SiPixelRawToDigi.siPixelRawToDigi_cfi import siPixelRawToDigi as _siPixelRawToDigi

siPixelDigisCPUlegacy = _siPixelRawToDigi.clone(
  InputLabel = cms.InputTag("rawDataCollector"),
)

from EventFilter.ScalersRawToDigi.scalersRawToDigi_cfi import scalersRawToDigi
RawToDigiTask_pixelOnlylegacy = cms.Task(scalersRawToDigi, siPixelDigisCPUlegacy)
RawToDigi_pixelOnlylegacy = cms.Sequence(RawToDigiTask_pixelOnlylegacy)
raw2digi_steplegacy = cms.Path(RawToDigi_pixelOnlylegacy)


##RECO step
##from dump
'''
RECO step modules:
['siPixelClustersPreSplitting', 'siPixelRecHitsPreSplitting', 'offlineBeamSpot', 'siPixelClusterShapeCachePreSplitting', 'pixelTracksTrackingRegions', 'pixelFitterByHelixProjections', 'pixelTrackFilterByKinematics', 'pixelTracksSeedLayers', 'pixelTracksHitDoublets', 'pixelTracksHitQuadruplets', 'pixelTracks', 'pixelVertices']
'''
##only need the following for local reco
from RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi import siPixelClusters as _siPixelClusters
#change input to digi collection as above
siPixelClustersPreSplittingCPUlegacy = _siPixelClusters.clone(
    src = 'siPixelDigisCPUlegacy'  
)

from RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi import siPixelRecHits, _siPixelRecHitsPreSplitting
siPixelRecHitsPreSplittingCPUlegacy= siPixelRecHits.clone(
    src = cms.InputTag('siPixelClustersPreSplittingCPUlegacy')#change input cluster  collection to above
)

reconstruction_pixelTrackingOnlyCPUlegacy=cms.Sequence(siPixelClustersPreSplittingCPUlegacy*siPixelRecHitsPreSplittingCPUlegacy)
reconstruction_steplegacy = cms.Path(reconstruction_pixelTrackingOnlyCPUlegacy)

##Now clone and define a validation step

##Now clone the Validation step
from Validation.SiPixelPhase1ConfigV.SiPixelPhase1OfflineDQM_sourceV_cff import *


pixelOnlyDigisAnalyzerVlegacy=pixelOnlyDigisAnalyzerV.clone(
    src = cms.InputTag("siPixelDigisCPUlegacy"),
)
for pset in pixelOnlyDigisAnalyzerVlegacy.histograms:
    pset.topFolderName = "PixelPhase1LegacyV/Digis"

pixelOnlyTrackClustersAnalyzerVlegacy = pixelOnlyTrackClustersAnalyzerV.clone(
    clusters = 'siPixelClustersPreSplittingCPUlegacy',
    tracks = 'pixelTracks'
)#although pixekTracks for legacy is not run.
for pset in pixelOnlyTrackClustersAnalyzerVlegacy.histograms:
    pset.topFolderName = "PixelPhase1LegacyV/Clusters"

# Pixel rechit analyzer
pixelOnlyRecHitsAnalyzerVlegacy = pixelOnlyRecHitsAnalyzerV.clone(
    src = 'siPixelRecHitsPreSplittingCPUlegacy',
)
for pset in pixelOnlyRecHitsAnalyzerVlegacy.histograms:
    pset.topFolderName = "PixelPhase1LegacyV/RecHits"

validation_legacy_sequence = cms.Sequence(pixelOnlyDigisAnalyzerVlegacy*pixelOnlyTrackClustersAnalyzerVlegacy*pixelOnlyRecHitsAnalyzerVlegacy)
validation_legacy = cms.Path(validation_legacy_sequence)
