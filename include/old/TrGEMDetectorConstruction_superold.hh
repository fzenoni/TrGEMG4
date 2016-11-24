#ifndef TRGEMDETECTORCONSTRUCTION_HH
#define TRGEMDETECTORCONSTRUCTION_HH

class G4LogicalVolume ;
class G4VPhysicalVolume ;


// Standard includes
#include <list>
#include <string>
#include <cmath>

// Geant4 includes
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

// TrGEM includes
#include "TrdLayer.hh"

/*
 * Class TrGEMDetectorConstruction.
 * Inherit from G4VUserDetectorConstruction mandatory base class.
 * Define the GEM prototype geometry.
 */

class TrGEMDetectorConstruction : public G4VUserDetectorConstruction
{
   public :
      // Constructor
      TrGEMDetectorConstruction() ;
      // Destructor
      ~TrGEMDetectorConstruction() ;

      G4VPhysicalVolume* Construct() ;
      void PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume *pMotherLogical) ;

   private :
      // Logical Volumes
      G4LogicalVolume* worldLog ;
      // Physical Volumes
      G4VPhysicalVolume* worldPhys ;


      TrdLayer *cathodeLayer ;
      TrdLayer *kapton0 ;
      TrdLayer *g10Layer1 ;

      TrdLayer *gasGap1 ;

      TrdLayer *copper11;
      TrdLayer *kapton1 ;
      TrdLayer *copper12;

      TrdLayer *gasGap2 ;

      TrdLayer *copper21;
      TrdLayer *kapton2 ;
      TrdLayer *copper22;

      TrdLayer *gasGap3 ;

      TrdLayer *copper31;
      TrdLayer *kapton3 ;
      TrdLayer *copper32;

      TrdLayer *gasGap4 ;

      TrdLayer *g10Layer2 ;
      TrdLayer *readoutLayer ;

      G4double tripleGemThinBase ;
      G4double tripleGemLargeBase ;
      G4double tripleGemHeight ;

      std::vector<TrdLayer*> trdLayerCollection ;


};

#endif // TRGEMDETECTORCONSTRUCTION_HH

