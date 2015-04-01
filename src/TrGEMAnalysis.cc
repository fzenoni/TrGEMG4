#include "TrGEMAnalysis.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Run.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TNtuple.h>

TrGEMAnalysis* TrGEMAnalysis::singleton = 0 ;

TrGEMAnalysis* TrGEMAnalysis::GetInstance() {
   if ( singleton == 0 ) {
      static TrGEMAnalysis analysis;
      singleton = &analysis;
   }
   return singleton;
}

TrGEMAnalysis::~TrGEMAnalysis() 
{}

TrGEMAnalysis::TrGEMAnalysis() 
{
   m_ROOT_file = 0;
}

void TrGEMAnalysis::PrepareNewEvent(const G4Event* /*anEvent*/)
{
   //Reset variables relative to this event
   isNewEvent = true ;
   thisEventTotEM = 0;
   thisEventCentralEM = 0;
   thisEventSecondaries = 0;

   elexevt = 0 ;
   posxevt = 0 ;
   gammaxevt = 0 ;
   secoxevt = 0 ;

   driftSensitivityA = false ;
   driftSensitivityB = false ;
   transferSensitivityA = false ;
   transferSensitivityB = false ;
   driftEdepA = 0. ;
   driftEdepB = 0. ;
   transferEdepA = 0. ;
   transferEdepB = 0. ;
   neutronSensitivityA = false ;
   neutronSensitivityB = false ;
   kickstart = false ;

   // Clearing of the vectors, ready for coming event
   eventID.clear() ;
   charge.clear() ;
   parentID.clear() ;
   trackID.clear() ;
   globalTime.clear() ;
   pdgCode.clear() ;
   particle.clear() ;
   kineticEnergy.clear() ;
   positionX.clear() ;
   positionY.clear() ;
   positionZ.clear() ;
   momentumDirectionX.clear() ;
   momentumDirectionY.clear() ;
   momentumDirectionZ.clear() ; 
   process.clear() ;
   volume.clear() ;

}

void TrGEMAnalysis::PrepareNewRun(const G4Run* /*aRun*/)
{

   eventCounter = 0 ;
   //Reset variables relative to the run
   thisRunTotEM  = 0;
   thisRunTotEM2 = 0;
   thisRunCentralEM  = 0;
   thisRunCentralEM2 = 0;
   n_gamma = 0;
   n_electron = 0;
   n_positron = 0;
   pVecProcess = &vecProcess ; 
   pVecVolume = &vecVolume ; 

   // create ROOT file
   m_ROOT_file = new TFile("task3.root","RECREATE");
   if(m_ROOT_file) 
      G4cout << "ROOT file task3.root is created " << G4endl;
   else {
      //G4Exception("ROOT file task3.root has not been created!");
      G4cerr << "ROOT file task3.root has not been created!" << G4endl;
   }

   // create histograms
   m_ROOT_histo0 = new TH1D("etot","Total energy deposit normalized to beam energy",100,0,1);
   //m_ROOT_histo1 = new TH1D("e0","Energy deposit in central crystal normalized to beam energy",100,0,1);
   m_ROOT_histo2 = new TH1D("ez","Energy profile along the GEM (mm)",100,0,20);
   m_ROOT_histo3 = new TH1D("egap","Energy profile vs. GasGap",5,0,5);
   //ntuple = new TNtuple("gasgap1","secondaries in gas gap 1","gapGamma:gapElectron:gapPositron") ;

   // create tree and branch(es)
   t = new TTree("MyRun","Run with awesome data") ;
   t->Branch("elexevt",&elexevt,"elexevt/I") ;
   t->Branch("posxevt",&posxevt,"posxevt/I") ;
   t->Branch("gammaxevt",&gammaxevt,"gammaxevt/I") ;
   t->Branch("secoxevt",&secoxevt,"secoxevt/I") ;
   t->Branch("driftSensitivityA",&driftSensitivityA,"driftSensitivityA/O") ;
   t->Branch("driftSensitivityB",&driftSensitivityB,"driftSensitivityB/O") ;
   t->Branch("transferSensitivityA",&transferSensitivityA,"transferSensitivityA/O") ;
   t->Branch("transferSensitivityB",&transferSensitivityB,"transferSensitivityB/O") ;
   t->Branch("driftEdepA",&driftEdepA,"driftEdepA/D") ;
   t->Branch("driftEdepB",&driftEdepB,"driftEdepB/D") ;
   t->Branch("transferEdepA",&transferEdepA, "transferEdepA/D") ;
   t->Branch("transferEdepB",&transferEdepB, "transferEdepB/D") ;
   t->Branch("neutronSensitivityA",&neutronSensitivityA, "neutronSensitivityA/O") ;
   t->Branch("neutronSensitivityB",&neutronSensitivityB, "neutronSensitivityB/O") ;

   //g = new TTree("Signal","Quantities related to secondaries that provide a signal") ;
   t->Branch("eventID",&eventID) ;
   t->Branch("parentID",&parentID) ;
   t->Branch("trackID",&eventID) ;
   t->Branch("charge",&charge) ;
   t->Branch("globalTime",&globalTime) ;
   t->Branch("pdgCode",&pdgCode) ;
   t->Branch("particle",&particle) ;
   t->Branch("kineticEnergy",&kineticEnergy) ;
   t->Branch("positionX",&positionX) ;
   t->Branch("positionY",&positionY) ;
   t->Branch("positionZ",&positionZ) ;
   t->Branch("momentumDirectionX",&momentumDirectionX) ;
   t->Branch("momentumDirectionY",&momentumDirectionY) ;
   t->Branch("momentumDirectionZ",&momentumDirectionZ) ;
   t->Branch("process",&process) ;
   t->Branch("volume",&volume) ;

}

