#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
#include <string>
#include <sstream>
#include <iostream>

#include "TrGEMAnalysis.hh"
#include "GasGapSensitiveDetector.hh"
#include "GasGapHit.hh"

   GasGapSensitiveDetector::GasGapSensitiveDetector(G4String SDname)
: G4VSensitiveDetector(SDname),
   driftDepA(0.),
   driftDepB(0.),
   transferDepA(0.),
   transferDepB(0.),
   charge(0),
   neutSensitiveA(false),
   neutSensitiveB(false),
   acceptance(false),
   kickstart(false),
   trackID_tmp(0) 

{
   G4cout << "*************************************" << G4endl ;
   G4cout << "*** Creating SD with name: "<< SDname << " ***" << G4endl ;  
   G4cout << "*************************************" << G4endl ;

   // Declaration of the hit collection name
   G4String myCollectionName = "GasGapHitCollection" ;
   collectionName.insert(myCollectionName) ;


}


GasGapSensitiveDetector::~GasGapSensitiveDetector() {

}

G4bool GasGapSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *)
{
   //This method is called every time a G4Step is performed in the logical volume
   //to which this SD is attached: the GAS GAP.

   //To identify where the step is we use the touchable navigation,
   //Remember we need to use PreStepPoint!
   G4Track* track = step->GetTrack() ;
   G4int trackID = track->GetTrackID() ;
   G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
   G4int copyNo = touchable->GetVolume(0)->GetCopyNo();
   G4int layerIndex = copyNo;
   G4String volName = touchable->GetVolume(0)->GetName();
   //We get now the energy deposited by this step
   G4double edep = step->GetTotalEnergyDeposit() ;
   G4double nonionedep = step->GetNonIonizingEnergyDeposit() ;
   edep -= nonionedep ;

   // Get information from particle coming to the gas gap for the first time

   // G4double timeWindow = 1.E8*ns ;
   // the following number is greater than any other (infinite)
   G4double timeWindow = std::numeric_limits<double>::infinity() ;

   charge = track->GetParticleDefinition()->GetPDGCharge() ;

   if(trackID_tmp != trackID) {
      kickstart = false ;
   }

   // Senstivity algorithms
   if(track->GetGlobalTime() < timeWindow) {

      if(volName == "fakeA" || volName == "fakeB") acceptance = true ;

      if(volName == "GasGap1A") {
	 // we're in drift gap
	 if(edep != 0) driftDepA += edep ;

	 // special algorithm for "charged" sensitivity
	 if(charge != 0) {
	    neutSensitiveA = true ;
	    kickstart = true ;
	 }
      }

      if(volName == "GasGap2A") {
	 // we're in transfer1 gap
	 if(edep != 0) transferDepA += edep ;

	 // special algorithm for "charged" sensitivity
	 if(charge != 0) {
	    neutSensitiveA = true ;
	    kickstart = true ;
	 }
      }

      if(volName == "GasGap1B") {
	 // we're in drift gap
	 if(edep != 0) driftDepB += edep ;

	 // special algorithm for "charged" sensitivity
	 if(charge != 0) {
	    neutSensitiveB = true ;
	    kickstart = true ;
	 }
      }

      if(volName == "GasGap2B") {
	 // we're in transfer1 gap
	 if(edep != 0) transferDepB += edep ;

	 // special algorithm for "charged" sensitivity
	 if(charge != 0) {
	    neutSensitiveB = true ;
	    kickstart = true ;
	 }
      } 
   }

   //This line is used to store in Analysis class the energy deposited in this layer
   //The Analysis class will sum up this edep to the current event total energy in this layer
   //Pass the number directly to the Analysis manager. No Hits objects are created in 
   //this case
   //Analysis::GetInstance()->AddEDepHad(layerIndex,edep);


   //check if edep is from primary or secondary:
   G4String isPri = track->GetTrackID() == 1 ? "Yes" : "No";

   // Tricks to implement hits 
   hitMap_t::iterator it = hitMap.find(layerIndex) ;
   GasGapHit* aHit = 0 ;
   if( it != hitMap.end() ) {
      aHit = it->second ;
   }
   else {
      aHit = new GasGapHit(layerIndex) ;
      hitMap.insert( std::make_pair(layerIndex, aHit) ) ;
      hitCollection->insert(aHit) ;
   }
   aHit->AddEdep(edep) ;

   trackID_tmp = trackID ; // updating the variable
   
   TrGEMAnalysis::GetInstance()->SetKickstart(kickstart) ;
   //kickstart = false ;

   return true;
}

void GasGapSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
   hitCollection = new GasGapHitCollection(GetName(), collectionName[0]) ;
   static G4int HCID = -1 ;
   if(HCID<0) HCID = GetCollectionID(0) ; // <<-- this is to get an ID for collectionName[0]
   HCE->AddHitsCollection(HCID, hitCollection) ;

   // Reset map of hits
   hitMap.clear() ;

   trackID_tmp = 0 ;
}

void GasGapSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
   if(acceptance) { // if the particle is in the geometry acceptance 

      G4double ionizationPotential = 0.45*26*eV + 0.15*33*eV + 0.4*54*eV ; // Ar:CO2:CF4 (45:15:40)
      //G4double ionizationPotential = 0.7*26*eV + 0.3*33*eV ; // Ar:CO2 (70:30)
      //G4double ionizationPotential = 0.97*14*eV+0.03*10.6*eV ; // RPC GAS 
      // Updated peer-reviewed values. Effective energy to generate a pair. (Sauli '77, Sharma)  
      
      // G4double ionizationPotential = 0.45*15.8*eV + 0.15*13.78*eV + 0.4*15.9*eV ; // Ar:CO2:CF4 (45:15:40)
      // These are values previously used. They represent the minimum ionization potential.
      
      G4int factor = 0 ;
      if(driftDepA > factor*ionizationPotential) {
	 TrGEMAnalysis::GetInstance()->SetDriftSensitivityA(driftDepA) ;
	 //G4cout << "The Drift Gap is sensitive (" << G4BestUnit(driftDep,"Energy") << ")" << G4endl ; 
      }
      else if(transferDepA > factor*ionizationPotential) { 
	 TrGEMAnalysis::GetInstance()->SetTransferSensitivityA(transferDepA) ;
	 //G4cout << "The Transfer Gap 1 is sensitive (" << G4BestUnit(transferDep,"Energy") << ")" << G4endl ;
      }

      if(driftDepB > factor*ionizationPotential) {
	 TrGEMAnalysis::GetInstance()->SetDriftSensitivityB(driftDepB) ;
	 //G4cout << "The Drift Gap is sensitive (" << G4BestUnit(driftDep,"Energy") << ")" << G4endl ; 
      }
      else if(transferDepB > factor*ionizationPotential) { 
	 TrGEMAnalysis::GetInstance()->SetTransferSensitivityB(transferDepB) ;
	 //G4cout << "The Transfer Gap 1 is sensitive (" << G4BestUnit(transferDep,"Energy") << ")" << G4endl ;
      }

      driftDepA = 0. ;
      transferDepA = 0. ;
      driftDepB = 0. ;
      transferDepB = 0. ;

      //hitCollection->PrintAllHits() ;

      TrGEMAnalysis::GetInstance()->SetNeutronSensitivityA(neutSensitiveA) ;
      TrGEMAnalysis::GetInstance()->SetNeutronSensitivityB(neutSensitiveB) ;

      // resetting neutron sensitivity
      neutSensitiveA = false ;
      neutSensitiveB = false ;
   }
   else {
      G4cout << "Detector missed" << G4endl ;
   }

   TrGEMAnalysis::GetInstance()->SetAcceptance(acceptance) ;
   // resetting neutron sensitivity
   neutSensitiveA = false ;
   neutSensitiveB = false ;
   // resetting acceptance
   acceptance = false ;

}

