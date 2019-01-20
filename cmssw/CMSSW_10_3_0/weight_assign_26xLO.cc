// -*- C++ -*-
//
// Package:    Analyzer/weight_assign_26xLO
// Class:      weight_assign_26xLO
// 
/**\class weight_assign_26xLO weight_assign_26xLO.cc Analyzer/weight_assign_26xLO/plugins/weight_assign_26xLO.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  JunHo Choi
//         Created:  Fri, 23 Mar 2018 03:24:18 GMT
//
//

//vector<reco::GenParticle>             "prunedGenParticles"        ""                "PAT"     

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"


using namespace edm;
using namespace reco;
using namespace std;

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Run.h"//to use edm::Run


#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include "GEN_Analyzer/JHanalyzer/interface/weightinfo.h"

//
// Class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.



//class weight_assign_26xLO : public edm::one::EDAnalyzer<edm::one::SharedResources>  {

class weight_assign_26xLO : public edm::one::EDAnalyzer<edm::one::WatchRuns,edm::one::SharedResources>  {
//class weight_assign_26xLO : public edm::EDAnalyzer  {
   public:
      explicit weight_assign_26xLO(const edm::ParameterSet&);
      ~weight_assign_26xLO();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
 
  //  virtual void beginJob(edm::Run const& iEvent, edm::EventSetup const &) override;
  //  void beginRun(edm::Run const&, edm::EventSetup const&) override;//to get LHERunInfoProduct//add new method
  //void endRun(edm::Run const&, edm::EventSetup const&) override;
private:
  virtual void beginJob() override;
  //  virtual void beginJob(edm::Run const& iRun) override;
  //  virtual void beginJob(edm::Run const& iRun, edm::EventSetup const &iSetup) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  //  virtual void doBeginRun_(edm::Run const&, edm::EventSetup const&) override;
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginRun(edm::Run const& iEvent, edm::EventSetup const&) override;
  //virtual void beginRun(edm::Run const& iRun, edm::EventSetup const &iSetup ) override;//to get LHERunInfoProduct//add new method
  //virtual void beginRun() override;//to get LHERunInfoProduct//add new method
  //GenEventInfoProduct                   "generator"                 ""                "SIM"   

//  void beginRun(edm::Run const& iEvent, edm::EventSetup const&) override;

  edm::EDGetTokenT<GenParticleCollection> genParticles_Token;
  edm::EDGetTokenT<GenEventInfoProduct> genInfo_Token;
  edm::EDGetTokenT<LHEEventProduct> LHEInfo_Token;
  //  edm::EDGetTokenT<LHERunInfoProduct> extLHEInfo_Token; 
  void make_weight_infos();
  

  std::vector<weightinfo> LO26xInfo;



  TTree *DYkinematics;


  double Z_pt,Z_mass,Z_eta,Z_phi;
  double lep1_pt,lep1_eta,lep1_phi;
  double lep2_pt,lep2_eta,lep2_phi;
  double dilep_pt,dilep_mass,dilep_eta,dilep_phi;


  TTree *DYweights;

      // ----------member data ---------------------------
};
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
weight_assign_26xLO::weight_assign_26xLO(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
 
  //vector<reco::GenParticle>             "genParticles"              ""                "SIM"     

  usesResource("TFileService");
  genParticles_Token = consumes<GenParticleCollection>(edm::InputTag("genParticles"));
  genInfo_Token = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  LHEInfo_Token = consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
  //extLHEInfo_Token = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));  
  //  extLHEInfo_Token= consumes<LHERunInfoProduct,edm::InRun>(edm::InputTag("externalLHEProducer",""));
 
 //Let's make vector for find weights!//
  make_weight_infos();
 

}


weight_assign_26xLO::~weight_assign_26xLO()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
weight_assign_26xLO::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
 
  Z_pt=0;Z_mass=0;Z_eta=0;Z_phi=0;
  lep1_pt=0;lep1_eta=0;lep1_phi=0;
  lep2_pt=0;lep2_eta=0;lep2_phi=0;
  dilep_pt=0;dilep_mass=0;dilep_eta=0;dilep_phi=0;


  edm::Handle<LHEEventProduct> LHEInfo;
  iEvent.getByToken(LHEInfo_Token, LHEInfo);

  //veto tau events//                             
  const lhef::HEPEUP& lheEvent = LHEInfo->hepeup();
  std::vector<lhef::HEPEUP::FiveVector> lheParticles = lheEvent.PUP;
  Int_t nLHEParticle = lheParticles.size();
  for( Int_t idxParticle = 0; idxParticle < nLHEParticle; ++idxParticle ){

    Int_t id = lheEvent.IDUP[idxParticle];
    if(fabs(id)==15) return;
  }
  //////////end of veto tau/////

  ////////////initialize/////////////
  //Get weight//


  int lheinfoweightsize= LHEInfo->weights().size();
  int lheinfocommentssize = LHEInfo->comments_size();

  double w0=LHEInfo->originalXWGTUP();
  int LO26xInfosize=LO26xInfo.size();
  //  for (int i =0; i < lheinfoweightsize; i++){
    //cout<< LHEInfo->weights()[i].id<<endl;
    //    cout<< LHEInfo->weights()[i].wgt/w0<<endl;
  //}

  for (int i26x =0; i26x < LO26xInfosize;i26x++){
    for (int i_lhe =0; i_lhe < lheinfoweightsize; i_lhe++){ 
      if(LHEInfo->weights()[i_lhe].id==LO26xInfo[i26x].id()){
	LO26xInfo[i26x].set_weight(LHEInfo->weights()[i_lhe].wgt/w0);
      }
    }
  }

  //for (int i26x =0; i26x < LO26xInfosize;i26x++){
  //  for (int i26x =0; i26x < 9;i26x++){
  //     cout<<LO26xInfo[i26x].id()<<"=>"<<    LO26xInfo[i26x].weight()<<endl;
    //   cout<<    LO26xInfo[i26x].id()<<endl;
  //}
  //  for (int i =0; i < lheinfocommentssize; i++){
    //    cout<<"comment i ="<<i<<"=" << LHEInfo->getComment(i)<<endl;
  //} 


  //weight id info

  //  edm::Handle<LHERunInfoProduct> run;
 
  //  iEvent.getByToken( extLHEInfo_Token, run );
  //iEvent.getByLabel( "externalLHEProducer", run );
  // typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
  //  LHERunInfoProduct myLHERunInfoProduct = *(run.product());



  int leppid=11;

   using namespace edm;
   Handle<reco::GenParticleCollection> genParticles;
   iEvent.getByToken(genParticles_Token, genParticles);//genParticle                                                                                         
   edm::Handle<GenEventInfoProduct> genInfo;
   iEvent.getByToken(genInfo_Token, genInfo);
   //GenEventInfoProduct                   "generator"                 ""                "SIM"   //
   double weight=1;
   weight=genInfo->weight();
   if(weight<-99) cout<<weight<<endl;
   //   cout<<"weight="<<weight<<endl;
   vector<int> i_leptons1;
   vector<int> i_leptons2;
   vector<int> i_photons;

   int gensize= genParticles->size();
   
   for(int i = 0; i < gensize; ++ i) {///scan all gen particles
     //tau veto   
     const GenParticle & p = (*genParticles)[i];
     int id = p.pdgId();
     int status = p.status();
     if(status!=1) continue;
     //double px = p.px();
     ///double py = p.py();
     //double pz = p.pz();
     //double ee = p.energy();
     if(id==leppid) i_leptons1.push_back(i);
     else if(id== -leppid) i_leptons2.push_back(i);
     else if (id==22)  i_photons.push_back(i);
   }
   if(i_leptons1.size()<1) return;//require exact 2 leptons
   if(i_leptons2.size()<1) return;//require exact 2 leptons
   int i_lep1=i_leptons1[0];
   int i_lep2=i_leptons2[0];
   //Set 1st lep1 and lep2 as DY leptons(simply..)
   //The same as DY validation code :https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/Validation/EventGenerator/plugins/DrellYanValidation.cc

   TLorentzVector v1,v2;
   v1.SetPxPyPzE((*genParticles)[i_lep1].px(), (*genParticles)[i_lep1].py(),(*genParticles)[i_lep1].pz(),(*genParticles)[i_lep1].energy());
   v2.SetPxPyPzE((*genParticles)[i_lep2].px(), (*genParticles)[i_lep2].py(),(*genParticles)[i_lep2].pz(),(*genParticles)[i_lep2].energy());

   if((v1+v2).M()< 60 ) return;
   
   TLorentzVector vdilep=v1+v2;
   TLorentzVector vZ=v1+v2;
   //vector<int> i_fsr;
   //photons for dressed lepton//
   int photonsize=i_photons.size();
   for(int i =0 ; i < photonsize; i++){
     const GenParticle & p = (*genParticles)[i];
     double px = p.px();
     double py = p.py();
     double pz = p.pz();                                                                                                                     
     double ee = p.energy();                                                                                                                 
     TLorentzVector vfsr;
     vfsr.SetPxPyPzE(px,py,pz,ee);
     double R1=vfsr.DeltaR(v1);
     double R2=vfsr.DeltaR(v2);
     if(R1<0.1 || R2<0.1){ //i_fsr.push_back(i);
       vZ+=vfsr;
     }
   }
   

   Z_pt=vZ.Perp(); Z_mass=vZ.M(); Z_eta=vZ.Eta(); Z_phi=vZ.Phi();
   lep1_pt=v1.Perp();lep1_eta=v1.Eta();lep1_phi=v1.Phi();
   lep2_pt=v2.Perp();lep2_eta=v2.Eta();lep2_phi=v2.Phi();
   dilep_pt=vdilep.Perp(); dilep_mass=vdilep.M(); dilep_eta=vdilep.Eta(); dilep_phi=vdilep.Phi();
   
   DYkinematics->Fill();
   DYweights->Fill();
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   //Handle<ExampleData> pIn;
   // iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   // ESHandle<SetupData> pSetup;
   //iSetup.get<SetupRecord>().get(pSetup);
#endif
}





// ------------ method called once each job just before starting event loop  ------------

void 
weight_assign_26xLO::beginJob()
//weight_assign_26xLO::beginJob(edm::Run const& iRun)
//weight_assign_26xLO::beginJob(edm::Run const& iRun, edm::EventSetup const &iSetup)
{
  cout<<"begin job"<<endl;
 
  edm::Service<TFileService> fs;
  DYkinematics=fs->make<TTree>("DYkinematics","DYkinematics");
  DYkinematics->Branch("Z_pt",&Z_pt,"Z_pt/D");
  DYkinematics->Branch("Z_mass",&Z_mass,"Z_mass/D");
  DYkinematics->Branch("Z_eta",&Z_eta,"Z_eta/D");
  DYkinematics->Branch("Z_phi",&Z_phi,"Z_phi/D");

  DYkinematics->Branch("lep1_pt",&lep1_pt,"lep1_pt/D");
  DYkinematics->Branch("lep1_eta",&lep1_eta,"lep1_eta/D");
  DYkinematics->Branch("lep1_phi",&lep1_phi,"lep1_phi/D");

  DYkinematics->Branch("lep2_pt",&lep2_pt,"lep2_pt/D");
  DYkinematics->Branch("lep2_eta",&lep2_eta,"lep2_eta/D");
  DYkinematics->Branch("lep2_phi",&lep2_phi,"lep2_phi/D");

  DYkinematics->Branch("dilep_pt",&dilep_pt,"dilep_pt/D");
  DYkinematics->Branch("dilep_mass",&dilep_mass,"dilep_mass/D");
  DYkinematics->Branch("dilep_eta",&dilep_eta,"dilep_eta/D");
  DYkinematics->Branch("dilep_phi",&dilep_phi,"dilep_phi/D");

  DYweights=fs->make<TTree>("DYweights","DYweights");
  for(unsigned int i = 0;i <LO26xInfo.size();i++){ 
  // DYweights->Branch();
    DYweights->Branch(LO26xInfo[i].name()+"_"+LO26xInfo[i].pdf(),&LO26xInfo[i]._weight,LO26xInfo[i].name()+"_"+LO26xInfo[i].pdf()+"/D");
  }
  cout<<"end of beginjob"<<endl;
}

// ------------ method called once each job just after ending the event loop  ------------

void 
weight_assign_26xLO::endJob() 
{
  cout<<"endjob"<<endl;
}



void 
weight_assign_26xLO::beginRun(const Run &iEvent, EventSetup const &iSetup ){
  cout<<" beginrun"<<endl;
  cout<<"end of beginrun"<<endl;
}



void                                                                                                                                                                              
weight_assign_26xLO::endRun(edm::Run const& iEvent, edm::EventSetup const&) 
{
  cout<<"doendrun"<<endl;

}
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------


void
weight_assign_26xLO::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//////defined by jhchoi/////
void 
weight_assign_26xLO::make_weight_infos(){
  cout<<"make_weight_infos"<<endl;

///LO26x///
LO26xInfo.push_back(weightinfo("1001", "247000", "muR_1.0_muF_1.0", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1006", "247000", "muR_2.0_muF_1.0", 2.0, 1.0));
LO26xInfo.push_back(weightinfo("1011", "247000", "muR_0.5_muF_1.0", 0.5, 1.0));
LO26xInfo.push_back(weightinfo("1016", "247000", "muR_1.0_muF_2.0", 1.0, 2.0));
LO26xInfo.push_back(weightinfo("1021", "247000", "muR_2.0_muF_2.0", 2.0, 2.0));
LO26xInfo.push_back(weightinfo("1026", "247000", "muR_0.5_muF_2.0", 0.5, 2.0));
LO26xInfo.push_back(weightinfo("1031", "247000", "muR_1.0_muF_0.5", 1.0, 0.5));
LO26xInfo.push_back(weightinfo("1036", "247000", "muR_2.0_muF_0.5", 2.0, 0.5));
LO26xInfo.push_back(weightinfo("1041", "247000", "muR_0.5_muF_0.5", 0.5, 0.5));
LO26xInfo.push_back(weightinfo("1046", "306000", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1047", "306001", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1048", "306002", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1049", "306003", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1050", "306004", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1051", "306005", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1052", "306006", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1053", "306007", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1054", "306008", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1055", "306009", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1056", "306010", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1057", "306011", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1058", "306012", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1059", "306013", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1060", "306014", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1061", "306015", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1062", "306016", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1063", "306017", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1064", "306018", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1065", "306019", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1066", "306020", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1067", "306021", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1068", "306022", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1069", "306023", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1070", "306024", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1071", "306025", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1072", "306026", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1073", "306027", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1074", "306028", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1075", "306029", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1076", "306030", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1077", "306031", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1078", "306032", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1079", "306033", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1080", "306034", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1081", "306035", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1082", "306036", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1083", "306037", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1084", "306038", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1085", "306039", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1086", "306040", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1087", "306041", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1088", "306042", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1089", "306043", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1090", "306044", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1091", "306045", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1092", "306046", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1093", "306047", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1094", "306048", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1095", "306049", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1096", "306050", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1097", "306051", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1098", "306052", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1099", "306053", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1100", "306054", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1101", "306055", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1102", "306056", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1103", "306057", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1104", "306058", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1105", "306059", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1106", "306060", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1107", "306061", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1108", "306062", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1109", "306063", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1110", "306064", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1111", "306065", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1112", "306066", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1113", "306067", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1114", "306068", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1115", "306069", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1116", "306070", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1117", "306071", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1118", "306072", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1119", "306073", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1120", "306074", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1121", "306075", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1122", "306076", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1123", "306077", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1124", "306078", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1125", "306079", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1126", "306080", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1127", "306081", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1128", "306082", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1129", "306083", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1130", "306084", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1131", "306085", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1132", "306086", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1133", "306087", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1134", "306088", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1135", "306089", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1136", "306090", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1137", "306091", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1138", "306092", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1139", "306093", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1140", "306094", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1141", "306095", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1142", "306096", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1143", "306097", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1144", "306098", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1145", "306099", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1146", "306100", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1147", "306101", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1148", "306102", "NNPDF31_nnlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1149", "322500", "NNPDF31_nnlo_as_0108", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1150", "322700", "NNPDF31_nnlo_as_0110", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1151", "322900", "NNPDF31_nnlo_as_0112", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1152", "323100", "NNPDF31_nnlo_as_0114", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1153", "323300", "NNPDF31_nnlo_as_0117", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1154", "323500", "NNPDF31_nnlo_as_0119", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1155", "323700", "NNPDF31_nnlo_as_0122", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1156", "323900", "NNPDF31_nnlo_as_0124", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1157", "305800", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1158", "305801", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1159", "305802", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1160", "305803", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1161", "305804", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1162", "305805", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1163", "305806", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1164", "305807", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1165", "305808", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1166", "305809", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1167", "305810", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1168", "305811", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1169", "305812", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1170", "305813", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1171", "305814", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1172", "305815", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1173", "305816", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1174", "305817", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1175", "305818", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1176", "305819", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1177", "305820", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1178", "305821", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1179", "305822", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1180", "305823", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1181", "305824", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1182", "305825", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1183", "305826", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1184", "305827", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1185", "305828", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1186", "305829", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1187", "305830", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1188", "305831", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1189", "305832", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1190", "305833", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1191", "305834", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1192", "305835", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1193", "305836", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1194", "305837", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1195", "305838", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1196", "305839", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1197", "305840", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1198", "305841", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1199", "305842", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1200", "305843", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1201", "305844", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1202", "305845", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1203", "305846", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1204", "305847", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1205", "305848", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1206", "305849", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1207", "305850", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1208", "305851", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1209", "305852", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1210", "305853", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1211", "305854", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1212", "305855", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1213", "305856", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1214", "305857", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1215", "305858", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1216", "305859", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1217", "305860", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1218", "305861", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1219", "305862", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1220", "305863", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1221", "305864", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1222", "305865", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1223", "305866", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1224", "305867", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1225", "305868", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1226", "305869", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1227", "305870", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1228", "305871", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1229", "305872", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1230", "305873", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1231", "305874", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1232", "305875", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1233", "305876", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1234", "305877", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1235", "305878", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1236", "305879", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1237", "305880", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1238", "305881", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1239", "305882", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1240", "305883", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1241", "305884", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1242", "305885", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1243", "305886", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1244", "305887", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1245", "305888", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1246", "305889", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1247", "305890", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1248", "305891", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1249", "305892", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1250", "305893", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1251", "305894", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1252", "305895", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1253", "305896", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1254", "305897", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1255", "305898", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1256", "305899", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1257", "305900", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1258", "305901", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1259", "305902", "NNPDF31_nlo_hessian_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1260", "13000", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1261", "13001", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1262", "13002", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1263", "13003", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1264", "13004", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1265", "13005", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1266", "13006", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1267", "13007", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1268", "13008", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1269", "13009", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1270", "13010", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1271", "13011", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1272", "13012", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1273", "13013", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1274", "13014", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1275", "13015", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1276", "13016", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1277", "13017", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1278", "13018", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1279", "13019", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1280", "13020", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1281", "13021", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1282", "13022", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1283", "13023", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1284", "13024", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1285", "13025", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1286", "13026", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1287", "13027", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1288", "13028", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1289", "13029", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1290", "13030", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1291", "13031", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1292", "13032", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1293", "13033", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1294", "13034", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1295", "13035", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1296", "13036", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1297", "13037", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1298", "13038", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1299", "13039", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1300", "13040", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1301", "13041", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1302", "13042", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1303", "13043", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1304", "13044", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1305", "13045", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1306", "13046", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1307", "13047", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1308", "13048", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1309", "13049", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1310", "13050", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1311", "13051", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1312", "13052", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1313", "13053", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1314", "13054", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1315", "13055", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1316", "13056", "CT14nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1317", "13065", "CT14nnlo_as_0116", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1318", "13069", "CT14nnlo_as_0120", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1319", "13100", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1320", "13101", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1321", "13102", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1322", "13103", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1323", "13104", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1324", "13105", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1325", "13106", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1326", "13107", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1327", "13108", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1328", "13109", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1329", "13110", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1330", "13111", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1331", "13112", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1332", "13113", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1333", "13114", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1334", "13115", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1335", "13116", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1336", "13117", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1337", "13118", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1338", "13119", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1339", "13120", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1340", "13121", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1341", "13122", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1342", "13123", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1343", "13124", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1344", "13125", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1345", "13126", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1346", "13127", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1347", "13128", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1348", "13129", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1349", "13130", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1350", "13131", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1351", "13132", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1352", "13133", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1353", "13134", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1354", "13135", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1355", "13136", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1356", "13137", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1357", "13138", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1358", "13139", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1359", "13140", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1360", "13141", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1361", "13142", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1362", "13143", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1363", "13144", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1364", "13145", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1365", "13146", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1366", "13147", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1367", "13148", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1368", "13149", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1369", "13150", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1370", "13151", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1371", "13152", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1372", "13153", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1373", "13154", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1374", "13155", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1375", "13156", "CT14nlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1376", "13163", "CT14nlo_as_0116", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1377", "13167", "CT14nlo_as_0120", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1378", "13200", "CT14lo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1379", "25200", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1380", "25201", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1381", "25202", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1382", "25203", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1383", "25204", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1384", "25205", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1385", "25206", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1386", "25207", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1387", "25208", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1388", "25209", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1389", "25210", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1390", "25211", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1391", "25212", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1392", "25213", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1393", "25214", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1394", "25215", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1395", "25216", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1396", "25217", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1397", "25218", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1398", "25219", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1399", "25220", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1400", "25221", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1401", "25222", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1402", "25223", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1403", "25224", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1404", "25225", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1405", "25226", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1406", "25227", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1407", "25228", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1408", "25229", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1409", "25230", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1410", "25231", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1411", "25232", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1412", "25233", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1413", "25234", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1414", "25235", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1415", "25236", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1416", "25237", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1417", "25238", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1418", "25239", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1419", "25240", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1420", "25241", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1421", "25242", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1422", "25243", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1423", "25244", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1424", "25245", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1425", "25246", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1426", "25247", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1427", "25248", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1428", "25249", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1429", "25250", "MMHT2014nlo68clas118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1430", "25300", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1431", "25301", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1432", "25302", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1433", "25303", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1434", "25304", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1435", "25305", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1436", "25306", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1437", "25307", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1438", "25308", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1439", "25309", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1440", "25310", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1441", "25311", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1442", "25312", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1443", "25313", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1444", "25314", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1445", "25315", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1446", "25316", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1447", "25317", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1448", "25318", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1449", "25319", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1450", "25320", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1451", "25321", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1452", "25322", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1453", "25323", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1454", "25324", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1455", "25325", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1456", "25326", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1457", "25327", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1458", "25328", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1459", "25329", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1460", "25330", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1461", "25331", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1462", "25332", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1463", "25333", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1464", "25334", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1465", "25335", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1466", "25336", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1467", "25337", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1468", "25338", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1469", "25339", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1470", "25340", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1471", "25341", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1472", "25342", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1473", "25343", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1474", "25344", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1475", "25345", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1476", "25346", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1477", "25347", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1478", "25348", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1479", "25349", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1480", "25350", "MMHT2014nnlo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1481", "25000", "MMHT2014lo68cl", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1482", "42780", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1483", "42781", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1484", "42782", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1485", "42783", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1486", "42784", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1487", "42785", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1488", "42786", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1489", "42787", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1490", "42788", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1491", "42789", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1492", "42790", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1493", "42791", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1494", "42792", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1495", "42793", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1496", "42794", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1497", "42795", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1498", "42796", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1499", "42797", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1500", "42798", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1501", "42799", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1502", "42800", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1503", "42801", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1504", "42802", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1505", "42803", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1506", "42804", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1507", "42805", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1508", "42806", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1509", "42807", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1510", "42808", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1511", "42809", "ABMP16als118_5_nnlo", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1512", "90200", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1513", "90201", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1514", "90202", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1515", "90203", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1516", "90204", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1517", "90205", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1518", "90206", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1519", "90207", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1520", "90208", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1521", "90209", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1522", "90210", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1523", "90211", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1524", "90212", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1525", "90213", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1526", "90214", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1527", "90215", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1528", "90216", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1529", "90217", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1530", "90218", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1531", "90219", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1532", "90220", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1533", "90221", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1534", "90222", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1535", "90223", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1536", "90224", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1537", "90225", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1538", "90226", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1539", "90227", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1540", "90228", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1541", "90229", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1542", "90230", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1543", "90231", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1544", "90232", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1545", "90233", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1546", "90234", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1547", "90235", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1548", "90236", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1549", "90237", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1550", "90238", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1551", "90239", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1552", "90240", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1553", "90241", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1554", "90242", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1555", "90243", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1556", "90244", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1557", "90245", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1558", "90246", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1559", "90247", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1560", "90248", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1561", "90249", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1562", "90250", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1563", "90251", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1564", "90252", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1565", "90253", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1566", "90254", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1567", "90255", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1568", "90256", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1569", "90257", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1570", "90258", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1571", "90259", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1572", "90260", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1573", "90261", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1574", "90262", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1575", "90263", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1576", "90264", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1577", "90265", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1578", "90266", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1579", "90267", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1580", "90268", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1581", "90269", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1582", "90270", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1583", "90271", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1584", "90272", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1585", "90273", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1586", "90274", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1587", "90275", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1588", "90276", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1589", "90277", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1590", "90278", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1591", "90279", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1592", "90280", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1593", "90281", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1594", "90282", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1595", "90283", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1596", "90284", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1597", "90285", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1598", "90286", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1599", "90287", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1600", "90288", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1601", "90289", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1602", "90290", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1603", "90291", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1604", "90292", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1605", "90293", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1606", "90294", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1607", "90295", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1608", "90296", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1609", "90297", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1610", "90298", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1611", "90299", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1612", "90300", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1613", "90301", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1614", "90302", "PDF4LHC15_nlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1615", "91200", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1616", "91201", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1617", "91202", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1618", "91203", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1619", "91204", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1620", "91205", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1621", "91206", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1622", "91207", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1623", "91208", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1624", "91209", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1625", "91210", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1626", "91211", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1627", "91212", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1628", "91213", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1629", "91214", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1630", "91215", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1631", "91216", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1632", "91217", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1633", "91218", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1634", "91219", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1635", "91220", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1636", "91221", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1637", "91222", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1638", "91223", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1639", "91224", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1640", "91225", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1641", "91226", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1642", "91227", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1643", "91228", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1644", "91229", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1645", "91230", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1646", "91231", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1647", "91232", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1648", "91233", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1649", "91234", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1650", "91235", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1651", "91236", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1652", "91237", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1653", "91238", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1654", "91239", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1655", "91240", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1656", "91241", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1657", "91242", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1658", "91243", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1659", "91244", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1660", "91245", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1661", "91246", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1662", "91247", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1663", "91248", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1664", "91249", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1665", "91250", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1666", "91251", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1667", "91252", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1668", "91253", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1669", "91254", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1670", "91255", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1671", "91256", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1672", "91257", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1673", "91258", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1674", "91259", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1675", "91260", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1676", "91261", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1677", "91262", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1678", "91263", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1679", "91264", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1680", "91265", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1681", "91266", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1682", "91267", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1683", "91268", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1684", "91269", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1685", "91270", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1686", "91271", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1687", "91272", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1688", "91273", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1689", "91274", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1690", "91275", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1691", "91276", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1692", "91277", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1693", "91278", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1694", "91279", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1695", "91280", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1696", "91281", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1697", "91282", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1698", "91283", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1699", "91284", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1700", "91285", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1701", "91286", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1702", "91287", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1703", "91288", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1704", "91289", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1705", "91290", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1706", "91291", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1707", "91292", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1708", "91293", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1709", "91294", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1710", "91295", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1711", "91296", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1712", "91297", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1713", "91298", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1714", "91299", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1715", "91300", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1716", "91301", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1717", "91302", "PDF4LHC15_nnlo_100_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1718", "90400", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1719", "90401", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1720", "90402", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1721", "90403", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1722", "90404", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1723", "90405", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1724", "90406", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1725", "90407", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1726", "90408", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1727", "90409", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1728", "90410", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1729", "90411", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1730", "90412", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1731", "90413", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1732", "90414", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1733", "90415", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1734", "90416", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1735", "90417", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1736", "90418", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1737", "90419", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1738", "90420", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1739", "90421", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1740", "90422", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1741", "90423", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1742", "90424", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1743", "90425", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1744", "90426", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1745", "90427", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1746", "90428", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1747", "90429", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1748", "90430", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1749", "90431", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1750", "90432", "PDF4LHC15_nlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1751", "91400", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1752", "91401", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1753", "91402", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1754", "91403", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1755", "91404", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1756", "91405", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1757", "91406", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1758", "91407", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1759", "91408", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1760", "91409", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1761", "91410", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1762", "91411", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1763", "91412", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1764", "91413", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1765", "91414", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1766", "91415", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1767", "91416", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1768", "91417", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1769", "91418", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1770", "91419", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1771", "91420", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1772", "91421", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1773", "91422", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1774", "91423", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1775", "91424", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1776", "91425", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1777", "91426", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1778", "91427", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1779", "91428", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1780", "91429", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1781", "91430", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1782", "91431", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1783", "91432", "PDF4LHC15_nnlo_30_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1784", "61100", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1785", "61101", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1786", "61102", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1787", "61103", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1788", "61104", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1789", "61105", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1790", "61106", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1791", "61107", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1792", "61108", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1793", "61109", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1794", "61110", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1795", "61111", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1796", "61112", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1797", "61113", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1798", "61114", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1799", "61115", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1800", "61116", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1801", "61117", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1802", "61118", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1803", "61119", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1804", "61120", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1805", "61121", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1806", "61122", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1807", "61123", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1808", "61124", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1809", "61125", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1810", "61126", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1811", "61127", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1812", "61128", "HERAPDF20_NLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1813", "61130", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1814", "61131", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1815", "61132", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1816", "61133", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1817", "61134", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1818", "61135", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1819", "61136", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1820", "61137", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1821", "61138", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1822", "61139", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1823", "61140", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1824", "61141", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1825", "61142", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1826", "61143", "HERAPDF20_NLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1827", "61200", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1828", "61201", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1829", "61202", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1830", "61203", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1831", "61204", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1832", "61205", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1833", "61206", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1834", "61207", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1835", "61208", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1836", "61209", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1837", "61210", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1838", "61211", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1839", "61212", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1840", "61213", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1841", "61214", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1842", "61215", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1843", "61216", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1844", "61217", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1845", "61218", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1846", "61219", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1847", "61220", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1848", "61221", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1849", "61222", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1850", "61223", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1851", "61224", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1852", "61225", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1853", "61226", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1854", "61227", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1855", "61228", "HERAPDF20_NNLO_EIG", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1856", "61230", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1857", "61231", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1858", "61232", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1859", "61233", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1860", "61234", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1861", "61235", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1862", "61236", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1863", "61237", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1864", "61238", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1865", "61239", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1866", "61240", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1867", "61241", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1868", "61242", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1869", "61243", "HERAPDF20_NNLO_VAR", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1870", "13400", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1871", "13401", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1872", "13402", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1873", "13403", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1874", "13404", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1875", "13405", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1876", "13406", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1877", "13407", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1878", "13408", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1879", "13409", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1880", "13410", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1881", "13411", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1882", "13412", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1883", "13413", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1884", "13414", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1885", "13415", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1886", "13416", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1887", "13417", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1888", "13418", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1889", "13419", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1890", "13420", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1891", "13421", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1892", "13422", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1893", "13423", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1894", "13424", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1895", "13425", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1896", "13426", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1897", "13427", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1898", "13428", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1899", "13429", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1900", "13430", "CT14qed_inc_proton", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1901", "82200", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1902", "82201", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1903", "82202", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1904", "82203", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1905", "82204", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1906", "82205", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1907", "82206", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1908", "82207", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1909", "82208", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1910", "82209", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1911", "82210", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1912", "82211", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1913", "82212", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1914", "82213", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1915", "82214", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1916", "82215", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1917", "82216", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1918", "82217", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1919", "82218", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1920", "82219", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1921", "82220", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1922", "82221", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1923", "82222", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1924", "82223", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1925", "82224", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1926", "82225", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1927", "82226", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1928", "82227", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1929", "82228", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1930", "82229", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1931", "82230", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1932", "82231", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1933", "82232", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1934", "82233", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1935", "82234", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1936", "82235", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1937", "82236", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1938", "82237", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1939", "82238", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1940", "82239", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1941", "82240", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1942", "82241", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1943", "82242", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1944", "82243", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1945", "82244", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1946", "82245", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1947", "82246", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1948", "82247", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1949", "82248", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1950", "82249", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1951", "82250", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1952", "82251", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1953", "82252", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1954", "82253", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1955", "82254", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1956", "82255", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1957", "82256", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1958", "82257", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1959", "82258", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1960", "82259", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1961", "82260", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1962", "82261", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1963", "82262", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1964", "82263", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1965", "82264", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1966", "82265", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1967", "82266", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1968", "82267", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1969", "82268", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1970", "82269", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1971", "82270", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1972", "82271", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1973", "82272", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1974", "82273", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1975", "82274", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1976", "82275", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1977", "82276", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1978", "82277", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1979", "82278", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1980", "82279", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1981", "82280", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1982", "82281", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1983", "82282", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1984", "82283", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1985", "82284", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1986", "82285", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1987", "82286", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1988", "82287", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1989", "82288", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1990", "82289", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1991", "82290", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1992", "82291", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1993", "82292", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1994", "82293", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1995", "82294", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1996", "82295", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1997", "82296", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1998", "82297", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("1999", "82298", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2000", "82299", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2001", "82300", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2002", "82301", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2003", "82302", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2004", "82303", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2005", "82304", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2006", "82305", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2007", "82306", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2008", "82307", "LUXqed17_plus_PDF4LHC15_nnlo_100", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2009", "292200", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2010", "292201", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2011", "292202", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2012", "292203", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2013", "292204", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2014", "292205", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2015", "292206", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2016", "292207", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2017", "292208", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2018", "292209", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2019", "292210", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2020", "292211", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2021", "292212", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2022", "292213", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2023", "292214", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2024", "292215", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2025", "292216", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2026", "292217", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2027", "292218", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2028", "292219", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2029", "292220", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2030", "292221", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2031", "292222", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2032", "292223", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2033", "292224", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2034", "292225", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2035", "292226", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2036", "292227", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2037", "292228", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2038", "292229", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2039", "292230", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2040", "292231", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2041", "292232", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2042", "292233", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2043", "292234", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2044", "292235", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2045", "292236", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2046", "292237", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2047", "292238", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2048", "292239", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2049", "292240", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2050", "292241", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2051", "292242", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2052", "292243", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2053", "292244", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2054", "292245", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2055", "292246", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2056", "292247", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2057", "292248", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2058", "292249", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2059", "292250", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2060", "292251", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2061", "292252", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2062", "292253", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2063", "292254", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2064", "292255", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2065", "292256", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2066", "292257", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2067", "292258", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2068", "292259", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2069", "292260", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2070", "292261", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2071", "292262", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2072", "292263", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2073", "292264", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2074", "292265", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2075", "292266", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2076", "292267", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2077", "292268", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2078", "292269", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2079", "292270", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2080", "292271", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2081", "292272", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2082", "292273", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2083", "292274", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2084", "292275", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2085", "292276", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2086", "292277", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2087", "292278", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2088", "292279", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2089", "292280", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2090", "292281", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2091", "292282", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2092", "292283", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2093", "292284", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2094", "292285", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2095", "292286", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2096", "292287", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2097", "292288", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2098", "292289", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2099", "292290", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2100", "292291", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2101", "292292", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2102", "292293", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2103", "292294", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2104", "292295", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2105", "292296", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2106", "292297", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2107", "292298", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2108", "292299", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2109", "292300", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2110", "292301", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2111", "292302", "NNPDF30_nlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2112", "292600", "NNPDF30_nnlo_nf_5_pdfas", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2113", "315000", "NNPDF31_lo_as_0118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2114", "315200", "NNPDF31_lo_as_0130", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2115", "262000", "NNPDF30_lo_as_0118", 1.0, 1.0));
LO26xInfo.push_back(weightinfo("2116", "263000", "NNPDF30_lo_as_0130", 1.0, 1.0));
///End of v26x
}


//define this as a plug-in
DEFINE_FWK_MODULE(weight_assign_26xLO);