void TrGEMAnalysis::EndOfEvent(const G4Event* /*anEvent*/)
{
   //Accumulate over the run
   thisRunTotEM += thisEventTotEM;

   //Uncomment these lines for more verbosity:
   //G4cout<<"Event: "<< anEvent->GetEventID() <<" Energy in EM calo: "
   //<<G4BestUnit(thisEventTotEM,"Energy")<<" Secondaries: "<<thisEventSecondaries<<G4endl;
   //G4cout<<G4BestUnit(thisEventCentralEM,"Energy")<<G4endl;

   // save information to ROOT
   m_ROOT_histo0->Fill(thisEventTotEM/beamEnergy, 1.0);
   //ntuple->Fill(double(n_gapGamma[0]),double(n_gapElectron[0]),double(n_gapPositron[0])) ;

   if(/*acceptance*/1) t->Fill() ;

}

void TrGEMAnalysis::EndOfRun(const G4Run* aRun)
{

   //Some print outs
   G4int numEvents = aRun->GetNumberOfEvent();
   if(numEvents == 0) { return; }

   //G4double norm = numEvents*beamEnergy;
   G4cout << "================="<<G4endl;
   G4cout << "Summary for run: " << aRun->GetRunID() << G4endl ;
   G4cout << "  Beam of " << beamParticle->GetParticleName() 
      << " kinetic energy: "<< G4BestUnit(beamEnergy,"Energy") << G4endl ;
   G4cout << "  Event processed:         " << numEvents << G4endl ;
   G4cout << "  Average number of gamma: "<<(G4double)n_gamma/(G4double)numEvents<<G4endl ;
   G4cout << "  Average number of e-   : "<<(G4double)n_electron/(G4double)numEvents<<G4endl ;
   G4cout << "  Average number of e+   : "<<(G4double)n_positron/(G4double)numEvents<<G4endl ;
   for(G4int i = 0; i < 4; i++) {
      G4cout << "Gap number " << i + 1 << G4endl ;
      G4cout << "  Average number of gamma: " << (G4double)n_gapGamma[i]/(G4double)numEvents << G4endl ;
      G4cout << "  Average number of e-   : " << (G4double)n_gapElectron[i]/(G4double)numEvents << G4endl ;
      G4cout << "  Average number of e+   : " << (G4double)n_gapPositron[i]/(G4double)numEvents << G4endl ;
      G4cout << "  - - - - - - - - - - " << G4endl ;
   }
   G4cout << "  Average energy deposition in TrGEM: "
      << G4BestUnit(thisRunTotEM/(G4double)numEvents,"Energy") << G4endl ;

   // Writing and closing the ROOT file
   m_ROOT_file->cd() ;
   t->Write() ;
   G4cout << "ROOT: files writing..." << G4endl;
   m_ROOT_file->Write();
   G4cout << "ROOT: files closing..." << G4endl;
   m_ROOT_file->Close();
   delete m_ROOT_file;

}

void TrGEMAnalysis::AddSecondary(const G4ParticleDefinition* part)
{
   if(part == G4Gamma::Gamma())            { ++n_gamma; }
   else if(part == G4Electron::Electron()) { ++n_electron; }  
   else if(part == G4Positron::Positron()) { ++n_positron; }
}


void TrGEMAnalysis::AddGapSecondary(const G4ParticleDefinition* part, G4int gapNum)
{
   gapNum-- ;
   if(part == G4Gamma::Gamma())            { ++n_gapGamma[gapNum]; }
   else if(part == G4Electron::Electron()) { ++n_gapElectron[gapNum]; }  
   else if(part == G4Positron::Positron()) { ++n_gapPositron[gapNum]; }
}

