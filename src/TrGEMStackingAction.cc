#include "TrGEMStackingAction.hh"
#include "TrGEMAnalysis.hh"

#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

TrGEMStackingAction::TrGEMStackingAction()
{}

TrGEMStackingAction::~TrGEMStackingAction() 
{}

G4ClassificationOfNewTrack TrGEMStackingAction::ClassifyNewTrack( const G4Track * aTrack ) 
{
   // always "urgent" in current applications
   G4ClassificationOfNewTrack result( fUrgent );

   // Saving here the "Garfield" variables
   charge = aTrack->GetParticleDefinition()->GetPDGCharge() ;
   globalTime = aTrack->GetGlobalTime() ;
   pdgCode = aTrack->GetParticleDefinition()->GetPDGEncoding() ;
   kineticEnergy = aTrack->GetVertexKineticEnergy() ;
   positionX = aTrack->GetVertexPosition().x() ;
   positionY = aTrack->GetVertexPosition().y() ;
   positionZ = aTrack->GetVertexPosition().z() ;
   //G4cout << positionZ << G4endl ;
   momentumDirectionX = aTrack->GetVertexMomentumDirection().x() ; 
   momentumDirectionY = aTrack->GetVertexMomentumDirection().y() ; 
   momentumDirectionZ = aTrack->GetVertexMomentumDirection().z() ;
   //const G4VProcess* proc = point2->GetProcessDefinedStep();
   //const G4String procname = proc->GetProcessName();
   //process = procname ;
   if(aTrack->GetCreatorProcess() != NULL) {
      const G4VProcess* proc = aTrack->GetCreatorProcess() ;
      const G4String procname = proc->GetProcessName() ;
      //G4cout << procname << G4endl;
      process = procname ;
   }
   //else G4cout << "No process?" << G4endl ;
   //if(process != "Transportation" && process != "eIoni" && process != "hIoni" && process != "ionIoni") G4cout << process << G4endl ;
   //if(volName == "WorldSpace") G4cout << process << G4endl ;

   /*
   TrGEMAnalysis::GetInstance()->SaveGarfieldQuantities(
	 1,
	 charge,
	 globalTime,
	 pdgCode,
	 kineticEnergy,
	 positionX,
	 positionY,
	 positionZ,
	 momentumDirectionX, 
	 momentumDirectionY, 
	 momentumDirectionZ,
	 process) ;
   */
   // Let's save the "Garfield" variables
   // Commented hoping the simulation will be faster
   //TrGEMAnalysis::GetInstance()->EndOfStack(aTrack) ;


   if ( aTrack->GetParentID() > 0 ) // This is a secondary
   {
      TrGEMAnalysis::GetInstance()->AddSecondary(aTrack->GetDefinition());
      //G4String orTouch = aTrack->GetOriginTouchable()->GetVolume()->GetName() ;
      //G4String GasGaps[4] = {"GasGap1", "GasGap2", "GasGap3", "GasGap4"} ;
      //for(G4int i = 0; i < 4; i++) {
      //if(orTouch == GasGaps[i]) // This is a secondary born in GasGap #i 
      //TrGEMAnalysis::GetInstance()->AddGapSecondary(aTrack->GetDefinition(), i);
      //
   }
   else // This is primary
   {
      TrGEMAnalysis::GetInstance()->SetBeam(aTrack->GetDefinition(), aTrack->GetKineticEnergy());
   }


   return result;

   }

