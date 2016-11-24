//NIM A 506 (2003) 101-109

#include "CfRPCDetectorConstruction.hh"
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
#include "G4Tubs.hh"

CfRPCDetectorConstruction::CfRPCDetectorConstruction() :
   fG10Mat(0), fGraphiteMat(0), fBakeliteMat(0), fGasMat(0), fEmptyMat(0), fWoodMat(0), fAirMat(0),
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

   SETUP = 4;

   //il rivelatore ha superficie attiva di 35x35 cm^2
   // Squared shape dimensions
   tripleGemPx = 350.*mm ;  
   tripleGemPy = 350.*mm ;

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

CfRPCDetectorConstruction::~CfRPCDetectorConstruction() {

   delete fGasDetectorCuts ;

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CfRPCDetectorConstruction::DefineMaterials() {

   G4NistManager* manager = G4NistManager::Instance() ;
   // define Elements
   G4Element* elH  = manager->FindOrBuildElement(1);
   G4Element* elC  = manager->FindOrBuildElement(6);
   G4Element* elO  = manager->FindOrBuildElement(8);
   G4Element* elF  = manager->FindOrBuildElement(9);
   G4Element* elBa  = manager->FindOrBuildElement(56);
   G4Element* elN  = manager->FindOrBuildElement(7);

   // define Materials
   G4Material *H  = G4NistManager::Instance()->FindOrBuildMaterial("G4_H") ;
   G4Material *C  = G4NistManager::Instance()->FindOrBuildMaterial("G4_C") ;
   G4Material *O  = G4NistManager::Instance()->FindOrBuildMaterial("G4_O") ;
   G4Material *N  = G4NistManager::Instance()->FindOrBuildMaterial("G4_N") ;
   G4Material *Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si") ;
   G4Material *Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu") ;

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

   //RPCgas
   G4double fractionMass(0.) ;
   density = 3.569*mg/cm3;
   G4Material* RPCgas = new G4Material(name = "RPCGas", density, numel=2) ;
   RPCgas->AddMaterial(isobutane, fractionMass = 3.* perCent) ;
   RPCgas->AddMaterial(C2H2F4, fractionMass = 97.* perCent) ;

   //Air
   //G4Material* Air = new G4Material(name = "Air", density= 1.29*mg/cm3, numel=2);
   //Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_N"), fractionMass = 70.*perCent);
   //Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionMass = 30.*perCent);
   G4Material* Air = manager->FindOrBuildMaterial("G4_AIR"); 
   fAirMat = Air ;

   // Choice of the gas
   fGasMat = RPCgas ;

   // Graphite
   G4int z(0) ;
   G4double a(0.) ;
   //G4Material* graphite = new G4Material("graphite", z=6, a= 12.0107*g/mole, density= 2.2*g/cm3);
   G4Material* graphite = manager->FindOrBuildMaterial("G4_GRAPHITE"); 
   fGraphiteMat = graphite ;

   // Bakelite
   //G4Material* bakelite = new G4Material("bakelite", density = 1.4*g/cm3, numel=3) ;
   //bakelite->AddElement(elC, natoms=1) ;
   //bakelite->AddElement(elH, natoms=4) ;
   //bakelite->AddElement(elO, natoms=2) ;
   G4Material* bakelite = manager->FindOrBuildMaterial("G4_BAKELITE"); 
   fBakeliteMat = bakelite ;

   // BaF2
   //G4Material* scint_crystal = new G4Material("scint_crystal", density = 4.88*g/cm3, numel=2) ;
   //scint_crystal->AddElement(elBa,natoms=1) ;
   //scint_crystal->AddElement(elF, natoms=2) ;
   G4Material* scint_crystal = manager->FindOrBuildMaterial("G4_BARIUM_FLUORIDE"); 
   fScintMat = scint_crystal ;

   // Wood
   G4Material* wood = new G4Material(name="wood", density=0.9*g/cm3, numel=4);
   wood->AddElement(elH , fractionMass=6.*perCent);
   wood->AddElement(elO , fractionMass=43.*perCent); //was 42%
   wood->AddElement(elC , fractionMass=50.*perCent);
   wood->AddElement(elN , fractionMass=1.*perCent);
   fWoodMat = wood ;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

G4VPhysicalVolume* CfRPCDetectorConstruction::Construct() {


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
   //   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fEmptyMat, "WorldLog") ;
   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, fAirMat, "WorldLog") ;
   // Set visual attributes
   G4VisAttributes *worldAttributes = new G4VisAttributes ;
   worldAttributes->SetVisibility(true) ;
   worldLog->SetVisAttributes(worldAttributes) ;
   G4VPhysicalVolume* worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLog, "WorldSpace", 0, false, 0) ;

   // Rotation Matrix for layers
   G4RotationMatrix* rotationPlacement = new G4RotationMatrix() ;
   //rotationPlacement->rotateY(M_PI / 2.0) ;
   rotationPlacement->rotateY(0) ;
   //rotationPlacement->rotateX(M_PI / 2.0) ;
   rotationPlacement->rotateX(0) ;


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
   //     DOUBLE GAP RPC
   //***************************************************

   // Fake A
   G4Box* fakeA = RPCBox("fakeA", 1.*nm) ;
   G4LogicalVolume* fakeALog = new G4LogicalVolume(fakeA, fEmptyMat, "fakeA") ;
   trdCollection.push_back(fakeA) ;
   trdLogCollection.push_back(fakeALog) ;
   GasGapSensitiveDetector* sensitive = new GasGapSensitiveDetector("/GasGap") ;
   sdman->AddNewDetector(sensitive) ;
   fakeALog->SetSensitiveDetector(sensitive) ;
   //-------------------------------------------------------------------------------------

   G4Box* woodBottom = RPCBox("woodBottom", 10.*mm) ;
   G4LogicalVolume* woodBottomLog = new G4LogicalVolume(woodBottom, fWoodMat, "woodBottomLog") ;
   woodBottomLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(woodBottom) ;
   trdLogCollection.push_back(woodBottomLog) ;
   //-------------------------------------------------------------------------------------

   G4Box* copperBottom = RPCBox("copperBottom", 0.019*mm) ;
   G4LogicalVolume* copperBottomLog = new G4LogicalVolume(copperBottom, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperBottomLog") ;
   copperBottomLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(copperBottom) ;
   trdLogCollection.push_back(copperBottomLog) ;

   //-------------------------------------------------------------------------------------

   G4Box* pethylene0 = RPCBox("pethylene0", 0.38*mm) ;
   G4LogicalVolume* pethylene0Log = new G4LogicalVolume(pethylene0, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene0Log") ;
   pethylene0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene0) ;
   trdLogCollection.push_back(pethylene0Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* graphite0 = RPCBox("graphite0", 0.02*mm) ;
   G4LogicalVolume* graphite0Log = new G4LogicalVolume(graphite0, fGraphiteMat, "graphite0Log") ;
   graphite0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite0) ;
   trdLogCollection.push_back(graphite0Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* bakelite0 = RPCBox("bakelite0", 2.*mm) ;
   G4LogicalVolume* bakelite0Log = new G4LogicalVolume(bakelite0, fBakeliteMat, "bakelite0Log") ; 
   bakelite0Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite0) ;
   trdLogCollection.push_back(bakelite0Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* GasGap1 = RPCBox("GasGap1B", 2.*mm) ;
   G4LogicalVolume* GasGap1Log = new G4LogicalVolume(GasGap1, fGasMat, "GasGap1BLog") ; 
   GasGap1Log->SetVisAttributes(new G4VisAttributes(*rpcAttributes)) ;
   trdCollection.push_back(GasGap1) ;
   trdLogCollection.push_back(GasGap1Log) ;
   GasGap1Log->SetSensitiveDetector(sensitive) ;
   //-------------------------------------------------------------------------------------

   G4Box* bakelite1 = RPCBox("bakelite1", 2.*mm) ;
   G4LogicalVolume* bakelite1Log = new G4LogicalVolume(bakelite1, fBakeliteMat, "bakelite1Log") ; 
   bakelite1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite1) ;
   trdLogCollection.push_back(bakelite1Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* graphite1 = RPCBox("graphite1", 0.02*mm) ;
   G4LogicalVolume* graphite1Log = new G4LogicalVolume(graphite1, fGraphiteMat, "graphite1Log") ;
   graphite1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite1) ;
   trdLogCollection.push_back(graphite1Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* pethylene1 = RPCBox("pethylene1", 0.38*cm) ;
   G4LogicalVolume* pethylene1Log = new G4LogicalVolume(pethylene1,       G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene1Log") ;
   pethylene1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene1) ;
   trdLogCollection.push_back(pethylene1Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* copperStrips = RPCBox("copperStrips", 0.019*mm) ;
   G4LogicalVolume* copperStripsLog = new G4LogicalVolume(copperStrips, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperStripsLog") ;
   copperStripsLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(copperStrips) ;
   trdLogCollection.push_back(copperStripsLog) ;
   //-------------------------------------------------------------------------------------

   G4Box* pethylene2 = RPCBox("pethylene2", 0.38*mm) ;
   G4LogicalVolume* pethylene2Log = new G4LogicalVolume(pethylene2, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene2Log") ;
   pethylene2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene2) ;
   trdLogCollection.push_back(pethylene2Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* graphite2 = RPCBox("graphite2", 0.02*mm) ;
   G4LogicalVolume* graphite2Log = new G4LogicalVolume(graphite2, fGraphiteMat, "graphite2Log") ;
   graphite2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite2) ;
   trdLogCollection.push_back(graphite2Log) ;

   //-------------------------------------------------------------------------------------

   G4Box* bakelite2 = RPCBox("bakelite2", 2.*mm) ;
   G4LogicalVolume* bakelite2Log = new G4LogicalVolume(bakelite2, fBakeliteMat, "bakelite2Log") ; 
   bakelite2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite2) ;
   trdLogCollection.push_back(bakelite2Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* GasGap2 = RPCBox("GasGap2B", 2.*mm) ;
   G4LogicalVolume* GasGap2Log = new G4LogicalVolume(GasGap2, fGasMat, "GasGap2BLog") ; 
   GasGap2Log->SetVisAttributes(new G4VisAttributes(*rpcAttributes)) ;
   trdCollection.push_back(GasGap2) ;
   trdLogCollection.push_back(GasGap2Log) ;
   GasGap2Log->SetSensitiveDetector(sensitive) ;
   //-------------------------------------------------------------------------------------

   G4Box* bakelite3 = RPCBox("bakelite3", 2.*mm) ;
   G4LogicalVolume* bakelite3Log = new G4LogicalVolume(bakelite3, fBakeliteMat, "bakelite3Log") ; 
   bakelite3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(bakelite3) ;
   trdLogCollection.push_back(bakelite3Log) ;
   //-------------------------------------------------------------------------------------

   G4Box* graphite3 = RPCBox("graphite3", 0.02*mm) ;
   G4LogicalVolume* graphite3Log = new G4LogicalVolume(graphite3,  fGraphiteMat, "graphite3Log") ;
   graphite3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(graphite3) ;
   trdLogCollection.push_back(graphite3Log) ;

   //-------------------------------------------------------------------------------------

   G4Box* pethylene3 = RPCBox("pethylene3", 0.38*mm) ;
   G4LogicalVolume* pethylene3Log = new G4LogicalVolume(pethylene3, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "pethylene3Log") ;
   pethylene3Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(pethylene3) ;
   trdLogCollection.push_back(pethylene3Log) ;
   //-------------------------------------------------------------------------------------


   G4Box* copperTop = RPCBox("copperTop", 0.019*mm) ;
   G4LogicalVolume* copperTopLog = new G4LogicalVolume(copperTop, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "copperTopLog") ;
   copperTopLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(copperTop) ;
   trdLogCollection.push_back(copperTopLog) ;
   //-------------------------------------------------------------------------------------

   G4Box* woodTop = RPCBox("woodTop", 10.*mm) ;
   G4LogicalVolume* woodTopLog = new G4LogicalVolume(woodTop, fWoodMat, "woodTopLog") ;
   woodTopLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(woodTop) ;
   trdLogCollection.push_back(woodTopLog) ;
   //-------------------------------------------------------------------------------------

   G4Box* aluminiumTop = RPCBox("aluminiumTop", 2.5*mm) ;
   G4LogicalVolume* aluminiumTopLog = new G4LogicalVolume(aluminiumTop, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "aluminiumTopLog") ;
   aluminiumTopLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(aluminiumTop) ;
   trdLogCollection.push_back(aluminiumTopLog) ;
   //-------------------------------------------------------------------------------------

   // Fake B
   G4Box* fakeB = RPCBox("fakeB", 1.*nm) ;
   G4LogicalVolume* fakeBLog = new G4LogicalVolume(fakeB, fEmptyMat, "fakeB") ;
   trdCollection.push_back(fakeB) ;
   trdLogCollection.push_back(fakeBLog) ;
   fakeBLog->SetSensitiveDetector(sensitive) ;

   //***************************************************
   //     ALUMINUM SUPPORT
   //*************************************************** 

   //due supporti di alluminio

   G4Box* AlBox =    
      new G4Box(
	    "Alsupport",                    //its name
	    0.5*AlPx,
	    0.5*AlPy,
	    0.5*AlPz); //its size

   G4LogicalVolume* AlsupportLog =
      new G4LogicalVolume(
	    AlBox,            //its solid
	    G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),             //its material
	    "Alsupport");         //its name
   AlsupportLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(AlBox) ;
   trdLogCollection.push_back(AlsupportLog) ;

   G4Box* Al2Box =    
      new G4Box(
	    "Al2support",                    //its name
	    0.5*AlPx,
	    0.5*AlPy,
	    0.5*AlPz); //its size

   G4LogicalVolume* Al2supportLog =
      new G4LogicalVolume(
	    AlBox,            //its solid
	    G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),             //its material
	    "Al2support");         //its name
   Al2supportLog->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
   trdCollection.push_back(Al2Box) ;
   trdLogCollection.push_back(Al2supportLog) ;

   //***************************************************
   //     SCINTILLATORS
   //***************************************************

   G4Box* Scint1Box = 
      new G4Box(
	    "Scint1",
	    ScintPx/2,
	    ScintPy/2,
	    ScintPz/2) ;

   G4LogicalVolume* Scint1Log =
      new G4LogicalVolume(
	    Scint1Box,            //its solid
	    fScintMat,           //its material
	    "Scint1");         //its name
   Scint1Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(Scint1Box) ;
   trdLogCollection.push_back(Scint1Log) ;

   G4Box* Scint2Box = 
      new G4Box(
	    "Scint2",
	    ScintPx/2,
	    ScintPy/2,
	    ScintPz/2) ;

   G4LogicalVolume* Scint2Log =
      new G4LogicalVolume(
	    Scint2Box,            //its solid
	    fScintMat,           //its material
	    "Scint2");         //its name
   Scint2Log->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
   trdCollection.push_back(Scint2Box) ;
   trdLogCollection.push_back(Scint2Log) ;


   //***************************************************
   //     SHIELDING
   //***************************************************

   if(SETUP==2){
      //-------------------------------------------------------------------------------------

      G4Box* PbLayer1 = SchermoBox("PbLayer1", 6*cm) ;
      G4LogicalVolume* PbLayer1Log = new G4LogicalVolume(PbLayer1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "PbLayer1Log") ;
      PbLayer1Log->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
      trdCollection.push_back(PbLayer1) ;
      trdLogCollection.push_back(PbLayer1Log) ;
      //-------------------------------------------------------------------------------------

   }
   if(SETUP==3){
      //-------------------------------------------------------------------------------------

      G4Box* PbLayer1 = SchermoBox("PbLayer1", 5*cm) ;
      G4LogicalVolume* PbLayer1Log = new G4LogicalVolume(PbLayer1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "PbLayer1Log") ;
      PbLayer1Log->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
      trdCollection.push_back(PbLayer1) ;
      trdLogCollection.push_back(PbLayer1Log) ;
      //-------------------------------------------------------------------------------------
      G4Box* PELayer = RPCBox("PELayer", 4*cm) ;
      G4LogicalVolume* PELayerLog = new G4LogicalVolume(PELayer, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "PELayerLog") ;
      PELayerLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
      trdCollection.push_back(PELayer) ;
      trdLogCollection.push_back(PELayerLog) ;
      //-------------------------------------------------------------------------------------
      G4Box* PbLayer2 = SchermoBox("PbLayer2", 1*cm) ;
      G4LogicalVolume* PbLayer2Log = new G4LogicalVolume(PbLayer2, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "PbLayer2Log") ;
      PbLayer2Log->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
      trdCollection.push_back(PbLayer2) ;
      trdLogCollection.push_back(PbLayer2Log) ;

   }

   if(SETUP==4){
      //-------------------------------------------------------------------------------------

      G4Box* PbLayer1 = SchermoBox("PbLayer1", 5*cm) ;
      G4LogicalVolume* PbLayer1Log = new G4LogicalVolume(PbLayer1, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "PbLayer1Log") ;
      PbLayer1Log->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
      trdCollection.push_back(PbLayer1) ;
      trdLogCollection.push_back(PbLayer1Log) ;
      //-------------------------------------------------------------------------------------
      G4Box* PELayer = RPCBox("PELayer", 10*cm) ;
      G4LogicalVolume* PELayerLog = new G4LogicalVolume(PELayer, G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE"), "PELayerLog") ;
      PELayerLog->SetVisAttributes(new G4VisAttributes(*insAttributes)) ;
      trdCollection.push_back(PELayer) ;
      trdLogCollection.push_back(PELayerLog) ;
      //-------------------------------------------------------------------------------------
      G4Box* PbLayer2 = SchermoBox("PbLayer2", 1*cm) ;
      G4LogicalVolume* PbLayer2Log = new G4LogicalVolume(PbLayer2, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "PbLayer2Log") ;
      PbLayer2Log->SetVisAttributes(new G4VisAttributes(*cathodeAttributes)) ;
      trdCollection.push_back(PbLayer2) ;
      trdLogCollection.push_back(PbLayer2Log) ;

   }

   PlaceGeometry(rotationPlacement,G4ThreeVector(0.,0.,0.),worldLog) ;

   return worldPhys ;

}
//***************************************************
//     SHIELDING
//***************************************************

G4Box* CfRPCDetectorConstruction::SchermoBox(G4String name, G4double width) {
   G4Box* shape = new G4Box(
	 name,
	 SchermoPx/2,
	 SchermoPy/2,
	 width/2) ;
   return shape ;


}


//***************************************************
//     DOUBLE GAP RPC
//***************************************************

//RPC
G4Box* CfRPCDetectorConstruction::RPCBox(G4String name, G4double width) {
   G4Box* shape = new G4Box(
	 name,
	 tripleGemPx/2,
	 tripleGemPy/2,
	 width/2) ;
   return shape ;
}


void CfRPCDetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume* pMotherLogical) {

   G4cout<<"SETUP "<<SETUP<<G4endl;
   //G4bool checkOverlaps = true;
   G4double ZTranslation = 0 ;
   G4double XTranslation = 0 ;
   G4ThreeVector position;
   G4double halflenght = 0;

   for(size_t i=0 ; i<trdCollection.size() ; i++) {
      // i counts as the copyNo
      if(i<22){
	 ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
	 position = tlate +        G4ThreeVector(0,0,ZTranslation).transform(G4RotationMatrix(*pRot).inverse()) ;
	 G4cout << "RPC     Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " << G4BestUnit(ZTranslation,"Length") << G4endl ;
	 new G4PVPlacement(pRot,
	       position,
	       trdLogCollection.at(i),
	       trdCollection.at(i)->GetName(),
	       pMotherLogical,
	       false,
	       i) ;
	 ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
      }
      //_________________________________________________________________________________________________________________


      if(i==22){ //Al support
	 //ZTranslation=0;
	 //ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
	 //position = G4ThreeVector(0,0,-50)+G4ThreeVector(0,0,ZTranslation ) ;
	 //new G4PVPlacement(0, 		    //no rotation
	 //      position,         
	 //      trdLogCollection.at(i),	    //its logical volume
	 //      trdCollection.at(i)->GetName(),  	    //its name
	 //      pMotherLogical,	   //its mother  volume
	 //      false,		   //no boolean operation
	 //      i			   //copy number
	 //      );	    
	 //G4cout << "        Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " <<G4BestUnit(position.getZ(),"Length") << G4endl ;

      }
      //_____________________________________________________________________________________________________________________

      if(i==24){ //Scint1
	 XTranslation = trdCollection.at(i)->GetXHalfLength() ;
	 position = G4ThreeVector(30,0,-300)+G4ThreeVector(XTranslation,0,0 ) ;
	 new G4PVPlacement(0, 		    //no rotation
	       position,         
	       trdLogCollection.at(i),	    //its logical volume
	       trdCollection.at(i)->GetName(),  	    //its name
	       pMotherLogical,	   //its mother  volume
	       false,		   //no boolean operation
	       i			   //copy number
	       );	    
	 G4cout << "        Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " <<G4BestUnit(position.getZ(),"Length") << G4endl ;


      }

      if(i==25){ //Scint1
	 XTranslation = trdCollection.at(i)->GetXHalfLength() ;
	 position = G4ThreeVector(-30,0,-300)-G4ThreeVector(XTranslation,0,0 ) ;
	 new G4PVPlacement(0, 		    //no rotation
	       position,         
	       trdLogCollection.at(i),	    //its logical volume
	       trdCollection.at(i)->GetName(),  	    //its name
	       pMotherLogical,	   //its mother  volume
	       false,		   //no boolean operation
	       i			   //copy number
	       );	    
	 G4cout << "        Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " <<G4BestUnit(position.getZ(),"Length") << G4endl ;

	 ZTranslation=0;
      }

      //_________________________________________________________________________________________________________________


      if(i>25){
	 ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
	 position = G4ThreeVector(0,0,-240) +G4ThreeVector(0,0,ZTranslation).transform(G4RotationMatrix(*pRot).inverse()) ;
	 G4cout << "SCHERMO Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " << G4BestUnit(position.getZ(),"Length") << G4endl ;
	 new G4PVPlacement(pRot,
	       position,
	       trdLogCollection.at(i),
	       trdCollection.at(i)->GetName(),
	       pMotherLogical,
	       false,
	       i) ;
	 ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
      }

      if(i==23){ //Al support
	 ZTranslation=0;
	 ZTranslation += trdCollection.at(i)->GetZHalfLength() ;
	 position = G4ThreeVector(0,0,-340)-G4ThreeVector(0,0,ZTranslation ) ;
	 new G4PVPlacement(0, 		    //no rotation
	       position,         
	       trdLogCollection.at(i),	    //its logical volume
	       trdCollection.at(i)->GetName(),  	    //its name
	       pMotherLogical,	   //its mother  volume
	       false,		   //no boolean operation
	       i			   //copy number
	       );	    
	 G4cout << "        Volume (" << i << ") " << trdCollection.at(i)->GetName() << " the position is " <<G4BestUnit(position.getZ(),"Length") << G4endl ;

      }


   }

}
