#include "TrGEMDetectorConstruction.hh"
#include "GasGapSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Torus.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4GeometryManager.hh"
//#include "G4PhysicalVolumeStore.hh"
//#include "G4LogicalVolumeStore.hh"
//#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UnitsTable.hh"

#include <iostream>
#include <fstream>
#include <string>

TrGEMDetectorConstruction::TrGEMDetectorConstruction() :
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

TrGEMDetectorConstruction::~TrGEMDetectorConstruction() {

   delete fGasDetectorCuts ;

}

void TrGEMDetectorConstruction::DefineMaterials() {

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
   fSiMat = Si ;

   G4Material *g10Material = new G4Material("G10", 1.9*g/cm3, 4) ;
   g10Material->AddMaterial(C,0.1323) ;
   g10Material->AddMaterial(H,0.03257) ;
   g10Material->AddMaterial(O,0.48316) ;
   g10Material->AddMaterial(Si,0.35194) ;
   fG10Mat = g10Material ;

   // define FR-4
   G4double density(0.), temperature(0.), pressure(0.) ;

   density = 2.65*g/cm3 ;
   G4Material* SiO2 = new G4Material("quartz",density, ncomponents=2);
   SiO2->AddElement(elSi, natoms=1);
   SiO2->AddElement(elO , natoms=2);

   density = 1.2*g/cm3;
   G4Material* Epoxy = new G4Material("Epoxy" , density, ncomponents=2);
   Epoxy->AddElement(elH, natoms=2);
   Epoxy->AddElement(elC, natoms=2);

   //FR4 (Glass + Epoxy)
   density = 1.85*g/cm3;
   G4Material* FR4 = new G4Material("FR4"  , density, ncomponents=2);
   FR4->AddMaterial(SiO2, fractionmass=0.528);
   FR4->AddMaterial(Epoxy, fractionmass=0.472);
   fFR4Mat = FR4 ;

   // gases at STP conditions 
   G4Material* Argon = manager->FindOrBuildMaterial("G4_Ar");
   G4Material* CarbonDioxide = manager->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
   G4Material* empty = manager->FindOrBuildMaterial("G4_Galactic");
   G4Material* air = manager->FindOrBuildMaterial("G4_AIR");
   fEmptyMat = empty ;

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
   G4cout << "Mixture Density = " << (mixtureDensity)/(g/cm3) << " g/cm3" << G4endl;
   G4Material *ArCO2CF4 = new G4Material("Ar/CO2/CF4",mixtureDensity,3) ;
   ArCO2CF4->AddMaterial(Argon, 0.45) ;
   ArCO2CF4->AddMaterial(CarbonDioxide,0.15) ;
   ArCO2CF4->AddMaterial(CF4,0.40) ;

   // Choice of the gas
   fGasMat = ArCO2CF4 ;

}

