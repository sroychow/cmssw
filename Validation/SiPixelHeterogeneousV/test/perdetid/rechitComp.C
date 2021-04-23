#include <TH1D.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include<vector>
#include<map>
#include<iostream>
#include "DetInfo.h"
#include <TString.h>
#include<algorithm>
#include<string>
#include<utility>

struct DetHisto {
  TH2D* hnrechits;
  TH1D* hrechitxRes;
  TH1D* hrechityRes;
  TH1D* hrechitxerrRes;
  TH1D* hrechityerrRes;

  //not matched
  

  bool isbarrel;
  unsigned int layer;//layer or ring
  int shell;//shell or halfCylinder
  std::string folderName;
  
  void setFoldername(unsigned int rawid) {
    if(isbarrel) {
      folderName = "PXBarrel/Shell_" + std::to_string(shell) + "/PXLayer_" + layer + "/" + std::to_string(rawid) + "/";     
    } else {
      folderName = "PXForward/HalfCylinder_" + std::to_string(shell) + "/PXRing_" +  layer + "/" + std::to_string(rawid) + "/";     
    }
  }
  
  void bookHistos(unsigned int rawid) {
    std::cout << "Booking histos for:" << rawid << std::endl;
    TString tag = TString(std::to_string(rawid));
    //rechits
    hnrechits = new TH2D("compnrechits" + tag, "#RecHits;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    hrechitxRes = new TH1D("comprechitXRes" + tag, "RecHit local x diff;x_{gpu} - x_{cpu};", 1000, -0.0001, 0.0001);
    hrechityRes = new TH1D("comprechitYRes" + tag, "RecHit local y diff;y_{gpu} - y_{cpu};", 1000, -0.0001, 0.0001);
    hrechitxerrRes = new TH1D("comprechitXerrRes" + tag, "RecHit local x error diff;xerr_{gpu} - xerr_{cpu};", 1000, -0.0001, 0.0001);
    hrechityerrRes = new TH1D("comprechitYerrRes" + tag, "RecHit local y error diff;yerr_{gpu} - yerr_{cpu};", 1000, -0.0001, 0.0001);
  }

};

class DetAnalyzer {
public:
  
  DetAnalyzer(bool plotdethisto) {
    fillHistosFordet_ = plotdethisto;
    if(!fillHistosFordet_)  std::cout << "Per det Histos will not be booked\n";
  }
  ~DetAnalyzer() {}
  
  void getTree(TString fname, bool isgpu) {
    TFile* fin = TFile::Open(fname);
    if(isgpu) {
      //gpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldigidetId/detinfo"));
      gpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldetId/detinfo"));
      setgpubranches();
    } else {
      cpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldigidetId/detinfo"));
      setcpubranches();
    }
  }
  
  void setgpubranches() {
    if(!gpuTree_)   return;
    gpudetmap_ = new std::map<unsigned int, DetInfo>();
    gpuTree_->SetBranchAddress("digiInfo", &gpudetmap_); 
  }
  
  void setcpubranches() {
    if(!cpuTree_)   return;
    cpudetmap_ = new std::map<unsigned int, DetInfo>();
    cpuTree_->SetBranchAddress("digiInfo", &cpudetmap_); 
  }
  
