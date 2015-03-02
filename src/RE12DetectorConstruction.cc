//NIM A 506 (2003) 101-109

#include "RE12DetectorConstruction.hh"
#include "GasGapSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
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

RE12DetectorConstruction::RE12DetectorConstruction() :
   fG10Mat(0), fGraphiteMat(0), fBakeliteMat(0), fGasMat(0), fEmptyMat(0), Al(0), fWoodMat(0), fAirMat(0),
   fGasDetectorCuts(0),
   tripleGemPx(0), tripleGemPy(0),
   AlPx(0), AlPy(0),AlPz(0),
   ScintPx(0), ScintPy(0), ScintPz(0),
   SchermoPx(0), SchermoPy(0),
   SETUP(0)
    
{
    //4 Possibili tipi di shielding
    //SETUP=1 - caso A - nessuno shielding
    //SETUP=2 - caso B - 6 cm Pb
    //SETUP=3 - caso C - 5 cm Pb + 4 cm Polietilene + 1 cm Pb
    //SETUP=4 - caso D - 5 cm Pb + 10 cm Polietilene + 1 cm Pb
   
     SETUP =1;
         
   //il rivelatore ha superficie attiva di 35x35 cm^2
   // Squared shape dimensions
 //  tripleGemPx = 200.*mm ;  
 //  tripleGemPy = 200.*mm ;
   tripleGemThinBase  = 489.0*mm ;
   tripleGemLargeBase = 805.0*mm ;
   tripleGemHeight    = 1804.0*mm ;

   //Supporto di alluminio      (dimensioni indicative)
   // Squared shape dimensions
   AlPx = 500.*mm ;  
   AlPy = 500.*mm ;
   AlPz = 5.*mm ;
 
   //Scintillatori     (dimensioni indicative)
   ScintPx = 200.*mm ;  
   ScintPy = 80.*mm ;
   ScintPz = 80.*mm ;

   //Schermo (dimensioni indicative)
   SchermoPx = 350.*mm ; 
   SchermoPy = 350.*mm ;


   //G4double cut = 1*mm ;
   G4double cut = 1*nm ;
   fGasDetectorCuts = new G4ProductionCuts() ;
   fGasDetectorCuts->SetProductionCut(cut,"gamma") ;
   fGasDetectorCuts->SetProductionCut(cut,"e-") ;
   fGasDetectorCuts->SetProductionCut(cut,"e+") ;
   fGasDetectorCuts->SetProductionCut(cut,"proton") ;

}