G4VPhysicalVolume* TrGEMDetectorConstruction::Construct() {

   // HERE FOLLOWS 'ODD' GEOMETRY

   // Cleanup old geometry
   G4GeometryManager::GetInstance()->OpenGeometry();

   //G4GeometryManager::GetInstance()->OpenGeometry();
   //G4PhysicalVolumeStore::GetInstance()->Clean();
   //G4LogicalVolumeStore::GetInstance()->Clean();
   //G4SolidStore::GetInstance()->Clean();

   // Define all materials and set global variables
   DefineMaterials() ;

   // SD Manager 
   G4SDManager* sdman = G4SDManager::GetSDMpointer() ;

   G4double worldSizeX = 10.*m;
   G4double worldSizeY = 10.*m;
   G4double worldSizeZ = 10.*m;

   // World definition and placement
   G4Box* worldBox = new G4Box("WorldBox", worldSizeX, worldSizeY, worldSizeZ) ;
   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fEmptyMat, "WorldLog") ;
   // Set visual attributes
   G4VisAttributes *worldAttributes = new G4VisAttributes ;
   worldAttributes->SetVisibility(false) ;
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
   G4VisAttributes *gasAttributes = new G4VisAttributes(G4Color::Cyan()) ;
   gasAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gemAttributes = new G4VisAttributes(G4Color::Green()) ;
   gemAttributes->SetForceWireframe(true) ;
   G4VisAttributes *vfatAttributes = new G4VisAttributes(G4Color::Yellow()) ;
   vfatAttributes->SetForceWireframe(true) ;
   
   
   // Beginning of geometry definition
   GasGapSensitiveDetector* sensitive = new GasGapSensitiveDetector("/GasGap") ;

   // Fake B
   G4Trd* fakeB = Trapezoid("fakeB", 1.*nm) ;
   G4LogicalVolume* fakeBLog = new G4LogicalVolume(fakeB, fEmptyMat, "fakeB") ;
   trdCollection.push_back(fakeB) ;
   trdLogCollection.push_back(fakeBLog) ;
   sdman->AddNewDetector(sensitive) ;
   fakeBLog->SetSensitiveDetector(sensitive) ;
   
   // GEM cover (1 mm)
   G4Trd* cover1B = Trapezoid("Cover1B", 1.*mm) ;
   G4LogicalVolume* cover1BLog = new G4LogicalVolume(cover1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "cover1BLog") ;
   cover1BLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(cover1B) ;
   trdLogCollection.push_back(cover1BLog) ;

   // Cooling pipe
   G4Trd* coolPipeB = Trapezoid("coolPipeB", 8.*mm) ;
   G4LogicalVolume* coolPipeBLog = new G4LogicalVolume(coolPipeB, fEmptyMat, "coolPipeB") ;
   trdCollection.push_back(coolPipeB) ;
   trdLogCollection.push_back(coolPipeBLog) ;

   // Cooling copper
   G4Trd* coolCuB = Trapezoid("coolCuB", 1.*mm) ;
   G4LogicalVolume* coolCuBLog = new G4LogicalVolume(coolCuB, fEmptyMat, "coolCuBLog") ;
   coolCuBLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(coolCuB) ;
   trdLogCollection.push_back(coolCuBLog) ;

   // VFAT2
   G4Trd* vfatB = Trapezoid("vfatB", 1.66*mm) ;
   G4LogicalVolume* vfatBLog = new G4LogicalVolume(vfatB, fEmptyMat, "vfatBLog") ;
   vfatBLog->SetVisAttributes(new G4VisAttributes(*vfatAttributes)) ;
   trdCollection.push_back(vfatB) ;
   trdLogCollection.push_back(vfatBLog) ;

   // Hybrid Pedestal
   G4Trd* pedestalB = Trapezoid("pedestalB", 5.5*mm) ;
   G4LogicalVolume* pedestalBLog = new G4LogicalVolume(pedestalB, fEmptyMat, "pedestalBLog") ;
   trdCollection.push_back(pedestalB) ;
   trdLogCollection.push_back(pedestalBLog) ;

   // GEB board B composition
   // Copper plane 1
   G4Trd* gebB_copper1 = Trapezoid("gebB_copper1", 35*um) ;
   G4LogicalVolume* gebB_copper1Log = new G4LogicalVolume(gebB_copper1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper1Log") ;
   gebB_copper1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper1) ;
   trdLogCollection.push_back(gebB_copper1Log) ;
   // Insulator FR4 1
   G4Trd* gebB_insulator1 = Trapezoid("gebB_insulator1", 218*um) ;
   G4LogicalVolume* gebB_insulator1Log = new G4LogicalVolume(gebB_insulator1, fFR4Mat, "gebB_insulator1Log") ;
   gebB_insulator1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator1) ;
   trdLogCollection.push_back(gebB_insulator1Log) ;
   // Routing Plane 1
   G4Trd* gebB_routing1 = Trapezoid("gebB_routing1", 17.5*um) ;
   G4LogicalVolume* gebB_routing1Log = new G4LogicalVolume(gebB_routing1, fFR4Mat, "gebB_routing1Log") ;
   gebB_routing1Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_routing1) ;
   trdLogCollection.push_back(gebB_routing1Log) ;
   // Insulator FR4 2
   G4Trd* gebB_insulator2 = Trapezoid("gebB_insulator2", 150*um) ;
   G4LogicalVolume* gebB_insulator2Log = new G4LogicalVolume(gebB_insulator2, fFR4Mat, "gebB_insulator2Log") ;
   gebB_insulator2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator2) ;
   trdLogCollection.push_back(gebB_insulator2Log) ;
   // Copper plane 2
   G4Trd* gebB_copper2 = Trapezoid("gebB_copper2", 17.5*um) ;
   G4LogicalVolume* gebB_copper2Log = new G4LogicalVolume(gebB_copper2, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper2Log") ;
   gebB_copper2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper2) ;
   trdLogCollection.push_back(gebB_copper2Log) ;
   // Insulator FR4 3
   G4Trd* gebB_insulator3 = Trapezoid("gebB_insulator3", 120*um) ;
   G4LogicalVolume* gebB_insulator3Log = new G4LogicalVolume(gebB_insulator3, fFR4Mat, "gebB_insulator3Log") ;
   gebB_insulator3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator3) ;
   trdLogCollection.push_back(gebB_insulator3Log) ;
   // Copper plane 3
   G4Trd* gebB_copper3 = Trapezoid("gebB_copper3", 17.5*um) ;
   G4LogicalVolume* gebB_copper3Log = new G4LogicalVolume(gebB_copper3, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper3Log") ;
   gebB_copper3Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper3) ;
   trdLogCollection.push_back(gebB_copper3Log) ;
   // Insulator FR4 4
   G4Trd* gebB_insulator4 = Trapezoid("gebB_insulator4", 150*um) ;
   G4LogicalVolume* gebB_insulator4Log = new G4LogicalVolume(gebB_insulator4, fFR4Mat, "gebB_insulator4Log") ;
   gebB_insulator4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator4) ;
   trdLogCollection.push_back(gebB_insulator4Log) ;
   // Routing Plane 2
   G4Trd* gebB_routing2 = Trapezoid("gebB_routing2", 17.5*um) ;
   G4LogicalVolume* gebB_routing2Log = new G4LogicalVolume(gebB_routing2, fFR4Mat, "gebB_routing2Log") ;
   gebB_routing2Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_routing2) ;
   trdLogCollection.push_back(gebB_routing2Log) ;
   // Insulator FR4 5
   G4Trd* gebB_insulator5 = Trapezoid("gebB_insulator5", 218*um) ;
   G4LogicalVolume* gebB_insulator5Log = new G4LogicalVolume(gebB_insulator5, fFR4Mat, "gebB_insulator5Log") ;
   gebB_insulator5Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_insulator5) ;
   trdLogCollection.push_back(gebB_insulator5Log) ;
   // Copper plane 4
   G4Trd* gebB_copper4 = Trapezoid("gebB_copper4", 35*um) ;
   G4LogicalVolume* gebB_copper4Log = new G4LogicalVolume(gebB_copper4, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "gebB_copper4Log") ;
   gebB_copper4Log->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(gebB_copper4) ;
   trdLogCollection.push_back(gebB_copper4Log) ;
   
   ///* this stays
   //// Spacer (air/void)
   //G4Trd* spacerB = Trapezoid("spacerB", 1.*mm) ;
   //G4LogicalVolume* spacerBLog = new G4LogicalVolume(spacerB, fEmptyMat, "spacerBLog") ;
   //spacerBLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   //trdCollection.push_back(spacerB) ;
   //trdLogCollection.push_back(spacerBLog) ;
   //*/ //this stays
   
   // Readout Board
   G4Trd* copper5B = Trapezoid("Copper5B", 35.*um/*5.*um*/) ;
   G4LogicalVolume* copper5BLog = new G4LogicalVolume(copper5B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5BLog") ;
   copper5BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5B) ;
   trdLogCollection.push_back(copper5BLog) ;   

   G4Trd* g10_2B = Trapezoid("g10_2B", 3.2*mm) ;
   G4LogicalVolume* g10_2BLog = new G4LogicalVolume(g10_2B, fFR4Mat, "g10_2BLog") ; 
   g10_2BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2B) ;
   trdLogCollection.push_back(g10_2BLog) ;
   
   // Readout strips
   G4Trd* copper4B = Trapezoid("Copper4B", 35.*um/*5.*um*/) ;
   
   G4LogicalVolume* copper4BLog = new G4LogicalVolume(copper4B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4BLog") ;
   copper4BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4B) ;
   trdLogCollection.push_back(copper4BLog) ;

   G4Trd* gasGap4B = Trapezoid("GasGap4B", 1.*mm) ;
   G4LogicalVolume* gasGap4BLog = new G4LogicalVolume(gasGap4B, fGasMat, "gasGap4BLog") ;
   gasGap4BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap4B) ;
   trdLogCollection.push_back(gasGap4BLog) ;

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
   gasGap2BLog->SetSensitiveDetector(sensitive) ;

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
   gasGap1BLog->SetSensitiveDetector(sensitive) ;
   
   G4Trd* copper02B = Trapezoid("Copper02B", 35.*um) ;
   G4LogicalVolume* copper02BLog = new G4LogicalVolume(copper02B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper02BLog") ;
   copper02BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper02B) ;
   trdLogCollection.push_back(copper02BLog) ;

   G4Trd* g10_1B = Trapezoid("g10_1B", 3.2*mm) ;
   G4LogicalVolume* g10_1BLog = new G4LogicalVolume(g10_1B, fFR4Mat, "G10_1BLog") ;
   g10_1BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1B) ;
   trdLogCollection.push_back(g10_1BLog) ;

   G4Trd* copper01B = Trapezoid("Copper01B", 35.*um) ;
   G4LogicalVolume* copper01BLog = new G4LogicalVolume(copper01B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01BLog") ;
   copper01BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01B) ;
   trdLogCollection.push_back(copper01BLog) ;
   
   // Fake A
   G4Trd* fakeA = Trapezoid("fakeA", 1.*nm) ;
   G4LogicalVolume* fakeALog = new G4LogicalVolume(fakeA, fEmptyMat, "fakeA") ;
   trdCollection.push_back(fakeA) ;
   trdLogCollection.push_back(fakeALog) ;
   fakeALog->SetSensitiveDetector(sensitive) ;


   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}