  //Function to compare 2 clusterss
  bool compareCluster(const DetInfo& ginfo, const DetInfo& cinfo, const unsigned int gi, const unsigned int ci) {
    if(ginfo.cluster_minPixelRow[gi]   == cinfo.cluster_minPixelRow[ci]
       && ginfo.cluster_maxPixelRow[gi] == cinfo.cluster_maxPixelRow[ci]
       && ginfo.cluster_minPixelCol[gi] == cinfo.cluster_minPixelCol[ci]
       && ginfo.cluster_maxPixelCol[gi] == cinfo.cluster_maxPixelCol[ci] 
       )  return true;
    return false;
  }

  
  void bookGlobalHistos() {
    //rechits
    hnrechits = new TH2D("compnrechits", "#RecHits;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    hrechitxRes = new TH1D("comprechitXres", "RecHit local x diff;x_{gpu} - x_{cpu};", 1000, -0.001, 0.001);
    hrechityRes = new TH1D("comprechitYRes", "RecHit local y diff;y_{gpu} - y_{cpu}", 1000, -0.001, 0.001);
    hrechitxerrRes = new TH1D("comprechitXerrRes", "RecHit local x error diff;xerr_{gpu} - xerr_{cpu};", 1000, -0.0001, 0.0001);
    hrechityerrRes = new TH1D("comprechitYerrRes", "RecHit local y error diff;yerr_{gpu} - yerr_{cpu};", 1000, -0.0001, 0.0001);
    hncpumatched = new TH1D("ncpurechitsmatched", "#legacy rechits matched to 1 GPU rechit", 11, -0.5, 10.5);
    hngpumatched = new TH1D("ngpurechitmatched", "#GPU rechits matched to 1 legacy rechit", 11, -0.5, 10.5);

  }
   
  void fillDetHistos(DetInfo& gpuinfo, DetInfo& cpuinfo, unsigned int rawid, unsigned int jentry) {
    //Fill number of objects  histos
    hnrechits->Fill(gpuinfo.nrechit, cpuinfo.nrechit);
    if(fillHistosFordet_) { 
      histomap_[rawid].hnrechits->Fill(gpuinfo.nrechit, cpuinfo.nrechit);
    }
    
    //if(jentry==724) std::cout << "#nrechits gpu:" << gpuinfo.nrechit << "\t gpu=" << gpuinfo.rechit_x.size() << std::endl;
    std::vector<int> nmatchedcpu(gpuinfo.rechit_x.size(), 0);
    std::vector<int> nmatchedgpu(cpuinfo.rechit_x.size(), 0);

    for(unsigned int gi = 0; gi < gpuinfo.rechit_x.size(); gi++) {
      double g_x = gpuinfo.rechit_x[gi];
      double g_y = gpuinfo.rechit_y[gi];
      double g_xerr = gpuinfo.rechit_xerror[gi];
      double g_yerr = gpuinfo.rechit_yerror[gi];
      int g_clsidx = gpuinfo.rechit_clusteridx[gi]; 
      //std::cout << "GPU cluster ref:" << gpuinfo.rechit_clusteridx[gi] << std::endl;    
  
      for(unsigned int ci = 0; ci <  cpuinfo.rechit_x.size(); ci++) {
        double c_x = cpuinfo.rechit_x[ci];
        double c_y = cpuinfo.rechit_y[ci];
        double c_xerr = cpuinfo.rechit_xerror[ci];
        double c_yerr = cpuinfo.rechit_yerror[ci];
	int c_clsidx = cpuinfo.rechit_clusteridx[ci]; 
	if(compareCluster(gpuinfo, cpuinfo, g_clsidx, c_clsidx)) {
	  nmatchedcpu[gi]++;
	  nmatchedgpu[ci]++;
	  hrechitxRes->Fill(g_x - c_x);
	  hrechityRes->Fill(g_y - c_y);
	  hrechitxerrRes->Fill(g_xerr - c_xerr);
	  hrechityerrRes->Fill(g_yerr - c_yerr);
	  if(fillHistosFordet_) {
	    histomap_[rawid].hrechitxRes->Fill(g_x - c_x);
	    histomap_[rawid].hrechityRes->Fill(g_y - c_y);
	    histomap_[rawid].hrechitxerrRes->Fill(g_xerr - c_xerr);
	    histomap_[rawid].hrechityerrRes->Fill(g_yerr - c_yerr);
	  }
	}
      }
    }
    
    for(auto& c : nmatchedgpu) hngpumatched->Fill(c);
    for(auto& c : nmatchedcpu) hncpumatched->Fill(c);
  }

  void Loop() {
    bookGlobalHistos();
    Long64_t cpuentries = cpuTree_->GetEntries();
    Long64_t gpuentries = gpuTree_->GetEntries();
    Long64_t toprocess = cpuentries; 
    if (cpuentries != gpuentries) {
      std::cout << "#Entries in the 2 trees are not same\n";
      toprocess = std::min(cpuentries, gpuentries);
    }
    Long64_t nb;
    std::cout << "#Entries to process:" << toprocess << std::endl;

    for(Long64_t jentry=0; jentry<toprocess;jentry++) {
      nb = cpuTree_->GetEntry(jentry);
      nb = gpuTree_->GetEntry(jentry);
      std::cout << "Processed entries:" << jentry << std::endl;
      //Clusters
      for(auto& detcls : *gpudetmap_) {
        auto rawid = detcls.first;
        if(cpudetmap_->find(rawid) == cpudetmap_->end())  {
          std::cout << "Detid " << rawid << " not found in CPU Cluster map!!\n";
          continue;
        }  

        auto gpuinfo = detcls.second;
        auto cpuinfo = cpudetmap_->at(rawid);

        //clusters
        if(fillHistosFordet_ && histomap_.find(rawid) == histomap_.end()) {
          DetHisto hd;
          hd.isbarrel = detcls.second.isbarrel;
          hd.layer = detcls.second.layer;//layer or ring
          hd.shell = detcls.second.shell;
          hd.setFoldername(rawid);
          hd.bookHistos(rawid);
          histomap_.insert({rawid, hd});
        }
        fillDetHistos(gpuinfo, cpuinfo, rawid, jentry);

      }//end loop over gpu clusters

 
    }//end event loop
    std::cout << "Loop end\n";
  }


  void writeHistos() {
    TFile* fout = TFile::Open("rechitComp.root", "recreate");
    fout->mkdir("rechit");
    fout->cd("rechit");
    hnrechits->Write();
    hrechitxRes->Write();
    hrechityRes->Write();
    hrechitxerrRes->Write();
    hrechityerrRes->Write();
    hngpumatched->Write();
    hncpumatched->Write();
    fout->cd();
    if(fillHistosFordet_) {
    for(auto& hd : histomap_) {
      TString fn(hd.second.folderName.c_str());
      fout->mkdir(fn);
      fout->cd(fn);
      //rechits
      hd.second.hnrechits->Write();
      hd.second.hrechitxRes->Write();
      hd.second.hrechityRes->Write();
      hd.second.hrechitxerrRes->Write();
      hd.second.hrechityerrRes->Write();
      fout->cd();
    }
    }
    fout->Save();
    fout->Close();
  }

private:
  TTree* gpuTree_;
  TTree* cpuTree_;
  std::map<unsigned int, DetInfo>* gpudetmap_;
  std::map<unsigned int, DetInfo>* cpudetmap_;
  
  bool fillHistosFordet_;
  TH2D* hnrechits;
  TH1D* hrechitxRes;
  TH1D* hrechityRes;
  TH1D* hrechitxerrRes;
  TH1D* hrechityerrRes;
  TH1D* hngpumatched;
  TH1D* hncpumatched;
  std::map<unsigned int, DetHisto> histomap_;
};

int main() {
  DetAnalyzer dt(0);
  //dt.getTree("input/gpuNew_input.root", 1);
  dt.getTree("input/legacy_input_generic.root", 1);
  dt.getTree("input/legacyNew_input.root", 0);
  dt.Loop();
  dt.writeHistos();
}
