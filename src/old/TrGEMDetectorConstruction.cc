#include "TrGEMDetectorConstruction.hh"
#include "GasGapSensitiveDetector.hh"

// Geant4 includes
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSNofSecondary.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"

//3GEM includes
#include "MaterialFactory.hh"

using namespace std ;

TrGEMDetectorConstruction::TrGEMDetectorConstruction() :
   worldLog(0), worldPhys(0)
{
   tripleGemThinBase  = 220.0*mm ;
   tripleGemLargeBase = 445.0*mm ;
   tripleGemHeight    = 990.0*mm ;
}


TrGEMDetectorConstruction::~TrGEMDetectorConstruction()
{}

G4VPhysicalVolume* TrGEMDetectorConstruction::Construct() {

   //G4cout << "Construct method called !" << G4endl ;

   // NIST DataBase for G4 Elements and Materials
   G4NistManager *nistMan = G4NistManager::Instance() ;
   nistMan->SetVerbose(0) ;

   // Define the G4 World Box
   G4double worldBox_x = 1.*m ;
   G4double worldBox_y = 1.*m ;
   G4double worldBox_z = 10.*cm ;
   G4Box* worldBox = new G4Box("WorldBox", worldBox_x, worldBox_y, worldBox_z) ;
   G4Material* vacuum  = nistMan->FindOrBuildMaterial("G4_Galactic") ;
   worldLog = new G4LogicalVolume(worldBox, vacuum, "WorldLog", 0, 0, 0) ;
   G4VisAttributes *worldAttributes = new G4VisAttributes ;
   worldAttributes->SetVisibility(true) ;
   worldLog->SetVisAttributes(worldAttributes) ;

   G4RotationMatrix* rotationPlacement = new G4RotationMatrix() ;
   rotationPlacement->rotateY(M_PI / 2.0) ;
   rotationPlacement->rotateX(M_PI / 2.0) ;

   worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLog, "WorldSpace", 0, false, 0) ;

   //geometry = GeometryFactory::GetGeometry(geometryModel) ;
   //geometry->Build() ;
   //geometry->PlaceGeometry(rotationPlacement,G4ThreeVector(),worldVolume) ;

   //if(worldPhyVol == NULL) {
   //   LOG4CPLUS_FATAL(logFile,"Geometry has not been built ! worldPhyVol is NULL !") ;
   //   exit(1) ;
   //}
   //else LOG4CPLUS_INFO(logFile,"Geometry built") ;

   //
   // ====================================
   // T R I P L E  G E M  G E O M E T R Y
   // ====================================
   //

   // ----- Visual attributes (colors and visualization) --------------
   G4VisAttributes *cathodeLayerAttributes = new G4VisAttributes(G4Color::Grey()) ;
   cathodeLayerAttributes->SetForceWireframe(true) ;
   G4VisAttributes *g10LayerAttributes = new G4VisAttributes(G4Color::White()) ;
   g10LayerAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gasLayerAttributes = new G4VisAttributes(G4Color::Red()) ;
   gasLayerAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gemLayerAttributes = new G4VisAttributes(G4Color::Green()) ;
   gemLayerAttributes->SetForceWireframe(true) ;

   cathodeLayer = new TrdLayer("CathodeLayer"
	 ,(3./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al")) ;
   trdLayerCollection.push_back(cathodeLayer) ;
   cathodeLayer->SetVisAttributes(new G4VisAttributes(*cathodeLayerAttributes)) ;
   // I want Al to be sensitive. Temporary code, so names are not adapted.
   //cathodeLayer->SetUserLimits(new G4UserLimits (40*um) ) ;
   //G4VSensitiveDetector *gasSensitiveDetector = new GasSensitiveDetector("/GasGapSensitive") ;
   //G4SDManager::GetSDMpointer()->AddNewDetector(gasSensitiveDetector) ;
   //cathodeLayer->SetSensitiveDetector(gasSensitiveDetector) ;

   kapton0 = new TrdLayer("DriftCathodeFoil"
	 ,(250./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON")) ;
   trdLayerCollection.push_back(kapton0) ;
   kapton0->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   g10Layer1 = new TrdLayer("G10Layer1"
	 ,(3./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,MaterialFactory::GetMaterial("G10")) ;
   trdLayerCollection.push_back(g10Layer1) ;
   g10Layer1->SetVisAttributes(new G4VisAttributes(*g10LayerAttributes)) ;


   /*G4MultiFunctionalDetector *SDgasGap1 = new G4MultiFunctionalDetector("GasGapLayer1") ;
     G4VPrimitiveScorer* primitive ;
     primitive = new G4PSEnergyDeposit("eDep",0) ;
     SDgasGap1->RegisterPrimitive(primitive) ;
     primitive = new G4PSNofSecondary("nSecondaries",0) ;
     SDgasGap1->RegisterPrimitive(primitive) ;*/

   gasGap1 = new TrdLayer("GasGapLayer1"
	 ,(3./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,MaterialFactory::GetMaterial("Ar_45_CO2_15_CF4_40")) ;
   trdLayerCollection.push_back(gasGap1) ;
   gasGap1->SetVisAttributes(new G4VisAttributes(*gasLayerAttributes)) ;
   gasGap1->SetUserLimits(new G4UserLimits (1*um) ) ;
   GasGapSensitiveDetector* sensitive1 = new GasGapSensitiveDetector("/DriftGap") ;
   G4SDManager* sdman = G4SDManager::GetSDMpointer() ;
   sdman->AddNewDetector(sensitive1) ;
   gasGap1->SetSensitiveDetector(sensitive1) ;

   copper11 = new TrdLayer("CopperLayer11"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper11) ;
   copper11->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;
   // I want Cu to be sensitive. Temporary code, so names are not adapted.
   //copper11->SetUserLimits(new G4UserLimits (40*um) ) ; // was 40*um
   //G4VSensitiveDetector *gasSensitiveDetector = new GasSensitiveDetector("/GasGapSensitive") ;
   //G4SDManager::GetSDMpointer()->AddNewDetector(gasSensitiveDetector) ;
   //copper11->SetSensitiveDetector(gasSensitiveDetector) ;

   kapton1 = new TrdLayer("KaptonLayer1"
	 ,(50./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON")) ;
   trdLayerCollection.push_back(kapton1) ;
   kapton1->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   copper12 = new TrdLayer("CopperLayer12"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper12) ;
   copper12->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   gasGap2 = new TrdLayer("GasGapLayer2"
	 ,(1./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,MaterialFactory::GetMaterial("Ar_45_CO2_15_CF4_40")) ;
   trdLayerCollection.push_back(gasGap2) ;
   gasGap2->SetVisAttributes(new G4VisAttributes(*gasLayerAttributes)) ;
   gasGap2->SetUserLimits(new G4UserLimits (1*um) ) ;
   GasGapSensitiveDetector* sensitive2 = new GasGapSensitiveDetector("/TransferGap1") ;
   sdman->AddNewDetector(sensitive2) ;
   gasGap2->SetSensitiveDetector(sensitive2) ;

   copper21 = new TrdLayer("CopperLayer21"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper21) ;
   copper21->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   kapton2 = new TrdLayer("KaptonLayer2"
	 ,(50./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON")) ;
   trdLayerCollection.push_back(kapton2) ;
   kapton2->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   copper22 = new TrdLayer("CopperLayer22"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper22) ;
   copper22->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   gasGap3 = new TrdLayer("GasGapLayer3"
	 ,(2./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,MaterialFactory::GetMaterial("Ar_45_CO2_15_CF4_40")) ;
   trdLayerCollection.push_back(gasGap3) ;
   gasGap3->SetVisAttributes(new G4VisAttributes(*gasLayerAttributes)) ;
   gasGap3->SetUserLimits(new G4UserLimits (1*um) ) ;
   GasGapSensitiveDetector* sensitive3 = new GasGapSensitiveDetector("/TransferGap2") ;
   sdman->AddNewDetector(sensitive3) ;
   gasGap3->SetSensitiveDetector(sensitive3) ;

   copper31 = new TrdLayer("CopperLayer31"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper31) ;
   copper31->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   kapton3 = new TrdLayer("KaptonLayer3"
	 ,(50./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON")) ;
   trdLayerCollection.push_back(kapton3) ;
   kapton3->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   copper32 = new TrdLayer("CopperLayer32"
	 ,(5./2)*um
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu")) ;
   trdLayerCollection.push_back(copper32) ;
   copper32->SetVisAttributes(new G4VisAttributes(*gemLayerAttributes)) ;

   gasGap4 = new TrdLayer("GasGapLayer4"
	 ,(1./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,MaterialFactory::GetMaterial("Ar_45_CO2_15_CF4_40")) ;
   trdLayerCollection.push_back(gasGap4) ;
   gasGap4->SetVisAttributes(new G4VisAttributes(*gasLayerAttributes)) ;
   gasGap4->SetUserLimits(new G4UserLimits (1*um) ) ;
   GasGapSensitiveDetector* sensitive4 = new GasGapSensitiveDetector("/InductionGap") ;
   sdman->AddNewDetector(sensitive4) ;
   gasGap4->SetSensitiveDetector(sensitive4) ;

   // Spacer + Readout Board
   g10Layer2 = new TrdLayer("G10Layer2"
	 ,(3./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G10")) ; // was G4_BAKELITE
   trdLayerCollection.push_back(g10Layer2) ;
   g10Layer2->SetVisAttributes(new G4VisAttributes(*g10LayerAttributes)) ;

   // BOX cover (1 mm) + GEM cover (1 mm)
   readoutLayer = new TrdLayer("ReadoutLayer"
	 ,(2./2)*mm
	 ,tripleGemLargeBase/2
	 ,tripleGemThinBase/2
	 ,tripleGemHeight/2
	 ,G4NistManager::Instance()->FindOrBuildMaterial("G4_Al")) ;
   trdLayerCollection.push_back(readoutLayer) ;
   readoutLayer->SetVisAttributes(new G4VisAttributes(*cathodeLayerAttributes)) ;

   // ==================================== //
   // E N D  O F  3 G E M  G E O M E T R Y //
   // ==================================== //


   PlaceGeometry(rotationPlacement,G4ThreeVector(),worldLog) ;

   return worldPhys ;

}


void TrGEMDetectorConstruction::PlaceGeometry(G4RotationMatrix *pRot, G4ThreeVector tlate, G4LogicalVolume *pMotherLogical) {

   G4double XTranslation = 0 ;

   for(unsigned int i=0 ; i<trdLayerCollection.size() ; i++) {

      XTranslation += trdLayerCollection.at(i)->GetTrdSolid()->GetXHalfLength1() ;
      G4ThreeVector position = tlate + G4ThreeVector(XTranslation,0,0).transform(G4RotationMatrix(*pRot).inverse()) ;
      //cout << "Volume " << trdLayerCollection.at(i)->GetTrdSolid()->GetName() << " the position is " << XTranslation/mm << endl ;
      trdLayerCollection.at(i)->PlaceLayer(pRot , position , pMotherLogical, i) ;
      XTranslation += trdLayerCollection.at(i)->GetTrdSolid()->GetXHalfLength1() ;
   }
}

