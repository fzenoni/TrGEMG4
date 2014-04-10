#include "TrGEMTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "TrGEMAnalysis.hh"

TrGEMTrackingAction::TrGEMTrackingAction() :
   eventID(0),
   parentID(0),
   trackID(0),
   charge(0),
   globalTime(0.),
   pdgCode(0),
   particle(""),
   kineticEnergy(0.),
   positionX(0.),
   positionY(0.),
   positionZ(0.),
   momentumDirectionX(0.),
   momentumDirectionY(0.),
   momentumDirectionZ(0.),
   volume("")
{;}

TrGEMTrackingAction::~TrGEMTrackingAction()
{;}

void TrGEMTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
   //fpTrackingManager->SetStoreTrajectory(true) ;
   //fpTrackingManager->SetTrajectory(new TrGEMTrajectory(aTrack)) ;

   if( aTrack->GetParentID() > 0 ) {
      eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID() ;
      //G4cout << eventID << G4endl ;
      parentID = aTrack->GetParentID() ;
      //G4cout << parentID << G4endl ;
      trackID = aTrack->GetTrackID() ;
      //G4cout << trackID << G4endl ;
      charge = aTrack->GetParticleDefinition()->GetPDGCharge() ;
      //G4cout << charge << G4endl ;
      globalTime = aTrack->GetGlobalTime() ;
      //G4cout << globalTime << G4endl ;
      pdgCode = aTrack->GetParticleDefinition()->GetPDGEncoding() ;
      //G4cout << pdgCode << G4endl ;
      particle = aTrack->GetDynamicParticle()->GetDefinition()->GetParticleName() ;
      //G4cout << particle << G4endl ;
      kineticEnergy = aTrack->GetVertexKineticEnergy() ;
      //G4cout << kineticEnergy << G4endl ;
      positionX = aTrack->GetVertexPosition().x() ;
      //G4cout << positionX << G4endl ;
      positionY = aTrack->GetVertexPosition().y() ;
      //G4cout << positionY << G4endl ;
      positionZ = aTrack->GetVertexPosition().z() ;
      //G4cout << positionZ << G4endl ;
      momentumDirectionX = aTrack->GetVertexMomentumDirection().x() ; 
      //G4cout << momentumDirectionX << G4endl ;
      momentumDirectionY = aTrack->GetVertexMomentumDirection().y() ; 
      //G4cout << momentumDirectionY << G4endl ;
      momentumDirectionZ = aTrack->GetVertexMomentumDirection().z() ;
      //G4cout << momentumDirectionZ << G4endl ;
      process = aTrack->GetCreatorProcess()->GetProcessName() ;
      //G4cout << "bug" << G4endl ;
      //GetDaughter breaks everything, the problem must be in the index
      volume = aTrack->GetLogicalVolumeAtVertex()/*->GetDaughter(0)*/->GetName() ;

   }

}

void TrGEMTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{

   TrGEMAnalysis::GetInstance()->SaveProcessQuantities(
	 eventID,
	 parentID,
	 trackID,
	 charge,
	 globalTime,
	 pdgCode,
	 particle,
	 kineticEnergy,
	 positionX,
	 positionY,
	 positionZ,
	 momentumDirectionX, 
	 momentumDirectionY, 
	 momentumDirectionZ,
	 process,
	 volume) ;

   
   TrGEMAnalysis::GetInstance()->EndOfTrack(aTrack) ;

}
