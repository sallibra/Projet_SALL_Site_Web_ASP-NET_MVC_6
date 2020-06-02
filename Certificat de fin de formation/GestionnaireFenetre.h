
#ifndef _GESTIONNAIREFENETRE_H_
#define _GESTIONNAIREFENETRE_H_

#include "GestionnaireFenetreVente.h"
#include "InterventionInProgressWindow.h"
#include <map>

////////////////////////////////////////////////////
// Definitions des messages utilisateurs
#define WM_ARRET                    (WM_USER + 1)
#define WM_AFFICHAGE_ECRAN          (WM_USER + 2)
#define WM_ARMERTEMPOINACTION       (WM_USER + 3)
#define WM_INHIBER_ROULEAU_BOUTONS  (WM_USER + 4)
#define WM_ACTIVER_VEROUILLAGEMAJ   (WM_USER + 5)
#define WM_DEPLACEMENT_ROULEAU      (WM_USER + 7)
#define WM_VALIDATION_BOUTON        (WM_USER + 8)
#define WM_ANNULATION_BOUTON        (WM_USER + 9)
#define WM_CHANGEMENT_MODE          (WM_USER + 10)
#define WM_INITIALISATION           (WM_USER + 11)
#define WM_LECTURE_TEXTE            (WM_USER + 12)
#define WM_CHANGEMENT_RENDU         (WM_USER + 13)
#define WM_ARRET_LECTURE_TEXTE      (WM_USER + 14)
#define WM_CHANGER_FOND_ECRAN       (WM_USER + 15)
#define WM_DEPLACEMENT_ROULEAU_HAUT (WM_USER + 16)
#define WM_DEPLACEMENT_ROULEAU_BAS  (WM_USER + 17)
#define WM_CHANGEMENT_ECRAN         (WM_USER + 18)
#define WM_LIBERER_RESSOURCES       (WM_USER + 19)
#define WM_ACTIVER_BUZZER           (WM_USER + 20)
#define WM_DESACTIVER_BUZZER        (WM_USER + 21)
#define WM_LIRE_FICHIER_AUDIO       (WM_USER + 22)
#define WM_STOP_FICHIER_AUDIO       (WM_USER + 23)
#define WM_FIN_FICHIER_AUDIO        (WM_USER + 24)
#define WM_SET_OUT_OF_ORDER_TEXT    (WM_USER + 25)

////////////////////////////////////////////////////
// Definitions des lettres
#define LETTRE_CR_COMPTE_RENDU         1
#define LETTRE_CR_SELECTION            2
#define LETTRE_CR_TEMPO_INACTION       3
#define LETTRE_CR_INITIALISATION       4
#define LETTRE_CR_LIBERATION_RESSOURCE 5
#define LETTRE_CR_TEMPO_TOOLTIP        6

////////////////////////////////////////////////////
// Nombre de touche dans la sequence secrete
#define MAX_TOUCHE_SEQUENCE      16

class MonitorsInfos
{
public:
   MonitorsInfos();
   virtual ~MonitorsInfos();

   void Add(long Id, RECT rect, bool isMainMonitor = false);
   bool  IsPresent(long monitorId);
   bool IsPresent(const RECT& rect);
   const RECT Rect(long monitorId);
   const long MainMonitorId();
   void Clear();

private:
   std::map<long, RECT > data_;
   long mainMonitorId_;
};

class T_RhmTraceAgent
{
public:
   T_RhmTraceAgent()
   {
      memset(Trace, 0, sizeof(Trace));
   }

   TCHAR Trace[TAILLE_MAX_ECRAN_TEXTE + 2];
};

class T_GestionnaireFenetre : public T_FenetreBase, public T_ServeurIHM
{
public:

   ////////////////////////////////////////////////////
   // Constructeur - Destructeur
   T_GestionnaireFenetre(T_NoTache TacheClient_P);
   virtual ~T_GestionnaireFenetre();

   ////////////////////////////////////////////////////
  // Gestion de la fenetre
   void Creer(unsigned int TypeEcran_P, unsigned int TypeEcranMaintenance_P,
      unsigned int SortieAffichageVente_P, unsigned int SortieAffichageMaintenance_P,
      bool SortieEcranTotal_P);
   void Liberer(unsigned int ChargementDynamique_P = FAUX);

   void UnloadAgentScreensElements();

   virtual void ChangerRendu(T_RhmTypeRendu Rendu_P, bool Force_P = false);

