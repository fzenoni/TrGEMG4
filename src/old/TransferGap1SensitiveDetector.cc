#include "TransferGap1SensitiveDetector.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4UnitsTable.hh"

#include "TrGEMAnalysis.hh"

TransferGap1SensitiveDetector::TransferGap1SensitiveDetector(G4String SDname)
   : G4VSensitiveDetector(SDname), layerIndex(0), isSensitive(0) {
      G4cout << "Creating SD with name: " << SDname << G4endl ;
      // 'collectionName' is a protected data member of base class G4VSensitiveDetector.
      // Here we declare the name of the collection we will be using.
      ////G4String myCollectionName = "GasGapHitCollection" ;

      // Note that we may add as many collection names we would wish: ie
      // a sensitive detector can have many collections.

   }

TransferGap1SensitiveDetector::~TransferGap1SensitiveDetector()
{}

G4bool TransferGap1SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *)
{
   //This method is called every time a G4Step is performed in the logical volume
   //to which this SD is attached: the gas gap.

   // To identify where the step is we use the touchable navigation,
   // remember we need to use PreStepPoint!
   G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
   G4int copyNo = touchable->GetVolume()->GetCopyNo();
   //We get now the energy deposited by this step
   G4double edep = step->GetTotalEnergyDeposit();
   
   if(edep == 0) return true ;
    // Whatever happens after this line implies an energy deposition in the SD volumes
   
   G4double ioniE = 21.45*eV ;
   if(edep >= 5*ioniE) isSensitive = true ;

   //This line is used to store in Analysis class the energy deposited in this layer
   //The Analysis class will sum up this edep to the current event total energy in this layer
   TrGEMAnalysis::GetInstance()->AddEDepSD(edep,layerIndex);

   //check if edep is from primary or secondary:
   G4String isPri = step->GetTrack()->GetTrackID() == 1 ? "Yes" : "No";

   //--------------
   //Exercise 2 task4a
   //-------------
   //G4cout << "GasGap: " << layerIndex << " (volume CopyNo: " << copyNo << ") Edep=" << G4BestUnit(edep,"Energy") << " isPrimary? " << isPri ;
   //G4cout << " (name=" << step->GetTrack()->GetDefinition()->GetParticleName() << ")" << G4endl;
   //
   //Hits in calorimeters are tricky, to avoid to create too many hits we make them
   //accumulate energy on each plane.
   //Thus we need to know if the hit for a specific layer has already been created or not.
   //We use the mapHit map to verify this.
   //We could have done searching the hit in the hitCollection,
   //but a std::map is more efficient in searching.
   //Since this is called several times for each event, this is a place of the code
   //to be optimized
   /*hitMap_t::iterator it = hitMap.find(layerIndex);
     GasGapHit* aHit = 0;
     if ( it != hitMap.end() )
     {
   //Hit for this layer already exists
   //remember the hit pointer
   aHit = it->second;
   }
   else
   {
   //Hit for this layer does not exists,
   //we create it
   aHit = new GasGapHit(layerIndex);
   hitMap.insert(std::make_pair(layerIndex,aHit));
   G4cout << "Cerco un bug..." << G4endl ;
   hitCollection->insert(aHit);
   G4cout << "L'ho forse trovato?" << G4endl ;
   }
   aHit->AddEdep(edep) ;*/
   return true;
}

void TransferGap1SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{

   // -- Creation of the collection
   // -- collectionName[0] is "SiHitCollection", as declared in constructor

   //---------------
   // Exercise 2 task4c
   //----------------
   // Create the hit collection, remember the hit collection constructor wants two
   // strings: the name of the SD and the name of the collection:
   // hitCollection = new HadCaloHitCollection( EdITME:SD_name, EDITME:collection_name )
   // Hint 1: Get the SD_name with the GetName() function,
   // Hint 2: Get the collection_name from the collectionName vector: your collection is at position 0: collectionName[0]


   // -- and attachment of this collection to the "Hits Collection of this Event":
   // -- To insert the collection, we need to get an index for it. This index
   // -- is unique to the collection. It is provided by the GetCollectionID(...)
   // -- method (which calls what is needed in the kernel to get this index).
   /*static G4int HCID = -1;
     if (HCID<0) HCID = GetCollectionID(0); // <<-- this is to get an ID for collectionName[0]
     HCE->AddHitsCollection(HCID, hitCollection);

   //Reset map of hits
   hitMap.clear();*/

}

void TransferGap1SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
   // Here I must save the following variables with an TrGEMAnalysisMethod

   TrGEMAnalysis::GetInstance()->SaveTransferGap(isSensitive) ;

   isSensitive = false ; 
   //G4cout << "I'm looking for..." << G4endl ;   
   //hitCollection->PrintAllHits() ;
   //G4cout << "... a bug." << G4endl ;   
}
