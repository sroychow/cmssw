import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

######Double Mu + Single Ele#############
dimu9ele9caloIdLTrackIdLdz_effmu = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_muegRef/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_subleadMuonPt          'Sub-leading muon pT turnON;            pT [GeV]; efficiency'     sflep2Pt_numerator          sflep2Pt_denominator",
        "effic_subleadMuonPt_variable 'Sub-leading muon pT turnON;            pT [GeV]; efficiency'     sflep2Pt_variable_numerator sflep2Pt_variable_denominator",
        "effic_subleadMuonPhi       'Efficiency vs Sub-leading muon #phi;  #phi [rad]; efficiency' sflep2Phi_numerator       sflep2Phi_denominator",
        "effic_subleadMuonEta       'Efficiency vs Sub-leading muon #eta;  #eta; efficiency' sflep2Eta_numerator       sflep2Eta_denominator",
        "effic_subleadMuonPtEta       'Efficiency;p_T; #eta;' sflep2PtEta_numerator       sflep2PtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_subleadMuonPt_vs_LS 'Sub-leading muon p_T efficiency vs LS; LS; Sub-leading muon p_T efficiency' sflep2PtVsLS_numerator sflep2PtVsLS_denominator"
    ),
)

dimu9ele9caloIdLTrackIdLdz_effele = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_dimuRef/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_leadElectronPt          'Electron pT turnON;            pT [GeV]; efficiency'     cflepPt_numerator          cflepPt_denominator",
        "effic_leadElectronPt_variable 'Electron pT turnON;            pT [GeV]; efficiency'     cflepPt_variable_numerator cflepPt_variable_denominator",
        "effic_leadElectronPhi       'Efficiency vs electron #phi;  #phi [rad]; efficiency' cflepPhi_numerator       cflepPhi_denominator",
        "effic_leadElectronEta       'Efficiency vs Sub-leading electron #eta;  #eta; efficiency' cflepEta_numerator       cflepEta_denominator",
        "effic_leadElectronPtEta     'Efficiency;p_T; #eta' cflepPtEta_numerator       cflepPtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_ElectronPt_vs_LS 'Electron p_T efficiency vs LS; LS; Trailing lepton p_T efficiency' cflepPtVsLS_numerator cflepPtVsLS_denominator"
    ),
)

######Double Electron + Single Muon######

mu8diEle12CaloIdLTrackIdL_effele = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_muegRef/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_subleadElectronPt          'Sub-leading electron pT turnON;            pT [GeV]; efficiency'     sflep2Pt_numerator          sflep2Pt_denominator",
        "effic_subleadElectronPt_variable 'Sub-leading electron pT turnON;            pT [GeV]; efficiency'     sflep2Pt_variable_numerator sflep2Pt_denominator",
        "effic_subleadElectronPhi       'Efficiency vs subleading electron phi;  phi [rad]; efficiency' sflep2Phi_numerator	   sflep2Phi_denominator",
        "effic_subleadElectronEta       'Efficiency vs Sub-leading electron #eta;  #eta; efficiency' sflep2Eta_numerator       sflep2Eta_denominator",
        "effic_subleadElectronPtEta     'Efficiency;p_T; #eta;' sflep2PtEta_numerator       sflep2PtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_subleadElectronPt_vs_LS 'Sub-leading electron p_T efficiency vs LS; LS; Sub-leading electron p_T efficiency' sflep2PtVsLS_numerator sflep2PtVsLS_denominator"
    ),
) 

mu8diEle12CaloIdLTrackIdL_effmu = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_dieleRef/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_leadMuonPt          'Muon pT turnON;            pT [GeV]; efficiency'     cflepPt_numerator          cflepPt_denominator",
        "effic_leadMuonPt_variable 'Muon pT turnON;            pT [GeV]; efficiency'     cflepPt_variable_numerator cflepPt_variable_denominator",
        "effic_leadMuonPhi       'Efficiency vs muon phi;  phi [rad]; efficiency' cflepPhi_numerator       cflepPhi_denominator",
        "effic_leadMuonEta       'Efficiency vs Sub-leading muon #eta;  #eta; efficiency' cflepEta_numerator       cflepEta_denominator",
        "effic_leadMuonPtEta     'Efficiency;p_T; #eta' cflepPtEta_numerator       cflepPtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_MuonPt_vs_LS 'Muon p_T efficiency vs LS; LS; Muon p_T efficiency' cflepPtVsLS_numerator cflepPtVsLS_denominator"
    ),
)

#####Triple Electron#####
ele16ele12ele8caloIdLTrackIdL =  DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_trailingElectronPt          'Electron pT turnON;            pT [GeV]; efficiency'     lep3Pt_numerator          lep3Pt_denominator",
        "effic_trailingElectronPt_variable 'Electron pT turnON;            pT [GeV]; efficiency'     lep3Pt_variable_numerator lep3Pt_variable_denominator",
        "effic_trailingElectronPhi       'Efficiency vs trailing electron #phi;  #phi [rad]; efficiency' lep3Phi_numerator       lep3Phi_denominator",
        "effic_trailingElectronEta       'Efficiency vs trailing electron #eta;  #eta [rad]; efficiency' lep3Eta_numerator       lep3Eta_denominator",
        "effic_trailingElectronPtEta       'Efficiency;p_T; #eta' lep3PtEta_numerator       lep3PtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_trailingElectronPt_vs_LS 'Trailing electron p_T efficiency vs LS; LS; Trailing electron p_T efficiency' lep3PtVsLS_numerator lep3PtVsLS_denominator"
    ),
)

