#include "TrGEMSquareDetectorConstruction.hh"
#include "GasGapSensitiveDetector.hh"
#include "TrGEMHolesParameterisation.hh"

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
#include "G4Tubs.hh"

TrGEMSquareDetectorConstruction::TrGEMSquareDetectorConstruction() :
   fG10Mat(0), fGasMat(0), fEmptyMat(0), fGasDetectorCuts(0),
   tripleGemThinBase(0), tripleGemLargeBase(0), tripleGemHeight(0)
{

   // Trapezoid shape dimensions
   tripleGemThinBase  = 220.0*mm ;
   tripleGemLargeBase = 445.0*mm ;
   tripleGemHeight    = 990.0*mm ;

   // Squared shape dimensions
   tripleGemPx = 100.*mm ;
   tripleGemPy = 100.*mm ;

   G4double cut = 10*um ;
   fGasDetectorCuts = new G4ProductionCuts() ;
   fGasDetectorCuts->SetProductionCut(cut,"gamma") ;
   fGasDetectorCuts->SetProductionCut(cut,"e-") ;
   fGasDetectorCuts->SetProductionCut(cut,"e+") ;
   fGasDetectorCuts->SetProductionCut(cut,"proton") ;

}

TrGEMSquareDetectorConstruction::~TrGEMSquareDetectorConstruction() {

   delete fGasDetectorCuts ;

}

void TrGEMSquareDetectorConstruction::DefineMaterials() {

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

   // gases at STP conditions 
   G4Material* Argon = manager->FindOrBuildMaterial("G4_Ar");
   G4Material* CarbonDioxide = manager->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
   G4Material* empty  = manager->FindOrBuildMaterial("G4_Galactic");
   G4Material* air  = manager->FindOrBuildMaterial("G4_AIR");
   fEmptyMat = air;

   // CF4 must be defined by hand
   G4int numel(0) ;
   G4double density(0.), temperature(0.), pressure(0.) ;
   G4String name, symbol ;
   G4Material* CF4 = new G4Material(name="CF4", density=0.003884*g/cm3, numel=2, kStateGas, temperature = 273.15*kelvin, pressure=1.0*atmosphere);
   CF4->AddElement(elC, 1) ;
   CF4->AddElement(elF, 4) ; 
   
   // SiO2
   density = 2.65*g/cm3 ;
   G4Material* SiO2 = new G4Material("quartz",density, ncomponents=2);
   SiO2->AddElement(elSi, natoms=1);
   SiO2->AddElement(elO , natoms=2);

   // Epoxy
   density = 1.2*g/cm3;
   G4Material* Epoxy = new G4Material("Epoxy" , density, ncomponents=2);
   Epoxy->AddElement(elH, natoms=2);
   Epoxy->AddElement(elC, natoms=2);

   //FR4 (Glass + Epoxy)
   density = 1.85*g/cm3;
   G4Material* FR4 = new G4Material("FR4", density, ncomponents=2);
   FR4->AddMaterial(SiO2, fractionmass=0.528);
   FR4->AddMaterial(Epoxy, fractionmass=0.472);
   fFR4Mat = FR4 ;

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
   //fGasMat = ArCO2 ;

}

