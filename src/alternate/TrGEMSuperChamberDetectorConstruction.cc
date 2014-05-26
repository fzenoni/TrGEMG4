#include "TrGEMSuperChamberDetectorConstruction.hh"
#include "GasGapSensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4GeometryManager.hh"
//#include "G4PhysicalVolumeStore.hh"
//#include "G4LogicalVolumeStore.hh"
//#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UnitsTable.hh"

TrGEMSuperChamberDetectorConstruction::TrGEMSuperChamberDetectorConstruction() :
   fG10Mat(0), fGasMat(0), fEmptyMat(0), fGasDetectorCuts(0),
   tripleGemThinBase(0), tripleGemLargeBase(0), tripleGemHeight(0)
{

   tripleGemThinBase  = 220.0*mm ;
   tripleGemLargeBase = 445.0*mm ;
   tripleGemHeight    = 990.0*mm ;

   G4double cut = 1*mm ;
   fGasDetectorCuts = new G4ProductionCuts() ;
   fGasDetectorCuts->SetProductionCut(cut,"gamma") ;
   fGasDetectorCuts->SetProductionCut(cut,"e-") ;
   fGasDetectorCuts->SetProductionCut(cut,"e+") ;
   fGasDetectorCuts->SetProductionCut(cut,"proton") ;

}

TrGEMSuperChamberDetectorConstruction::~TrGEMSuperChamberDetectorConstruction() {

   delete fGasDetectorCuts ;

}

void TrGEMSuperChamberDetectorConstruction::DefineMaterials() {

   G4NistManager* manager = G4NistManager::Instance() ;
   // define Elements
   G4Element* elC  = manager->FindOrBuildElement(6);
   G4Element* elF  = manager->FindOrBuildElement(9);

   // define Materials
   G4Material *H  = G4NistManager::Instance()->FindOrBuildMaterial("G4_H") ;
   G4Material *C  = G4NistManager::Instance()->FindOrBuildMaterial("G4_C") ;
   G4Material *O  = G4NistManager::Instance()->FindOrBuildMaterial("G4_O") ;
   G4Material *Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si") ;

   G4Material *g10Material = new G4Material("G10", 1.9*g/cm3, 4) ;
   g10Material->AddMaterial(C,0.1323) ;
   g10Material->AddMaterial(H,0.03257) ;
   g10Material->AddMaterial(O,0.48316) ;
   g10Material->AddMaterial(Si,0.35194) ;
   fG10Mat = g10Material ;

   // gases at STP conditions 
   G4Material* Argon = manager->FindOrBuildMaterial("G4_Ar");
   G4Material* CarbonDioxide = manager->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
   G4Material* empty = manager->FindOrBuildMaterial("G4_Galactic");
   fEmptyMat = empty ;

   // CF4 must be defined by hand
   G4int numel(0) ;
   G4double density(0.), temperature(0.), pressure(0.) ;
   G4String name, symbol ;
   G4Material* CF4 = new G4Material(name="CF4", density=0.003884*g/cm3, numel=2, kStateGas, temperature = 273.15*kelvin, pressure=1.0*atmosphere);
   CF4->AddElement(elC, 1) ;
   CF4->AddElement(elF, 4) ; 

   // Ar:CO2 (70:30) @ STP conditions
   G4double mixtureDensity = (Argon->GetDensity() * 70/100.0 + CarbonDioxide->GetDensity() * 30/100.0) ;
   G4Material *ArCO2 = new G4Material("Ar/CO2",mixtureDensity,2) ;
   ArCO2->AddMaterial(Argon, 0.7) ;
   ArCO2->AddMaterial(CarbonDioxide, 0.3) ;

   // Ar:CO2:CF4 (45:15:40) @ STP conditions
   mixtureDensity = (Argon->GetDensity() * 45/100.0 + CarbonDioxide->GetDensity() * 15/100.0 + CF4->GetDensity() * 40/100.0) ;
   G4Material *ArCO2CF4 = new G4Material("Ar/CO2/CF4",mixtureDensity,3) ;
   ArCO2CF4->AddMaterial(Argon, 0.45) ;
   ArCO2CF4->AddMaterial(CarbonDioxide,0.15) ;
   ArCO2CF4->AddMaterial(CF4,0.40) ;

   // Choice of the gas
   fGasMat = ArCO2CF4 ;

}

