// Had problems with type definitions.
// I don't know which of the following includes solved the problem.
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"
#include <string>
#include <sstream>
#include <iostream>

#include "TrGEMSteppingAction.hh"
#include "TrGEMAnalysis.hh"

#include "G4SteppingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

   TrGEMSteppingAction::TrGEMSteppingAction()
: globalTime(0.),
   pdgCode(0),
   kineticEnergy(0.),
   positionX(0.),
   positionY(0.),
   positionZ(0.),
   momentumX(0.),
   momentumY(0.),
   momentumZ(0.),
   momentumDirectionX(0.),
   momentumDirectionY(0.),
   momentumDirectionZ(0.),
   charge(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrGEMSteppingAction::UserSteppingAction(const G4Step* aStep)
{
   const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
   //G4int volCopyNum = touchable->GetVolume()->GetCopyNo();
   G4String volName = touchable->GetVolume()->GetName() ;

   //G4String processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() ;

   G4int parentID = aStep->GetTrack()->GetParentID() ;

   //if(parentID == 0 /*aStep->IsFirstStepInVolume()*/ && (processName == "compt" || processName == "phot" )) {
   //if(processName == "compt") G4int procNo = 1 ;
   //if(processName == "phot") G4int procNo = 2 ;
   //TrGEMAnalysis::GetInstance()->SaveStepProcess(procNo,volName) ;
   //G4cout << "ParentID is " << parentID << G4endl ;
   //G4cout << "PDG code is " << aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() << G4endl ;
   //G4cout << G4endl ;
   //}

   G4double edep(0.) ;
   edep = aStep->GetTotalEnergyDeposit() ;

   //G4cout << "This is the VolCopyNum: " << volCopyNum << " (" << volName << ")" << G4endl ;

   // Find out position along axis Z as a random point 
   // between pre- and post step points.
   // This randomisation allows to smooth histogram profile independently
   // on histogram binning
   G4double z1 = aStep->GetPreStepPoint()->GetPosition().z() ;
   G4double z2 = aStep->GetPostStepPoint()->GetPosition().z() ;
   G4double z  = z1 + G4UniformRand()*(z2 - z1) ;
   //G4cout << "z coordinate " << z << G4endl ;

   // Save energy deposition 
   if(edep != 0) TrGEMAnalysis::GetInstance()->AddEDep(edep, z) ;

   // This are the variables to be put in "Garfield" Tree.
   // There were previously in SD class but we don't want that anymore.
   G4Track* track = aStep->GetTrack() ;
   G4StepPoint* point1 = aStep->GetPreStepPoint() ;
   G4StepPoint* point2 = aStep->GetPostStepPoint() ;

   charge = track->GetParticleDefinition()->GetPDGCharge() ;
   globalTime = track->GetGlobalTime() ;
   pdgCode = track->GetParticleDefinition()->GetPDGEncoding() ;
   kineticEnergy = track->GetVertexKineticEnergy() ;
   positionX = track->GetVertexPosition().x() ;
   positionY = track->GetVertexPosition().y() ;
   positionZ = track->GetVertexPosition().z() ;
   // there is no such thing as GetVertexMomentum()... btw, why? 
   //momentumX = track->GetVertexMomentum().x() ;
   momentumX = 0. ;
   //momentumY = track->GetVertexMomentum().y() ;
   momentumY = 0. ;
   //momentumZ = track->GetVertexMomentum().z() ;
   momentumZ = 0. ;
   momentumDirectionX = track->GetVertexMomentumDirection().x() ; 
   momentumDirectionY = track->GetVertexMomentumDirection().y() ; 
   momentumDirectionZ = track->GetVertexMomentumDirection().z() ;
   const G4VProcess* proc = point2->GetProcessDefinedStep();
   const G4String procname = proc->GetProcessName();
   process = procname ;
   //const G4VProcess* proc = track->GetCreatorProcess() ;
   //const G4String procname = proc->GetProcessName() ;
   //G4cout << procname << G4endl;
   //process = procname ;
   //if(process != "Transportation" && process != "eIoni" && process != "hIoni" && process != "ionIoni") G4cout << process << G4endl ;
   //if(volName == "WorldSpace") G4cout << process << G4endl ;

   TrGEMAnalysis::GetInstance()->SaveGarfieldQuantities(
	 1,
	 charge,
	 globalTime,
	 pdgCode,
	 kineticEnergy,
	 positionX,
	 positionY,
	 positionZ,
	 momentumX, 
	 momentumY, 
	 momentumZ,
	 momentumDirectionX, 
	 momentumDirectionY, 
	 momentumDirectionZ,
	 process) ;

   // Let's save the "Garfield" variables
   TrGEMAnalysis::GetInstance()->EndOfStep(aStep) ;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

