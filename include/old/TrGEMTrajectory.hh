#ifndef TrGEMTrajectory_h
#define TrGEMTrajectory_h 1

#include <vector>
#include <stdlib.h>

#include "G4VTrajectory.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Allocator.hh"
#include "G4Track.hh"

typedef std::vector<G4VTrajectoryPoint*> TrGEMTrajectoryPointContainer ;

class TrGEMTrajectory : public G4VTrajectory
{
   public:
      TrGEMTrajectory() ;
      TrGEMTrajectory(const G4Track* aTrack) ;
      //TrGEMTrajectory(TrGEMTrajectory & ) {;}
      virtual ~TrGEMTrajectory() {;}

      /*inline void* operator new(size_t) ;
	inline void  operator delete(void*) ;
	inline int operator == (const TrGEMTrajectory& right) const
	{return (this == &right);}*/

      inline G4int GetTrackID() const { return fTrackID; }
      inline G4int GetParentID() const { return fParentID; }
      inline G4String GetParticleName() const { return particleName; }
      inline G4double GetCharge() const { return PDGCharge; }
      inline G4int GetPDGEncoding() const { return PDGEncoding; }
      /*inline const G4ThreeVector& GetMomentum() const
	{ return momentum; }*/
      inline G4ThreeVector GetInitialMomentum() const
      { return initialMomentum; }
      /*inline const G4ThreeVector& GetVertexPosition() const
	{ return vertexPosition; }
	inline G4double GetGlobalTime() const
	{ return globalTime; }*/
      virtual int GetPointEntries() const { return 0; }
      virtual G4VTrajectoryPoint* GetPoint(G4int /*i*/) const  { return 0 ; }
      //virtual void ShowTrajectory(std::ostream& /*os=G4cout*/) const {;}
      //virtual void DrawTrajectory(G4int /*i_mode = 0*/) const {;}
      virtual void AppendStep(const G4Step* /*aStep*/) {;}
      virtual void MergeTrajectory(G4VTrajectory* /*secondTrajectory*/) {;}
      // A new method
      G4String GetOrigVolName() { return origVolName ; } 

   private:
      //TrGEMTrajectoryPointContainer* positionRecord ;
      G4int                          fTrackID;
      G4int                          fParentID;
      const G4ParticleDefinition*    fpParticleDefinition;
      G4String                       particleName;
      G4double                       PDGCharge;
      G4int                          PDGEncoding;
      //G4ThreeVector                momentum;
      G4ThreeVector                  initialMomentum;
      //G4ThreeVector                vertexPosition;
      //G4double                     globalTime;
      G4String                       origVolName ;

};

extern G4Allocator<TrGEMTrajectory> myTrajectoryAllocator ;


/*inline void* TrGEMTrajectory::operator new(size_t)
  {
  void* aTrajectory;
  aTrajectory = (void*)myTrajectoryAllocator.MallocSingle();
  return aTrajectory;
  }

  inline void TrGEMTrajectory::operator delete(void* aTrajectory)
  {
  myTrajectoryAllocator.FreeSingle((TrGEMTrajectory*)aTrajectory);
  }*/

#endif
