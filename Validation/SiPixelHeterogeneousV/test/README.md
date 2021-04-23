# setup 12X area then

```
git cms-merge-topic sroychow:spixelhetero12x
scram b -j 4
```

The configs of interest are in Validation/SiPixelHeterogeneousV/test

* To run RECO + DQM(gpu + legacy): cmsRun step3_gpu_hybrid.py

* Harvesting step: cmsRun step4_HARVESTING.py

The output DQM root file should have folders named

```
SiPixelPhase1V
SiPixelPhase1LegacyV
```