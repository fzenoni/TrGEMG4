#ifndef TransferGap1SensitiveDetector_h
#define TransferGap1SensitiveDetector_h 1

/**
 * @file
 * @brief Defines TransferGap1SensitiveDetector class.
 */

#include "G4VSensitiveDetector.hh"
#include "GasGapHit.hh"

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;           // <<- means "H(it) C(ollections) of This Event"


/*!
 * \brief Defines sensitve part of HAD calo detector geometry.
 *
 * Stores Hits with 
 *  * deposited energy
 *  * position
 * in <i>Hit Collections of This Event</i>
 *
 * /sa ProcessHits()
 */

class TransferGap1SensitiveDetector : public G4VSensitiveDetector
{
   public:
      /// Constructor
      TransferGap1SensitiveDetector(G4String SDname);
      /// Destructor
      ~TransferGap1SensitiveDetector();

   public:
      /// @name methods from base class G4VSensitiveDetector
      //@{
      /// Mandatory base class method : it must to be overloaded:
      G4bool ProcessHits(G4Step *step, G4TouchableHistory *ROhist);

      /// (optional) method of base class G4VSensitiveDetector
      void Initialize(G4HCofThisEvent* HCE);
      /// (optional) method of base class G4VSensitiveDetector
      void EndOfEvent(G4HCofThisEvent* HCE);
      //@}

      //G4bool isSensitive ;
      G4int layerIndex ;

   private:
      //typedef std::map<G4int,GasGapHit*> hitMap_t; //< Helper mapping layer number with hit
      //hitMap_t hitMap;
      GasGapHitCollection* hitCollection;          //< Collection of gas gap hits
      G4bool isSensitive ;

};

#endif