G4VPhysicalVolume* TrGEMSuperChamberDetectorConstruction::Construct() {


   // Cleanup old geometry
   G4GeometryManager::GetInstance()->OpenGeometry();

   G4GeometryManager::GetInstance()->OpenGeometry();
   //G4PhysicalVolumeStore::GetInstance()->Clean();
   //G4LogicalVolumeStore::GetInstance()->Clean();
   //G4SolidStore::GetInstance()->Clean();

   // Define all materials and set global variables
   DefineMaterials() ;

   // SD Manager 
   G4SDManager* sdman = G4SDManager::GetSDMpointer() ;

   G4double worldSizeX = 10*m;
   G4double worldSizeY = 10*m;
   G4double worldSizeZ = 10*m;

   // World definition and placement
   G4Box* worldBox = new G4Box("WorldBox", worldSizeX, worldSizeY, worldSizeZ) ;
   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fEmptyMat, "WorldLog") ;
   // Set visual attributes
   G4VisAttributes *worldAttributes = new G4VisAttributes ;
   worldAttributes->SetVisibility(true) ;
   worldLog->SetVisAttributes(worldAttributes) ;
   G4VPhysicalVolume* worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLog, "WorldSpace", 0, false, 0) ;

   // Rotation Matrix for layers
   G4RotationMatrix* rotationPlacement = new G4RotationMatrix() ;
   rotationPlacement->rotateY(M_PI / 2.0) ;
   rotationPlacement->rotateX(M_PI / 2.0) ;

   // Visual attributes
   G4VisAttributes *cathodeAttributes = new G4VisAttributes(G4Color::Grey()) ;
   cathodeAttributes->SetForceWireframe(true) ;
   G4VisAttributes *g10Attributes = new G4VisAttributes(G4Color::White()) ;
   g10Attributes->SetForceWireframe(true) ;
   G4VisAttributes *gasAttributes = new G4VisAttributes(G4Color::Red()) ;
   gasAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gemAttributes = new G4VisAttributes(G4Color::Green()) ;
   gemAttributes->SetForceWireframe(true) ;


   // Beginning of geometry definition

   // GEM cover (1 mm)
   G4Trd* readoutB = Trapezoid("ReadoutB", 1.*mm) ;
   G4LogicalVolume* readoutBLog = new G4LogicalVolume(readoutA, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "readoutBLog") ;
   readoutBLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(readoutB) ;
   trdLogCollection.push_back(readoutBLog) ;

   // GEB board (1.16 mm)
   G4Trd* gebB = Trapezoid("GEBB", 1.16*mm) ;
   G4LogicalVolume* gebBLog = new G4LogicalVolume(gebB, fG10Mat, "gebBLog") ;
   gebLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB) ;
   trdLogCollection.push_back(gebBLog) ;

   // Spacer (air/void)
   G4Trd* spacerB = Trapezoid("spacerB", 1.8*mm) ;
   G4LogicalVolume* spacerBLog = new G4LogicalVolume(spacerB, fEmptyMat, "spacerBLog") ;
   spacerBLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(spacerB) ;
   trdLogCollection.push_back(spacerBLog) ;

   // Readout Board
   G4Trd* copper5B = Trapezoid("Copper5B", 5.*um) ;
   G4LogicalVolume* copper5BLog = new G4LogicalVolume(copper5B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5BLog") ;
   copper5BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5B) ;
   trdLogCollection.push_back(copper5BLog) ;   

   G4Trd* g10_2B = Trapezoid("g10_2B", 3.*mm) ;
   G4LogicalVolume* g10_2BLog = new G4LogicalVolume(g10_2B, fG10Mat, "g10_2BLog") ; // was G4_BAKELITE
   g10_2BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2B) ;
   trdLogCollection.push_back(g10_2BLog) ;

   // Readout strips
   G4Trd* copper4B = Trapezoid("Copper4B", 5.*um) ;
   G4LogicalVolume* copper4BLog = new G4LogicalVolume(copper4B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4BLog") ;
   copper4BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4B) ;
   trdLogCollection.push_back(copper4BLog) ;
   
   G4Trd* gasGap4B = Trapezoid("GasGap4B", 1.*mm) ;
   G4LogicalVolume* gasGap4BLog = new G4LogicalVolume(gasGap4B, fGasMat, "gasGap4BLog") ;
   gasGap4BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap4B) ;
   trdLogCollection.push_back(gasGap4LogB) ;

   G4Trd* copper32B = Trapezoid("Copper32B", 5.*um) ;
   G4LogicalVolume* copper32BLog = new G4LogicalVolume(copper32B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper32BLog") ;
   copper32BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper32B) ;
   trdLogCollection.push_back(copper32BLog) ;

   G4Trd* kapton3B = Trapezoid("Kapton3B", 50.*um) ;
   G4LogicalVolume* kapton3BLog = new G4LogicalVolume(kapton3B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton3BLog") ;
   kapton3BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton3B) ;
   trdLogCollection.push_back(kapton3BLog) ;

   G4Trd* copper31B = Trapezoid("Copper31B", 5.*um) ;
   G4LogicalVolume* copper31BLog = new G4LogicalVolume(copper31B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper31BLog") ;
   copper31BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper31B) ;
   trdLogCollection.push_back(copper31BLog) ;
   
   G4Trd* gasGap3B = Trapezoid("GasGap3B", 2.*mm) ;
   G4LogicalVolume* gasGap3BLog = new G4LogicalVolume(gasGap3B, fGasMat, "GasGap3BLog") ;
   gasGap3BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap3B) ;
   trdLogCollection.push_back(gasGap3BLog) ;
   
   G4Trd* copper22B = Trapezoid("Copper22B", 5.*um) ;
   G4LogicalVolume* copper22BLog = new G4LogicalVolume(copper22B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper22BLog") ;
   copper22BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper22B) ;
   trdLogCollection.push_back(copper22BLog) ;

   G4Trd* kapton2B = Trapezoid("Kapton2B", 50.*um) ;
   G4LogicalVolume* kapton2BLog = new G4LogicalVolume(kapton2B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton2BLog") ;
   kapton2BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton2B) ;
   trdLogCollection.push_back(kapton2BLog) ;

   G4Trd* copper21B = Trapezoid("Copper21B", 5.*um) ;
   G4LogicalVolume* copper21BLog = new G4LogicalVolume(copper21B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper21BLog") ;
   copper21BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper21B) ;
   trdLogCollection.push_back(copper21BLog) ;

   G4Trd* gasGap2B = Trapezoid("GasGap2B", 1.*mm) ;
   G4LogicalVolume* gasGap2BLog = new G4LogicalVolume(gasGap2B, fGasMat, "gasGap2BLog") ;
   gasGap2BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap2B) ;
   trdLogCollection.push_back(gasGap2BLog) ;

   G4Trd* copper12B = Trapezoid("Copper12B",5.*um) ;
   G4LogicalVolume* copper12BLog = new G4LogicalVolume(copper12B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper12BLog") ;
   copper12BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper12B) ;
   trdLogCollection.push_back(copper12BLog) ;

   G4Trd* kapton1B = Trapezoid("Kapton1B", 50.*um) ;
   G4LogicalVolume* kapton1BLog = new G4LogicalVolume(kapton1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton1BLog") ;
   kapton1BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton1B) ;
   trdLogCollection.push_back(kapton1BLog) ;

   // First GEM Foil - beginning
   G4Trd* copper11B = Trapezoid("Copper11B", 5*um) ;
   G4LogicalVolume* copper11BLog = new G4LogicalVolume(copper11B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper11BLog") ; 
   copper11BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper11B) ;
   trdLogCollection.push_back(copper11BLog) ;

   G4Trd* gasGap1B = Trapezoid("GasGap1B", 3.*mm) ;
   G4LogicalVolume* gasGap1BLog = new G4LogicalVolume(gasGap1B, fGasMat, "gasGap1BLog") ; 
   gasGap1BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap1B) ;
   trdLogCollection.push_back(gasGap1BLog) ;

   G4Trd* copper02B = Trapezoid("Copper02B",5.*um) ;
   G4LogicalVolume* copper02BLog = new G4LogicalVolume(copper02B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper02BLog") ;
   copper02BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper02B) ;
   trdLogCollection.push_back(copper02BLog) ;

   G4Trd* g10_1B = Trapezoid("g10_1B", 3.*mm) ;
   G4LogicalVolume* g10_1BLog = new G4LogicalVolume(g10_1B, fG10Mat, "G10_1BLog") ;
   g10_1BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1B) ;
   trdLogCollection.push_back(g10_1LogB) ;

   G4Trd* copper01B = Trapezoid("Copper01B",5.*um) ;
   G4LogicalVolume* copper01BLog = new G4LogicalVolume(copper01B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01BLog") ;
   copper01BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01B) ;
   trdLogCollection.push_back(copper01BLog) ;

   // END OF FIRST CHAMBER
   // BEGINNING OF SECOND CHAMBER

   G4Trd* copper01A = Trapezoid("Copper01A", 5*um) ;
   G4LogicalVolume* copper01ALog = new G4LogicalVolume(copper01A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01ALog") ; 
   copper01ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01A) ;
   trdLogCollection.push_back(copper01ALog) ;

   G4Trd* g10_1A = Trapezoid("g10_1A", 3.*mm) ;
   G4LogicalVolume* g10_1ALog = new G4LogicalVolume(g10_1A, fG10Mat, "G10_1ALog") ;
   g10_1ALog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1A) ;
   trdLogCollection.push_back(g10_1LogA) ;

   G4Trd* copper01B = Trapezoid("Copper01B", 5*um) ;
   G4LogicalVolume* copper01BLog = new G4LogicalVolume(copper01B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01BLog") ; 
   copper01BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01B) ;
   trdLogCollection.push_back(copper01BLog) ;

   G4Trd* gasGap1A = Trapezoid("GasGap1A", 3.*mm) ;
   G4LogicalVolume* gasGap1ALog = new G4LogicalVolume(gasGap1A, fGasMat, "gasGap1ALog") ; 
   gasGap1ALog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap1A) ;
   trdLogCollection.push_back(gasGap1ALog) ;
   GasGapSensitiveDetector* sensitive = new GasGapSensitiveDetector("/GasGap") ;
   sdman->AddNewDetector(sensitive) ;
   gasGap1ALog->SetSensitiveDetector(sensitive) ;

   // First GEM Foil - beginning
   G4Trd* copper11A = Trapezoid("Copper11A", 5*um) ;
   G4LogicalVolume* copper11ALog = new G4LogicalVolume(copper11A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper11ALog") ; 
   copper11ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper11A) ;
   trdLogCollection.push_back(copper11LogA) ;

   G4Trd* kapton1A = Trapezoid("Kapton1A", 50.*um) ;
   G4LogicalVolume* kapton1ALog = new G4LogicalVolume(kapton1A, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton1ALog") ;
   kapton1ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton1A) ;
   trdLogCollection.push_back(kapton1ALog) ;

   G4Trd* copper12A = Trapezoid("Copper12A",5.*um) ;
   G4LogicalVolume* copper12ALog = new G4LogicalVolume(copper12A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper12ALog") ;
   copper12ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper12A) ;
   trdLogCollection.push_back(copper12ALog) ;
   // First GEM Foil - end

   G4Trd* gasGap2A = Trapezoid("GasGap2A", 1.*mm) ;
   G4LogicalVolume* gasGap2ALog = new G4LogicalVolume(gasGap2A, fGasMat, "gasGap2ALog") ;
   gasGap2ALog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap2A) ;
   trdLogCollection.push_back(gasGap2ALog) ;
   gasGap2ALog->SetSensitiveDetector(sensitive) ;

   G4Trd* copper21A = Trapezoid("Copper21A", 5.*um) ;
   G4LogicalVolume* copper21ALog = new G4LogicalVolume(copper21A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper21ALog") ;
   copper21ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper21A) ;
   trdLogCollection.push_back(copper21ALog) ;

   G4Trd* kapton2A = Trapezoid("Kapton2A", 50.*um) ;
   G4LogicalVolume* kapton2ALog = new G4LogicalVolume(kapton2A, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton2ALog") ;
   kapton2ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton2A) ;
   trdLogCollection.push_back(kapton2ALog) ;

   G4Trd* copper22A = Trapezoid("Copper22A", 5.*um) ;
   G4LogicalVolume* copper22ALog = new G4LogicalVolume(copper22A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper22ALog") ;
   copper22ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper22A) ;
   trdLogCollection.push_back(copper22ALog) ;

   G4Trd* gasGap3A = Trapezoid("GasGap3A", 2.*mm) ;
   G4LogicalVolume* gasGap3ALog = new G4LogicalVolume(gasGap3A, fGasMat, "GasGap3ALog") ;
   gasGap3ALog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap3A) ;
   trdLogCollection.push_back(gasGap3ALog) ;

   G4Trd* copper31A = Trapezoid("Copper31A", 5.*um) ;
   G4LogicalVolume* copper31ALog = new G4LogicalVolume(copper31A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper31ALog") ;
   copper31ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper31A) ;
   trdLogCollection.push_back(copper31ALog) ;

   G4Trd* kapton3A = Trapezoid("Kapton3A", 50.*um) ;
   G4LogicalVolume* kapton3ALog = new G4LogicalVolume(kapton3A, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton3ALog") ;
   kapton3ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton3A) ;
   trdLogCollection.push_back(kapton3ALog) ;

   G4Trd* copper32A = Trapezoid("Copper32A", 5.*um) ;
   G4LogicalVolume* copper32ALog = new G4LogicalVolume(copper32A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper32ALog") ;
   copper32ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper32A) ;
   trdLogCollection.push_back(copper32ALog) ;

   G4Trd* gasGap4A = Trapezoid("GasGap4A", 1.*mm) ;
   G4LogicalVolume* gasGap4ALog = new G4LogicalVolume(gasGap4A, fGasMat, "gasGap4ALog") ;
   gasGap4ALog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap4A) ;
   trdLogCollection.push_back(gasGap4LogA) ;

   // Readout strips
   G4Trd* copper4A = Trapezoid("Copper4A", 5.*um) ;
   G4LogicalVolume* copper4ALog = new G4LogicalVolume(copper4A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4ALog") ;
   copper4ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4A) ;
   trdLogCollection.push_back(copper4ALog) ;

   // Readout Board
   G4Trd* g10_2A = Trapezoid("g10_2A", 3.*mm) ;
   G4LogicalVolume* g10_2ALog = new G4LogicalVolume(g10_2A, fG10Mat, "g10_2ALog") ; // was G4_BAKELITE
   g10_2ALog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2A) ;
   trdLogCollection.push_back(g10_2ALog) ;

   G4Trd* copper5A = Trapezoid("Copper5A", 5.*um) ;
   G4LogicalVolume* copper5ALog = new G4LogicalVolume(copper5A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5ALog") ;
   copper5ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5A) ;
   trdLogCollection.push_back(copper5ALog) ;

   // Spacer (air/void)
   G4Trd* spacerA = Trapezoid("spacerA", 1.8*mm) ;
   G4LogicalVolume* spacerALog = new G4LogicalVolume(spacerA, fEmptyMat, "spacerALog") ;
   spacerALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(spacerA) ;
   trdLogCollection.push_back(spacerALog) ;

   // GEB board (1.16 mm)
   G4Trd* gebA = Trapezoid("GEBA", 1.16*mm) ;
   G4LogicalVolume* gebALog = new G4LogicalVolume(gebA, fG10Mat, "gebALog") ;
   gebALog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA) ;
   trdLogCollection.push_back(gebALog) ;

   // GEM cover (1 mm)
   G4Trd* readoutA = Trapezoid("ReadoutA", 1.*mm) ;
   G4LogicalVolume* readoutALog = new G4LogicalVolume(readoutA, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "readoutALog") ;
   readoutALog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(readoutA) ;
   trdLogCollection.push_back(readoutALog) ;

   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}

G4Trd* TrGEMSuperChamberDetectorConstruction::Trapezoid(G4String name, G4double width) {
   G4Trd* shape = new G4Trd(name,
	 width/2, width/2,
	 tripleGemThinBase/2,
	 tripleGemLargeBase/2,
	 tripleGemHeight/2) ;
   return shape ;
}

void TrGEMSuperChamberDetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {

   G4double XTranslation = 0 ;

   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
      XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
      G4ThreeVector position = tlate + G4ThreeVector(XTranslation,0,0).transform(G4RotationMatrix(*pRot).inverse()) ;
      G4cout << "Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " << G4BestUnit(XTranslation,"Length") << G4endl ;
      new G4PVPlacement(pRot,
	    position,
	    trdLogCollection.at(i),
	    trdCollection.at(i)->GetName(),
	    pMotherLogical,
	    false,
	    i) ;
      XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
   }
}

