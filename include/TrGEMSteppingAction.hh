#ifndef TrGEMSteppingAction_h
#define TrGEMSteppingAction_h 1

#include "G4UserSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TrGEMSteppingAction : public G4UserSteppingAction
{
  public:
    TrGEMSteppingAction();
   ~TrGEMSteppingAction(){};

    void UserSteppingAction(const G4Step*);

  private:
      G4double globalTime ;
      G4int    pdgCode ;
      G4double kineticEnergy ;
      G4double positionX ;
      G4double positionY ;
      G4double positionZ ;
      G4double momentumX ;
      G4double momentumY ;
      G4double momentumZ ;
      G4double momentumDirectionX ;
      G4double momentumDirectionY ;
      G4double momentumDirectionZ ;
      std::string process ;
      G4int charge ;
      
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
