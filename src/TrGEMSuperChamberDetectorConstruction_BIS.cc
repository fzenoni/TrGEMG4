#include "TrGEMSuperChamberDetectorConstruction_BIS.hh"
#include "GasGapSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
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

TrGEMSuperChamberDetectorConstruction_BIS::TrGEMSuperChamberDetectorConstruction_BIS() :
   fG10Mat(0), fGasMat(0), fEmptyMat(0), fGasDetectorCuts(0),
   tripleGemThinBase(0), tripleGemLargeBase(0), tripleGemHeight(0)
{

   tripleGemThinBase  = 279.*mm ; //220.0*mm ;
   tripleGemLargeBase = 510.*mm ; //445.0*mm ;
   tripleGemHeight    = 1283.*mm ; //990.0*mm ;

   G4double cut = 1*mm ;
   fGasDetectorCuts = new G4ProductionCuts() ;
   fGasDetectorCuts->SetProductionCut(cut,"gamma") ;
   fGasDetectorCuts->SetProductionCut(cut,"e-") ;
   fGasDetectorCuts->SetProductionCut(cut,"e+") ;
   fGasDetectorCuts->SetProductionCut(cut,"proton") ;

}

TrGEMSuperChamberDetectorConstruction_BIS::~TrGEMSuperChamberDetectorConstruction_BIS() {

   delete fGasDetectorCuts ;

}

