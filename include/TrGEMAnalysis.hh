#ifndef TRGEMANALYSIS_HH
#define TRGEMANALYSIS_HH 1

#include "globals.hh"
#include <vector>

class G4Run;
class G4Track;
class G4Event;
class G4Step;
class G4ParticleDefinition;
class TFile;
class TH1D;
class TNtuple;
class TTree;

class TrGEMAnalysis {

   public:

      static TrGEMAnalysis* GetInstance();
      ~TrGEMAnalysis();

      void PrepareNewEvent(const G4Event* anEvent);
      void PDGHisto(std::vector<G4double> PDGvector) ;
      void EndOfEvent(const G4Event* anEvent);
      void PrepareNewRun(const G4Run* aRun);
      void EndOfRun(const G4Run* aRun);
      void AddSecondary(const G4ParticleDefinition* part);
      void AddGapSecondary(const G4ParticleDefinition* part, G4int gapNum);
      void AddEDep(G4double edep, G4double z);
      void AddEDepSD(G4double z, G4int copyno);
      //void SetEcalZposition(G4double val) { eCalZposition = val; };
      void SetBeam(const G4ParticleDefinition* part, G4double energy);
      //void CreateBranch(G4String name, G4int evtNo, G4String type) ;
      void AddParticlesPerEvent(G4int PDGCode) ;
      void SetDriftSensitivityA(G4double someDriftEdep) ;
      void SetDriftSensitivityB(G4double someDriftEdep) ;
      void SetTransferSensitivityA(G4double someTransferEdep) ;
      void SetTransferSensitivityB(G4double someTransferEdep) ;
      void SetNeutronSensitivityA(G4bool someBool) ;
      void SetNeutronSensitivityB(G4bool someBool) ;
      //void SaveStepProcess(int process, std::string volume) ;
      void SaveProcessQuantities(
	    G4int    anEventID,
	    G4int    aParentID,
	    G4int    aTrackID,
	    G4int    aCharge,
	    G4double aGlobalTime,
	    G4int    aPdgCode,
	    std::string aParticle,
	    G4double aKineticEnergy,
	    G4double aPositionX, 
	    G4double aPositionY, 
	    G4double aPositionZ,
	    G4double aMomentumDirectionX, 
	    G4double aMomentumDirectionY, 
	    G4double aMomentumDirectionZ,
	    std::string aProcess,
	    std::string aVolume) ;

      void SetAcceptance(G4bool anAcceptance) ;
      void SetKickstart(G4bool aKickstart) ;

   private:

      TrGEMAnalysis();
      static TrGEMAnalysis* singleton;

      bool isNewEvent ;
      G4int eventCounter ;

      // beam and calorimeter geometry
      const G4ParticleDefinition* beamParticle;
      G4double beamEnergy;
      //G4double eCalZposition;

      // simple analysis parameters
      G4double thisEventTotEM;
      G4double thisEventCentralEM;
      G4double thisRunTotEM;
      G4double thisRunTotEM2;
      G4double thisRunCentralEM;
      G4double thisRunCentralEM2;

      // counters
      G4int thisEventSecondaries;
      G4int n_gamma;
      G4int n_electron;
      G4int n_positron;
      G4int n_gapGamma[4];
      G4int n_gapElectron[4];
      G4int n_gapPositron[4];

      //const G4int kMaxTrack ;
      G4int elexevt ;
      G4int posxevt ;
      G4int gammaxevt ;
      G4int secoxevt ;
      G4bool driftSensitivityA ;
      G4bool transferSensitivityA ;
      G4double driftEdepA ;
      G4double transferEdepA ;
      G4bool driftSensitivityB ;
      G4bool transferSensitivityB ;
      G4double driftEdepB ;
      G4double transferEdepB ;
      std::vector<std::string> vecProcess ;
      std::vector<std::string>* pVecProcess ;
      std::vector<std::string> vecVolume ;
      std::vector<std::string>* pVecVolume ;
      G4int vecProcNo ;
      G4bool neutronSensitivityA ;
      G4bool neutronSensitivityB ;
      G4bool kickstart ;

      // PROCESS quantities
      std::vector<G4int> eventID ;
      std::vector<G4int> parentID ;
      std::vector<G4int> trackID ;
      std::vector<G4int> charge ;
      std::vector<G4double> globalTime ;
      std::vector<G4int> pdgCode ;
      std::vector<std::string> particle ;
      std::vector<G4double> kineticEnergy ;
      std::vector<G4double> positionX ;
      std::vector<G4double> positionY ;
      std::vector<G4double> positionZ ;
      std::vector<G4double> momentumDirectionX ;
      std::vector<G4double> momentumDirectionY ;
      std::vector<G4double> momentumDirectionZ ;
      std::vector<std::string> process ;
      std::vector<std::string> volume ;

      G4bool acceptance ;

      // ROOT objects
      TFile*    m_ROOT_file;
      TH1D*     m_ROOT_histo0;
      //TH1D*     m_ROOT_histo1;
      TH1D*     m_ROOT_histo2;
      TH1D*     m_ROOT_histo3;
      //TNtuple*  ntuple;
      TTree     *t ;

};

#endif /* TRGEMANALYSIS_HH */
