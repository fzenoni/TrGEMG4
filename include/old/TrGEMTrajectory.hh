#ifndef TrGEMTrajectory_h
#define TrGEMTrajectory_h 1

#include <vector>
#include <stdlib.h>

#include "G4Trajectory.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Allocator.hh"
#include "G4Track.hh"

//typedef std::vector<G4VTrajectoryPoint*> TrGEMTrajectoryPointContainer ;

class TrGEMTrajectory : public G4Trajectory
{
   public:
      //TrGEMTrajectory() {} ;
      //virtual ~TrGEMTrajectory() {} ;

      TrGEMTrajectory(const G4Track* aTrack) ;
};
      
#endif
