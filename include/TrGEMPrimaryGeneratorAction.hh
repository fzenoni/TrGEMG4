#ifndef TrGEMPrimaryGeneratorAction_h
#define TrGEMPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "TH1F.h"
#include "TF1.h"


class TrGEMPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {

   public :
      TrGEMPrimaryGeneratorAction(
	    const G4String& particleName = "gamma",
	    G4double energy = 500*MeV,
	    G4ThreeVector position = G4ThreeVector(0,0,0),
	    G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)) ;
      
      ~TrGEMPrimaryGeneratorAction() ;

      // methods
      void GeneratePrimaries(G4Event*) ;

   private :
      // data members

      G4GeneralParticleSource *gps = new G4GeneralParticleSource() ;
      G4ParticleGun* fParticleGun; // pointer to a G4 service class
      G4GeneralParticleSource *gun ;
      TF1* fit_cosx ;
      TF1* fit_cosy ;
      G4double prob_neg_y ;
      G4double prob_neg_z ;

      //TH1F* dummy_x ;
      //TH1F* dummy_y ;
      //TH1F* dummy_z ;
};

#endif
