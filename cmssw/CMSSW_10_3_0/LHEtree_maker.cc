// -*- C++ -*-
//
// Package:    Analyzer/LHEtree_maker
// Class:      LHEtree_maker
// 
/**\class LHEtree_maker LHEtree_maker.cc Analyzer/LHEtree_maker/plugins/LHEtree_maker.cc

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
//#include "GEN_Analyzer/JHanalyzer/interface/weightinfo.h"

//
// Class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.



//class LHEtree_maker : public edm::one::EDAnalyzer<edm::one::SharedResources>  {

class LHEtree_maker : public edm::one::EDAnalyzer<edm::one::WatchRuns,edm::one::SharedResources>  {
//class LHEtree_maker : public edm::EDAnalyzer  {
   public:
      explicit LHEtree_maker(const edm::ParameterSet&);
      ~LHEtree_maker();

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
LHEtree_maker::LHEtree_maker(const edm::ParameterSet& iConfig)

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
 
 

}


LHEtree_maker::~LHEtree_maker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
LHEtree_maker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
  // int LO242Infosize=LO242Info.size();
  //  for (int i =0; i < lheinfoweightsize; i++){
    //cout<< LHEInfo->weights()[i].id<<endl;
    //    cout<< LHEInfo->weights()[i].wgt/w0<<endl;
  //}

  //  for (int i242 =0; i242 < LO242Infosize;i242++){
  // for (int i_lhe =0; i_lhe < lheinfoweightsize; i_lhe++){ 
  //   if(LHEInfo->weights()[i_lhe].id==LO242Info[i242].id()){
  //	LO242Info[i242].set_weight(LHEInfo->weights()[i_lhe].wgt/w0);
  // }
  //  }
  // }

  //for (int i242 =0; i242 < LO242Infosize;i242++){
  //  for (int i242 =0; i242 < 9;i242++){
  //     cout<<LO242Info[i242].id()<<"=>"<<    LO242Info[i242].weight()<<endl;
    //   cout<<    LO242Info[i242].id()<<endl;
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

   // if((v1+v2).M()< 60 ) return;
   
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
LHEtree_maker::beginJob()
//LHEtree_maker::beginJob(edm::Run const& iRun)
//LHEtree_maker::beginJob(edm::Run const& iRun, edm::EventSetup const &iSetup)
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
  //for(unsigned int i = 0;i <LO242Info.size();i++){ 
  // DYweights->Branch();
    //    DYweights->Branch(LO242Info[i].name()+"_"+LO242Info[i].pdf(),&LO242Info[i]._weight,LO242Info[i].name()+"_"+LO242Info[i].pdf()+"/D");
  //}
  cout<<"end of beginjob"<<endl;
}

// ------------ method called once each job just after ending the event loop  ------------

void 
LHEtree_maker::endJob() 
{
  cout<<"endjob"<<endl;
}



void 
LHEtree_maker::beginRun(const Run &iEvent, EventSetup const &iSetup ){
  cout<<" beginrun"<<endl;
  cout<<"end of beginrun"<<endl;
}



void                                                                                                                                                                              
LHEtree_maker::endRun(edm::Run const& iEvent, edm::EventSetup const&) 
{
  cout<<"doendrun"<<endl;

}
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------


void
LHEtree_maker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}




//define this as a plug-in
DEFINE_FWK_MODULE(LHEtree_maker);
