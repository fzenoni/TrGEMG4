#ifndef TRGEMSTACKINACTION_H
#define TRGEMSTACKINACTION_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"


class G4Track;

class TrGEMStackingAction : public G4UserStackingAction {

   public:
      // Constructor & Destructor
      TrGEMStackingAction();
      virtual ~TrGEMStackingAction();

      virtual G4ClassificationOfNewTrack ClassifyNewTrack( const G4Track* aTrack );

   private:
      G4int charge ;
      G4double globalTime ;
      G4int    pdgCode ;
      G4double kineticEnergy ;
      G4double positionX ;
      G4double positionY ;
      G4double positionZ ;
      G4double momentumDirectionX ;
      G4double momentumDirectionY ;
      G4double momentumDirectionZ ;
      std::string process ;
      
};

#endif