G4Trd* TrGEMDetectorConstruction::Trapezoid(G4String name, G4double width) {
   G4Trd* shape = new G4Trd(name,
	 width/2, width/2,
	 tripleGemThinBase/2,
	 tripleGemLargeBase/2,
	 tripleGemHeight/2) ;
   return shape ;
}

void TrGEMDetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {
   
   G4VisAttributes *coolingAttributes = new G4VisAttributes(G4Color::Blue()) ;
   coolingAttributes->SetForceWireframe(true) ;
   G4VisAttributes *copperAttributes = new G4VisAttributes(G4Color(1.0, 0.65, 0.)) ;
   copperAttributes->SetForceWireframe(true) ;
   
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
	 x.push_back(std::strtod(coordX,NULL)) ; 
	 y.push_back(std::strtod(coordY,NULL)) ; 
      }
      coordFile.close() ;
   }
   
   // Rotation Matrix for pipes
   G4RotationMatrix* rotationPlacement1 = new G4RotationMatrix() ;
   rotationPlacement1->rotateY(M_PI / 2.0) ;
   rotationPlacement1->rotateX(0.) ;
   rotationPlacement1->rotateZ(-M_PI / 2.0) ;
   
   G4RotationMatrix* rotationPlacement2 = new G4RotationMatrix() ;
   rotationPlacement2->rotateY(0.) ;
   rotationPlacement2->rotateX(M_PI ) ;
   rotationPlacement2->rotateZ(0.) ;

   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
      G4String layerName = trdCollection.at(i)->GetName() ;
      XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
      G4ThreeVector position = tlate + G4ThreeVector(XTranslation,0,0).transform(G4RotationMatrix(*pRot).inverse()) ;
      G4cout << "Volume (" << i << ") " << layerName << " the position is " << G4BestUnit(XTranslation,"Length") << G4endl ;

      if(layerName == "coolPipeA" || layerName == "coolPipeB") {
	 G4double inRadius = 6./2.*mm ; // 3. mm
	 G4double outRadius = 8./2.*mm ; // 4. mm
	 G4double shift = 120.*mm ;
	 G4double pipeLength = 0.565*m ;
	 G4double aperture = 100.*mm ;
	 G4Torus* bendPipe = new G4Torus("bendPipe", inRadius, outRadius, aperture, 0.*degree, 180.*degree) ; 
	 G4Torus* bendWater = new G4Torus("bendPipe", 0., inRadius, aperture, 0.*degree, 180.*degree) ; 
	 G4LogicalVolume* bendPipeLog = new G4LogicalVolume(bendPipe, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "bendPipeLog") ; 
         bendPipeLog->SetVisAttributes(new G4VisAttributes(*coolingAttributes)) ;
	 G4LogicalVolume* bendWaterLog = new G4LogicalVolume(bendWater, G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER"), "bendWaterLog") ; 
	 G4Tubs* straightPipe = new G4Tubs("straightPipe", inRadius, outRadius, pipeLength, 0., 2*M_PI) ;
	 G4Tubs* straightWater = new G4Tubs("straightWater", 0., inRadius, pipeLength, 0., 2*M_PI) ;
	 G4LogicalVolume* straightPipeLog = new G4LogicalVolume(straightPipe, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "straightPipeLog") ;
         straightPipeLog->SetVisAttributes(new G4VisAttributes(*coolingAttributes)) ;
	 G4LogicalVolume* straightWaterLog = new G4LogicalVolume(straightWater, G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER"), "straightWaterLog") ;
	 new G4PVPlacement(rotationPlacement1, G4ThreeVector(0.,0.,-tripleGemHeight/2.+shift), bendPipeLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(rotationPlacement1, G4ThreeVector(0.,0.,-tripleGemHeight/2.+shift), bendWaterLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(rotationPlacement2, G4ThreeVector(0.,aperture,-tripleGemHeight/2.+shift+pipeLength), straightPipeLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(rotationPlacement2, G4ThreeVector(0.,aperture,-tripleGemHeight/2.+shift+pipeLength), straightWaterLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(rotationPlacement2, G4ThreeVector(0.,-aperture,-tripleGemHeight/2.+shift+pipeLength), straightPipeLog, layerName, trdLogCollection.at(i), false, i) ;
	 new G4PVPlacement(rotationPlacement2, G4ThreeVector(0.,-aperture,-tripleGemHeight/2.+shift+pipeLength), straightWaterLog, layerName, trdLogCollection.at(i), false, i) ;
      }

      if(layerName == "coolCuA" || layerName == "coolCuB") {
	 G4double coolThick = 1.*mm ;
	 G4double coolWidth = 188.*mm ;
	 G4double coolShortHeight = 51.*mm ;
	 G4double coolLongHeight = 150.*mm ;
	 // big cooler
	 G4Box* coolBig = new G4Box("coolBig", coolThick/2., coolWidth/2., coolLongHeight/2.) ; 
	 G4LogicalVolume* coolBigLog = new G4LogicalVolume(coolBig, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "coolBigLog") ;
         coolBigLog->SetVisAttributes(new G4VisAttributes(*copperAttributes)) ;
	 new G4PVPlacement(0, G4ThreeVector(0., 0., -tripleGemHeight/2. + coolShortHeight + coolLongHeight/2.), coolBigLog, layerName, trdLogCollection.at(i), false, i) ;
	 // small coolers
	 G4Box* coolSmall = new G4Box("coolSmall", coolThick/2., coolWidth/2., coolShortHeight/2.) ; 
	 G4LogicalVolume* coolSmallLog = new G4LogicalVolume(coolSmall, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "coolSmallLog") ;
         coolSmallLog->SetVisAttributes(new G4VisAttributes(*copperAttributes)) ;
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
	 G4double vfatX = 46*mm ;
	 G4double vfatY = 43*mm ;
	 G4double vfatThick = 1.*mm ;
	 G4double copperThick = 17.5*um ;
	 G4double insulatorThick = 310*um ;
	 G4double chipX = 7.*mm ;
	 G4double chipY = 9.*mm ;
	 G4double chipThick = 350*um ;
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
	    G4Box* chip = new G4Box("chip", chipThick/2., chipX/2., chipY/2.) ;
	    G4LogicalVolume* chipLog = new G4LogicalVolume(chip, fSiMat, "chipLog") ;
	    new G4PVPlacement(0, G4ThreeVector(vfatThick/2.-copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(vfatThick/2.-copperThick-insulatorThick/2., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(insulatorThick/2.+copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-insulatorThick/2.-copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-vfatThick/2.+copperThick+insulatorThick/2., 0., 0.), insulatorLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-vfatThick/2.+copperThick/2., 0., 0.), CuLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
	    new G4PVPlacement(0, G4ThreeVector(-vfatThick/2.-chipThick/2., 0., 0.), chipLog, "vfatModule", vfatModuleLog, false, copyNo++) ; 
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

