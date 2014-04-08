#include "TrGEMTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "TrGEMTrajectory.hh"

TrGEMTrackingAction::TrGEMTrackingAction()
{;}

TrGEMTrackingAction::~TrGEMTrackingAction()
{;}

void TrGEMTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
   fpTrackingManager->SetStoreTrajectory(true) ;
   fpTrackingManager->SetTrajectory(new TrGEMTrajectory(aTrack)) ;
}

void TrGEMTrackingAction::PostUserTrackingAction(const G4Track* /*aTrack*/)
{;}
