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