RE12DetectorConstruction::~RE12DetectorConstruction() {

   delete fGasDetectorCuts ;

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void RE12DetectorConstruction::DefineMaterials() {

   G4NistManager* manager = G4NistManager::Instance() ;
   // define Elements
   G4Element* elH  = manager->FindOrBuildElement(1);
   G4Element* elC  = manager->FindOrBuildElement(6);
   G4Element* elO  = manager->FindOrBuildElement(8);
   G4Element* elF  = manager->FindOrBuildElement(9);
   G4Element* elBa  = manager->FindOrBuildElement(56);
   G4Element* elN  = manager->FindOrBuildElement(7);
   G4Element* elS  = manager->FindOrBuildElement(16);

   // define Materials
   G4Material *H  = G4NistManager::Instance()->FindOrBuildMaterial("G4_H") ;
   G4Material *C  = G4NistManager::Instance()->FindOrBuildMaterial("G4_C") ;
   G4Material *O  = G4NistManager::Instance()->FindOrBuildMaterial("G4_O") ;
   G4Material *N  = G4NistManager::Instance()->FindOrBuildMaterial("G4_N") ;
   G4Material *Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si") ;
   G4Material *Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu") ;
   G4Material *Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al") ;
 

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
   fEmptyMat = empty ;

   // CF4 must be defined by hand
   G4int numel(0), natoms(0) ;
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


   // RPC mixture gas components
   // iso-Butane (methylpropane), STP
   density = 2.67*mg/cm3;
   G4Material* isobutane = new G4Material(name = "isoC4H10", density, numel=2) ;
   isobutane->AddElement(elC,4);
   isobutane->AddElement(elH,10);

   // Freon
   density = 4.55*mg/cm3;
   G4Material* C2H2F4 = new G4Material(name = "Freon", density, numel=3) ;
   C2H2F4->AddElement(elC, natoms=2);
   C2H2F4->AddElement(elH, natoms=2);
   C2H2F4->AddElement(elF, natoms=4);
 
   // esafluoruro di zolfo SF6
   density = 6.14*mg/cm3;
   G4Material* SF6 = new G4Material(name = "SF6", density, numel=2) ;
   SF6->AddElement(elS, natoms=1);
   SF6->AddElement(elF, natoms=6);

   //RPCgas
//    G4double fractionMass(0.) ;
//    density = 3.569*mg/cm3;
//    G4Material* RPCgas = new G4Material(name = "RPCGas", density, numel=2) ;
//    RPCgas->AddMaterial(isobutane, fractionMass = 3.* perCent) ;
//    RPCgas->AddMaterial(C2H2F4, fractionMass = 97.* perCent) ;
 
   G4double fractionMass(0.) ;
   density = 3.569*mg/cm3;
   G4Material* RPCgas = new G4Material(name = "RPCGas", density, numel=3) ;
   RPCgas->AddMaterial(isobutane, fractionMass = 4.5* perCent) ;
   RPCgas->AddMaterial(C2H2F4, fractionMass = 95.2* perCent) ;
   RPCgas->AddMaterial(SF6, fractionMass = 0.3* perCent) ;
  
   //Air
   G4Material* Air = new G4Material(name = "Air", density= 1.29*mg/cm3, numel=2);
    Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_N"), fractionMass = 70.*perCent);
    Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionMass = 30.*perCent);
   fAirMat = Air ;

   // Choice of the gas
   fGasMat = RPCgas ;

   // Graphite
   G4int z(0) ;
   G4double a(0.) ;
   G4Material* graphite = new G4Material("graphite", z=6, a= 12.0107*g/mole, density= 2.2*g/cm3);
   fGraphiteMat = graphite ;

   // Bakelite
   G4Material* bakelite = new G4Material("bakelite", density = 1.4*g/cm3, numel=3) ;
   bakelite->AddElement(elC, natoms=1) ;
   bakelite->AddElement(elH, natoms=4) ;
   bakelite->AddElement(elO, natoms=2) ;
   fBakeliteMat = bakelite ;
 
   // BaF2
   G4Material* scint_crystal = new G4Material("scint_crystal", density = 4.88*g/cm3, numel=2) ;
   scint_crystal->AddElement(elBa,natoms=1) ;
   scint_crystal->AddElement(elF, natoms=2) ;
   fScintMat = scint_crystal ;

   // Wood
   G4Material* wood = new G4Material(name="wood", density=0.9*g/cm3, numel=3);
   wood->AddElement(elH , natoms=4);
   wood->AddElement(elO , natoms=1);
   wood->AddElement(elC , natoms=2);
   fWoodMat = wood ;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

G4VPhysicalVolume* RE12DetectorConstruction::Construct() {


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

//***************************************************
//     WORLD
//***************************************************

   G4double worldSizeX = 2*m ;
   G4double worldSizeY = 2*m ;
   G4double worldSizeZ = 2*m ;

   // World definition and placement
   G4Box* worldBox = new G4Box("WorldBox", worldSizeX, worldSizeY, worldSizeZ) ;
   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fEmptyMat, "WorldLog") ;
 //  G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fAirMat, "WorldLog") ;
   // Set visual attributes
   G4VisAttributes *worldAttributes = new G4VisAttributes ;
   worldAttributes->SetVisibility(true) ;
   worldLog->SetVisAttributes(worldAttributes) ;
   G4VPhysicalVolume* worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLog, "WorldSpace", 0, false, 0) ;

   // Rotation Matrix for layers
   G4RotationMatrix* rotationPlacement = new G4RotationMatrix() ;
   rotationPlacement->rotateY(M_PI / 2.0) ;
   //rotationPlacement->rotateY(0) ;
   rotationPlacement->rotateX(M_PI / 2.0) ;
   //rotationPlacement->rotateX(0) ;


//***************************************************
//     VISUAL ATTRIBUTES
//***************************************************

   // Visual attributes
   G4VisAttributes *cathodeAttributes = new G4VisAttributes(G4Color::Grey()) ;
   cathodeAttributes->SetForceWireframe(true) ;
   G4VisAttributes *insAttributes = new G4VisAttributes(G4Color::White()) ;
   insAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gasAttributes = new G4VisAttributes(G4Color::Red()) ;
   gasAttributes->SetForceWireframe(true) ;
   G4VisAttributes *rpcAttributes = new G4VisAttributes(G4Color::Green()) ;
   rpcAttributes->SetForceWireframe(true) ;