void TrGEMSuperChamberDetectorConstruction_BIS::DefineMaterials() {

   G4int ncomponents, natoms;
   G4double abundance, fractionmass;

   G4NistManager* manager = G4NistManager::Instance() ;
   // define Elements
   G4Element* elH  = manager->FindOrBuildElement(1);
   G4Element* elC  = manager->FindOrBuildElement(6);
   G4Element* elO  = manager->FindOrBuildElement(8);
   G4Element* elF  = manager->FindOrBuildElement(9);
   G4Element* elSi = manager->FindOrBuildElement(14);

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

   // define FR-4
   G4double density(0.), temperature(0.), pressure(0.) ;
   density = 1.2*g/cm3;

   G4Material* SiO2 = new G4Material("quartz",density= 2.200*g/cm3, ncomponents=2);
   SiO2->AddElement(elSi, natoms=1);
   SiO2->AddElement(elO , natoms=2);

   G4Material* Epoxy = new G4Material("Epoxy" , density, ncomponents=2);
   Epoxy->AddElement(elH, natoms=2);
   Epoxy->AddElement(elC, natoms=2);

   //FR4 (Glass + Epoxy)
   density = 1.86*g/cm3;
   G4Material* FR4 = new G4Material("FR4"  , density, ncomponents=2);
   FR4->AddMaterial(SiO2, fractionmass=0.528);
   FR4->AddMaterial(Epoxy, fractionmass=0.472);
   fFR4Mat = FR4 ;

   // gases at STP conditions 
   G4Material* Argon = manager->FindOrBuildMaterial("G4_Ar");
   G4Material* CarbonDioxide = manager->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
   G4Material* empty = manager->FindOrBuildMaterial("G4_Galactic");
   G4Material* air = manager->FindOrBuildMaterial("G4_AIR");
   fEmptyMat = air ;

   // CF4 must be defined by hand
   G4int numel(0) ;
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

G4VPhysicalVolume* TrGEMSuperChamberDetectorConstruction_BIS::Construct() {

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
   G4VisAttributes *vfatAttributes = new G4VisAttributes(G4Color::Yellow()) ;
   vfatAttributes->SetForceWireframe(true) ;


   // Beginning of geometry definition

   G4Trd* copper01A = Trapezoid("Copper01A", 35.*um) ;
   G4LogicalVolume* copper01ALog = new G4LogicalVolume(copper01A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01ALog") ; 
   copper01ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01A) ;
   trdLogCollection.push_back(copper01ALog) ;

   G4Trd* g10_1A = Trapezoid("g10_1A", 3.2*mm) ;
   G4LogicalVolume* g10_1ALog = new G4LogicalVolume(g10_1A, fFR4Mat, "G10_1ALog") ;
   g10_1ALog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1A) ;
   trdLogCollection.push_back(g10_1ALog) ;

   G4Trd* copper02A = Trapezoid("Copper01B", 35.*um) ;
   G4LogicalVolume* copper02ALog = new G4LogicalVolume(copper02A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper02ALog") ; 
   copper02ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper02A) ;
   trdLogCollection.push_back(copper02ALog) ;

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
   trdLogCollection.push_back(copper11ALog) ;

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
   trdLogCollection.push_back(gasGap4ALog) ;

   // Readout strips
   G4Trd* copper4A = Trapezoid("Copper4A", 35.*um) ;
   G4LogicalVolume* copper4ALog = new G4LogicalVolume(copper4A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4ALog") ;
   copper4ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4A) ;
   trdLogCollection.push_back(copper4ALog) ;

   // Readout Board
   G4Trd* g10_2A = Trapezoid("g10_2A", 3.2*mm) ;
   G4LogicalVolume* g10_2ALog = new G4LogicalVolume(g10_2A, fFR4Mat, "g10_2ALog") ; // was G4_BAKELITE
   g10_2ALog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2A) ;
   trdLogCollection.push_back(g10_2ALog) ;

   G4Trd* copper5A = Trapezoid("Copper5A", 35.*um) ;
   G4LogicalVolume* copper5ALog = new G4LogicalVolume(copper5A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5ALog") ;
   copper5ALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5A) ;
   trdLogCollection.push_back(copper5ALog) ;

   // Spacer (air/void)
   G4Trd* spacerA = Trapezoid("spacerA", 1.*mm) ;
   G4LogicalVolume* spacerALog = new G4LogicalVolume(spacerA, fEmptyMat, "spacerALog") ;
   spacerALog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(spacerA) ;
   trdLogCollection.push_back(spacerALog) ;

   // GEB board A composition
   // Copper plane 4
   G4Trd* gebA_copper4 = Trapezoid("gebA_copper4", 35*um) ;
   G4LogicalVolume* gebA_copper4Log = new G4LogicalVolume(gebA_copper4, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebA_copper4Log") ;
   gebA_copper4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_copper4) ;
   trdLogCollection.push_back(gebA_copper4Log) ;
   // Insulator FR4 5
   G4Trd* gebA_insulator5 = Trapezoid("gebA_insulator5", 218*um) ;
   G4LogicalVolume* gebA_insulator5Log = new G4LogicalVolume(gebA_insulator5, fFR4Mat, "gebA_insulator5Log") ;
   gebA_insulator5Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_insulator5) ;
   trdLogCollection.push_back(gebA_insulator5Log) ;
   // Routing Plane 2
   G4Trd* gebA_routing2 = Trapezoid("gebA_routing2", 17.5*um) ;
   G4LogicalVolume* gebA_routing2Log = new G4LogicalVolume(gebA_routing2, fFR4Mat, "gebA_routing2Log") ;
   gebA_routing2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_routing2) ;
   trdLogCollection.push_back(gebA_routing2Log) ;
   // Insulator FR4 4
   G4Trd* gebA_insulator4 = Trapezoid("gebA_insulator4", 150*um) ;
   G4LogicalVolume* gebA_insulator4Log = new G4LogicalVolume(gebA_insulator4, fFR4Mat, "gebA_insulator4Log") ;
   gebA_insulator4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_insulator4) ;
   trdLogCollection.push_back(gebA_insulator4Log) ;
   // Copper plane 3
   G4Trd* gebA_copper3 = Trapezoid("gebA_copper3", 17.5*um) ;
   G4LogicalVolume* gebA_copper3Log = new G4LogicalVolume(gebA_copper3, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebA_copper3Log") ;
   gebA_copper3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_copper3) ;
   trdLogCollection.push_back(gebA_copper3Log) ;
   // Insulator FR4 3
   G4Trd* gebA_insulator3 = Trapezoid("gebA_insulator3", 120*um) ;
   G4LogicalVolume* gebA_insulator3Log = new G4LogicalVolume(gebA_insulator3, fFR4Mat, "gebA_insulator3Log") ;
   gebA_insulator3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_insulator3) ;
   trdLogCollection.push_back(gebA_insulator3Log) ;
   // Copper plane 2
   G4Trd* gebA_copper2 = Trapezoid("gebA_copper2", 17.5*um) ;
   G4LogicalVolume* gebA_copper2Log = new G4LogicalVolume(gebA_copper2, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebA_copper2Log") ;
   gebA_copper2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_copper2) ;
   trdLogCollection.push_back(gebA_copper2Log) ;
   // Insulator FR4 2
   G4Trd* gebA_insulator2 = Trapezoid("gebA_insulator2", 150*um) ;
   G4LogicalVolume* gebA_insulator2Log = new G4LogicalVolume(gebA_insulator2, fFR4Mat, "gebA_insulator2Log") ;
   gebA_insulator2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_insulator2) ;
   trdLogCollection.push_back(gebA_insulator2Log) ;
   // Routing Plane 1
   G4Trd* gebA_routing1 = Trapezoid("gebA_routing1", 17.5*um) ;
   G4LogicalVolume* gebA_routing1Log = new G4LogicalVolume(gebA_routing1, fFR4Mat, "gebA_routing1Log") ;
   gebA_routing1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_routing1) ;
   trdLogCollection.push_back(gebA_routing1Log) ;
   // Insulator FR4 1
   G4Trd* gebA_insulator1 = Trapezoid("gebA_insulator1", 218*um) ;
   G4LogicalVolume* gebA_insulator1Log = new G4LogicalVolume(gebA_insulator1, fFR4Mat, "gebA_insulator1Log") ;
   gebA_insulator1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_insulator1) ;
   trdLogCollection.push_back(gebA_insulator1Log) ;
   // Copper plane 1
   G4Trd* gebA_copper1 = Trapezoid("gebA_copper1", 35*um) ;
   G4LogicalVolume* gebA_copper1Log = new G4LogicalVolume(gebA_copper1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebA_copper1Log") ;
   gebA_copper1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebA_copper1) ;
   trdLogCollection.push_back(gebA_copper1Log) ;

   // VFAT2
   G4Trd* vfatA = Trapezoid("vfatA", 1.6*mm) ;
   G4LogicalVolume* vfatALog = new G4LogicalVolume(vfatA, fEmptyMat, "vfatALog") ;
   vfatALog->SetVisAttributes(new G4VisAttributes(*vfatAttributes)) ;
   trdCollection.push_back(vfatA) ;
   trdLogCollection.push_back(vfatALog) ;
   
   // Cooling copper
   G4Trd* coolCuA = Trapezoid("coolCuA", 1.*mm) ;
   G4LogicalVolume* coolCuALog = new G4LogicalVolume(coolCuA, fEmptyMat, "coolCuALog") ;
   coolCuALog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(coolCuA) ;
   trdLogCollection.push_back(coolCuALog) ;

   // Cooling pipe
   G4Trd* coolPipeA = Trapezoid("coolPipeA", 8.*mm) ;
   G4LogicalVolume* coolPipeALog = new G4LogicalVolume(coolPipeA, fEmptyMat, "coolPipeA") ;
   trdCollection.push_back(coolPipeA) ;
   trdLogCollection.push_back(coolPipeALog) ;

   // GEM cover (1 mm)
   G4Trd* cover2A = Trapezoid("Cover2A", 1.*mm) ;
   G4LogicalVolume* cover2ALog = new G4LogicalVolume(cover2A, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "cover2ALog") ;
   cover2ALog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(cover2A) ;
   trdLogCollection.push_back(cover2ALog) ;

   // END OF FIRST CHAMBER
   //
   G4Trd* spacerMiddle = Trapezoid("spacerMiddle", 3.7*mm) ;
   G4LogicalVolume* spacerMiddleLog = new G4LogicalVolume(spacerMiddle, fEmptyMat, "spacerMiddleLog") ;
   spacerMiddleLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(spacerMiddle) ;
   trdLogCollection.push_back(spacerMiddleLog) ;
   //
   // BEGINNING OF SECOND CHAMBER

   G4Trd* copper01B = Trapezoid("Copper01B",35.*um) ;
   G4LogicalVolume* copper01BLog = new G4LogicalVolume(copper01B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01BLog") ;
   copper01BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01B) ;
   trdLogCollection.push_back(copper01BLog) ;

   G4Trd* g10_1B = Trapezoid("g10_1B", 3.2*mm) ;
   G4LogicalVolume* g10_1BLog = new G4LogicalVolume(g10_1B, fFR4Mat, "G10_1BLog") ;
   g10_1BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1B) ;
   trdLogCollection.push_back(g10_1BLog) ;

   G4Trd* copper02B = Trapezoid("Copper02B",35.*um) ;
   G4LogicalVolume* copper02BLog = new G4LogicalVolume(copper02B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper02BLog") ;
   copper02BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper02B) ;
   trdLogCollection.push_back(copper02BLog) ;

   G4Trd* gasGap1B = Trapezoid("GasGap1B", 3.*mm) ;
   G4LogicalVolume* gasGap1BLog = new G4LogicalVolume(gasGap1B, fGasMat, "gasGap1BLog") ; 
   gasGap1BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap1B) ;
   trdLogCollection.push_back(gasGap1BLog) ;
   gasGap1BLog->SetSensitiveDetector(sensitive) ;

   // First GEM Foil - beginning
   G4Trd* copper11B = Trapezoid("Copper11B", 5*um) ;
   G4LogicalVolume* copper11BLog = new G4LogicalVolume(copper11B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper11BLog") ; 
   copper11BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper11B) ;
   trdLogCollection.push_back(copper11BLog) ;

   G4Trd* kapton1B = Trapezoid("Kapton1B", 50.*um) ;
   G4LogicalVolume* kapton1BLog = new G4LogicalVolume(kapton1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton1BLog") ;
   kapton1BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton1B) ;
   trdLogCollection.push_back(kapton1BLog) ;

   G4Trd* copper12B = Trapezoid("Copper12B",5.*um) ;
   G4LogicalVolume* copper12BLog = new G4LogicalVolume(copper12B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper12BLog") ;
   copper12BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper12B) ;
   trdLogCollection.push_back(copper12BLog) ;

   G4Trd* gasGap2B = Trapezoid("GasGap2B", 1.*mm) ;
   G4LogicalVolume* gasGap2BLog = new G4LogicalVolume(gasGap2B, fGasMat, "gasGap2BLog") ;
   gasGap2BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap2B) ;
   trdLogCollection.push_back(gasGap2BLog) ;
   gasGap2BLog->SetSensitiveDetector(sensitive) ;

   G4Trd* copper21B = Trapezoid("Copper21B", 5.*um) ;
   G4LogicalVolume* copper21BLog = new G4LogicalVolume(copper21B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper21BLog") ;
   copper21BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper21B) ;
   trdLogCollection.push_back(copper21BLog) ;

   G4Trd* kapton2B = Trapezoid("Kapton2B", 50.*um) ;
   G4LogicalVolume* kapton2BLog = new G4LogicalVolume(kapton2B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton2BLog") ;
   kapton2BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton2B) ;
   trdLogCollection.push_back(kapton2BLog) ;

   G4Trd* copper22B = Trapezoid("Copper22B", 5.*um) ;
   G4LogicalVolume* copper22BLog = new G4LogicalVolume(copper22B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper22BLog") ;
   copper22BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper22B) ;
   trdLogCollection.push_back(copper22BLog) ;

   G4Trd* gasGap3B = Trapezoid("GasGap3B", 2.*mm) ;
   G4LogicalVolume* gasGap3BLog = new G4LogicalVolume(gasGap3B, fGasMat, "GasGap3BLog") ;
   gasGap3BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap3B) ;
   trdLogCollection.push_back(gasGap3BLog) ;

   G4Trd* copper31B = Trapezoid("Copper31B", 5.*um) ;
   G4LogicalVolume* copper31BLog = new G4LogicalVolume(copper31B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper31BLog") ;
   copper31BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper31B) ;
   trdLogCollection.push_back(copper31BLog) ;

   G4Trd* kapton3B = Trapezoid("Kapton3B", 50.*um) ;
   G4LogicalVolume* kapton3BLog = new G4LogicalVolume(kapton3B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton3BLog") ;
   kapton3BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton3B) ;
   trdLogCollection.push_back(kapton3BLog) ;

   G4Trd* copper32B = Trapezoid("Copper32B", 5.*um) ;
   G4LogicalVolume* copper32BLog = new G4LogicalVolume(copper32B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper32BLog") ;
   copper32BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper32B) ;
   trdLogCollection.push_back(copper32BLog) ;

   G4Trd* gasGap4B = Trapezoid("GasGap4B", 1.*mm) ;
   G4LogicalVolume* gasGap4BLog = new G4LogicalVolume(gasGap4B, fGasMat, "gasGap4BLog") ;
   gasGap4BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap4B) ;
   trdLogCollection.push_back(gasGap4BLog) ;

   // Readout strips
   G4Trd* copper4B = Trapezoid("Copper4B", 35.*um) ;
   G4LogicalVolume* copper4BLog = new G4LogicalVolume(copper4B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4BLog") ;
   copper4BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4B) ;
   trdLogCollection.push_back(copper4BLog) ;

   G4Trd* g10_2B = Trapezoid("g10_2B", 3.2*mm) ;
   G4LogicalVolume* g10_2BLog = new G4LogicalVolume(g10_2B, fFR4Mat, "g10_2BLog") ; // was G4_BAKELITE
   g10_2BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2B) ;
   trdLogCollection.push_back(g10_2BLog) ;

   // Readout Board
   G4Trd* copper5B = Trapezoid("Copper5B", 35.*um) ;
   G4LogicalVolume* copper5BLog = new G4LogicalVolume(copper5B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5BLog") ;
   copper5BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5B) ;
   trdLogCollection.push_back(copper5BLog) ;   

   // Spacer (air/void)
   G4Trd* spacerB = Trapezoid("spacerB", 1.*mm) ;
   G4LogicalVolume* spacerBLog = new G4LogicalVolume(spacerB, fEmptyMat, "spacerBLog") ;
   spacerBLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(spacerB) ;
   trdLogCollection.push_back(spacerBLog) ;

   // GEB board B composition
   // Copper plane 4
   G4Trd* gebB_copper4 = Trapezoid("gebB_copper4", 35*um) ;
   G4LogicalVolume* gebB_copper4Log = new G4LogicalVolume(gebB_copper4, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper4Log") ;
   gebB_copper4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper4) ;
   trdLogCollection.push_back(gebB_copper4Log) ;
   // Insulator FR4 5
   G4Trd* gebB_insulator5 = Trapezoid("gebB_insulator5", 218*um) ;
   G4LogicalVolume* gebB_insulator5Log = new G4LogicalVolume(gebB_insulator5, fFR4Mat, "gebB_insulator5Log") ;
   gebB_insulator5Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator5) ;
   trdLogCollection.push_back(gebB_insulator5Log) ;
   // Routing Plane 2
   G4Trd* gebB_routing2 = Trapezoid("gebB_routing2", 17.5*um) ;
   G4LogicalVolume* gebB_routing2Log = new G4LogicalVolume(gebB_routing2, fFR4Mat, "gebB_routing2Log") ;
   gebB_routing2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_routing2) ;
   trdLogCollection.push_back(gebB_routing2Log) ;
   // Insulator FR4 4
   G4Trd* gebB_insulator4 = Trapezoid("gebB_insulator4", 150*um) ;
   G4LogicalVolume* gebB_insulator4Log = new G4LogicalVolume(gebB_insulator4, fFR4Mat, "gebB_insulator4Log") ;
   gebB_insulator4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator4) ;
   trdLogCollection.push_back(gebB_insulator4Log) ;
   // Copper plane 3
   G4Trd* gebB_copper3 = Trapezoid("gebB_copper3", 17.5*um) ;
   G4LogicalVolume* gebB_copper3Log = new G4LogicalVolume(gebB_copper3, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper3Log") ;
   gebB_copper3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper3) ;
   trdLogCollection.push_back(gebB_copper3Log) ;
   // Insulator FR4 3
   G4Trd* gebB_insulator3 = Trapezoid("gebB_insulator3", 120*um) ;
   G4LogicalVolume* gebB_insulator3Log = new G4LogicalVolume(gebB_insulator3, fFR4Mat, "gebB_insulator3Log") ;
   gebB_insulator3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator3) ;
   trdLogCollection.push_back(gebB_insulator3Log) ;
   // Copper plane 2
   G4Trd* gebB_copper2 = Trapezoid("gebB_copper2", 17.5*um) ;
   G4LogicalVolume* gebB_copper2Log = new G4LogicalVolume(gebB_copper2, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper2Log") ;
   gebB_copper2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper2) ;
   trdLogCollection.push_back(gebB_copper2Log) ;
   // Insulator FR4 2
   G4Trd* gebB_insulator2 = Trapezoid("gebB_insulator2", 150*um) ;
   G4LogicalVolume* gebB_insulator2Log = new G4LogicalVolume(gebB_insulator2, fFR4Mat, "gebB_insulator2Log") ;
   gebB_insulator2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator2) ;
   trdLogCollection.push_back(gebB_insulator2Log) ;
   // Routing Plane 1
   G4Trd* gebB_routing1 = Trapezoid("gebB_routing1", 17.5*um) ;
   G4LogicalVolume* gebB_routing1Log = new G4LogicalVolume(gebB_routing1, fFR4Mat, "gebB_routing1Log") ;
   gebB_routing1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_routing1) ;
   trdLogCollection.push_back(gebB_routing1Log) ;
   // Insulator FR4 1
   G4Trd* gebB_insulator1 = Trapezoid("gebB_insulator1", 218*um) ;
   G4LogicalVolume* gebB_insulator1Log = new G4LogicalVolume(gebB_insulator1, fFR4Mat, "gebB_insulator1Log") ;
   gebB_insulator1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator1) ;
   trdLogCollection.push_back(gebB_insulator1Log) ;
   // Copper plane 1
   G4Trd* gebB_copper1 = Trapezoid("gebB_copper1", 35*um) ;
   G4LogicalVolume* gebB_copper1Log = new G4LogicalVolume(gebB_copper1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper1Log") ;
   gebB_copper1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper1) ;
   trdLogCollection.push_back(gebB_copper1Log) ;

   // VFAT2
   G4Trd* vfatB = Trapezoid ("vfatB", 1.6*mm) ;
   G4LogicalVolume* vfatBLog = new G4LogicalVolume(vfatB, fEmptyMat, "vfatBLog") ;
   vfatBLog->SetVisAttributes(new G4VisAttributes(*vfatAttributes)) ;
   trdCollection.push_back(vfatB) ;
   trdLogCollection.push_back(vfatBLog) ;
   
   // Cooling copper
   G4Trd* coolCuB = Trapezoid("coolCuB", 1.*mm) ;
   G4LogicalVolume* coolCuBLog = new G4LogicalVolume(coolCuB, fEmptyMat, "coolCuBLog") ;
   coolCuBLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(coolCuB) ;
   trdLogCollection.push_back(coolCuBLog) ;

   // Cooling pipe
   G4Trd* coolPipeB = Trapezoid("coolPipeB", 8.*mm) ;
   G4LogicalVolume* coolPipeBLog = new G4LogicalVolume(coolPipeB, fEmptyMat, "coolPipeB") ;
   trdCollection.push_back(coolPipeB) ;
   trdLogCollection.push_back(coolPipeBLog) ;

   // GEM cover (1 mm)
   G4Trd* cover1B = Trapezoid("Cover1B", 1.*mm) ;
   G4LogicalVolume* cover1BLog = new G4LogicalVolume(cover1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "cover1BLog") ;
   cover1BLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(cover1B) ;
   trdLogCollection.push_back(cover1BLog) ;

   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}

