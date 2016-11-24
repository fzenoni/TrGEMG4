//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file radioactivedecay/rdecay01/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
// $Id$
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "CfRPCPrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include <fstream>

G4double xx[100],yy[100],maxx,maxy;
G4double xxg[24],yyg[24],maxxg,maxyg;
G4int ngamma = 10;
G4int nneutron = 4;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CfRPCPrimaryGeneratorAction::CfRPCPrimaryGeneratorAction()
{
   G4int n_particle = 1;
   //  fParticleGun  = new G4ParticleGun(n_particle);
   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition* particlen = particleTable->FindParticle("neutron");
   //  fParticleGun -> SetParticleDefinition(particlen); 
   G4ParticleDefinition* particleg = particleTable->FindParticle("gamma");
   //  fParticleGung -> SetParticleDefinition(particleg); 

   for ( G4int i=0; i<nneutron; i++){
      fParticleGun[i]  = new G4ParticleGun(particlen,n_particle);
      fParticleGun[i]->SetParticlePosition(G4ThreeVector(0.,0.,-300.));
   }
   for ( G4int i=0; i<ngamma; i++){
      fParticleGung[i]  = new G4ParticleGun(particleg,n_particle);
      fParticleGung[i]->SetParticlePosition(G4ThreeVector(0.,0.,-300.));
   }


   /* It opens the file with the initialization settings */
   std::ifstream in;
   in.open("/Users/fzenoni/TrGEMG4/UserData/NeutronSpectrum.out");

   G4double xtemp,ytemp;

   in >> maxx >> maxy ;
   for(G4int i=0; i<100;i++){
      in >> xtemp >> ytemp ;
      xx[i]=xtemp;        
      yy[i]=ytemp;        
   }

   in.close();   

   std::ifstream ing;
   ing.open("/Users/fzenoni/TrGEMG4/UserData/GammaSpectrum.out");

   ing >> maxxg >> maxyg ;
   for(G4int i=0; i<24;i++){
      ing >> xtemp >> ytemp ;
      xxg[i]=xtemp;        
      yyg[i]=ytemp;        
   }
   ing.close();   



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CfRPCPrimaryGeneratorAction::~CfRPCPrimaryGeneratorAction()
{
   for ( G4int i=0; i<nneutron; i++){
      delete fParticleGun[i];}
   for ( G4int i=0; i<ngamma; i++){
      delete fParticleGung[i];}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CfRPCPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
   G4double xxx,yyy;
   G4int  ifunc=-1;
   G4double ene;

   for ( G4int i=0; i<nneutron; i++){
      // set direction
      // neutron
      G4double cosTheta = -1.0 + 2.0 * G4UniformRand();
      G4double phi = twopi*G4UniformRand();
      G4double sinTheta = sqrt(1. - cosTheta*cosTheta);
      // these are the cosines for an isotropic direction
      fParticleGun[i]->SetParticleMomentumDirection((G4ThreeVector(sinTheta*cos(phi),
		  sinTheta*sin(phi),
		  cosTheta))); 
   }
   // gammas
   for ( G4int i=0; i<ngamma; i++){
      G4double cosTheta = -1.0 + 2.0 * G4UniformRand();
      G4double phi = twopi*G4UniformRand();
      G4double sinTheta = sqrt(1. - cosTheta*cosTheta);
      // these are the cosines for an isotropic direction
      fParticleGung[i]->SetParticleMomentumDirection((G4ThreeVector(sinTheta*cos(phi),
		  sinTheta*sin(phi),
		  cosTheta)));
   }
   // set energy
   // neutron
   for ( G4int i=0; i<nneutron; i++){
      ifunc=-1;
      while (1) {
	 xxx=G4UniformRand()*maxx;
	 yyy=G4UniformRand()*maxy;

	 for(G4int j=0; j<100;j++){
	    if(xxx<xx[j]) {ifunc=j;break;}
	 }
	 if( (xx[ifunc]-xxx) > (xxx-xx[ifunc-1])) ifunc=ifunc-1;

	 if(yyy<yy[ifunc]) break;

      }

      ene = xxx; 
      fParticleGun[i]->SetParticleEnergy(ene*MeV);
   }
   //gammas

   for ( G4int i=0; i<ngamma; i++){
      ifunc=-1;
      while (1){
	 xxx=G4UniformRand()*maxxg;
	 yyy=G4UniformRand()*maxyg;

	 for(G4int j=0; j<24;j++){
	    if(xxx<xxg[j]) {ifunc=j;break;}
	 }
	 if( (xxg[ifunc]-xxx) >  (xxx-xxg[ifunc-1])) ifunc=ifunc-1;

	 if(yyy<yyg[ifunc]) break;

      }

      ene = xxx; 
      fParticleGung[i]->SetParticleEnergy(ene*MeV);
   }

   //create vertex
   //   
   for ( G4int i=0; i<nneutron; i++){
      fParticleGun[i]->GeneratePrimaryVertex(anEvent);}
   for ( G4int i=0; i<ngamma; i++){
      fParticleGung[i]->GeneratePrimaryVertex(anEvent);}

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
