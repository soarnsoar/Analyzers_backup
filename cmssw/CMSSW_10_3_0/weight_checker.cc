// -*- C++ -*-
//
// Package:    Analyzer/weight_checker
// Class:      weight_checker
// 
/**\class weight_checker weight_checker.cc Analyzer/weight_checker/plugins/weight_checker.cc

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


//
// Class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

//class weight_checker : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
class weight_checker : public edm::one::EDAnalyzer<edm::one::WatchRuns,edm::one::SharedResources>  {
//class weight_checker : public edm::EDAnalyzer  {
   public:
      explicit weight_checker(const edm::ParameterSet&);
      ~weight_checker();

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
  edm::EDGetTokenT<LHERunInfoProduct> extLHEInfo_Token;   

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
weight_checker::weight_checker(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
 
  //vector<reco::GenParticle>             "genParticles"              ""                "SIM"     

  usesResource("TFileService");
  genParticles_Token = consumes<GenParticleCollection>(edm::InputTag("genParticles"));
  genInfo_Token = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  LHEInfo_Token = consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
  //extLHEInfo_Token = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));  
  extLHEInfo_Token= consumes<LHERunInfoProduct,edm::InRun>(edm::InputTag("externalLHEProducer",""));
 
 //
}


weight_checker::~weight_checker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
weight_checker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  cout<<"analyzer"<<endl;
  ////////////initialize/////////////
  //Get weight//
  edm::Handle<LHEEventProduct> LHEInfo;
  iEvent.getByToken(LHEInfo_Token, LHEInfo);
  int lheinfoweightsize= LHEInfo->weights().size();
  int lheinfocommentssize = LHEInfo->comments_size();
  cout<<"lheinfocommentssize="<<lheinfocommentssize<<endl;
  cout<<"lheinfoweightsize="<<lheinfoweightsize<<endl;
  for (int i =0; i < lheinfoweightsize; i++){
    //    cout<< LHEInfo->weights()[i].id<<endl;
  }

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
   //double weight=1;
   // weight=genInfo->weight();




   ///////Define Hard PID/////

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
weight_checker::beginJob()
//weight_checker::beginJob(edm::Run const& iRun)
//weight_checker::beginJob(edm::Run const& iRun, edm::EventSetup const &iSetup)
{
  cout<<"begin job"<<endl;
 
  edm::Service<TFileService> fs;

  /*
  edm::Handle<LHERunInfoProduct> run;
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;

  //  extLHEInfo_Token = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));                                                                                       

  // iRun.getByLabel( "externalLHEProducer", run );
  //iEvent.getByToken( extLHEInfo_Token, run );                                                                                                                                   
  //  iRun.getByToken( extLHEInfo_Token, run );                                                                                                                                     
  cout<<"LHERunInfo"<<endl;
  LHERunInfoProduct myLHERunInfoProduct = *(run.product());
  for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
    std::cout << iter->tag() << std::endl;
    std::vector<std::string> lines = iter->lines();
    for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
      std::cout << lines.at(iLine);
    }
  }

  */
  cout<<"end of beginjob"<<endl;
}

// ------------ method called once each job just after ending the event loop  ------------

void 
weight_checker::endJob() 
{
  cout<<"endjob"<<endl;
}



void 
weight_checker::beginRun(const Run &iEvent, EventSetup const &iSetup ){
  cout<<" beginrun"<<endl;
  cout<<"end of beginrun"<<endl;
}



void                                                                                                                                                                              
weight_checker::endRun(edm::Run const& iEvent, edm::EventSetup const&)                                                                                                                                                 
{                                                                                                                                                                                 cout<<"doendrun"<<endl;
  //  edm::EDGetTokenT<LHERunInfoProduct> extLHEInfo_Token;                                                                                                                      
  //extLHEInfo_Token = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));  
  edm::Handle<LHERunInfoProduct> run;
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;

  //  extLHEInfo_Token = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));                                                                                       
  //cout<<"!"<<endl;                                                                                                                                                              
  // iEvent.getByLabel( "externalLHEProducer", run );                                                                                                                             
  //    iEvent.getByLabel( "externalLHEProducer", run );
      iEvent.getByToken( extLHEInfo_Token, run );                                                                                                                                 
  //  iRun.getByToken( extLHEInfo_Token, run );                                                                                                                                  
                                                                                                                                                                                  
  // cout<<"@"<<endl;                                                                                                                                                             

  LHERunInfoProduct myLHERunInfoProduct = *(run.product());
  for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
    cout<<" iter->tag() "<<endl;
    std::cout << iter->tag() << std::endl;
    std::vector<std::string> lines = iter->lines();

    for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
      std::cout << lines.at(iLine);
    }
  }
 

}                                                                                                                                                                                 



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------


void
weight_checker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(weight_checker);
