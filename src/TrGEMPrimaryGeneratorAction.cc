#include "TrGEMPrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SPSPosDistribution.hh"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TRandom.h"

TrGEMPrimaryGeneratorAction::TrGEMPrimaryGeneratorAction(
      const G4String& particleName,
      G4double energy,
      G4ThreeVector position,
      G4ThreeVector momentumDirection) :
   G4VUserPrimaryGeneratorAction(),
   fParticleGun(0)
{

   /*
      G4int nofParticles = 1 ;
      fParticleGun = new G4ParticleGun(nofParticles) ;

   // default particle kinematic
   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable() ;
   G4ParticleDefinition* particle = particleTable->FindParticle(particleName) ;
   fParticleGun->SetParticleDefinition(particle) ;
   fParticleGun->SetParticleEnergy(energy) ;
   fParticleGun->SetParticlePosition(position) ;
   fParticleGun->SetParticleMomentumDirection(momentumDirection) ;
   */


   gps->GetCurrentSource()->GetEneDist()->SetMonoEnergy(2.0*GeV);
   gps->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0.0*cm, 0.0*cm, -5.0*cm));
   gps->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

   gun = gps;

   TFile* angularFile = new TFile("/Users/fzenoni/TrGEMG4/angular.root") ;

   TH1F* h_cosx = (TH1F*)angularFile->Get("cosx_ph") ;
   h_cosx->Fit("pol4") ;
   fit_cosx = h_cosx->GetFunction("pol4") ;

   TH1F* h_cosy = (TH1F*)angularFile->Get("cosy_ph") ;
   prob_neg_y = h_cosy->Integral(1,h_cosy->GetNbinsX()/2)/h_cosy->Integral() ;
   h_cosy->Fit("pol4") ;
   fit_cosy = h_cosy->GetFunction("pol4") ;

   TH1F* h_cosz = (TH1F*)angularFile->Get("cosz_ph") ;
   prob_neg_z = h_cosz->Integral(1,h_cosz->GetNbinsX()/2)/h_cosz->Integral() ;
   // no fit needed for z
   //
   //dummy_x = new TH1F("dummy_x", "dummy_x",100,-1.,1.) ;
   //dummy_y = new TH1F("dummy_y", "dummy_y",100,-1.,1.) ;
   //dummy_z = new TH1F("dummy_z", "dummy_z",100,-1.,1.) ;
}


TrGEMPrimaryGeneratorAction::~TrGEMPrimaryGeneratorAction() {

   //delete fParticleGun ;
   delete gun ;

}

void TrGEMPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

   bool alfredo = true ;
   if(alfredo) {
      G4double cosx = fit_cosx->GetRandom(-1.,1.) ;
      //dummy_x->Fill(cosx) ;

      G4double cosy(0.) ;
      if(gRandom->Uniform() > prob_neg_y) cosy = fit_cosy->GetRandom(0.,sqrt(1-cosx*cosx)) ;
      else cosy = fit_cosy->GetRandom(-sqrt(1-cosx*cosx),0.) ;
      //dummy_y->Fill(cosy) ;

      G4double cosz(0.) ;
      double z_source(0) ;
      if(gRandom->Uniform() < prob_neg_z) {
	 cosz = sqrt(1-cosx*cosx-cosy*cosy) ;
	 z_source = 7.3*cm ; // good for superchamber
      }
      else {
	 cosz = -sqrt(1-cosx*cosx-cosy*cosy) ;
	 z_source = -1*mm ;
      }
      //dummy_z->Fill(cosz) ;

      /*G4cout << "cosx is " << cosx << G4endl ;
	G4cout << "cosy is " << cosy << G4endl ;
	G4cout << "cosz is " << cosz << G4endl ;*/
      gps->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(-cosx,-cosy,-cosz));

      G4SPSPosDistribution *posDist = gps->GetCurrentSource()->GetPosDist();
      posDist->SetPosDisType("Plane");  // or Point,Plane,Volume,Beam
      posDist->SetPosDisShape("Rectangle");  // or Circle, Annulus, Ellipse, Square  
      posDist->SetCentreCoords(G4ThreeVector(0.0*cm,0.0*cm,z_source));
      posDist->SetHalfY(1283.*mm/2) ;
      posDist->SetHalfX(279.*mm/2) ;

   }
   //fParticleGun->GeneratePrimaryVertex(anEvent) ;
   //
   gun->GeneratePrimaryVertex(anEvent) ;

   //dummy_x->SaveAs("dummy_x.C") ;
   //dummy_y->SaveAs("dummy_y.C") ;
   //dummy_z->SaveAs("dummy_z.C") ;

}