   ////////////////////////////////////////////////////
  // Interface T_ServeurIHM
   virtual unsigned int DonneTypeRhm();

   virtual void Redessiner();
   virtual void Rafraichir(T_FenetreVirtuelle* Fenetre_P = NULL, RECT* Zone_P = NULL);

   virtual void BloquerRafraichissement();
   virtual void DebloquerRafraichissement();

   virtual void BloquerInteractions();
   virtual void DebloquerInteractions();
   virtual void TemporiserClic(unsigned int = 0);

   virtual void AfficherFenetreConfirmation(unsigned int Fonction_P, T_RhmEcranAgent* Donnees_P);
   virtual void AfficherFenetreMemorisee();

   virtual void InitialiserRouleau();

   virtual void CompteRendu(T_RhmCr Code_P, unsigned int Id_P, unsigned int NoEcran_P);
   virtual void Selection(unsigned int CompteRendu_P, TCHAR Saisie_P[RHM_TAILLE_MAX_SAISIE], unsigned int NoEcran_P);

   virtual HDC DonnerDC();
   virtual void RendreDC(HDC DC_P);
   virtual I_Renderer* DonnerRendu();
   virtual unsigned int DonnerTypeRendu();

   virtual void InitierChrono(T_FenetreVirtuelle* Fenetre_P, unsigned int Duree_P, unsigned int TypeChrono_P);
   virtual void ArreterChrono(unsigned int TypeChrono_P, T_FenetreVirtuelle* Fenetre_P = NULL);

   void LogMonitorsConfiguration();
   virtual unsigned int InitDialog(unsigned int ChargementDynamique_P = FAUX);

   void FiltrerMessageWindows(MSG *Msg_P);

   void ActivateValidationOnPresentation();

   void SetDoorHandle();

protected:

   ////////////////////////////////////////////////////
   // Gestion des messages
   virtual long GestionMessageLocal(UINT Message_P, WPARAM wParam_P, LPARAM lParam_P);

   unsigned int InitGenericFenetre();
   void InitialiserImages();

   void DeplacerFichier(char* CheminOrigine_P, char* CheminDestination_P);

   ////////////////////////////////////////////////////
   // Gestion interne 
   void ChangementEcran(unsigned int TypeEcran_P, void* Arg_P);
   void ChangerFondVente(char* NomImage_P);

   void Retaillage();

   void AjouterZoneARafraichir(long Gauche_P, long Haut_P, long Droite_P, long Bas_P);
   void ReinitialiserZoneARafraichir();

   // Pointeurs sur les dernières données reçues de l'application (pour désallocation)
   void *PtrEcran;
   T_RhmChoix *PtrMenu;

   ////////////////////////////////////////////////////
   // Tempo inaction
   void RedemarrageTempoInaction(unsigned int Tempo_P);
   void RedemarrageTempoInaction();
   void DemarrageTempoToolTip();
   void PosterCrTempoToolTip(T_NoTache * NoClient_P);
   void ArretTempoInaction();

   void PasserEnPremierPlan(void);

   unsigned int TempoDemarree;
   unsigned int ValeurTempo;
   unsigned int ValeurTempoToolTip;
   unsigned int IncidentReadOnce;
   bool _statetoolTip = false;
   T_FenetreVirtuelle* ChronosAnimation[NB_TIMER_ANIME];

   // *************************************************
   // *** Attributs

   // Instance de la DLL
   HINSTANCE Instance;

   // Fenetre courante 
   T_FenetreVirtuelle*                       FenetreCourante;

   T_FenetreVirtuelle*                       FenetreMemorisee;

   ///////////////////////////
   // Fenetres disponibles
   T_FenetreAgentMenu                        FenetreAgentMenu;
   T_FenetreAgentMenu                        FenetreAgentSousMenu;

   // Pool de fenetre
   // - Vente
   T_GestionnaireFenetreVente*               GestionnaireFenetreVente;

   InterventionInProgressWindow* interventionInProgressWnd;

