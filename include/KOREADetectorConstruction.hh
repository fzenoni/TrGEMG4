#ifndef KOREADetectorConstruction_hh
#define KOREADetectorConstruction_hh

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
#include "G4Box.hh"


class KOREADetectorConstruction : public G4VUserDetectorConstruction
{
   public :
      // Constructor
      KOREADetectorConstruction() ;
      // Destructor
      ~KOREADetectorConstruction() ;

      void DefineMaterials() ;
      G4VPhysicalVolume* Construct() ;
      G4Box* RPCBox(G4String name, G4double width) ;
      G4Box* SchermoBox(G4String name, G4double width) ;
      void PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume *pMotherLogical) ; 

   private :
      G4Material*        fG10Mat;
      G4Material*        fGraphiteMat;
      G4Material*        fBakeliteMat;
      G4Material*        fGasMat;
      G4Material*        fEmptyMat;
      G4Material*        fWoodMat;
      G4Material*        fScintMat;
      G4Material*        Al;
      G4Material*        fAirMat;
      G4ProductionCuts*  fGasDetectorCuts;
      G4double           tripleGemPx;
      G4double           tripleGemPy;
      G4double           AlPx;
      G4double           AlPy;
      G4double           AlPz;
      G4double           ScintPx;
      G4double           ScintPy;
      G4double           ScintPz;
      G4double           SchermoPx;
      G4double           SchermoPy;
      G4int              SETUP;

      std::vector<G4Box*>           trdCollection ;
      std::vector<G4LogicalVolume*> trdLogCollection ;

};

#endif