//***************************************************
//     DUOBLE GAP RPC
//***************************************************
  
   G4Trd* FakeBottom = RPCBox("FakeBottom", 0.1*mm) ;
   G4LogicalVolume* FakeBottomLog = new G4LogicalVolume(FakeBottom, fAirMat, "FakeBottomLog") ;
   FakeBottomLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(FakeBottom) ;
   trdLogCollection.push_back(FakeBottomLog) ;
   GasGapSensitiveDetector* sensitive = new GasGapSensitiveDetector("/GasGap") ;
   sdman->AddNewDetector(sensitive) ;
   FakeBottomLog->SetSensitiveDetector(sensitive) ;

//***************************************************

   G4Trd* copperBottom = RPCBox("copperBottom", 0.005*cm) ;
   G4LogicalVolume* copperBottomLog = new G4LogicalVolume(copperBottom, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperBottomLog") ;
   copperBottomLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(copperBottom) ;
   trdLogCollection.push_back(copperBottomLog) ;
copperBottomLog->SetSensitiveDetector(sensitive);

//-------------------------------------------------------------------------------------

   G4Trd* pethylene0 = RPCBox("pethylene0", 0.06*cm) ;
   G4LogicalVolume* pethylene0Log = new G4LogicalVolume(pethylene0, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene0Log") ;
   pethylene0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene0) ;
   trdLogCollection.push_back(pethylene0Log) ;
   pethylene0Log->SetSensitiveDetector(sensitive);
   
//-------------------------------------------------------------------------------------
   
   G4Trd* graphite0 = RPCBox("graphite0", 0.002*cm) ;
   G4LogicalVolume* graphite0Log = new G4LogicalVolume(graphite0, fGraphiteMat, "graphite0Log") ;
   graphite0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite0) ;
   trdLogCollection.push_back(graphite0Log) ;
   graphite0Log->SetSensitiveDetector(sensitive);
   
//-------------------------------------------------------------------------------------

   G4Trd* bakelite0 = RPCBox("bakelite0", 0.2*cm) ;
   G4LogicalVolume* bakelite0Log = new G4LogicalVolume(bakelite0, fBakeliteMat, "bakelite0Log") ; 
   bakelite0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite0) ;
   trdLogCollection.push_back(bakelite0Log) ;
   bakelite0Log->SetSensitiveDetector(sensitive);
   
//-------------------------------------------------------------------------------------
   
   G4Trd* GasGap1 = RPCBox("GasGap1", 0.2*cm) ;
   G4LogicalVolume* GasGap1Log = new G4LogicalVolume(GasGap1, fGasMat, "GasGap1Log") ; 
   GasGap1Log->SetVisAttributes(new G4VisAttributes(*rpcAttributes)) ;
   trdCollection.push_back(GasGap1) ;
   trdLogCollection.push_back(GasGap1Log) ;
   GasGap1Log->SetSensitiveDetector(sensitive) ;
   
   
//-------------------------------------------------------------------------------------

   G4Trd* bakelite1 = RPCBox("bakelite1", 0.2*cm) ;
   G4LogicalVolume* bakelite1Log = new G4LogicalVolume(bakelite1, fBakeliteMat, "bakelite1Log") ; 
   bakelite1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite1) ;
   trdLogCollection.push_back(bakelite1Log) ;
   bakelite1Log->SetSensitiveDetector(sensitive);
   
//-------------------------------------------------------------------------------------
  
   G4Trd* graphite1 = RPCBox("graphite1", 0.002*cm) ;
   G4LogicalVolume* graphite1Log = new G4LogicalVolume(graphite1, fGraphiteMat, "graphite1Log") ;
   graphite1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite1) ;
   trdLogCollection.push_back(graphite1Log) ;
   graphite1Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------
  
   G4Trd* pethylene1 = RPCBox("pethylene1", 0.06*cm) ;
   G4LogicalVolume* pethylene1Log = new G4LogicalVolume(pethylene1,       G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene1Log") ;
   pethylene1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene1) ;
   trdLogCollection.push_back(pethylene1Log) ;
   pethylene1Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------
  
   G4Trd* copperStrips = RPCBox("copperStrips", 0.005*cm) ;
   G4LogicalVolume* copperStripsLog = new G4LogicalVolume(copperStrips, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperStripsLog") ;
   copperStripsLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(copperStrips) ;
   trdLogCollection.push_back(copperStripsLog) ;
   copperStripsLog->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------
  
   G4Trd* pethylene2 = RPCBox("pethylene2", 0.06*cm) ;
   G4LogicalVolume* pethylene2Log = new G4LogicalVolume(pethylene2,       G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene2Log") ;
   pethylene2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene2) ;
   trdLogCollection.push_back(pethylene2Log) ;
   pethylene2Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------
  
   G4Trd* graphite2 = RPCBox("graphite2", 0.002*cm) ;
   G4LogicalVolume* graphite2Log = new G4LogicalVolume(graphite2, fGraphiteMat, "graphite2Log") ;
   graphite2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite2) ;
   trdLogCollection.push_back(graphite2Log) ;
   graphite2Log->SetSensitiveDetector(sensitive) ;

