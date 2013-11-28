#include "TrGEMTrajectory.hh"

G4Allocator<TrGEMTrajectory> myTrajectoryAllocator ;

TrGEMTrajectory::TrGEMTrajectory(const G4Track* aTrack)
{
   fpParticleDefinition = aTrack->GetParticleDefinition() ;
   origVolName = "none" ;
   if ( aTrack->GetParentID() > 0 ) { // This is a secondary
      origVolName  = aTrack->GetOriginTouchable()->GetVolume()->GetName() ;
      particleName = fpParticleDefinition->GetParticleName() ;
      PDGEncoding  = fpParticleDefinition->GetPDGEncoding() ;
      //G4cout << particleName << " (" << PDGEncoding << ") comes from " << origVolName << G4endl ;
   }
}

//TrGEMTrajectory::~TrGEMTrajectory() {
//}