G4VPhysicalVolume* TrGEMSquareDetectorConstruction::Construct() {


   // Cleanup old geometry
   G4GeometryManager::GetInstance()->OpenGeometry();

   //G4PhysicalVolumeStore::GetInstance()->Clean();
   //G4LogicalVolumeStore::GetInstance()->Clean();
   //G4SolidStore::GetInstance()->Clean();

   // Define all materials and set global variables
   DefineMaterials() ;

   // SD Manager 
   G4SDManager* sdman = G4SDManager::GetSDMpointer() ;

   G4double worldSizeX = /*tripleGemPx*1.5*/ 50*cm;
   G4double worldSizeY = /*tripleGemPy*1.5*/ 50*cm;
   G4double worldSizeZ = 50*cm;

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
   //rotationPlacement->rotateY(M_PI / 2.0) ;
   //rotationPlacement->rotateX(M_PI / 2.0) ;
   rotationPlacement->rotateY(0.) ;
   rotationPlacement->rotateX(0.) ;

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
   GasGapSensitiveDetector* sensitive = new GasGapSensitiveDetector("/GasGap") ;

   // Fake B
   G4Box* fakeB = GemBox("fakeB", 1.*nm) ;
   G4LogicalVolume* fakeBLog = new G4LogicalVolume(fakeB, fEmptyMat, "fakeB") ;
   trdCollection.push_back(fakeB) ;
   trdLogCollection.push_back(fakeBLog) ;
   sdman->AddNewDetector(sensitive) ;
   fakeBLog->SetSensitiveDetector(sensitive) ;

   // GEM cover (1 mm)
   G4Box* cover1B = GemBox("Cover1B", 1.*mm) ;
   G4LogicalVolume* cover1BLog = new G4LogicalVolume(cover1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "cover1BLog") ;
   cover1BLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(cover1B) ;
   trdLogCollection.push_back(cover1BLog) ;

   // Readout Board
   G4Box* copper5B = GemBox("Copper5B", 35.*um/*5.*um*/) ;
   G4LogicalVolume* copper5BLog = new G4LogicalVolume(copper5B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper5BLog") ;
   copper5BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper5B) ;
   trdLogCollection.push_back(copper5BLog) ;   

   G4Box* g10_2B = GemBox("g10_2B", 3.2*mm) ;
   G4LogicalVolume* g10_2BLog = new G4LogicalVolume(g10_2B, fFR4Mat, "g10_2BLog") ; 
   g10_2BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_2B) ;
   trdLogCollection.push_back(g10_2BLog) ;

   // Readout strips
   G4Box* copper4B = GemBox("Copper4B", 35.*um/*5.*um*/) ;
   G4LogicalVolume* copper4BLog = new G4LogicalVolume(copper4B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper4BLog") ;
   copper4BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper4B) ;
   trdLogCollection.push_back(copper4BLog) ;

   G4Box* gasGap4B = GemBox("GasGap4B", 1.*mm) ;
   G4LogicalVolume* gasGap4BLog = new G4LogicalVolume(gasGap4B, fGasMat, "gasGap4BLog") ;
   gasGap4BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap4B) ;
   trdLogCollection.push_back(gasGap4BLog) ;

   G4Box* copper32B = GemBox("Copper32B", 5.*um) ;
   G4LogicalVolume* copper32BLog = new G4LogicalVolume(copper32B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper32BLog") ;
   copper32BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper32B) ;
   trdLogCollection.push_back(copper32BLog) ;

   G4Box* kapton3B = GemBox("Kapton3B", 50.*um) ;
   G4LogicalVolume* kapton3BLog = new G4LogicalVolume(kapton3B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton3BLog") ;
   kapton3BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton3B) ;
   trdLogCollection.push_back(kapton3BLog) ;

   G4Box* copper31B = GemBox("Copper31B", 5.*um) ;
   G4LogicalVolume* copper31BLog = new G4LogicalVolume(copper31B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper31BLog") ;
   copper31BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper31B) ;
   trdLogCollection.push_back(copper31BLog) ;

   G4Box* gasGap3B = GemBox("GasGap3B", 2.*mm) ;
   G4LogicalVolume* gasGap3BLog = new G4LogicalVolume(gasGap3B, fGasMat, "GasGap3BLog") ;
   gasGap3BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap3B) ;
   trdLogCollection.push_back(gasGap3BLog) ;

   G4Box* copper22B = GemBox("Copper22B", 5.*um) ;
   G4LogicalVolume* copper22BLog = new G4LogicalVolume(copper22B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper22BLog") ;
   copper22BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper22B) ;
   trdLogCollection.push_back(copper22BLog) ;

   G4Box* kapton2B = GemBox("Kapton2B", 50.*um) ;
   G4LogicalVolume* kapton2BLog = new G4LogicalVolume(kapton2B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton2BLog") ;
   kapton2BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton2B) ;
   trdLogCollection.push_back(kapton2BLog) ;

   G4Box* copper21B = GemBox("Copper21B", 5.*um) ;
   G4LogicalVolume* copper21BLog = new G4LogicalVolume(copper21B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper21BLog") ;
   copper21BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper21B) ;
   trdLogCollection.push_back(copper21BLog) ;

   G4Box* gasGap2B = GemBox("GasGap2B", 1.*mm) ;
   G4LogicalVolume* gasGap2BLog = new G4LogicalVolume(gasGap2B, fGasMat, "gasGap2BLog") ;
   gasGap2BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap2B) ;
   trdLogCollection.push_back(gasGap2BLog) ;
   gasGap2BLog->SetSensitiveDetector(sensitive) ;

   G4Box* copper12B = GemBox("Copper12B",5.*um) ;
   G4LogicalVolume* copper12BLog = new G4LogicalVolume(copper12B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper12BLog") ;
   copper12BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper12B) ;
   trdLogCollection.push_back(copper12BLog) ;

   G4Box* kapton1B = GemBox("Kapton1B", 50.*um) ;
   G4LogicalVolume* kapton1BLog = new G4LogicalVolume(kapton1B, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"), "kapton1BLog") ;
   kapton1BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(kapton1B) ;
   trdLogCollection.push_back(kapton1BLog) ;

   // First GEM Foil - beginning
   G4Box* copper11B = GemBox("Copper11B", 5*um) ;
   G4LogicalVolume* copper11BLog = new G4LogicalVolume(copper11B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper11BLog") ; 
   copper11BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper11B) ;
   trdLogCollection.push_back(copper11BLog) ;

   G4Box* gasGap1B = GemBox("GasGap1B", 3.*mm) ;
   G4LogicalVolume* gasGap1BLog = new G4LogicalVolume(gasGap1B, fGasMat, "gasGap1BLog") ; 
   gasGap1BLog->SetVisAttributes(new G4VisAttributes(*gasAttributes)) ;
   trdCollection.push_back(gasGap1B) ;
   trdLogCollection.push_back(gasGap1BLog) ;
   gasGap1BLog->SetSensitiveDetector(sensitive) ;

   G4Box* copper02B = GemBox("Copper02B", 35.*um) ;
   G4LogicalVolume* copper02BLog = new G4LogicalVolume(copper02B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper02BLog") ;
   copper02BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper02B) ;
   trdLogCollection.push_back(copper02BLog) ;

   G4Box* g10_1B = GemBox("g10_1B", 3.2*mm) ;
   G4LogicalVolume* g10_1BLog = new G4LogicalVolume(g10_1B, fFR4Mat, "G10_1BLog") ;
   g10_1BLog->SetVisAttributes(new G4VisAttributes(*g10Attributes)) ;
   trdCollection.push_back(g10_1B) ;
   trdLogCollection.push_back(g10_1BLog) ;

   G4Box* copper01B = GemBox("Copper01B", 35.*um) ;
   G4LogicalVolume* copper01BLog = new G4LogicalVolume(copper01B, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copper01BLog") ;
   copper01BLog->SetVisAttributes(new G4VisAttributes(*gemAttributes)) ;
   trdCollection.push_back(copper01B) ;
   trdLogCollection.push_back(copper01BLog) ;

   // Fake A
   G4Box* fakeA = GemBox("fakeA", 1.*nm) ;
   G4LogicalVolume* fakeALog = new G4LogicalVolume(fakeA, fEmptyMat, "fakeA") ;
   trdCollection.push_back(fakeA) ;
   trdLogCollection.push_back(fakeALog) ;
   fakeALog->SetSensitiveDetector(sensitive) ;


  
   G4double holePitch = 140.*um ;
   G4double spacingX = holePitch ;
   G4double spacingY = holePitch*sqrt(3.)/2 ;
   G4int NbOfHolesX = floor((tripleGemPx - 2.*80.E-3)/spacingX) ; // Dynamic computation of nb of holes 
   G4int NbOfHolesY = floor((tripleGemPy - 2.*80.E-3)/spacingY) ; // Dynamic computation of nb of holes
   //G4cout << tripleGemPx << " " << holePitch << " " << NbOfHolesY << G4endl ;
   G4double rmax = sqrt((35*35.+35*25.+25*25.)/3)*um; // for a volume equivalent to the true gem hole's one
   G4Tubs* hole = new G4Tubs("Hole", 0, rmax, 50.*um/2, 0.*deg, 360.*deg) ;
   G4LogicalVolume* holeLog = new G4LogicalVolume(hole, fGasMat, "holeLog") ;
   G4int holeCopyNo = 1000 ;
   G4double startX = -(tripleGemPx/2/um - 80)*um ; 
   G4double startY = -(tripleGemPy/2/um - 80)*um ;
   //G4cout << startX << " " << startY << G4endl ; 
   for(G4int i = 0; i < NbOfHolesY ; i++) {
      G4double positionY = startY + i*spacingY ;
      G4double xShift = 0. ;
      if(i%2 != 0) xShift = holePitch/2 ;
      for(G4int j = 0; j < NbOfHolesX ; j++) {
	 G4double positionX = startX + xShift + j*spacingX ;
	 G4ThreeVector position(positionX,positionY,0) ;
	 holeCopyNo++ ;
	 new G4PVPlacement(0,position,holeLog,"holePlacement1", kapton1BLog, false, holeCopyNo);
	 new G4PVPlacement(0,position,holeLog,"holePlacement2", kapton2BLog, false, 1E6+holeCopyNo);
	 new G4PVPlacement(0,position,holeLog,"holePlacement3", kapton3BLog, false, 2E6+holeCopyNo);
      }
   }
    

   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}

G4Trd* TrGEMSquareDetectorConstruction::Trapezoid(G4String name, G4double width) {
   G4Trd* shape = new G4Trd(name,
	 width/2, width/2,
	 tripleGemThinBase/2,
	 tripleGemLargeBase/2,
	 tripleGemHeight/2) ;
   return shape ;
}

G4Box* TrGEMSquareDetectorConstruction::GemBox(G4String name, G4double width) {
   G4Box* shape = new G4Box(
	 name,
	 tripleGemPx/2,
	 tripleGemPy/2,
	 width/2) ;
   return shape ;

}

void TrGEMSquareDetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {

   G4double ZTranslation = 0 ;

   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
      ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
      G4ThreeVector position = tlate + G4ThreeVector(0,0,ZTranslation).transform(G4RotationMatrix(*pRot).inverse()) ;
      G4cout << "Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " << G4BestUnit(ZTranslation,"Length") << G4endl ;
      new G4PVPlacement(pRot,
	    position,
	    trdLogCollection.at(i),
	    trdCollection.at(i)->GetName(),
	    pMotherLogical,
	    false,
	    i) ;
      ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
   }
}