//-------------------------------------------------------------------------------------
 
   G4Trd* bakelite2 = RPCBox("bakelite2", 0.2*cm) ;
   G4LogicalVolume* bakelite2Log = new G4LogicalVolume(bakelite2, fBakeliteMat, "bakelite2Log") ; 
   bakelite2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite2) ;
   trdLogCollection.push_back(bakelite2Log) ;
   bakelite2Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------

   G4Trd* GasGap2 = RPCBox("GasGap2", 0.2*cm) ;
   G4LogicalVolume* GasGap2Log = new G4LogicalVolume(GasGap2, fGasMat, "GasGap2Log") ; 
   GasGap2Log->SetVisAttributes(new G4VisAttributes(*rpcAttributes)) ;
   trdCollection.push_back(GasGap2) ;
   trdLogCollection.push_back(GasGap2Log) ;
   GasGap2Log->SetSensitiveDetector(sensitive) ;
   
   
//-------------------------------------------------------------------------------------

   G4Trd* bakelite3 = RPCBox("bakelite3", 0.2*cm) ;
   G4LogicalVolume* bakelite3Log = new G4LogicalVolume(bakelite3, fBakeliteMat, "bakelite3Log") ; 
   bakelite3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite3) ;
   trdLogCollection.push_back(bakelite3Log) ;
   bakelite3Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------
  
   G4Trd* graphite3 = RPCBox("graphite3", 0.002*cm) ;
   G4LogicalVolume* graphite3Log = new G4LogicalVolume(graphite3,  fGraphiteMat, "graphite3Log") ;
   graphite3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite3) ;
   trdLogCollection.push_back(graphite3Log) ;
   graphite3Log->SetSensitiveDetector(sensitive) ;

//-------------------------------------------------------------------------------------

   G4Trd* pethylene3 = RPCBox("pethylene3", 0.06*cm) ;
   G4LogicalVolume* pethylene3Log = new G4LogicalVolume(pethylene3, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene3Log") ;
   pethylene3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene3) ;
   trdLogCollection.push_back(pethylene3Log) ;
   pethylene3Log->SetSensitiveDetector(sensitive) ;
   
//-------------------------------------------------------------------------------------

 
   G4Trd* copperTop = RPCBox("copperTop", 0.005*cm) ;
   G4LogicalVolume* copperTopLog = new G4LogicalVolume(copperTop, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperTopLog") ;
   copperTopLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(copperTop) ;
   trdLogCollection.push_back(copperTopLog) ;
   copperTopLog->SetSensitiveDetector(sensitive) ;
   

//-------------------------------------------------------------------------------------
   G4Trd* FakeTop = RPCBox("FakeTop", 0.1*mm) ;
   G4LogicalVolume* FakeTopLog = new G4LogicalVolume(FakeTop, fAirMat, "FakeTopLog") ;
   FakeTopLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(FakeTop) ;
   trdLogCollection.push_back(FakeTopLog) ;
    FakeTopLog->SetSensitiveDetector(sensitive) ;




            
   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}


//***************************************************
//     DOUBLE GAP RPC
//***************************************************

//RPC
G4Trd* RE12DetectorConstruction::RPCBox(G4String name, G4double width) {
   G4Trd* shape = new G4Trd(
  //G4Trd* shape = new G4Trd(name,
         name,
	 width/2, width/2,
	 tripleGemThinBase/2,
	 tripleGemLargeBase/2,
	 tripleGemHeight/2) ;
   return shape ;
}
   

void RE12DetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {

G4cout<<"SETUP "<<SETUP<<G4endl;
//G4bool checkOverlaps = true;
G4double ZTranslation = 0 ;
G4double XTranslation = 0 ;
G4ThreeVector position;
G4double halflenght = 0;
   
   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
         XTranslation += trdCollection.at(i)->GetXHalfLength1() ;
         position = tlate +        G4ThreeVector(XTranslation,0,0).transform(G4RotationMatrix(*pRot).inverse()) ;
         G4cout << "RPC     Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " << G4BestUnit(XTranslation,"Length") << G4endl ;
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