void TrGEMAnalysis::AddEDep(G4double edep, G4double z)
{
   thisEventTotEM += edep;
   m_ROOT_histo2->Fill(z, edep);
}

void TrGEMAnalysis::AddEDepSD(G4double edep, G4int layerIndex)
{
   m_ROOT_histo3->Fill(layerIndex, edep);
}

void TrGEMAnalysis::SetBeam(const G4ParticleDefinition* part, G4double energy)
{
   beamParticle = part;
   beamEnergy = energy;
}

/*void TrGEMAnalysis::CreateBranch(G4String name, G4int evtNo, G4String type)
  {
#ifdef G4ANALYSIS_USE_ROOT
t->Branch(name, &evtNo, type) ;
#endif
}*/

void TrGEMAnalysis::AddParticlesPerEvent(G4int PDGCode) {
   secoxevt++ ;
   if(PDGCode == 11) elexevt++ ;
   else if(PDGCode == -11) posxevt++ ;
   else if(PDGCode == 22) gammaxevt++ ;
   else G4cout << "You must implement a new variable" << G4endl ; 
}

void TrGEMAnalysis::SetDriftSensitivityA(G4double someDriftEdep) {

   driftSensitivityA = true ;
   driftEdepA = someDriftEdep ;

}

void TrGEMAnalysis::SetTransferSensitivityA(G4double someTransferEdep) {

   transferSensitivityA = true ;
   transferEdepA = someTransferEdep ;

}

void TrGEMAnalysis::SetDriftSensitivityB(G4double someDriftEdep) {

   driftSensitivityB = true ;
   driftEdepB = someDriftEdep ;

}

void TrGEMAnalysis::SetTransferSensitivityB(G4double someTransferEdep) {

   transferSensitivityB = true ;
   transferEdepB = someTransferEdep ;

}

void TrGEMAnalysis::SetNeutronSensitivityA(G4bool someBool) {

   neutronSensitivityA = someBool ;

}

void TrGEMAnalysis::SetNeutronSensitivityB(G4bool someBool) {

   neutronSensitivityB = someBool ;

}

/*void TrGEMAnalysis::SaveStepProcess(G4int procNo, std::string volume) {

  if(isNewEvent) {
  eventCounter++ ;
//vecProcess.push_back(process) ;
vecVolume.push_back(volume) ;
isNewEvent = false ;
}

else {
//G4String proc_temp = vecProcess[vecProcess.size()-1] ;
vecProcess.erase(vecProcess.end()-1) ;
//vecProcess.push_back(proc_temp + "+" + process) ;
vecProcess.push_back(process) ;

vecVolume.erase(vecVolume.end()-1) ;
vecVolume.push_back(volume) ;
}

//G4cout << eventCounter << G4endl ;

}*/

void TrGEMAnalysis::SaveProcessQuantities(
      G4int anEventID,
      G4int aParentID,
      G4int aTrackID,
      G4int aCharge,
      G4double aGlobalTime,
      G4int aPdgCode,
      std::string aParticle,
      G4double aKineticEnergy,
      G4double aPositionX, 
      G4double aPositionY, 
      G4double aPositionZ,
      G4double aMomentumDirectionX, 
      G4double aMomentumDirectionY, 
      G4double aMomentumDirectionZ,
      std::string aProcess,
      std::string aVolume) {

   if(kickstart) { // it tells if the particle kickstarted an event (charged part. in drift and/or transfer 1)
      eventID.push_back(anEventID) ;
      charge.push_back(aCharge) ;
      parentID.push_back(aParentID) ;
      trackID.push_back(aTrackID) ;
      globalTime.push_back(aGlobalTime) ;
      pdgCode.push_back(aPdgCode) ;
      particle.push_back(aParticle) ;
      kineticEnergy.push_back(aKineticEnergy) ;
      positionX.push_back(aPositionX) ;
      positionY.push_back(aPositionY) ;
      positionZ.push_back(aPositionZ) ;
      momentumDirectionX.push_back(aMomentumDirectionX) ;
      momentumDirectionY.push_back(aMomentumDirectionY) ;
      momentumDirectionZ.push_back(aMomentumDirectionZ) ;
      process.push_back(aProcess) ;
      volume.push_back(aVolume) ;
   }

}

void TrGEMAnalysis::SetAcceptance(G4bool someAcceptance) {

   acceptance = someAcceptance ;

}

void TrGEMAnalysis::SetKickstart(G4bool someKickstart) {

   kickstart = someKickstart ;

}