   // - Vente pour malvoyants
   T_FenetreVenteAccueilMalvoyants                 FenetreVenteAccueilMalvoyants;
   T_FenetreVenteConsultationCSCMalvoyants         FenetreVenteConsultationCSCMalvoyants;
   T_FenetreVenteFabricationMalvoyants             FenetreVenteFabricationMalvoyants;
   T_FenetreVenteFinFabricationMalvoyants          FenetreVenteFinFabricationMalvoyants;
   T_FenetreVenteIncidentMalvoyants                FenetreVenteIncidentMalvoyants;
   T_FenetreVentePaiementMalvoyants                FenetreVentePaiementMalvoyants;
   T_FenetreVenteSelectionMalvoyants               FenetreVenteSelectionMalvoyants;
   T_FenetreVenteSelectionMoyenPaiementMalvoyants  FenetreVenteSelectionMoyenPaiementMalvoyants;

   // - Agent
   T_FenetreAgentTrace                       FenetreAgentTrace;
   T_FenetreAgentConsultation                FenetreAgentConsultation;
   T_FenetreAgentRechargementMonnaie         FenetreAgentRechargementMonnaie;
   T_FenetreAgentTestRouleau                 FenetreAgentTestRouleau;
   T_FenetreAgentPannesEtats                 FenetreAgentPannesEtats;
   T_FenetreAgentEntreesSorties              FenetreAgentEntreesSorties;
   T_FenetreAgentTestTouches                 FenetreAgentTestTouches;
   T_FenetreAgentAccesImprimante             FenetreAgentAccesImprimante;
   T_FenetreAgentMAJCptICS                   FenetreAgentMAJCptICS;
   T_FenetreAgentCptMny                      FenetreAgentCptMny;
   T_FenetreAgentMAJCptMny                   FenetreAgentMAJCptMny;
   T_FenetreAgentVidage                      FenetreAgentVidage;
   T_FenetreAgentFichier                     FenetreAgentFichier;
   T_FenetreAgentMaintenanceMPC              FenetreAgentMaintenanceMPC;
   T_FenetreAgentSaisie                      FenetreAgentSaisie;
   T_FenetreAgentSelFichier                  FenetreAgentSelFichier;
   T_FenetreAgentConfirmation                FenetreAgentConfirmation;


   // Retour vers l'applicatif
   T_NoTache TacheClient;

   unsigned int EstModeMalvoyants;
   bool sortieMaintenanceActive;
   unsigned int showInterventionScreenUntilDoorIsOpen;
   unsigned int WaitForDoorOpen;

   // Incrémenté à chaque Blocage et décrémenté à chaque déblocage
   unsigned int RafraichissementBloque;

   // Mis à VRAI à chaque blocage et mis à FAUX à chaque déblocage
   unsigned int InteractionsBloquees;

   unsigned int RouleauBoutonsInhibes;

   unsigned int NumeroFondVente;

   ///////////////////////////
   // Attributs graphiques

   // Gestion de l'affichage double-buffering

   // Gestionnaire du compteur des chronometres
   T_FenetreVirtuelle* FenetreTimer;

   // Gestion de la zone à rafraichir :
   // On mémorise le rectangle englobant toutes les zones à rafraichir
   // pendant que le rafraichissement est bloqué.
   unsigned int   ZoneARafraichirValide;
   RECT           ZoneARafraichir;

   // Sequence secrete pour le passage en maintenance
   unsigned char SequenceSecreteAutorisee;
   unsigned char IndiceSecretSequence;
   unsigned int SequenceSecreteSaisie[MAX_TOUCHE_SEQUENCE];

   void AutoriserSequenceSecrete(unsigned int TypeEcran_P);
   void InitialiserSequenceSecrete();
   unsigned int EstCeSequenceSecreteSaisie(void);
   void AjouterToucheDansSequence(unsigned int Touche_P);

   // Ecran virtuel : rectangle d'affichage de tous les moniteurs
   RECT RectVirtualScreen;
   MonitorsInfos monitorsInfos_;
   //RECT RectMoniteur[3];

   // Area where to display the optional "intervention in progress" window
   RECT optionalDisplayRect_;

   void ShowInterventionWindows(bool showWindow);

   // Display MMI ont target display.
   // bool saleDisplay = interface that will be displayed (true = sale, false = maintenance)
   // outputId = ID of the screen to use (as define by Windows Operation System). Between 1 and 3
   // optionalOutputID = ID of the optionl screen where "out of service" windows will be displayed(as define by Windows Operation System). Between 1 and 3
   void AfficherSortieEcran(bool saleDisplay, unsigned int outputId, unsigned int optionalOutputID = 0);

   bool BuzzerActif;
};

#endif //_GESTIONNAIREFENETRE_H_