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
  TH2D* hdigiadc;
  TH2D* hdigirow;
  TH2D* hdigicol;
  TH2D* hndigis;
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
    //digis
    hdigiadc = new TH2D("compadc" + tag, "Digi adc;gpu;cpu", 300, 0, 300, 300, 0, 300); 
    hdigirow = new TH2D("comprow" + tag, "Digi row;gpu;cpu", 200, 0, 200, 200, 0, 200);
    hdigicol = new TH2D("compcolumn" + tag, "Digi column;gpu;cpu", 300, 0, 300, 300, 0, 300);
    hndigis = new TH2D("compndigis" + tag, "#Digis;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
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
    //digis
    hdigiadc = new TH2D("compadc", "Digi adc;gpu;cpu", 300, 0, 300, 300, 0, 300); 
    hdigirow = new TH2D("comprow", "Digi row;gpu;cpu", 200, 0, 200, 200, 0, 200);
    hdigicol = new TH2D("compcolumn", "Digi column;gpu;cpu", 300, 0, 300, 300, 0, 300);
    hndigis = new TH2D("compndigis", "#Digis;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    hdigiADC_gpu = new TH2D("compdigiADC_gpu", "Digi ADC GPU;row;col", 200, 0, 200, 300, 0, 300);
    hdigiADC_cpu = new TH2D("compdigiADC_cpu", "Digi ADC Legacy;row;col", 200, 0, 200, 300, 0, 300);
    //Digis from GPU wf whcih are unmatched
    hdigiadc_gUM = new TH1D("gpuadcUM", "Unmateched GPU Digi adc;;", 300, 0, 300); 
    hdigirow_gUM = new TH1D("gpuprowUM", "Unmatched GPU Digi row;", 200, 0, 200);
    hdigicol_gUM = new TH1D("gpucolumnUM", "Unmateched GPU Digi column;", 300, 0, 300);
    //Digis from CPU wf whcih are unmatched
    hdigiadc_cUM = new TH1D("cpuadcUM", "Unmateched Legacy Digi adc;;", 300, 0, 300); 
    hdigirow_cUM = new TH1D("cpuprowUM", "Unmatched Legacy Digi row;", 200, 0, 200);
    hdigicol_cUM = new TH1D("cpucolumnUM", "Unmateched Legacy Digi column;", 300, 0, 300);

  }
   
  void fillDetHistos(DetInfo& gpuinfo, DetInfo& cpuinfo, unsigned int rawid, unsigned int jentry) {
    //Fill number of objects  histos
    hndigis->Fill(gpuinfo.ndigi, cpuinfo.ndigi);
    if(fillHistosFordet_) { 
      histomap_[rawid].hndigis->Fill(gpuinfo.ndigi, cpuinfo.ndigi);
    }
    
    std::vector<bool> isCPUdigimathed(cpuinfo.ndigi, false);
    for(unsigned int gi = 0; gi < gpuinfo.ndigi; gi++) {
          auto g_row = gpuinfo.digi_row[gi];
          auto g_col = gpuinfo.digi_col[gi];
          auto g_adc = gpuinfo.digi_adc[gi];
	  hdigiADC_gpu->Fill(g_row,g_col,g_adc);
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

    for(unsigned int ci = 0; ci < cpuinfo.ndigi; ci++) {
            auto c_row = cpuinfo.digi_row[ci];
            auto c_col = cpuinfo.digi_col[ci];
            auto c_adc = cpuinfo.digi_adc[ci];
      hdigiADC_cpu->Fill(c_row, c_col, c_adc);  
    }

    bool eventhasUM = false;
    //Fill histos for unmatched legacy digis
    for(unsigned int ci = 0; ci <  isCPUdigimathed.size(); ci ++) {
      if(isCPUdigimathed[ci])  continue;
      eventhasUM = true;
      hdigiadc_cUM->Fill(cpuinfo.digi_adc[ci]);
      hdigirow_cUM->Fill(cpuinfo.digi_row[ci]);
      hdigicol_cUM->Fill(cpuinfo.digi_col[ci]);
    }
    /*
    if(eventhasUM) {
     // std::cout << "Event has unmatched legacy clusters\nLegacy DIGI dump\n";
     // std::cout << "ROW:COL:ADC:ISMATCHED\n";
      for(unsigned int ci = 0; ci <  isCPUdigimathed.size(); ci ++) 
      //  std::cout << cpuinfo.digi_row[ci] << ":" << cpuinfo.digi_col[ci] << ":" << cpuinfo.digi_adc[ci] << ":" << isCPUdigimathed[ci] << std::endl;
      //std::cout << "GPU DIGI dump\nROW:COL:ADC\n";      
      for(unsigned int gi = 0; gi < gpuinfo.ndigi; gi++) 
        //std::cout << gpuinfo.digi_row[gi] << ":" << gpuinfo.digi_col[gi] << ":" << gpuinfo.digi_adc[gi] << std::endl;        
        //std::cout << "DUMP END" << std::endl;
    }*/
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
    TFile* fout = TFile::Open("compDigi.root", "recreate");
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
    hdigiADC_gpu->Write(); 
    hdigiADC_cpu->Write(); 

    fout->cd();
    if(fillHistosFordet_) {
    for(auto& hd : histomap_) {
      TString fn(hd.second.folderName.c_str());
      fout->mkdir(fn);
      fout->cd(fn);
      //digis
      hd.second.hdigiadc->Write();
      hd.second.hdigirow->Write();
      hd.second.hdigicol->Write();
      hd.second.hndigis->Write();
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
  TH2D* hdigiADC_gpu; 
  TH2D* hdigiADC_cpu; 

  std::map<unsigned int, DetHisto> histomap_;
};

int main() {
  DetAnalyzer dt(0);
  dt.getTree("input/gpuNew_input.root", 1);
  dt.getTree("input/legacyNew_input.root", 0);
  dt.Loop();
  dt.writeHistos();
}
