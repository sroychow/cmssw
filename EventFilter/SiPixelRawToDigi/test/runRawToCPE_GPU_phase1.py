import FWCore.ParameterSet.Config as cms

process = cms.Process("MyRawToDigi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometryExtended2017Reco_cff')
#process.load('Configuration.Geometry.GeometryExtended2017NewFPixReco_cff')

#process.load("Configuration.StandardSequences.MagneticField_38T_cff")
#process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load("Configuration.StandardSequences.Services_cff")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
# to use no All 
# 2015
#process.GlobalTag.globaltag = 'GR_P_V56' # for 247607
#process.GlobalTag.globaltag = 'PRE_R_71_V3' #2014

#2017
#process.GlobalTag.globaltag='81X_upgrade2017_realistic_v26'
# AUTO conditions 
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run1_data', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_design', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgrade2017', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '76X_upgrade2017_design_v8', '')
#process.GlobalTag.globaltag ="81X_dataRun2_relval_v14"
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2017_realistic', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(8))

process.source = cms.Source("PoolSource",
fileNames =  cms.untracked.vstring(
#2017 CMSSW_9_2_0
# download this file
# /store/relval/CMSSW_9_2_0/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_91X_upgrade2017_realistic_v5_PU50-v1/10000/7C654D7C-9E40-E711-8690-0025905A48BC.root
#'file:/afs/cern.ch/work/s/sdubey/data/Raw_Data_Phase1/7C654D7C-9E40-E711-8690-0025905A48BC.root'
'file:/home/fpantale/data/920/PU50/085D5AAF-9E40-E711-B12A-0025905A609E.root'
 )
)
# Cabling
#useLocal = False
useLocal = False
if useLocal:
  process.CablingReader = cms.ESSource("PoolDBESSource",
    DBParameters = cms.PSet(
       messageLevel = cms.untracked.int32(0),
       authenticationPath = cms.untracked.string('')
    ),
    toGet = cms.VPSet(
      cms.PSet(
        record = cms.string('SiPixelFedCablingMapRcd'),
        tag = cms.string('SiPixelFedCablingMap_phase1_v2')
    )),
    #connect = cms.string('sqlite_file:../../../../../DB/phase1/SiPixelFedCablingMap_phase1_v2.db')
    connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS')
  ) # end process
  process.prefer = cms.ESPrefer("PoolDBESSource","CablingReader")
# end if


process.load("EventFilter.SiPixelRawToDigi.SiPixelRawToDigi_cfi")
# for simultaions 
#process.siPixelDigis.InputLabel = 'siPixelRawData'
# for data
#process.siPixelDigis.InputLabel = 'source'

process.siPixelDigis.InputLabel = 'rawDataCollector'
process.siPixelDigis.IncludeErrors = False #True
process.siPixelDigis.Timing = False 
process.siPixelDigis.UsePhase1 = cms.bool(True)

process.MessageLogger = cms.Service("MessageLogger",
    #debugModules = cms.untracked.vstring('siPixelDigis'),
    destinations = cms.untracked.vstring('log'),
    log = cms.untracked.PSet( threshold = cms.untracked.string('WARNING'))
    #log = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

# process.out = cms.OutputModule("PoolOutputModule",
#      fileName =  cms.untracked.string('file:digis_phase1.root'),
# #    fileName =  cms.untracked.string('file:/afs/cern.ch/work/d/dkotlins/public/data/digis/digi_zb_248025.root'),
#     #fileName =  cms.untracked.string('file:/afs/cern.ch/work/d/dkotlins/public/MC/mu_phase1/pt100_81/digis/digis1_formatfix.root'),
#     #outputCommands = cms.untracked.vstring("drop *","keep *_siPixelDigis_*_*")
# )


process.p = cms.Path(process.siPixelDigis)
#process.p = cms.Path(process.siPixelDigis*process.a)
# process.ep = cms.EndPath(process.out)