G4Trd* TrGEMSuperChamberDetectorConstruction_BIS::Trapezoid(G4String name, G4double width) {
   G4Trd* shape = new G4Trd(name,
	 width/2, width/2,
	 tripleGemThinBase/2,
	 tripleGemLargeBase/2,
	 tripleGemHeight/2) ;
   return shape ;
}

void TrGEMSuperChamberDetectorConstruction_BIS::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {

   G4double XTranslation = 0. ;
   G4String coordX ;
   G4String coordY ;
   std::vector<G4double> x ;
   std::vector<G4double> y ;
   std::ifstream coordFile ("/Users/fzenoni/TrGEMG4/vfatCoord.txt", std::ios::in) ;
   if(coordFile) {
      while(!coordFile.eof()) {
	 coordFile >> coordX ; 
	 coordFile >> coordY ;
	 if(coordX == "" || coordY == "") continue ;
	 x.push_back(std::stod(coordX)) ; 
	 y.push_back(std::stod(coordY)) ; 
      }
      coordFile.close() ;
   }

   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
      G4String layerName = trdCollection.at(i)->GetName() ;
      XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
      G4ThreeVector position = tlate + G4ThreeVector(XTranslation,0,0).transform(G4RotationMatrix(*pRot).inverse()) ;
      G4cout << "Volume (" << i << ") " << layerName << " the position is " << G4BestUnit(XTranslation,"Length") << G4endl ;

      if(layerName == "coolCuA" || layerName == "coolCuB") {
	 G4cout << "God only knows how much I am sick of this junk" << G4endl ;
         G4double coolThick = 1.*mm ;
	 G4double coolWidth = 188.*mm ;
	 G4double coolShortHeight = 51.*mm ;
	 G4double coolLongHeight = 150.*mm ;
	 // big cooler
	 G4Box* coolBig = new G4Box("coolBig", coolThick/2., coolWidth/2., coolLongHeight/2.) ; 
	 G4LogicalVolume* coolBigLog = new G4LogicalVolume(coolBig, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "coolBigLog") ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + coolShortHeight + coolLongHeight/2.), coolBigLog, layerName, trdLogCollection.at(i), false, i) ;
	 // small coolers
	 G4Box* coolSmall = new G4Box("coolSmall", coolThick/2., coolWidth/2., coolShortHeight/2.) ; 
	 G4LogicalVolume* coolSmallLog = new G4LogicalVolume(coolSmall, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "coolSmallLog") ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (5./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (8./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (11./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (14./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (17./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (20./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + (23./1.5)*coolShortHeight + coolLongHeight), coolSmallLog, layerName, trdLogCollection.at(i), false, i) ;
      }

      if(layerName == "vfatA" || layerName == "vfatB") {
	 // here lies the positionning of every single vfat2 module
	 double vfatX = 46*mm ;
	 double vfatY = 43*mm ;
	 double vfatThick = 1.*mm ;
	 double copperThick = 17.5*um ;
	 double insulatorThick = 310*um ;
	 G4Box* vfatModule = new G4Box("vfatModule", vfatThick/2., vfatX/2., vfatY/2.) ; // G4Box wants half sizes
	 G4LogicalVolume* vfatModuleLog = new G4LogicalVolume(vfatModule, fEmptyMat, "vfatModuleLog") ;
	 for(G4int k = 0; k < 24; k++) {
	    new G4PVPlacement(0, G4ThreeVector(0., y[k], -x[k]+tripleGemHeight/2.), vfatModuleLog, layerName, trdLogCollection.at(i), false, i) ;
	    // vfat content
	    G4int copyNo = 0. ;
	    G4Box* Cu = new G4Box("Cu", copperThick/2., vfatX/2., vfatY/2.) ;
	    G4LogicalVolume* CuLog = new G4LogicalVolume(Cu, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "CuLog") ;
	    G4Box* insulator = new G4Box("insulator", insulatorThick/2., vfatX/2., vfatY/2.) ;
	    G4LogicalVolume* insulatorLog = new G4LogicalVolume(insulator, fFR4Mat, "insulatorLog") ;
	    new G4PVPlacement(0, G4ThreeVector(vfatThick/2.-copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(vfatThick/2.-copperThick-insulatorThick/2., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(insulatorThick/2.+copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-insulatorThick/2.-copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-vfatThick/2.+copperThick+insulatorThick/2., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-vfatThick/2.+copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	 }
	 // And now for the optohybrid
	 G4Box* optoHybrid = new G4Box("optoHybrid", 1.6*mm/2., 220.13/2*mm, 140.60/2.*mm) ;
	 G4LogicalVolume* optoHybridLog = new G4LogicalVolume(optoHybrid, fFR4Mat, "optoHybridLog") ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0.005, -80.39+tripleGemHeight/2.), optoHybridLog, layerName, trdLogCollection.at(i), false, i) ;
      }

      new G4PVPlacement(pRot,
	    position,
	    trdLogCollection.at(i),
	    layerName,
	    pMotherLogical,
	    false,
	    i) ;

      XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
   }
}

