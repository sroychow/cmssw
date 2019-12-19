from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.taus_cff import *
from PhysicsTools.NanoAOD.jets_cff import *
from PhysicsTools.NanoAOD.globals_cff import *
from PhysicsTools.NanoAOD.genparticles_cff import *
from PhysicsTools.NanoAOD.particlelevel_cff import *
from PhysicsTools.NanoAOD.lheInfoTable_cfi import *

# duplicate definition with nano_cff right now
genWeightsTable = cms.EDProducer(
    "GenWeightsTableProducer",
    genEvent = cms.InputTag("generator"),
    genLumiInfoHeader = cms.InputTag("generator"),
    lheInfo = cms.VInputTag(cms.InputTag("externalLHEProducer"), cms.InputTag("source")),
    preferredPDFs = cms.VPSet(  # see https://lhapdf.hepforge.org/pdfsets.html
        cms.PSet(name = cms.string("PDF4LHC15_nnlo_30_pdfas"), lhaid = cms.uint32(91400)),
        cms.PSet(name = cms.string("NNPDF31_nnlo_hessian_pdfas"), lhaid = cms.uint32(306000)),
        # for some 92X samples. Note that the nominal weight, 260000, is not included in the LHE ...
        cms.PSet(name = cms.string("NNPDF30_nlo_as_0118"), lhaid = cms.uint32(260000)),
        # some MLM 80X samples have only this (e.g. /store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/02A210D6-F5C3-E611-B570-008CFA197BD4.root )
        cms.PSet(name = cms.string("NNPDF30_lo_as_0130"), lhaid = cms.uint32(262000)),
        # some FXFX 80X samples have only this (e.g. WWTo1L1Nu2Q, WWTo4Q)
        cms.PSet(name = cms.string("NNPDF30_nlo_nf_4_pdfas"), lhaid = cms.uint32(292000)),
        # some FXFX 80X samples have only this (e.g. DYJetsToLL_Pt, WJetsToLNu_Pt, DYJetsToNuNu_Pt)
        cms.PSet(name = cms.string("NNPDF30_nlo_nf_5_pdfas"), lhaid = cms.uint32(292200)),
    ),
    namedWeightIDs = cms.vstring(),
    namedWeightLabels = cms.vstring(),
    lheWeightPrecision = cms.int32(14),
    maxPdfWeights = cms.uint32(150),
    debug = cms.untracked.bool(False),
)

lheWeightsTable = cms.EDProducer(
    "LHEWeightsTableProducer",
    lheInfo = cms.InputTag("externalLHEProducer"),
    lheWeightPrecision = cms.int32(14),
)

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

metGenTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = cms.InputTag("genMetTrue"),
    name = cms.string("GenMET"),
    doc = cms.string("Gen MET"),
    singleton = cms.bool(True),
    extension = cms.bool(False),
    variables = cms.PSet(
       pt  = Var("pt",  float, doc="pt", precision=10),
       phi = Var("phi", float, doc="phi", precision=10),
    ),
)

nanogenSequence = cms.Sequence(
    nanoMetadata+
    particleLevel+
    genJetTable+
    patJetPartons+
    genJetFlavourAssociation+
    genJetFlavourTable+
    genJetAK8Table+
    genJetAK8FlavourAssociation+
    genJetAK8FlavourTable+
    tauGenJets+
    tauGenJetsSelectorAllHadrons+
    genVisTaus+
    genVisTauTable+
    genTable+
    genWeightsTable+
    lheWeightsTable+
    genParticleTables+
    tautagger+
    rivetProducerHTXS+
    particleLevelTables+
    metGenTable+
    lheInfoTable
)

NANOAODGENoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('nanogen.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        "keep nanoaodFlatTable_*Table_*_*",     # event data
        "keep String_*_genModel_*",  # generator model data
        "keep nanoaodMergeableCounterTable_*Table_*_*", # accumulated per/run or per/lumi data
        "keep nanoaodUniqueString_nanoMetadata_*_*",   # basic metadata
    )
)

def customizeNanoGEN(process):
    process.genParticleTable.src = "genParticles"
    process.patJetPartons.particles = "genParticles"
    process.particleLevel.src = "generatorSmeared"
    process.rivetProducerHTXS.HepMCCollection = "generatorSmeared"

    process.genJetTable.src = "ak4GenJetsNoNu"
    process.genJetFlavourAssociation.jets = process.genJetTable.src
    process.genJetFlavourTable.src = process.genJetTable.src
    process.genJetFlavourTable.jetFlavourInfos = "genJetFlavourAssociation"
    process.genJetAK8Table.src = "ak8GenJetsNoNu"
    process.genJetAK8FlavourAssociation.jets = process.genJetAK8Table.src
    process.genJetAK8FlavourTable.src = process.genJetAK8Table.src
    process.tauGenJets.GenParticles = "genParticles"
    process.genVisTaus.srcGenParticles = "genParticles"
