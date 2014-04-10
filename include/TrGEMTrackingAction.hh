#ifndef TRGEMTRACKINGACTION_H
#define TRGEMTRACKINGACTION_H 1

#include "globals.hh"
#include "G4UserTrackingAction.hh"

class TrGEMTrackingAction : public G4UserTrackingAction {

   public:
      // Constructor & Destructor
      TrGEMTrackingAction() ;
      virtual ~TrGEMTrackingAction() ;

      // Member functions
      void SetTrackingManagerPointer(G4TrackingManager* pValue) ;
      virtual void PreUserTrackingAction(const G4Track*) ;
      virtual void PostUserTrackingAction(const G4Track*) ;

   private:
      G4int eventID ;
      G4int parentID ;
      G4int trackID ;
      G4int charge ;
      G4double globalTime ;
      G4int    pdgCode ;
      std::string particle ;
      G4double kineticEnergy ;
      G4double positionX ;
      G4double positionY ;
      G4double positionZ ;
      G4double momentumDirectionX ;
      G4double momentumDirectionY ;
      G4double momentumDirectionZ ;
      std::string process ;
      std::string volume ;

};

#endif