####Triple Muon#####
triplemu12mu10mu5 = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_TripleMu_12_10_5/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_trailingMuonPt          'Trailing muon pT turnON;            pT [GeV]; efficiency'     lep3Pt_numerator          lep3Pt_denominator",
        "effic_trailingMuonPt_variable 'Trailing muon pT turnON;            pT [GeV]; efficiency'     lep3Pt_variable_numerator lep3Pt_variable_denominator",
        "effic_trailingMuonPhi       'TriLeptonTrigger efficiency vs #phi;  phi [rad]; efficiency' lep3Phi_numerator	   lep3Phi_denominator",
        "effic_trailingMuonEta       'p_T;  #eta [rad]; efficiency' lep3Eta_numerator	   lep3Eta_denominator",
        "effic_trailingMuonPtEta     'Efficiency;p_T; #eta' lep3PtEta_numerator	   lep3PtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_trailingMuon_vs_LS 'Trailing muon p_T efficiency vs LS; LS; Trailing muon p_T efficiency' lep3PtVsLS_numerator lep3PtVsLS_denominator"
    ),
)

#####MuEG cross triggers#####
mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_effele =  DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/DiLepton/HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_muref/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_ElectronPt          'Electron pT turnON;            pT [GeV]; efficiency'     elePt_numerator          elePt_denominator",
        "effic_ElectronPt_variable 'Electron pT turnON;            pT [GeV]; efficiency'     elePt_variable_numerator elePt_variable_denominator",
        "effic_ElectronPhi       'Efficiency vs electron #phi;  phi [rad]; efficiency' elePhi_numerator       elePhi_denominator",
        "effic_ElectronEta       'Efficiency vs electron #eta;  phi [rad]; electron #eta efficiency' eleEta_numerator       eleEta_denominator",
        "effic_ElectronPtEta     'Efficiency;p_T; #eta' elePtEta_numerator       elePtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_ElectronPt_vs_LS 'Electron p_T efficiency vs LS; LS; electron p_T efficiency' elePtVsLS_numerator elePtVsLS_denominator"
    ),
)

mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_effmu = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/DiLepton/HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_eleref/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_MuonPt          'Muon pT turnON;            pT [GeV]; efficiency'     muPt_numerator          muPt_denominator",
        "effic_MuonPt_variable 'Muon pT turnON;            pT [GeV]; efficiency'     muPt_variable_numerator muPt_variable_denominator",
        "effic_MuonPhi       'Efficiency vs muon #phi;  #phi [rad]; efficiency' muPhi_numerator       muPhi_denominator",
        "effic_MuonEta       'Efficiency vs muon #eta;  #eta ; efficiency' muEta_numerator       muEta_denominator",
        "effic_MuonPtEta     'Efficiency;p_T; #eta' muPtEta_numerator       muPtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_MuonPt_vs_LS 'Muon p_T efficiency vs LS; LS; Muon p_T efficiency' muPtVsLS_numerator muPtVsLS_denominator"
    ),
)

mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_effele =  DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/DiLepton/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_muref/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_ElectronPt          'Electron pT turnON;            pT [GeV]; efficiency'     elePt_numerator          elePt_denominator",
        "effic_ElectronPt_variable 'Electron pT turnON;            pT [GeV]; efficiency'     elePt_variable_numerator elePt_variable_denominator",
        "effic_ElectronPhi       'Efficiency vs electron #phi;  phi [rad]; efficiency' elePhi_numerator       elePhi_denominator",
        "effic_ElectronEta       'Efficiency vs electron #eta;  phi [rad]; electron #eta efficiency' eleEta_numerator       eleEta_denominator",
        "effic_ElectronPtEta     'Efficiency;p_T; #eta' elePtEta_numerator       elePtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_ElectronPt_vs_LS 'Electron p_T efficiency vs LS; LS; electron p_T efficiency' elePtVsLS_numerator elePtVsLS_denominator"
    ),
)

mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_effmu = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/DiLepton/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_eleref/"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_MuonPt          'Muon pT turnON;            pT [GeV]; efficiency'     muPt_numerator          muPt_denominator",
        "effic_MuonPt_variable 'Muon pT turnON;            pT [GeV]; efficiency'     muPt_variable_numerator muPt_variable_denominator",
        "effic_MuonPhi       'Efficiency vs muon #phi;  #phi [rad]; efficiency' muPhi_numerator       muPhi_denominator",
        "effic_MuonEta       'Efficiency vs muon #eta;  #eta ; efficiency' muEta_numerator       muEta_denominator",
        "effic_MuonPtEta     'Efficiency;p_T; #eta' muPtEta_numerator       muPtEta_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_MuonPt_vs_LS 'Muon p_T efficiency vs LS; LS; Muon p_T efficiency' muPtVsLS_numerator muPtVsLS_denominator"
    ),
)


higgsClient = cms.Sequence(
  dimu9ele9caloIdLTrackIdLdz_effmu 
  + dimu9ele9caloIdLTrackIdLdz_effele 
  + mu8diEle12CaloIdLTrackIdL_effmu
  + mu8diEle12CaloIdLTrackIdL_effele
  + ele16ele12ele8caloIdLTrackIdL
  + triplemu12mu10mu5
  + mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_effele
  + mu23TrkIsoVVLEle8CaloIdLTrackIdLIsoVL_effmu
  + mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_effele
  + mu8TrkIsoVVLEle23CaloIdLTrackIdLIsoVL_effmu
)
