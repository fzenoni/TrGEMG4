#ifndef TRGEMTRACKINGACTION_H
#define TRGEMTRACKINGACTION_H 1

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

};

#endif
