#ifndef TrGEMSuperSimpleDetectorConstruction_BIS_hh
#define TrGEMSuperSimpleDetectorConstruction_BIS_hh

class G4LogicalVolume ;
class G4PhysicalVolume ;

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
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4Trd.hh"


class TrGEMSuperSimpleDetectorConstruction_BIS : public G4VUserDetectorConstruction
{
   public :
      // Constructor
      TrGEMSuperSimpleDetectorConstruction_BIS() ;
      // Destructor
      ~TrGEMSuperSimpleDetectorConstruction_BIS() ;

      void DefineMaterials() ;
      G4VPhysicalVolume* Construct() ;
      G4Trd* Trapezoid(G4String name, G4double width) ;
      void PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume *pMotherLogical) ; 

   private :
      G4Material*        fFR4Mat;
      G4Material*        fG10Mat;
      G4Material*        fGasMat;
      G4Material*        fEmptyMat;
      G4Material*        fSiMat;
      G4ProductionCuts*  fGasDetectorCuts;
      G4double           tripleGemThinBase; 
      G4double           tripleGemLargeBase;
      G4double           tripleGemHeight;

      std::vector<G4Trd*>           trdCollection ;
      std::vector<G4LogicalVolume*> trdLogCollection ;

};

#endif

