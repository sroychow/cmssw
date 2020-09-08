//#include "deInfo.h"
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
  TH2D* hclscharge;
  TH2D* hclsSizex;
  TH2D* hclsSizey; 
  TH2D* hncls;

  TH2D* hdigiadc;
  TH2D* hdigirow;
  TH2D* hdigicol;
  TH2D* hndigis;

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
    //clusters
    hclscharge = new TH2D("compcharge" + tag, "Cluster charge;gpu;cpu", 200, 0, 200e3, 200, 0, 200e3);      
    hclsSizex = new TH2D("compclswx" + tag, "Cluster size X;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hclsSizey = new TH2D("compclswy" + tag, "Cluster size Y;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hncls = new TH2D("compnclusters" + tag, "#clusters;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    //digis
    hdigiadc = new TH2D("compadc" + tag, "Digi adc;gpu;cpu", 300, 0, 300, 300, 0, 300); 
    hdigirow = new TH2D("comprow" + tag, "Digi row;gpu;cpu", 200, 0, 200, 200, 0, 200);
    hdigicol = new TH2D("compcolumn" + tag, "Digi column;gpu;cpu", 300, 0, 300, 300, 0, 300);
    hndigis = new TH2D("compndigis" + tag, "#Digis;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
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
      gpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldigidetId/detinfo"));
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
  
  //Function to compare 2 digis
  bool compareDigi(int g_row, int g_col, int c_row, int c_col) {
    if(g_row == c_row && g_col == c_col)  return true;
    return false;
  }
  
  void bookGlobalHistos() {
    //clusters
    hclscharge = new TH2D("compcharge", "Cluster charge;gpu;cpu", 200, 0, 200e3, 200, 0, 200e3);      
    hclsSizex = new TH2D("compclswx", "Cluster size X;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hclsSizey = new TH2D("compclswy", "Cluster size Y;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hncls = new TH2D("compnclusters", "#clusters;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    //digis
    hdigiadc = new TH2D("compadc", "Digi adc;gpu;cpu", 300, 0, 300, 300, 0, 300); 
    hdigirow = new TH2D("comprow", "Digi row;gpu;cpu", 200, 0, 200, 200, 0, 200);
    hdigicol = new TH2D("compcolumn", "Digi column;gpu;cpu", 300, 0, 300, 300, 0, 300);
    hndigis = new TH2D("compndigis", "#Digis;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    //Digis from GPU wf whcih are unmatched
    hdigiadc_gUM = new TH1D("gpuadcUM", "Unmateched GPU Digi adc;;", 300, 0, 300); 
    hdigirow_gUM = new TH1D("gpuprowUM", "Unmatched GPU Digi row;", 200, 0, 200);
    hdigicol_gUM = new TH1D("gpucolumnUM", "Unmateched GPU Digi column;", 300, 0, 300);
    //Digis from CPU wf whcih are unmatched
    hdigiadc_cUM = new TH1D("cpuadcUM", "Unmateched Legacy Digi adc;;", 300, 0, 300); 
    hdigirow_cUM = new TH1D("cpuprowUM", "Unmatched Legacy Digi row;", 200, 0, 200);
    hdigicol_cUM = new TH1D("cpucolumnUM", "Unmateched Legacy Digi column;", 300, 0, 300);

    //rechits
    hnrechits = new TH2D("compnrechits", "#RecHits;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    hrechitxRes = new TH1D("comprechitXres", "RecHit local x diff;x_{gpu} - x_{cpu};", 1000, -0.0001, 0.0001);
    hrechityRes = new TH1D("comprechitYRes", "RecHit local y diff;y_{gpu} - y_{cpu}", 1000, -0.0001, 0.0001);
    hrechitxerrRes = new TH1D("comprechitXerrRes", "RecHit local x error diff;xerr_{gpu} - xerr_{cpu};", 1000, -0.0001, 0.0001);
    hrechityerrRes = new TH1D("comprechitYerrRes", "RecHit local y error diff;yerr_{gpu} - yerr_{cpu};", 1000, -0.0001, 0.0001);
  }
   
  void fillDetHistos(DetInfo& gpuinfo, DetInfo& cpuinfo, unsigned int rawid, unsigned int jentry) {
    //Fill number of objects  histos
    hncls->Fill(gpuinfo.ncluster, cpuinfo.ncluster);
    hndigis->Fill(gpuinfo.ndigi, cpuinfo.ndigi);
    hnrechits->Fill(gpuinfo.nrechit, cpuinfo.nrechit);
    if(fillHistosFordet_) { 
      histomap_[rawid].hnrechits->Fill(gpuinfo.nrechit, cpuinfo.nrechit);
      histomap_[rawid].hndigis->Fill(gpuinfo.ndigi, cpuinfo.ndigi);
      histomap_[rawid].hncls->Fill(gpuinfo.ncluster, cpuinfo.ncluster);
    }
    for(unsigned int gi = 0; gi < gpuinfo.ncluster; gi++) {
      auto g_charge = gpuinfo.cluster_charge[gi];
      auto g_sizeX = gpuinfo.cluster_sizeX[gi];
      auto g_sizeY = gpuinfo.cluster_sizeY[gi];
      auto g_x = gpuinfo.cluster_x[gi];
      auto g_y = gpuinfo.cluster_y[gi];
      auto g_minPixelRow = gpuinfo.cluster_minPixelRow[gi];
      auto g_maxPixelRow = gpuinfo.cluster_maxPixelRow[gi];
      auto g_minPixelCol = gpuinfo.cluster_minPixelCol[gi];
      auto g_maxPixelCol = gpuinfo.cluster_maxPixelCol[gi];
      bool matched = false;
      for(unsigned int ci = 0; ci < cpuinfo.ncluster; ci++) {
        auto c_charge = cpuinfo.cluster_charge[ci];
 	auto c_sizeX = cpuinfo.cluster_sizeX[ci];
 	auto c_sizeY = cpuinfo.cluster_sizeY[ci];
 	auto c_x = cpuinfo.cluster_x[ci];
 	auto c_y = cpuinfo.cluster_y[ci];
 	auto c_minPixelRow = cpuinfo.cluster_minPixelRow[ci];
 	auto c_maxPixelRow = cpuinfo.cluster_maxPixelRow[ci];
 	auto c_minPixelCol = cpuinfo.cluster_minPixelCol[ci];
 	auto c_maxPixelCol = cpuinfo.cluster_maxPixelCol[ci];
	//define some matching condition
        if(fillHistosFordet_) {
	  histomap_[rawid].hclscharge->Fill(g_charge, c_charge);
	  histomap_[rawid].hclsSizex->Fill(g_sizeX, c_sizeX);
          histomap_[rawid].hclsSizey->Fill(g_sizeY, c_sizeY);
        }
	hclscharge->Fill(g_charge, c_charge);
        hclsSizex->Fill(g_sizeX, c_sizeX);
        hclsSizey->Fill(g_sizeX, c_sizeY);

      }
    }
    //DIGI comparison

    
    std::vector<bool> isCPUdigimathed(cpuinfo.ndigi, false);

    for(unsigned int gi = 0; gi < gpuinfo.ndigi; gi++) {
          auto g_row = gpuinfo.digi_row[gi];
          auto g_col = gpuinfo.digi_col[gi];
          auto g_adc = gpuinfo.digi_adc[gi];
	  bool matched = false;
          for(unsigned int ci = 0; ci < cpuinfo.ndigi; ci++) {
            auto c_row = cpuinfo.digi_row[ci];
            auto c_col = cpuinfo.digi_col[ci];
            auto c_adc = cpuinfo.digi_adc[ci];
            if(compareDigi(g_row, g_col, c_row, c_col) ) {
	      hdigiadc->Fill(g_adc, c_adc);
	      hdigicol->Fill(g_col, c_col);
	      hdigirow->Fill(g_row, c_row);
              if(fillHistosFordet_) {
	        histomap_[rawid].hdigiadc->Fill(g_adc, c_adc);
	        histomap_[rawid].hdigirow->Fill(g_row, c_row);
	        histomap_[rawid].hdigicol->Fill(g_col, c_col);
              }
	      matched=true;
              isCPUdigimathed[ci] = true;
	      //break;
	    }      
          }//loop over cpu digis
	  if(!matched) {
	    hdigiadc_gUM->Fill(g_adc);
	    hdigirow_gUM->Fill(g_row);
	    hdigicol_gUM->Fill(g_col);
	  }
    }//end loop over gpu digis
    //Fill histos for unmatched legacy digis
    for(unsigned int ci = 0; ci <  isCPUdigimathed.size(); ci ++) {
      if(isCPUdigimathed[ci])  continue;
      hdigiadc_cUM->Fill(cpuinfo.digi_adc[ci]);
      hdigirow_cUM->Fill(cpuinfo.digi_row[ci]);
      hdigicol_cUM->Fill(cpuinfo.digi_col[ci]);
    }
    
    //if(jentry==724) std::cout << "#nrechits gpu:" << gpuinfo.nrechit << "\t gpu=" << gpuinfo.rechit_x.size() << std::endl;
    for(unsigned int gi = 0; gi < gpuinfo.rechit_x.size(); gi++) {
      double g_x = gpuinfo.rechit_x[gi];
      double g_y = gpuinfo.rechit_y[gi];
      double g_xerr = gpuinfo.rechit_xerror[gi];
      double g_yerr = gpuinfo.rechit_yerror[gi];
      for(unsigned int ci = 0; ci <  cpuinfo.rechit_x.size(); ci++) {
        double c_x = cpuinfo.rechit_x[ci];
        double c_y = cpuinfo.rechit_y[ci];
        double c_xerr = cpuinfo.rechit_xerror[ci];
        double c_yerr = cpuinfo.rechit_yerror[ci];
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
    TFile* fout = TFile::Open("comparison.root", "recreate");
    fout->mkdir("cluster");
    fout->cd("cluster");
    hclscharge->Write();
    hclsSizex->Write();
    hclsSizey->Write();
    hncls->Write();
    fout->mkdir("digi");
    fout->cd("digi");
    hdigiadc->Write();
    hdigirow->Write();
    hdigicol->Write();
    hndigis->Write();
    hdigiadc_gUM->Write();
    hdigirow_gUM->Write();
    hdigicol_gUM->Write();
    hdigiadc_cUM->Write();
    hdigirow_cUM->Write();
    hdigicol_cUM->Write();

    fout->mkdir("rechit");
    fout->cd("rechit");
    hnrechits->Write();
    hrechitxRes->Write();
    hrechityRes->Write();
    hrechitxerrRes->Write();
    hrechityerrRes->Write();
    fout->cd();
    if(fillHistosFordet_) {
    for(auto& hd : histomap_) {
      TString fn(hd.second.folderName.c_str());
      fout->mkdir(fn);
      fout->cd(fn);
      hd.second.hclscharge->Write();
      hd.second.hclsSizex->Write();
      hd.second.hclsSizey->Write();
      hd.second.hncls->Write();
      //digis
      hd.second.hdigiadc->Write();
      hd.second.hdigirow->Write();
      hd.second.hdigicol->Write();
      hd.second.hndigis->Write();
      //rechits->Write();
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

  TH2D* hclscharge;
  TH2D* hclsSizex;
  TH2D* hclsSizey; 
  TH2D* hncls;

  TH2D* hdigiadc;
  TH2D* hdigirow;
  TH2D* hdigicol;
  TH2D* hndigis;
  TH1D* hdigiadc_gUM;
  TH1D* hdigirow_gUM;
  TH1D* hdigicol_gUM;
  TH1D* hdigiadc_cUM;
  TH1D* hdigirow_cUM;
  TH1D* hdigicol_cUM;

  TH2D* hnrechits;
  TH1D* hrechitxRes;
  TH1D* hrechityRes;
  TH1D* hrechitxerrRes;
  TH1D* hrechityerrRes;

  std::map<unsigned int, DetHisto> histomap_;
};

int main() {
  DetAnalyzer dt(0);
  dt.getTree("gpu_input.root", 1);
  dt.getTree("legacy_input.root", 0);
  dt.Loop();
  dt.writeHistos();
}
