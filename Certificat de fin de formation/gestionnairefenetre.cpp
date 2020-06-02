
#define _WIN32_WINDOWS 0x0500

#include <windows.h>
#include <winuser.h>
#include <stdio.h>

#include "util.h"
#include "es.h"

#include "rhm.h"
#include "ServeurIHM.h"
#include "FenetreBase.h"
#include "FenetreVirtuelle.h"

#include "Bitmap.h"
#include "Libelle.h"
#include "FenetreBouton.h"
#include "FenetreBoutonZones.h"
#include "FenetreMenu.h"
#include "FenetreAffichageTexte.h"
#include "FenetreRecycleur.h"
#include "FenetreLabel.h"
#include "FenetreTactile.h"
#include "FenetreEdit.h"
#include "FenetreImage.h"
#include "FenetreDiaporama.h"
#include "FenetreScroller.h"
#include "FenetreAffichageScroll.h"
#include "FenetreToolTip.h"
#include "FenetreItem.h"
#include "FenetreClavier.h"
#include "FenetreAscenseur.h"
#include "FenetreUpDown.h"
#include "FenetreMultipleUpDown.h"

#include "FenetreAgent.h"
#include "FenetreAgentMenu.h"
#include "FenetreAgentConfirmation.h"
#include "FenetreAgentConsultation.h"
#include "FenetreAgentPannesEtats.h"
#include "FenetreAgentRechargementMonnaie.h"
#include "FenetreAgentTestRouleau.h"
#include "FenetreAgentEntreesSorties.h"
#include "FenetreAgentTestTouches.h"
#include "FenetreAgentAccesImprimante.h"
#include "FenetreAgentMAJCptICS.h"
#include "FenetreAgentCptMny.h"
#include "FenetreAgentMAJCptMny.h"
#include "FenetreAgentVidage.h"
#include "FenetreAgentFichier.h"
#include "FenetreAgentMaintenanceMPC.h"
#include "FenetreAgentSaisie.h"
#include "FenetreAgentSelFichier.h"
#include "FenetreAgentTrace.h"

#include "FenetreVenteMalvoyants.h"
#include "FenetreVenteAccueilMalvoyants.h"
#include "FenetreVenteConsultationCSCMalvoyants.h"
#include "FenetreVenteFabricationMalvoyants.h"
#include "FenetreVenteFinFabricationMalvoyants.h"
#include "FenetreVenteIncidentMalvoyants.h"
#include "FenetreVentePaiementMalvoyants.h"
#include "FenetreVenteSelectionMalvoyants.h"
#include "FenetreVenteSelectionMoyenPaiementMalvoyants.h"

#include "GestionnaireFenetre.h"
#include "resource.h"

#include "tts.h"
#include <cfgmgr32.h>

#define TEMPO_TOOLTIP  4000 
// Sequence secrete a saisir
const int SEQUENCE_SECRETE[MAX_TOUCHE_SEQUENCE] = { WM_ANNULATION_BOUTON, WM_ANNULATION_BOUTON, WM_ANNULATION_BOUTON, WM_DEPLACEMENT_ROULEAU_BAS,
                                                   WM_VALIDATION_BOUTON, NULL, NULL, NULL,
                                                   NULL, NULL, NULL, NULL,
                                                   NULL, NULL, NULL, NULL };


// Recuperation de l'instance de la DLL (lors de l'appel au DLLMain)
HINSTANCE Instance_DLL_S;


/***************************************************************************
*
* Macro d'aide
*
***************************************************************************/
// TG : Suppresison du while et Visible(0) apres creation
#define CREATION_VENTE(Fenetre_P,Layout_P) { \
   Fenetre_P.Creation ( NULL, this, Layout_P);\
   Fenetre_P.Visible ( 0 );\
   Fenetre_P.Deplacer ( 0, 0, Largeur, Hauteur ); } 

#define CREATION(Fenetre_P) { \
   Fenetre_P.Creation ( NULL, this);\
   Fenetre_P.Visible ( 0 );\
   Fenetre_P.Deplacer ( 0, 0, Largeur, Hauteur ); } 

/*****************************************************************************
* But de la fonction : Point d'entrée de la DLL
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
BOOL WINAPI DllMain(
   HANDLE hinstDLL,
   DWORD dwReason,
   LPVOID lpvReserved
)
{
   // Recuperation du handle d'instance
   Instance_DLL_S = (HINSTANCE)hinstDLL;

   return TRUE;
}

/*****************************************************************************
* But de la fonction : Poster le compte-rendu IHM Vente.
* --------------------
*
* Parametres :
* ------------
*
* Entree : NoClient_P, tache cliente à qui poster le compte rendu.
*          Cr_P, compte rendu à poster.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void PosterCrCompteRendu(T_NoTache * NoClient_P, T_RhmCompteRendu Cr_P)
{
   PosterCompteRendu(NoClient_P,
      LETTRE_CR_COMPTE_RENDU,
      &Cr_P,
      sizeof(Cr_P));
}

/*****************************************************************************
* But de la fonction : Poster le compte-rendu IHM Agent.
* --------------------
*
* Parametres :
* ------------
*
* Entree : NoClient_P, tache cliente à qui poster le compte rendu.
*          Cr_P, compte rendu à poster.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void PosterCrSelection(T_NoTache * NoClient_P, T_RhmCrSelection Cr_P)
{
   PosterCompteRendu(NoClient_P,
      LETTRE_CR_SELECTION,
      &Cr_P,
      sizeof(Cr_P));
}

/*****************************************************************************
* But de la fonction : Poster le compte-rendu de la tempo d'inaction
* --------------------
*
* Parametres :
* ------------
*
* Entree : NoClient_P, tache cliente à qui poster le compte rendu.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void PosterCrTempoInaction(T_NoTache * NoClient_P)
{
   PosterCompteRendu(NoClient_P,
      LETTRE_CR_TEMPO_INACTION,
      NULL,
      NULL);
}

void T_GestionnaireFenetre::PosterCrTempoToolTip(T_NoTache * NoClient_P)
{
   PosterCompteRendu(NoClient_P,
      LETTRE_CR_TEMPO_TOOLTIP,
      NULL,
      NULL);
}

MonitorsInfos::MonitorsInfos()
   :mainMonitorId_(0)
{
   data_.clear();
}

MonitorsInfos::~MonitorsInfos()
{
}

void MonitorsInfos::Add(long id, RECT rect, bool isMainMonitor)
{
   if (!IsPresent(rect))
   {
      data_.insert(std::pair<long, RECT>(id, rect));
      if (isMainMonitor)
      {
         mainMonitorId_ = id;
      }
   }
}

bool MonitorsInfos::IsPresent(long monitorId)
{
   return (data_.find(monitorId) != data_.end());
}

bool MonitorsInfos::IsPresent(const RECT& rect)
{
   for (std::map<long, RECT>::iterator iter = data_.begin(); iter != data_.end(); ++iter)
   {
      const RECT existing = iter->second;
      if (existing.left == rect.left
         && existing.right == rect.right
         && existing.top == rect.top
         && existing.bottom == rect.bottom)
      {
         return true;
      }
   }

   return false;
}

const RECT MonitorsInfos::Rect(long monitorId)
{
   if (!IsPresent(monitorId))
   {
      const RECT dummy = { 0, 0, 0, 0 };
      return(dummy);
   }

   return data_.find(monitorId)->second;
}

const long MonitorsInfos::MainMonitorId()
{
   return mainMonitorId_;
}

void MonitorsInfos::Clear()
{
   mainMonitorId_ = 0;
   data_.clear();
}

/*****************************************************************************
* But de la fonction : Constructeur
* --------------------
*
* Parametres :
* ------------
*
* Entree : TacheClient_P, tache cliente de la RHM.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
T_GestionnaireFenetre::T_GestionnaireFenetre(T_NoTache TacheClient_P) : T_FenetreBase(), GestionnaireFenetreVente(T_GestionnaireFenetreVente::InstanceOf(this))
{
   Instance = Instance_DLL_S;

   FenetreCourante = NULL;
   FenetreMemorisee = NULL;

   TacheClient = TacheClient_P;

   RafraichissementBloque = 0;
   InteractionsBloquees = FAUX;
   RouleauBoutonsInhibes = FAUX;
   NumeroFondVente = 0;

   ValeurTempo = 0;
   TempoDemarree = FAUX;
   EstModeMalvoyants = FAUX;
   sortieMaintenanceActive = false;
   showInterventionScreenUntilDoorIsOpen = false;
   WaitForDoorOpen = false;

   FondAgent = NULL;
   FondAgentFormatVente = NULL;
   FondVenteConducteur = NULL;
   FondVenteMalvoyant = NULL;
   FondVente = NULL;

   FenetreTimer = NULL;

   interventionInProgressWnd = NULL;

   ZoneARafraichirValide = 0;
   memset(&ZoneARafraichir, 0, sizeof(ZoneARafraichir));

   SequenceSecreteAutorisee = 0;
   IndiceSecretSequence = 0;

   memset(SequenceSecreteSaisie, 0, sizeof(SequenceSecreteSaisie));

   PtrEcran = NULL;
   PtrMenu = NULL;

   BuzzerActif = false;

   memset(&RectVirtualScreen, 0, sizeof(RectVirtualScreen));
   memset(ChronosAnimation, 0, sizeof(ChronosAnimation));

   memset(&optionalDisplayRect_, 0, sizeof optionalDisplayRect_);

   //   monitorsInfos_.Clear();
}


/*****************************************************************************
* But de la fonction : Destructeur
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
T_GestionnaireFenetre::~T_GestionnaireFenetre()
{
   if (interventionInProgressWnd != NULL)
   {
      delete interventionInProgressWnd;
   }

   // Destruction de la fenetre en cours
   DestroyWindow(Wnd);

   delete GestionnaireFenetreVente;
}


/*****************************************************************************
* But de la fonction : Changer le Rendu
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return :
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ChangerRendu(T_RhmTypeRendu Rendu_P, bool Force_P)
{
   I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
   if (GestionnaireFenetreVente->ChangeFenetreVente(Rendu_P, Force_P) == true)
   {
      // Changement des Fonds : On libere les anciens
      if (FondVenteMalvoyant != NULL)
      {
         Rendu_L->LibererFond(FondVenteMalvoyant);
      }

      if (FondAgent != NULL)
      {
         Rendu_L->LibererFond(FondAgent);
      }

      if (FondAgentFormatVente != NULL)
      {
         Rendu_L->LibererFond(FondAgentFormatVente);
      }

      if (FondVente != NULL)
      {
         Rendu_L->LibererFond(FondVente);
      }

      if (FondVenteConducteur != NULL)
      {
         Rendu_L->LibererFond(FondVenteConducteur);
      }

      // On recree les nouveaux
      FondAgent = Rendu_L->PreparerFond(BitmapFondAgent.DonneHBitmap());
      FondAgentFormatVente = Rendu_L->PreparerFond(BitmapFondAgentFormatVente.DonneHBitmap());
      FondVenteMalvoyant = Rendu_L->PreparerFond(BitmapFondVenteMalvoyants.DonneHBitmap());
      FondVente = Rendu_L->PreparerFond(BitmapFondVente.DonneHBitmap());
      FondVenteConducteur = Rendu_L->PreparerFond(BitmapFondVenteConducteur.DonneHBitmap());

      // Refresh
      // Redessiner();
   }
}


/*****************************************************************************
* But de la fonction : Donner le type de RHM
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : RHM_TYPE_i.
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
unsigned int T_GestionnaireFenetre::DonneTypeRhm()
{
   return TypeEcran < 100 ? m_TypeRhmMaintenance : m_TypeRhmVente;
}

/*****************************************************************************
* But de la fonction : Redessiner la fenêtre principale dans son intégralité.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::Redessiner()
{
   Rafraichir(NULL, NULL);
}

/*****************************************************************************
* But de la fonction : Redessiner une zone spécifique d'une fenêtre.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Fenetre_P, la fenêtre à redessiner.
*          Zone_P, la zone à redesiner.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::Rafraichir(T_FenetreVirtuelle* Fenetre_P, RECT *Zone_P)
{
   if (Fenetre_P == NULL)
   {
      // Cas par defaut : on affiche la fenetre courante
      Fenetre_P = FenetreCourante;
   }

   if ((Fenetre_P != NULL) && (Fenetre_P->EstVisible() == VRAI))
   {
      RECT Rect_L;

      if (Zone_P == NULL)
      {
         Rect_L = Fenetre_P->DonnerRefreshRect();
      }
      else
      {
         Rect_L = *Zone_P;
      }

      // Blit de la zone en question
      if ((RafraichissementBloque == 0) && (FenetreCourante != NULL))
      {
         // On efface toute la fenetre puisqu'on va reconstruire la fenêtre en entier
         // On ne copiera par contre sur l'écran que la zone à rafraichir
         I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();

         RECT RectScreen_L = Rect_L;
         Rendu_L->ClientVersEcran(&RectScreen_L);
         Rendu_L->EffacerFond(FenetreCourante->DonnerFond(NumeroFondVente), RectScreen_L);

         // On redessine la fenetre parente
         FenetreCourante->Redessiner(Rendu_L, &Rect_L);

         // On affiche le buffer à l'écran
         RECT localRect = { 0,0,0,0 };
         //Rendu_L->FlipBuffer(&RectAffichage, &RectScreen_L);
         Rendu_L->FlipBuffer(&localRect, &RectScreen_L);
      }
      else
      {
         // Si le rafraichissement est bloqué on mémorise la zone à mettre à jour
         AjouterZoneARafraichir(Rect_L.left, Rect_L.top, Rect_L.right, Rect_L.bottom);
      }
   }
}

/*****************************************************************************
* But de la fonction : Créer la Fenêtre principale
* --------------------
*
* Parametres :
* ------------
*
* Entree : TypeEcran_P, le type de RHM
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::Creer(unsigned int TypeEcran_P, unsigned int TypeEcranMaintenance_P,
   unsigned int SortieAffichageVente_P, unsigned int SortieAffichageMaintenance_P,
   bool SortieEcranTotal_P)
{
   WNDCLASSEX WC_L;

   // Definition du type d'ecran : Le type d'ecran correspond au numéro du bit à 1
   // ansi, Type 1 = 0x1, Type 2 = 0x2, Type 3 = 0x4, ...
   //RHM hardcoded to sales size in order to have regular loading screen in 768x1024 resolution
   m_TypeRhm = TypeEcran_P;// TypeEcranMaintenance_P;
   m_TypeRhmVente = TypeEcran_P;
   m_TypeRhmMaintenance = TypeEcran_P;// TypeEcranMaintenance_P;
   m_TypeRhmMaintenanceOriginal = TypeEcranMaintenance_P;
   // Affichage sur le second écran
   SortieAffichageVente = SortieAffichageVente_P;
   SortieAffichageMaintenance = SortieAffichageMaintenance_P;
   SortieEcranTotal = SortieEcranTotal_P;
   firstInit = true; //first init flag used to reload maintenance screens in configured resolution after end of the initialization process
   refreshOnRhmTypeChange = false;

   RectVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
   RectVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
   RectVirtualScreen.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
   RectVirtualScreen.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);

   // Initialiser pour la première fois
   //Initialiser(VRAI);
   Initialiser(FAUX);

   // Init des fonds
   I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
   FondAgent = Rendu_L->PreparerFond(BitmapFondAgent.DonneHBitmap());
   FondAgentFormatVente = Rendu_L->PreparerFond(BitmapFondAgentFormatVente.DonneHBitmap());

   // Enregistrement de la classe de l'application
   WC_L.cbSize = sizeof(WNDCLASSEX);
   WC_L.style = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
   WC_L.lpfnWndProc = GestionMessage;
   WC_L.cbClsExtra = 0;
   WC_L.cbWndExtra = 0;
   WC_L.hInstance = (HINSTANCE)Instance_DLL_S;//hInstance_P;
   WC_L.hIcon = 0;
   WC_L.hCursor = LoadCursor(NULL, IDC_ARROW);
   WC_L.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
   WC_L.lpszMenuName = 0;
   WC_L.lpszClassName = RHM_CLASS_NAME;
   WC_L.hIconSm = NULL;


   RegisterClassEx(&WC_L);

   // Creation de la fenetre chassis 
   Wnd = CreateWindowEx(NULL,
      RHM_CLASS_NAME,
      NULL,
#ifndef _DEBUG
      WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
#else
      WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU,
#endif
      //0,0,640,480,
      RectVirtualScreen.left, RectVirtualScreen.top, RectVirtualScreen.right, RectVirtualScreen.bottom,
      NULL, NULL, (HINSTANCE)Instance_DLL_S,
      (void*)this);

   char buff[2];
   GetEnvironmentVariable("SHOW_MOUSE_CURSOR", buff, 2);

   if (buff[0] == '0')
   {
      ShowCursor(FALSE);
   }
   else
   {
      ShowCursor(TRUE);
      SetCursor(LoadCursor(NULL, IDC_ARROW));
   }

   ::ShowWindow(Wnd, SW_SHOW);
   UpdateWindow(Wnd);
   // se mettre au premier plan au demarrage
   SetForegroundWindow(Wnd);
}

/*****************************************************************************
* But de la fonction : Libérer les Fenêtres.
* --------------------
*
* Parametres :
* ------------
*
* Entree : ChargementDynamique_P, VRAI si on veut libérer les éléments dynamiques.
*                                 FAUX si on veut libérer les éléments statiques.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::Liberer(unsigned int ChargementDynamique_P)
{
   I_Renderer * renderer = GestionnaireRendu.DonnerRendu();

   // Liberation des contextes (si necessaire)
   GestionnaireRendu.Liberer();

   // Libérer les éléments non dynamiques
   if (ChargementDynamique_P == FAUX)
   {
      // Aucune fenetre courante
      FenetreCourante = NULL;

      //// Liberation des fonds statiques
      renderer->LibererFond(FondAgent);
      FondAgent = NULL;
      renderer->LibererFond(FondAgentFormatVente);
      FondAgentFormatVente = NULL;

      // Liberation des fenetres
      FenetreAgentMenu.Destruction();
      FenetreAgentSousMenu.Destruction();
      FenetreAgentTrace.Destruction();
      FenetreAgentConsultation.Destruction();
      FenetreAgentRechargementMonnaie.Destruction();
      FenetreAgentTestRouleau.Destruction();
      FenetreAgentPannesEtats.Destruction();
      FenetreAgentEntreesSorties.Destruction();
      FenetreAgentTestTouches.Destruction();
      FenetreAgentAccesImprimante.Destruction();
      FenetreAgentMAJCptICS.Destruction();
      FenetreAgentCptMny.Destruction();
      FenetreAgentMAJCptMny.Destruction();
      FenetreAgentVidage.Destruction();
      FenetreAgentFichier.Destruction();
      FenetreAgentMaintenanceMPC.Destruction();
      FenetreAgentSaisie.Destruction();
      FenetreAgentSelFichier.Destruction();
      FenetreAgentConfirmation.Destruction();
   }
   // Libérer les éléments dynamiques
   else
   {
      GestionnaireFenetreVente->DestruitFenetreVente();

      FenetreVenteAccueilMalvoyants.Destruction();
      FenetreVenteConsultationCSCMalvoyants.Destruction();
      FenetreVenteFabricationMalvoyants.Destruction();
      FenetreVenteFinFabricationMalvoyants.Destruction();
      FenetreVenteIncidentMalvoyants.Destruction();
      FenetreVentePaiementMalvoyants.Destruction();
      FenetreVenteSelectionMalvoyants.Destruction();
      FenetreVenteSelectionMoyenPaiementMalvoyants.Destruction();

      renderer->LibererFond(FondVente);
      FondVente = NULL;
      BitmapFondVente.LibererRessources();

      renderer->LibererFond(FondVenteConducteur);
      FondVenteConducteur = NULL;
      BitmapFondVenteConducteur.LibererRessources();

      renderer->LibererFond(FondVenteMalvoyant);
      FondVenteMalvoyant = NULL;
      BitmapFondVenteMalvoyants.LibererRessources();

      if (SelectionZoneSelectionnees != NULL)
      {
         SelectionZoneSelectionnees->LibererRessources();
         delete SelectionZoneSelectionnees;
         SelectionZoneSelectionnees = NULL;
      }
      if (SelectionZoneSelectionnables != NULL)
      {
         SelectionZoneSelectionnables->LibererRessources();
         delete SelectionZoneSelectionnables;
         SelectionZoneSelectionnables = NULL;
      }
      if (SelectionZoneNonSelectionnees != NULL)
      {
         SelectionZoneNonSelectionnees->LibererRessources();
         delete SelectionZoneNonSelectionnees;
         SelectionZoneNonSelectionnees = NULL;
      }
      if (SelectionZoneMasque != NULL)
      {
         SelectionZoneMasque->LibererRessources();
         delete SelectionZoneMasque;
         SelectionZoneMasque = NULL;
      }
   }
}

/*****************************************************************************
* But de la fonction : Initialiser la Fenêtre de base.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
unsigned int T_GestionnaireFenetre::InitGenericFenetre()
{
   InitDialog();
   return 0;
}

/*****************************************************************************
* But de la fonction : Deplacer un fichier
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::DeplacerFichier(char* CheminOrigine_P, char* CheminDestination_P)
{
   // Copier chaque image dans le répertoire de destination
   if (Copier(CheminOrigine_P, CheminDestination_P) != FICHIER_ABSENT)
   {
      Tracer("Copie des masques de zones :  %s\n", CheminDestination_P);
   }
   // Supprimer le fichier d'origine
   SupprimerFichier(CheminOrigine_P);
}

/*****************************************************************************
* But de la fonction : Callback pour EnumDisplayMonitors pour affiché la rhm
* en multiscreen en debug. Vachement pratique !
* --------------------
*
* Parametres :
* ------------
*
* Entree : Plein d'infos de moniteur
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
*****************************************************************************/
RECT Moniteur;

BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
   static int NbMoniteur = 0;
   static int NumMoniteur = 0;
   MONITORINFO MonitorInfos;
   //RECT *RectMoniteur = (RECT *)dwData;
   MonitorsInfos* monitorsInfo = (MonitorsInfos*)dwData;

   ++NbMoniteur;
   MonitorInfos.cbSize = sizeof(MonitorInfos);
   GetMonitorInfo(hMonitor, &MonitorInfos);
   if (NumMoniteur == 0 && (MonitorInfos.dwFlags & MONITORINFOF_PRIMARY) != 0)
   {
      NumMoniteur = NbMoniteur;
      Moniteur.left = lprcMonitor->left;
      Moniteur.top = lprcMonitor->top;
   }
   if ((MonitorInfos.dwFlags & MONITORINFOF_PRIMARY) == 0)
   {
      NumMoniteur = NbMoniteur;
      Moniteur.left = lprcMonitor->left;
      Moniteur.top = lprcMonitor->top;
   }

#ifndef TOULOUSE_NOT_SUPPORTED
   monitorsInfo->Add(NbMoniteur, *lprcMonitor, (MonitorInfos.dwFlags & MONITORINFOF_PRIMARY) == 1);

   //switch( NbMoniteur)
   //{
   //case 1:
   //case 2:
   //case 3:
//      RectMoniteur[NbMoniteur - 1] = *lprcMonitor;
//      break;
//   default :
//      Tracer(" Nombre maximum d'écrans supportés dépassé. \n");
//      break;
//   }
#else
   if ((MonitorInfos.dwFlags & MONITORINFOF_PRIMARY) != 0)
   {
      RectMoniteur[0] = *lprcMonitor;
   }
   else if (NbMoniteur <= 2)
   {
      RectMoniteur[1] = *lprcMonitor;
   }
#endif
   return TRUE;
}

/*****************************************************************************
* But de la fonction : Initialiser toutes les fenêtres utilisées.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Indicateur de chargement dynamique.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers : Cette fonction est d'abord appelée une fois pour initialiser
*                       les données statiques, puis elle est appelée une deuxième
*                       fois pour initialiser les données dynamiques.
*
*****************************************************************************/
unsigned int T_GestionnaireFenetre::InitDialog(unsigned int ChargementDynamique_P)
{
   const unsigned int initialTypeEcran = TypeEcran;
   const unsigned int initialTypeRhm = m_TypeRhm;

   // Creations et initialisations diverses
   Tracer(" > INITDIALOG : Retaillage des écrans \n");
   Retaillage();
   Tracer(" > INITDIALOG : Placement de la fenetre\n");

   if (firstInit)
   {
      EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, (LPARAM)&monitorsInfos_);
      LogMonitorsConfiguration();

      AfficherSortieEcran(false, SortieAffichageVente);
   }

   // Code page pour tous les écrans : UTF8
   DonnerCodePage(RHM_CODE_PAGE_UTF8);

   // Initialisation du renderer
   GestionnaireRendu.Initialisation(Wnd, Largeur, Hauteur);
   I_Renderer* renderer = GestionnaireRendu.DonnerRendu();

   Tracer(" > INITDIALOG : Affichage d'un fond agent\n");
   renderer->AfficherBitmap(&BitmapFondAgent, 0, 0, Largeur, Hauteur);

   renderer->AfficherBitmap(BmpInit,
      (Largeur / 2 - BmpInit->DonneBitmap()->GetWidth() / 2),
      (Hauteur / 2 - BmpInit->DonneBitmap()->GetHeight() / 2),
      BmpInit->DonneBitmap()->GetWidth(),
      BmpInit->DonneBitmap()->GetHeight());

   // Initialiser les éléments statiques dès le départ
   if (ChargementDynamique_P == FAUX)
   {
      Tracer(" > Création des écrans de Maintenance\n");
      TypeEcran = 1; // Pour que l'init sur chaque fenetre prenne le type écran maintenance et non vente
      Retaillage();
      GestionnaireRendu.Initialisation(Wnd, Largeur, Hauteur);
      ////////////////////////////////////////
      // Creation des fenetres agents
      CREATION(FenetreAgentMenu);
      CREATION(FenetreAgentSousMenu);
      CREATION(FenetreAgentConsultation);
      CREATION(FenetreAgentPannesEtats);
      CREATION(FenetreAgentRechargementMonnaie);
      CREATION(FenetreAgentEntreesSorties);
      CREATION(FenetreAgentAccesImprimante);
      CREATION(FenetreAgentMAJCptICS);
      CREATION(FenetreAgentCptMny);
      CREATION(FenetreAgentMAJCptMny);
      CREATION(FenetreAgentVidage);
      CREATION(FenetreAgentFichier);
      CREATION(FenetreAgentMaintenanceMPC);
      CREATION(FenetreAgentSaisie);
      CREATION(FenetreAgentSelFichier);
      CREATION(FenetreAgentTrace);
      CREATION(FenetreAgentConfirmation);

      if (m_TypeRhmVente <= 2)
      {
         CREATION(FenetreAgentTestRouleau);
      }

      else
      {
         CREATION(FenetreAgentTestTouches);
      }

      // Gestion du curseur
      SetTimer(Wnd, TIMER_CURSEUR, 500, NULL);

      ReinitialiserZoneARafraichir();
   }

   // Initialiser les éléments dynamiques
   else
   {
      Tracer(" > Créartion des écrans de Vente\n");
      TypeEcran = 101; // Pour que l'init sur chaque fenetre prenne le type écran vente et non maintenance
      Retaillage();
      GestionnaireRendu.Initialisation(Wnd, Largeur, Hauteur);

      ////////////////////////////////////////
      // Creation des fenetres vente
      GestionnaireFenetreVente->CreeFenetreVente(Largeur, Hauteur);

      CREATION(FenetreVenteAccueilMalvoyants);
      CREATION(FenetreVenteFabricationMalvoyants);
      CREATION(FenetreVenteConsultationCSCMalvoyants);
      CREATION(FenetreVenteFinFabricationMalvoyants);
      CREATION(FenetreVenteIncidentMalvoyants);
      CREATION(FenetreVentePaiementMalvoyants);
      CREATION(FenetreVenteSelectionMalvoyants);
      CREATION(FenetreVenteSelectionMoyenPaiementMalvoyants);
   }


   // Restore ecran type
   TypeEcran = initialTypeEcran;

   m_TypeRhm = initialTypeRhm;
   Retaillage();

   return TRUE;
}
/*****************************************************************************
* But de la fonction : Filtrer les messages windows
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type du message à traiter
*
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::FiltrerMessageWindows(MSG *Msg_P)
{
   unsigned int AutoriserDiffusion_L = VRAI;

   /* Si les interactions sont bloquées et que l'usager clique sur un bouton */
   if (ParamInit.TemporisationClic &&
      InteractionsBloquees &&
      (Msg_P->message == WM_LBUTTONDOWN || Msg_P->message == WM_LBUTTONUP))
   {
      /* Consommer l'événement */
      AutoriserDiffusion_L = FAUX;
   }

   /* Si l'événement doit être diffuser aux autres fenêtres */
   if (AutoriserDiffusion_L)
   {
      /* Faire passer le message */
      TranslateMessage(Msg_P);
      DispatchMessage(Msg_P);
   }
   else
   {
      /* Faire jouer un son d'erreur */
      PlaySound(MAKEINTRESOURCE(IDR_SON_INCORRECT_DEFAUT), Instance_DLL_S, SND_RESOURCE | SND_ASYNC);
   }
}

/*****************************************************************************
* But de la fonction : Traiter les messages recus
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type du message à traiter
*          Paramètres associés au message.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
long T_GestionnaireFenetre::GestionMessageLocal(UINT Message_P, WPARAM wParam_P, LPARAM lParam_P)
{
   //unsigned int i;
   unsigned int MessageInterrompu_L = FAUX;
   char CheminOrigine_L[32];
   char CheminDestination_L[32];
   long Return_L = 0;

   // Recherche des messages en provenance de l'application
   switch (Message_P)
   {
      // Messages users
   case WM_ARRET:
   {
      // Arret de l'interface
      PostQuitMessage(0);
   }
   break;

   case WM_AFFICHAGE_ECRAN:
   {
      FenetreCapturee = NULL;

      // Trace à activer si besoin de voir les demandes écran de l'appli.
      // ATTENTION : Dégrade fortement les performances, réservé au debug embarqué.
      //             Ne pas utiliser dans une version opérationnelle.
      // Tracer("WM_AFFICHAGE_ECRAN : %u\n",wParam_P);

      ChangementEcran(wParam_P, (void*)lParam_P);
      //ShowWindow( TRUE);

      if (ParamInit.EcranPremierPlan == VRAI)
      {
#ifndef _DEBUG
         SetForegroundWindow(Wnd);
#endif
      }
   }
   break;

   // Tempo inaction
   case WM_ARMERTEMPOINACTION:
   {
      unsigned int *Tempo_L = (unsigned int*)lParam_P;

      // Redemmarrage de la tempo inaction
      RedemarrageTempoInaction(*Tempo_L);

      delete Tempo_L;
   }
   break;

   case WM_INHIBER_ROULEAU_BOUTONS:
   {
      unsigned int *Inhibition_L = (unsigned int *)lParam_P;

      RouleauBoutonsInhibes = *Inhibition_L;

      delete Inhibition_L;
   }
   break;

   case WM_SYSCOMMAND:

      // Récupérer évenement ALT+F4
      if (wParam_P == SC_CLOSE ||
         wParam_P == SC_PREVWINDOW)
      {
         // Ne pas envoyer le message aux fenêtres
         Return_L = VRAI;
      }

      break;
   case WM_SYSKEYUP:
   {
      unsigned int CtrlEnfonce_L = (GetKeyState(VK_CONTROL) & 0x8000) ? 12 : 0;

      if ((FenetreCourante != NULL) && wParam_P == VK_F10)
      {
         FenetreCourante->ToucheFonction(VK_F10 - VK_F1, CtrlEnfonce_L);

         // Action utilisateur : on redémarre la tempo d'inaction
         RedemarrageTempoInaction();
      }
   }
   break;

   case WM_ACTIVER_VEROUILLAGEMAJ:
   {
      unsigned int *Activation_L = (unsigned int *)lParam_P;
#ifndef _DEBUG

      short KeyStateAvt_L, KeyStateApr_L, KeyStateEntreDeux_L;

      // RQ :
      // Si Caps Lock   : KeyStateAvt_L = 0x0001, KeyStateEntreDeux_L = 0xFF80, KeyStateApr_L = 0x0000
      // Si Caps Unlock : KeyStateAvt_L = 0x0000, KeyStateEntreDeux_L = 0xFF81, KeyStateApr_L = 0x0001

      KeyStateAvt_L = GetKeyState(VK_CAPITAL);

      if ((*Activation_L && KeyStateAvt_L == 0x0000) || (!(*Activation_L) && KeyStateAvt_L == 0x0001))
      {
         // Simulate a key press      
         keybd_event(VK_CAPITAL,
            0x45,
            KEYEVENTF_EXTENDEDKEY | 0,
            0);

         KeyStateEntreDeux_L = GetKeyState(VK_CAPITAL);

         // Simulate a key release
         keybd_event(VK_CAPITAL,
            0x45,
            KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
            0);

         KeyStateApr_L = GetKeyState(VK_CAPITAL);
      }
#endif
      delete Activation_L;

   }
   break;

   case WM_DESTROY:
   {
      // Libérer les ressources statiques
      Liberer(FAUX);

      // Libérer les ressources dynamiques
      Liberer(VRAI);
   }
   break;

   case WM_NCHITTEST:
   {
      Return_L = HTCLIENT;
   }
   break;

   case WM_CHAR:
   {
      if ((FenetreCourante != NULL))
      {
         // On transmet l'événement à la fenêtre courante
         if (FenetreCourante->NouvelleTouche((char)wParam_P) == VRAI)
         {
            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }

      }
   }
   break;

   case WM_TIMER:
   {
      if (wParam_P >= TIMER_BASE_ANIME)
      {
         int num_chronos = wParam_P - TIMER_BASE_ANIME;
         if (ChronosAnimation[num_chronos] != NULL)
         {
            ChronosAnimation[num_chronos]->Anime();
         }
      }
      else if (wParam_P == TIMER_CURSEUR)             // Curseur
      {
         if ((FenetreCourante != NULL))
         {
            FenetreCourante->Curseur(1);
         }
      }
      else if (wParam_P == TIMER_INACTION)       // Inaction
      {
         DebloquerInteractions();
         PosterCrTempoInaction(&TacheClient);
      }
      else if (wParam_P == TIMER_REVEIL)
      {
         FenetreTimer->Reveil();      // Reveil
      }
      else if (wParam_P == TIMER_AFFICHAGE)
      {
         /* Supprimer le timer */
         KillTimer(Wnd, TIMER_AFFICHAGE);

         DebloquerInteractions();
      }
      else if (wParam_P == TIMER_DATE_HEURE)
      {
         /* Supprimer le timer */
         KillTimer(Wnd, TIMER_DATE_HEURE);

         if (FenetreCourante != NULL)
         {
            FenetreCourante->Reveil();
         }
      }
      else if (wParam_P == TIMER_RAZ_SEQUENCE_SECRETE && DonneTypeRhm() == RHM_TYPE_1)
      {
         InitialiserSequenceSecrete(); // RAZ sequence secrete, autre essai...
      }

      else if (_statetoolTip == true)
      {
         DebloquerInteractions();
         PosterCrTempoToolTip(&TacheClient);
         _statetoolTip = false;
      }
   }
   break;

   case WM_LBUTTONDOWN:
   {
      // Clic souris : Ecran touchee
#ifdef TOULOUSE_NOT_SUPPORTED
      int xPos = LOWORD(lParam_P) - RectAffichage.left;
      int yPos = HIWORD(lParam_P) - RectAffichage.top;
#else
      int xPos = LOWORD(lParam_P);
      int yPos = HIWORD(lParam_P);
#endif

      // Conversion par le rendu
      I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
      Rendu_L->ConversionPoint(&xPos, &yPos);

      if (FenetreCapturee != NULL)
      {
         FenetreCapturee->Clic(xPos, yPos, 1);
      }
      if (FenetreCourante != NULL)
      {
         FenetreCourante->Clic(xPos, yPos, 1);
      }
   }
   break;

   case WM_LBUTTONUP:
   {
      if (ParamInit.TemporisationClic &&
         FenetreCourante != NULL &&
         InteractionsBloquees == FAUX)
      {
         /* Temporiser les actions sur les clics */
         FenetreCourante->TemporiserClic();
      }

      // Bouton relache : on relache aussi le focus
#ifdef TOULOUSE_NOT_SUPPORTED
      int xPos = LOWORD(lParam_P) - RectAffichage.left;
      int yPos = HIWORD(lParam_P) - RectAffichage.top;
#else
      int xPos = LOWORD(lParam_P);
      int yPos = HIWORD(lParam_P);
#endif
      // Conversion par le rendu
      I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
      Rendu_L->ConversionPoint(&xPos, &yPos);

      // On transmet l'événement à la fenêtre capturée.
      if (FenetreCapturee != NULL)
      {
         RECT Rect_L = FenetreCapturee->DonnerPosition();
         FenetreCapturee->Clic(xPos - Rect_L.left, yPos - Rect_L.top, 0);
      }
      // Pas de fenêtre capturée, on transmet l'événement à la fenêtre courante.
      else if (FenetreCourante != NULL)
      {
         RECT Rect_L = FenetreCourante->DonnerPosition();
         FenetreCourante->Clic(xPos - Rect_L.left, yPos - Rect_L.top, 0);
      }

      // Action utilisateur : on redémarre la tempo d'inaction
      RedemarrageTempoInaction();
   }
   break;

   case WM_MOUSEMOVE:
   {
#ifdef TOULOUSE_NOT_SUPPORTED
      int xPos = LOWORD(lParam_P) - RectAffichage.left;
      int yPos = HIWORD(lParam_P) - RectAffichage.top;
#else
      int xPos = LOWORD(lParam_P);
      int yPos = HIWORD(lParam_P);
#endif

      // Conversion par le rendu
      GestionnaireRendu.DonnerRendu()->ConversionPoint(&xPos, &yPos);

      if (FenetreCapturee != NULL)
      {
         RECT Rect_L = FenetreCapturee->DonnerPosition();
         FenetreCapturee->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
      }
      else if (FenetreCourante != NULL)
      {
         RECT Rect_L = FenetreCourante->DonnerPosition();
         FenetreCourante->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
      }
   }
   break;
   case WM_POINTERENTER:
   {
      int xPos = GET_X_LPARAM(lParam_P) - RectAffichage.left;
      int yPos = GET_Y_LPARAM(lParam_P) - RectAffichage.top;

      // Conversion par le rendu
      GestionnaireRendu.DonnerRendu()->ConversionPoint(&xPos, &yPos);

      if (FenetreCapturee != NULL)
      {
         RECT Rect_L = FenetreCapturee->DonnerPosition();
         //FenetreCapturee->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
         FenetreCapturee->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
      }
      else if (FenetreCourante != NULL)
      {
         RECT Rect_L = FenetreCourante->DonnerPosition();
         // FenetreCourante->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
         FenetreCourante->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
      }
   }
   break;

   case WM_POINTERLEAVE:
   {
      int xPos = GET_X_LPARAM(lParam_P) - RectAffichage.left;
      int yPos = GET_Y_LPARAM(lParam_P) - RectAffichage.top;

      // Conversion par le rendu
      GestionnaireRendu.DonnerRendu()->ConversionPoint(&xPos, &yPos);

      if (FenetreCapturee != NULL)
      {
         RECT Rect_L = FenetreCapturee->DonnerPosition();
         //FenetreCapturee->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
         FenetreCapturee->DeplacerSouris(0, 0);
      }
      else if (FenetreCourante != NULL)
      {
         RECT Rect_L = FenetreCourante->DonnerPosition();
         // FenetreCourante->DeplacerSouris(xPos - Rect_L.left, yPos - Rect_L.top);
         FenetreCourante->DeplacerSouris(0, 0);
      }
   }
   break;

   case WM_GETDLGCODE:
   {
      Return_L = DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTMESSAGE | DLGC_WANTCHARS;
   }
   break;

   case WM_KEYDOWN:
   {
      // Touche enfoncee
      switch (wParam_P)
      {
         // Touche de fonction (ie : Touche BR ?)
      case VK_F1:
      case VK_F2:
      case VK_F3:
      case VK_F4:
      case VK_F5:
      case VK_F6:
      case VK_F7:
      case VK_F8:
      case VK_F9:
      case VK_F10:
      case VK_F11:
      case VK_F12:
      {
         unsigned int CtrlEnfonce_L = (GetKeyState(VK_CONTROL) & 0x8000) ? 12 : 0;

         if ((FenetreCourante != NULL))
         {
            FenetreCourante->ToucheFonction(wParam_P - VK_F1, CtrlEnfonce_L);

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
      }
      break;
      case VK_F21:
      {
         int test = 1;
      }
      break;
      // Touche up/down/left/right (ie : Deplacement du rouleau/touches ?)
      case VK_DOWN:
         if ((FenetreCourante != NULL))
         {
            AjouterToucheDansSequence(WM_DEPLACEMENT_ROULEAU_BAS);
            FenetreCourante->DeplacerBas();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      case VK_UP:
         if ((FenetreCourante != NULL))
         {
            AjouterToucheDansSequence(WM_DEPLACEMENT_ROULEAU_HAUT);
            FenetreCourante->DeplacerHaut();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      case VK_LEFT:
         if ((FenetreCourante != NULL))
         {
            FenetreCourante->DeplacerGauche();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      case VK_RIGHT:
         if ((FenetreCourante != NULL))
         {
            FenetreCourante->DeplacerDroite();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

         // Touche Entree/Esc (ie : Valider / Annuler ?)
      case VK_ESCAPE:
         if ((FenetreCourante != NULL))
         {
            AjouterToucheDansSequence(WM_ANNULATION_BOUTON);
            FenetreCourante->Annuler();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;
      case VK_RETURN:
         if ((FenetreCourante != NULL))
         {
            AjouterToucheDansSequence(WM_VALIDATION_BOUTON);
            if (EstCeSequenceSecreteSaisie())
            {
               FenetreCourante->DemanderIdentificationAgent();
            }
            else
            {
               FenetreCourante->Valider();
            }

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      case VK_PRIOR:
      case VK_SUBTRACT:
         // Touche - : page precedente
         if ((FenetreCourante != NULL))
         {
            FenetreCourante->ToucheFonction(TOUCHE_SPECIALE_PRECEDENT, 0);

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      case VK_NEXT:
      case VK_ADD:
         // Touche + : page suivante
         if ((FenetreCourante != NULL))
         {
            FenetreCourante->ToucheFonction(TOUCHE_SPECIALE_SUIVANT, 0);

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
         break;

      default:
         if ((FenetreCourante != NULL))
         {
            if ((FenetreCourante->ToucheSpeciale(wParam_P)) == VRAI)
            {
               // Action utilisateur : on redémarre la tempo d'inaction
               RedemarrageTempoInaction();
            }
         }
      }
   }
   break;

   case WM_MOUSEWHEEL:
   {
      // Roulement souris : Deplacement du rouleau
      short zDelta = wParam_P >> 16;
      if ((zDelta > 0) && (FenetreCourante != NULL))

      {
         AjouterToucheDansSequence(WM_DEPLACEMENT_ROULEAU_HAUT);
         FenetreCourante->DeplacerHaut();

         // Action utilisateur : on redémarre la tempo d'inaction
         RedemarrageTempoInaction();
      }
      else if ((zDelta < 0) && (FenetreCourante != NULL))
      {
         AjouterToucheDansSequence(WM_DEPLACEMENT_ROULEAU_BAS);
         FenetreCourante->DeplacerBas();

         // Action utilisateur : on redémarre la tempo d'inaction
         RedemarrageTempoInaction();
      }
   }
   break;

   case WM_CHANGEMENT_RENDU:
   {
      ChangerRendu((T_RhmTypeRendu)wParam_P);
   }
   break;

   case WM_CHANGEMENT_ECRAN:
   {
      const bool enableMaintenanceDisplay = ((unsigned int)wParam_P == VRAI);

      // First parameter : must the screen be displayed on maintenance or on sale display device?
      unsigned int TypeEcran_L = FenetreCourante->DonneTypeEcran();
      sortieMaintenanceActive = enableMaintenanceDisplay;
      // Second parameter : wait for door open before displaying agent menu on external screen
      if ((unsigned int)lParam_P == VRAI)
      {
         WaitForDoorOpen = true;
      }

      if (TypeEcran_L != 0 && TypeEcran_L != AGT_LIGNESERVICE)
      {
         ChangementEcran(TypeEcran_L, NULL);
      }
   }
   break;

   case WM_CHANGEMENT_MODE:
   {
      T_RhmBasculerMode* BasculerMode_L = (T_RhmBasculerMode*)lParam_P;

      // Memoriser le mode dans lequel on se trouve
      EstModeMalvoyants = (BasculerMode_L->ModeRhm == RHM_MODE_MALVOYANTS ? 1 : 0);

      // Memoriser la valeur de la temporisation inaction
      ValeurTempo = BasculerMode_L->Temporisation;

      delete BasculerMode_L;

      if (EstModeMalvoyants == 0)
      {
         // Arreter la lecture d'aide vocale si lecture en cours
         ArreterLectureAideVocale();
      }

      // On redémarre la tempo (car pas la meme entre ecran normal et malvoyant !
      RedemarrageTempoInaction();

      // Lors de l'init, le type écran est à 0, on ne veut pas traiter le changement de mode
      if (FenetreCourante->DonneTypeEcran() != 0)
      {
         ChangementEcran(FenetreCourante->DonneTypeEcran(), NULL);
      }
   }
   break;

   case WM_DEPLACEMENT_ROULEAU:
   {
      if ((RouleauBoutonsInhibes == FAUX))
      {
         // Deplacement du rouleau
         int CompteurRouleau_L = (int)lParam_P;

         if (FenetreCourante != NULL)
         {
            FenetreCourante->TraiterDeplacementRouleau(CompteurRouleau_L);

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
      }
   }
   break;

   case WM_VALIDATION_BOUTON:
   {
      if ((RouleauBoutonsInhibes == FAUX))
      {
         if (FenetreCourante != NULL)
         {
            AjouterToucheDansSequence(WM_VALIDATION_BOUTON);
            if (EstCeSequenceSecreteSaisie())
            {
               FenetreCourante->DemanderIdentificationAgent();
            }
            else
            {
               FenetreCourante->Valider();
            }

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
      }
   }
   break;

   case WM_ANNULATION_BOUTON:
   {
      if ((RouleauBoutonsInhibes == FAUX))
      {
         if (FenetreCourante != NULL)
         {
            AjouterToucheDansSequence(WM_ANNULATION_BOUTON);
            FenetreCourante->Annuler();

            // Action utilisateur : on redémarre la tempo d'inaction
            RedemarrageTempoInaction();
         }
      }
   }
   break;

   case WM_INITIALISATION:
   {
      Tracer("INITIALISATION, DEBUT\n");
      T_RhmParametresInitialisation* ParamInit_L = (T_RhmParametresInitialisation*)lParam_P;

      ParamInit.EcranPremierPlan = ParamInit_L->EcranPremierPlan;
      ParamInit.TemporisationClic = ParamInit_L->TemporisationClic;
      ParamInit.MalvoyantActif = ParamInit_L->MalvoyantActif;
      ParamInit.AideVocaleWav = ParamInit_L->AideVocaleWav;
      ParamInit.AideVocaleTTS = ParamInit_L->AideVocaleTTS;
      ParamInit.AideVocaleTTSNuance = ParamInit_L->AideVocaleTTSNuance;
      ParamInit.TempoTooltip = ParamInit_L->TempoTooltip;
      strcpy(ParamInit.TtsLicenceString, ParamInit_L->TtsLicenceString);
      memcpy(&ParamInit.TtsVoixDispos, ParamInit_L->TtsVoixDispos, sizeof ParamInit.TtsVoixDispos);

      delete ParamInit_L;

      // Initialisation des Layouts
      GestionnaireFenetreVente->InitialiseLayouts(ParamInit);

      // Initialisation de la synthèse vocale
      if (ParamInit.AideVocaleTTS && Tts == NULL)
      {
         T_VocalEngine vocalEngine;
         if (ParamInit.AideVocaleTTSNuance)
         {
            vocalEngine = ENGINE_NUANCE;
         }
         else
         {
            vocalEngine = ENGINE_ACAPELA;
         }

         Tts = new T_ClassSyntheseVocale();
         Tts->Initialiser(ParamInit.TtsLicenceString, vocalEngine);

         // Ajoute les voix pour pouvoir les utiliser
         for (unsigned int i = 0; i < (RHM_BOUTONS_LANGUE_MAX + 1) && ParamInit.TtsVoixDispos[i].TypeLangue != 0; i++)
         {
            Tts->AjouterVoix(ParamInit.TtsVoixDispos[i].TypeLangue, ParamInit.TtsVoixDispos[i].NomVoix);
         }
      }
      if (ParamInit.AideVocaleWav && AudioPlayer == NULL)
      {
         AudioPlayer = new T_AudioPlayer(HandleFenetre());
      }

      // Liberer les éléments dynamiques
      Tracer("Libération des éléments dynamiques\n");
      Liberer(VRAI);

      // Raz des structures
      memset(CheminOrigine_L, 0, sizeof CheminOrigine_L);
      memset(CheminDestination_L, 0, sizeof CheminDestination_L);

      // Templates
      Tracer("Initialisation des images dynamiques\n");

      // Lancer le chargement des images dynamiques
      Initialiser(VRAI);

      Tracer("Initialisation des fenêtres dynamiques\n");
      // Lancer la création des fenêtres à recharger dynamiquement
      InitDialog(VRAI);

      InitialiserImages();

      Tracer("Poster compte-rendu d'initialisation\n");
      // Poster le compte rendu d'initialisation
      PosterCompteRendu(&TacheClient,
         LETTRE_CR_INITIALISATION,
         NULL,
         0);

      Tracer("INITIALISATION, FIN\n");
   }
   break;

   case WM_LIBERER_RESSOURCES:
   {
      // Libérer les ressources dynamiques
      //Liberer (VRAI);
      I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
      GestionnaireFenetreVente->DestruitFenetreVente();

      FenetreVenteAccueilMalvoyants.Destruction();
      FenetreVenteConsultationCSCMalvoyants.Destruction();
      FenetreVenteFabricationMalvoyants.Destruction();
      FenetreVenteFinFabricationMalvoyants.Destruction();
      FenetreVenteIncidentMalvoyants.Destruction();
      FenetreVentePaiementMalvoyants.Destruction();
      FenetreVenteSelectionMalvoyants.Destruction();
      FenetreVenteSelectionMoyenPaiementMalvoyants.Destruction();

      Rendu_L->LibererFond(FondVente);
      BitmapFondVente.LibererRessources();
      FondVente = NULL;

      Rendu_L->LibererFond(FondVenteConducteur);
      BitmapFondVenteConducteur.LibererRessources();

      FondVenteConducteur = NULL;

      if (SelectionZoneSelectionnees != NULL)
      {
         SelectionZoneSelectionnees->LibererRessources();
      }
      if (SelectionZoneSelectionnables != NULL)
      {
         SelectionZoneSelectionnables->LibererRessources();
      }
      if (SelectionZoneNonSelectionnees != NULL)
      {
         SelectionZoneNonSelectionnees->LibererRessources();
      }
      if (SelectionZoneMasque != NULL)
      {
         SelectionZoneMasque->LibererRessources();
      }

      // On affiche le fond d'écran agent avant de l'init de la RHM
      //GestionnaireRendu.Initialisation ( Wnd, Largeur, Hauteur);
      //Rendu_L->AfficherBitmap ( &BitmapFondAgent, 0, 0 , Largeur, Hauteur);

      // Poster le compte rendu de libération
      PosterCompteRendu(&TacheClient,
         LETTRE_CR_LIBERATION_RESSOURCE,
         NULL,
         0);
   }

   break;

   case WM_LECTURE_TEXTE:
   {
      // Lecture Texte
      if (EstModeMalvoyants == VRAI)
      {
         // Arreter la lecture d'aide vocale si lecture en cours
         ArreterLectureAideVocale();

         // Lancer une nouvelle lecture
         LireTTS((T_RhmMalvoyants*)lParam_P);

      }
      delete ((T_RhmMalvoyants*)lParam_P);
   }
   break;

   case WM_ARRET_LECTURE_TEXTE:
   {
      // Arreter la lecture d'aide vocale si lecture en cours
      ArreterLectureAideVocale();
   }
   break;

   case WM_ACTIVER_BUZZER:
   {
      PlaySound(MAKEINTRESOURCE(IDR_SON_BUZZER), Instance_DLL_S, SND_RESOURCE | SND_ASYNC | SND_LOOP);
      BuzzerActif = true;
   }
   break;

   case WM_DESACTIVER_BUZZER:
   {
      if (BuzzerActif)
      {
         PlaySound(NULL, NULL, SND_ASYNC);
         BuzzerActif = false;
      }
   }
   break;

   case WM_LIRE_FICHIER_AUDIO:
   {
      if (BuzzerActif)
      {
         PlaySound(NULL, NULL, SND_ASYNC);
      }

      ArreterLectureAideVocale();

      LireFichierAudio((char *)lParam_P);
      delete[](char *)lParam_P;
   }

   case WM_STOP_FICHIER_AUDIO:
   {
      ArreterFichierAudio();
   }
   break;

   case WM_FIN_FICHIER_AUDIO:
   {
      if (FenetreCourante != NULL && AudioPlayer->IsPlaying() == false)
      {
         AudioPlayer->Close(); // Libere la mémoire !!!
         FenetreCourante->LireFichierAudio();
      }
   }
   break;

   case WM_CHANGER_FOND_ECRAN:
   {
      // Change le fond d'écran
      unsigned int *NumFond_L = (unsigned int *)lParam_P;
      NumeroFondVente = *NumFond_L;
      delete NumFond_L;
   }
   break;

   case WM_SET_OUT_OF_ORDER_TEXT:
   {
      char* text = (char*)lParam_P;
      InterventionInProgressWindow::setText(text);
      delete text;
   }
   break;

   default:
   {
      break;
   }

   }

   return Return_L;
}


/*****************************************************************************
* But de la fonction : Traitement le passage d'un écran à l'autre
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type d'écran à afficher.
*          Données associées à l'écran.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ChangerFondVente(char* NomImage_P)
{
   I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();
   Rendu_L->LibererFond(FondVente);
   BitmapFondVente.LibererRessources();
   BitmapFondVente.LoadFile(NomImage_P);
   FondVente = Rendu_L->PreparerFond(BitmapFondVente.DonneHBitmap());
}

/*****************************************************************************
* But de la fonction : Traitement le passage d'un écran à l'autre
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type d'écran à afficher.
*          Données associées à l'écran.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ChangementEcran(unsigned int TypeEcran_P, void* Arg_P)
{
   if (Arg_P != NULL && TypeEcran_P != AGT_LIGNESERVICE)
   {
      delete PtrEcran;
      PtrEcran = Arg_P;

      if (PtrMenu != NULL)
      {
         LibererMemoire(PtrMenu); m_TypeRhm = m_TypeRhmVente;
         PtrMenu = NULL;
      }
   }

   // On bloque le rafraichissement pour le changement d'ecran sans stress
   BloquerRafraichissement();

   const bool previousScreenWasSaleScreen = (TypeEcran >= 100);
   const bool newScreenIsSaleScreen = (TypeEcran_P >= 100);
   const unsigned int oldTypeRhm = m_TypeRhm;
   bool saleDisplay = false;
   unsigned int outputId = 0, optionalOutputId = 0;

   TypeEcran = TypeEcran_P;
   if (TypeEcran != 0 && TypeEcran != AGT_LIGNESERVICE)
   {
      // sale or maintenance screen?
      if (TypeEcran < 100)
      {
         if (sortieMaintenanceActive)
         {
            m_TypeRhmMaintenance = m_TypeRhmMaintenanceOriginal;
            outputId = SortieAffichageMaintenance;
            optionalOutputId = SortieAffichageVente;
         }
         else
         {
            m_TypeRhmMaintenance = m_TypeRhmVente;
            outputId = SortieAffichageVente;
            optionalOutputId = 0;
         }

         m_TypeRhm = m_TypeRhmMaintenance;
         saleDisplay = false;
      }
      else
      {
         outputId = SortieAffichageVente;
         optionalOutputId = 0;

         m_TypeRhm = m_TypeRhmVente;
         saleDisplay = true;
      }

      // If sale type is different from the maintenance one, resize the screen.
      if (firstInit || (oldTypeRhm != m_TypeRhm) || (previousScreenWasSaleScreen != newScreenIsSaleScreen))
      {
         // Creations et initialisations diverses
         Tracer(" > ChangementEcran : Retaillage des écrans \n");
         Retaillage();

         refreshOnRhmTypeChange = true;
         AfficherSortieEcran(saleDisplay, outputId, optionalOutputId);

         GestionnaireRendu.Liberer();
         GestionnaireRendu.Initialisation(Wnd, Largeur, Hauteur);
      }
   }

   // Si on demande une mise à jour de l'écran d'accueil alors on ne redémarre pas la tempo d'inaction
   // Si on met à jour la ligne de service on ne la redémmarre pas non plus
   // Dans tous les autres cas on fait les traitements normalement.
   if (Arg_P != NULL)
   {
      if (((TypeEcran != VEN_ECRAN_ACCUEIL && TypeEcran != VEN_ECRAN_SEL_LANGUE) || ((T_RhmEcranAccueil*)Arg_P)->MiseAJour == FAUX)
         && (TypeEcran != AGT_LIGNESERVICE))
      {
         RedemarrageTempoInaction();
      }

      // Si on veut afficher la fenetre agent de traces alors on rend invisible la précédente
      if (FenetreCourante != NULL && TypeEcran != AGT_PRINTF && TypeEcran != AGT_LIGNESERVICE)
      {
         // On retire le focus
         FenetreCourante->DonnerFocus(NULL, 0);
         // On cache la fenetre
         FenetreCourante->Visible(0);
      }

      // Effacement de la fenetre printf en cas d'affichage d'un nouvel ecran
      // (hormis la commande Printf et LigneService)
      if (TypeEcran != AGT_LIGNESERVICE && TypeEcran != AGT_PRINTF)
      {
         FenetreAgentTrace.Effacer();
      }
   }

   // Sequence secrete autorisee suivant type d'écran
   AutoriserSequenceSecrete(TypeEcran);

   switch (TypeEcran)
   {
      //////////////////////////////////////
      // Ecran Agent
      //////////////////////////////////////
   case AGT_LISTE_FONCTION:
   {
      // Cas des donnes
      if (((T_RhmListeFonctions*)PtrEcran)->MiseAJour == FAUX)
      {
         // Mise a jour de l'ecran
         FenetreAgentSousMenu.RenseigneDonnees((T_RhmListeFonctions*)PtrEcran);
         FenetreCourante = &FenetreAgentSousMenu;
      }
      else
      {
         // Mise a jour de l'ecran
         FenetreAgentMenu.RenseigneDonnees((T_RhmListeFonctions*)PtrEcran);
         FenetreCourante = &FenetreAgentMenu;
      }

      break;
   }
   case AGT_CONSULTATION:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentConsultation.RenseigneDonnees((T_RhmConsultation*)PtrEcran);
      FenetreCourante = &FenetreAgentConsultation;
      break;
   }
   case AGT_PANNES_ETATS:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentPannesEtats.RenseigneDonnees((T_RhmPannesEtats*)PtrEcran);
      FenetreCourante = &FenetreAgentPannesEtats;
      break;
   }
   case AGT_RECHGT_MONNAIE:
   {
      // Cas des donnes : Changement de fenetre ou mise a jour ?
      // Mise a jour de l'ecran
      FenetreAgentRechargementMonnaie.RenseigneDonnees((T_RhmRechgtMonnaie*)PtrEcran);
      FenetreCourante = &FenetreAgentRechargementMonnaie;
   }
   break;
   case AGT_TEST_OUTILS_DIALOGUE:
   {
      if (DonneTypeRhm() == RHM_TYPE_1)
      {
         // RHM 1 : on veut tester le rouleau.
         // Mise a jour de l'ecran
         FenetreAgentTestRouleau.RenseigneDonnees((T_RhmTestOutilsDialogue*)PtrEcran);
         FenetreCourante = &FenetreAgentTestRouleau;
      }
      else
      {
         // RHM 2, 3 et 4 : on veut tester les touches de la borne ou l'écran tactile
         // Mise a jour de l'ecran
         FenetreAgentTestTouches.RenseigneDonnees((T_RhmTestOutilsDialogue*)PtrEcran);
         FenetreCourante = &FenetreAgentTestTouches;
      }
      break;
   }
   case AGT_ENTREES_SORTIES:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentEntreesSorties.RenseigneDonnees((T_RhmEntreesSorties*)PtrEcran);
      FenetreCourante = &FenetreAgentEntreesSorties;
      break;
   }
   case AGT_ACCES_IMPRIMANTE:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentAccesImprimante.RenseigneDonnees((T_RhmAccesImprimante*)PtrEcran);
      FenetreCourante = &FenetreAgentAccesImprimante;
      break;
   }
   case AGT_MAJCPTICS:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentMAJCptICS.RenseigneDonnees((T_RhmMajCptIcs*)PtrEcran);
      FenetreCourante = &FenetreAgentMAJCptICS;
      break;
   }
   case AGT_CPTMNY:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentCptMny.RenseigneDonnees((T_RhmMajCptMny*)PtrEcran);
      FenetreCourante = &FenetreAgentCptMny;
      break;
   }
   case AGT_MAJCPTMNY:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentMAJCptMny.RenseigneDonnees((T_RhmMajCptMny*)PtrEcran);
      FenetreCourante = &FenetreAgentMAJCptMny;
      break;
   }
   case AGT_VIDAGE:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentVidage.RenseigneDonnees((T_RhmVidage*)PtrEcran);
      FenetreCourante = &FenetreAgentVidage;
      break;
   }
   case AGT_AFF_FIC:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentFichier.RenseigneDonnees((T_RhmConsultation*)PtrEcran);
      FenetreCourante = &FenetreAgentFichier;
      break;
   }
   case AGT_MAINTENANCE_MPC:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentMaintenanceMPC.RenseigneDonnees((T_RhmListeFonctions*)PtrEcran);
      FenetreCourante = &FenetreAgentMaintenanceMPC;
      break;
   }
   case AGT_SAISIE:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentSaisie.RenseigneDonnees((T_RhmSaisie*)PtrEcran);
      FenetreCourante = &FenetreAgentSaisie;
      break;
   }
   case AGT_SELECTION_FIC:
   {
      // Cas des donnes
      // Mise a jour de l'ecran
      FenetreAgentSelFichier.RenseigneDonnees((T_RhmSelectionFichiers*)PtrEcran);
      FenetreCourante = &FenetreAgentSelFichier;
      break;
   }
   case AGT_LIGNESERVICE:
   {
      // Cas des donnes
      if (FenetreCourante != NULL)
      {
         // On garde PtrEcran pointé sur les données de la fenetre courante
         // On met a jour uniquement la ligne service avec les données pointé par Arg_P
         FenetreCourante->DonnerLigneService((T_RhmLigneService*)Arg_P);
         delete Arg_P;
      }
      break;
   }
   case AGT_PRINTF:
   {
      // Printf
      // Gestion du printf
      if (Arg_P)
      {
         FenetreAgentTrace.Afficher(((T_RhmTraceAgent*)PtrEcran)->Trace);
      }

      FenetreCourante = &FenetreAgentTrace;
      break;
   }
   case AGT_FENETRE_AGENT:
   {
      // Fenetre agent
      // Mise a jour de l'ecran
      if (Arg_P == NULL)
      {
         FenetreAgentTrace.Effacer();
      }
      else
      {
         FenetreAgentTrace.RenseigneDonnees((T_RhmFenetreAgent*)PtrEcran);
      }
      FenetreCourante = &FenetreAgentTrace;
      break;
   }
   case AGT_FENETRE_TRACE:
      break;

      //////////////////////////////////////
      // Ecrans de vente
      //////////////////////////////////////
   case VEN_ECRAN_ACCUEIL:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutAccueil()->BitmapFond.Bitmap);
      IncidentReadOnce = FAUX;
      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranAccueil*)PtrEcran);
      FenetreVenteAccueilMalvoyants.RenseigneDonnees((T_RhmEcranAccueil*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteAccueilMalvoyants;
         GestionnaireFenetreVente->FenetreVenteAccueil()->Visible(FAUX);
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteAccueil();
         GestionnaireFenetreVente->FenetreVenteAccueil()->Visible(VRAI);
      }
      GestionnaireFenetreVente->FenetreVenteAccueil()->SetConfigTooltip(ParamInit.TempoTooltip);
      break;
   }
   case VEN_ECRAN_CONSULT_CSC:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutConsultationCSC()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranConsultationCSC*)PtrEcran);
      FenetreVenteConsultationCSCMalvoyants.RenseigneDonnees((T_RhmEcranConsultationCSC*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteConsultationCSCMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteConsultationCSC();
      }
      GestionnaireFenetreVente->FenetreVenteConsultationCSC()->SetConfigTooltip(ParamInit.TempoTooltip);
      break;
   }
   case VEN_ECRAN_SELECTION:
   {
      T_RhmEcranSelection* Ecran_L = (T_RhmEcranSelection*)PtrEcran;
      PtrMenu = Ecran_L->Menu.Choix;

      ChangerFondVente(GestionnaireFenetreVente->LayoutSelection()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees(Ecran_L);
      FenetreVenteSelectionMalvoyants.RenseigneDonnees(Ecran_L);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteSelectionMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelection();
      }
      GestionnaireFenetreVente->FenetreVenteSelection()->SetConfigTooltip(ParamInit.TempoTooltip);
      break;
   }
   case VEN_ECRAN_SELECTION_QUANTITE:
   {
      T_RhmEcranSelectionQuantite* Ecran_L = (T_RhmEcranSelectionQuantite*)PtrEcran;
      PtrMenu = Ecran_L->Menu.Choix;

      ChangerFondVente(GestionnaireFenetreVente->LayoutSelectionQuantite()->BitmapFond.Bitmap);

      FenetreVenteSelectionMalvoyants.RenseigneDonnees(Ecran_L);
      GestionnaireFenetreVente->RenseigneDonnees(Ecran_L);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteSelectionMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelectionQuantite();
      }
      break;
   }
   case VEN_ECRAN_SELECTION_OD:
   {
      T_RhmEcranSelectionOD* Ecran_L = (T_RhmEcranSelectionOD*)PtrEcran;
      PtrMenu = Ecran_L->Menu.Choix;

      ChangerFondVente(GestionnaireFenetreVente->LayoutSelectionOD()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees(Ecran_L);
      FenetreVenteSelectionMalvoyants.RenseigneDonnees(Ecran_L);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteSelectionMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelectionOD();
      }
      break;
   }
   case VEN_ECRAN_SELECTION_ZONE:
   {
      //GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranVenteZonale*) PtrEcran);
      //FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelectionZone();
      break;
   }
   case VEN_ECRAN_CONFIRMATION:
   {
      T_RhmEcranConfirmation* Ecran_L = (T_RhmEcranConfirmation*)PtrEcran;
      PtrMenu = Ecran_L->Menu.Choix;

      ChangerFondVente(GestionnaireFenetreVente->LayoutConfirmation()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees(Ecran_L);
      FenetreCourante = GestionnaireFenetreVente->FenetreVenteConfirmation();
      break;
   }
   case VEN_ECRAN_SEL_MOYEN_PAIEMENT:
   {
      T_RhmEcranSelectionMoyenPaiement* Ecran_L = (T_RhmEcranSelectionMoyenPaiement*)PtrEcran;
      PtrMenu = Ecran_L->Menu.Choix;

      ChangerFondVente(GestionnaireFenetreVente->LayoutSelectionMoyenPaiement()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees(Ecran_L);
      FenetreVenteSelectionMoyenPaiementMalvoyants.RenseigneDonnees(Ecran_L);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteSelectionMoyenPaiementMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVentePaiementSelectionMoyen();
      }

      break;
   }
   case VEN_ECRAN_SEL_PAIEMENT:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutPaiementSelection()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranSelectionPaiement*)PtrEcran);
      FenetreVentePaiementMalvoyants.RenseigneDonnees((T_RhmEcranSelectionPaiement*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVentePaiementMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVentePaiementSelection();
      }
      break;
   }
   case VEN_ECRAN_PAIEMENT_CARTE:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutPaiementCarte()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranPaiementCarte*)PtrEcran);
      FenetreVentePaiementMalvoyants.RenseigneDonnees((T_RhmEcranPaiementCarte*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVentePaiementMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVentePaiementCarte();
      }
      break;
   }
   case VEN_ECRAN_PAIEMENT_ESPECE:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutPaiementEspeces()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranPaiementEspeces*)PtrEcran);
      FenetreVentePaiementMalvoyants.RenseigneDonnees((T_RhmEcranPaiementEspeces*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         // Arreter la lecture d'aide vocale si lecture en cours (pour les maj montant à payer)
         ArreterLectureAideVocale();

         FenetreCourante = &FenetreVentePaiementMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVentePaiementEspeces();
      }
      break;
   }
   case VEN_ECRAN_PAIEMENT_PME:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutPaiementPME()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranPaiementPME*)PtrEcran);
      FenetreCourante = GestionnaireFenetreVente->FenetreVentePaiementPME();
      break;
   }
   case VEN_ECRAN_FABRICATION:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutFabrication()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranFabrication*)PtrEcran);
      FenetreVenteFabricationMalvoyants.RenseigneDonnees((T_RhmEcranFabrication*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteFabricationMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteFabrication();
      }
      break;
   }
   case VEN_ECRAN_FIN_TRANSACTION:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutInformation()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranFinTransaction*)PtrEcran);
      FenetreVenteFinFabricationMalvoyants.RenseigneDonnees((T_RhmEcranFinTransaction*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteFinFabricationMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteFinFabrication();
      }
      break;
   }
   case VEN_ECRAN_INCIDENT:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutIncident()->BitmapFond.Bitmap);

      FenetreVenteIncidentMalvoyants.RenseigneDonnees((T_RhmEcranIncident*)PtrEcran);
      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranIncident*)PtrEcran);

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         FenetreCourante = &FenetreVenteIncidentMalvoyants;
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteIncident();
      }
      break;

   }
   case VEN_ECRAN_SEL_LANGUE:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutSelectionLangue()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees(static_cast<T_RhmEcranSelectionLangue*>(PtrEcran));
      // TODO: Pas d'ecran specifique FenetreVenteSelectionLangueMalvoyants, on affiche FenetreVenteSelectionLangue
      // FenetreVenteSelectionLangueMalvoyants.RenseigneDonnees ( static_cast<T_RhmEcranSelectionLangue*>(PtrEcran) );

      if (ParamInit.MalvoyantActif && EstModeMalvoyants == VRAI)
      {
         // TODO: Pas d'ecran specifique FenetreVenteSelectionLangueMalvoyants, on affiche FenetreVenteSelectionLangue
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelectionLangue();
      }
      else
      {
         FenetreCourante = GestionnaireFenetreVente->FenetreVenteSelectionLangue();
      }
      break;
   }
   case VEN_ECRAN_HORSSERVICE:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutHorsService()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranHorsService*)PtrEcran);
      FenetreCourante = GestionnaireFenetreVente->FenetreVenteHorsService();
      break;
   }
   case VEN_ECRAN_OFFLINE:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutOffline()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranOffline*)PtrEcran);
      FenetreCourante = GestionnaireFenetreVente->FenetreVenteOffline();
      break;
   }
   case VEN_ECRAN_SAISIE_NO_PV:
   {
      ChangerFondVente(GestionnaireFenetreVente->LayoutSaisieNoPv()->BitmapFond.Bitmap);

      GestionnaireFenetreVente->RenseigneDonnees((T_RhmEcranSaisieNoPV*)PtrEcran);
      FenetreCourante = GestionnaireFenetreVente->FenetreVenteSaisieNoPv();
      break;
   }
   }

   if (FenetreCourante != NULL)
   {
      FenetreCourante->Visible(VRAI);
   }

   // MoveWindow(RectAffichage.left, RectAffichage.top, Largeur, Hauteur, TRUE);
   Rafraichir(FenetreCourante);
   DebloquerRafraichissement();

   if (TypeEcran != VEN_ECRAN_ACCUEIL && FenetreCourante != NULL)
   {
      // On fait la synthèse vocale APRES l'affichage (pour les TTS synchronisés)
      if (ParamInit.AideVocaleTTS && EstModeMalvoyants == VRAI)
      {
         if (FenetreCourante == &FenetreVenteIncidentMalvoyants)
         {
            if (IncidentReadOnce == FAUX)
            {
               if (TypeEcran != AGT_LIGNESERVICE && TypeEcran != AGT_PRINTF)
               {
                  FenetreCourante->LireTTS();
                  IncidentReadOnce = VRAI;
               }
            }
            else
            {
               // Arreter la lecture d'aide vocale si lecture en cours
               ArreterLectureAideVocale();
            }
         }

         else
         {
            if (TypeEcran != AGT_LIGNESERVICE && TypeEcran != AGT_PRINTF)
            {
               // On lit sauf pour ces types d'écran qui peuvent être appelés très souvent
               FenetreCourante->LireTTS();

            }
         }
      }

      // Test pour lecture de wav
      if (ParamInit.AideVocaleWav/* && EstModeMalvoyants == VRAI*/)
      {
         if (TypeEcran != AGT_LIGNESERVICE && TypeEcran != AGT_PRINTF)
         {
            // On lit sauf pour ces types d'écran qui peuvent être appelés très souvent
            FenetreCourante->LireFichierAudio();
         }
      }
   }

   DebloquerInteractions();
}

/*****************************************************************************
* But de la fonction : Afficher la fenêtre de confirmation agent.
* --------------------
*
* Parametres :
* ------------
*
* Entree : TODO
*
* Sortie : Aucune
*
* return : aucun
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::AfficherFenetreConfirmation(unsigned int Fonction_P, T_RhmEcranAgent* Donnees_P)
{
   BloquerRafraichissement();

   FenetreMemorisee = FenetreCourante;

   FenetreAgentConfirmation.RenseigneDonnees(Donnees_P, Fonction_P);

   FenetreCourante = &FenetreAgentConfirmation;

   FenetreMemorisee->Visible(0);
   FenetreCourante->Visible(1);

   Rafraichir();

   DebloquerRafraichissement();
}

/*****************************************************************************
* But de la fonction : Afficher la fenêtre de confirmation agent.
* --------------------
*
* Parametres :
* ------------
*
* Entree : TODO
*
* Sortie : Aucune
*
* return : aucun
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::AfficherFenetreMemorisee()
{
   BloquerRafraichissement();

   FenetreCourante = FenetreMemorisee;

   FenetreMemorisee->Visible(0);
   FenetreCourante->Visible(1);
   FenetreMemorisee = NULL;

   Rafraichir();

   DebloquerRafraichissement();
}

/*****************************************************************************
* But de la fonction : Donner le contexte graphique.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Contexte graphique cherché.
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
HDC T_GestionnaireFenetre::DonnerDC()
{
   return GetDC(Wnd);
}

/*****************************************************************************
* But de la fonction : Liberer le contexte graphique.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Contexte graphique à libérer.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::RendreDC(HDC DC_P)
{
   ReleaseDC(Wnd, DC_P);
}

/*****************************************************************************
* But de la fonction : Donner le rendu
* --------------------
*
* Parametres :
* ------------
*
* Entree : aucun
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
I_Renderer* T_GestionnaireFenetre::DonnerRendu()
{
   return GestionnaireRendu.DonnerRendu();
}

/*****************************************************************************
* But de la fonction : Donner le rendu
* --------------------
*
* Parametres :
* ------------
*
* Entree : aucun
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
unsigned int T_GestionnaireFenetre::DonnerTypeRendu()
{
   return GestionnaireRendu.DonnerTypeRendu();
}

/*****************************************************************************
* But de la fonction : Poster un compte-rendu de vente.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type de compte rendu à envoyer.
*          Identifiant de compte-rendu.
*          Numéro d'écran correspondant
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::CompteRendu(T_RhmCr Code_P, unsigned int Id_P, unsigned int NoEcran_P)
{
   // On poste le compte rendu a la tache cliente
   T_RhmCompteRendu Cr_L;

   Cr_L.Cr = Code_P;
   Cr_L.Id = Id_P;
   Cr_L.NoEcran = NoEcran_P;

   // Trace à activer si besoin de voir les CR envoyés par la RHM à l'appli.
   // ATTENTION : Dégrade fortement les performances, réservé au debug embarqué.
   //             Ne pas utiliser dans une version opérationnelle.
   //Tracer("APP <= RHM : CR %u\n",Code_P);

   PosterCrCompteRendu(&TacheClient, Cr_L);
}

/*****************************************************************************
* But de la fonction : Poster un compte-rendu de sélection (écrans agent).
* --------------------
*
* Parametres :
* ------------
*
* Entree : Type de compte rendu à envoyer.
*          Données associées au compte-rendu.
*          Numéro d'écran correspondant
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::Selection(unsigned int CompteRendu_P, TCHAR Saisie_P[RHM_TAILLE_MAX_SAISIE], unsigned int NoEcran_P)
{
   // On poste le compte rendu a la tache cliente
   T_RhmCrSelection Sel_L;

   Sel_L.NoEcran = NoEcran_P;

   Sel_L.CompteRendu = CompteRendu_P;
   memcpy(Sel_L.Saisie, Saisie_P, RHM_TAILLE_MAX_SAISIE);

   // Trace à activer si besoin de voir les CR envoyés par la RHM à l'appli.
   // ATTENTION : Dégrade fortement les performances, réservé au debug embarqué.
   //             Ne pas utiliser dans une version opérationnelle.
   //Tracer("APP <= RHM : SEL %u\n",CompteRendu_P);

   PosterCrSelection(&TacheClient, Sel_L);
}

/*****************************************************************************
* But de la fonction : Bloquer le rafraichissement de la fenêtre
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::BloquerRafraichissement()
{
   RafraichissementBloque++;
}

/*****************************************************************************
* But de la fonction : Débloquer le rafraichissement de la fenêtre
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers : On redessine la fenêtre principale si le sémaphore
*                       RafraichissementBloque atteint 0.
*
*****************************************************************************/
void T_GestionnaireFenetre::DebloquerRafraichissement()
{
   if (RafraichissementBloque > 0)
   {
      RafraichissementBloque--;
   }
   else
   {
      Tracer("ERREUR : Appel à DebloquerRafraichissement alors que RafraichissementBloque = 0.\n");
   }

   if ((RafraichissementBloque == 0) && (ZoneARafraichirValide == VRAI))
   {
      // On fait appel a la methode de rafraichissement standard, limité à la zone à rafraichir.
      Rafraichir(FenetreCourante, &ZoneARafraichir);

      // Puis on remet à zéro la zone à rafraichir
      ReinitialiserZoneARafraichir();
   }
}

/*****************************************************************************
* But de la fonction : Bloquer les actions utilisateur (touches, boutons, ...etc)
*                      sur les écrans.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::BloquerInteractions(void)
{
   InteractionsBloquees = VRAI;
}

/*****************************************************************************
* But de la fonction : Debloquer les actions utilisateur (touches, boutons, ...etc)
*                      sur les écrans.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::DebloquerInteractions(void)
{
   InteractionsBloquees = FAUX;
}

/*****************************************************************************
* But de la fonction : Initialiser le timer.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::InitierChrono(T_FenetreVirtuelle* Fenetre_P, unsigned int Duree_P, unsigned int TypeChrono_P)
{
   if (TypeChrono_P >= TIMER_BASE_ANIME)
   {
      for (int i = 0; i < NB_TIMER_ANIME; i++)
      {
         if (ChronosAnimation[i] == Fenetre_P || ChronosAnimation[i] == NULL)
         {
            TypeChrono_P = TIMER_BASE_ANIME + i;
            ChronosAnimation[i] = Fenetre_P;
            break;
         }
      }
   }

   // Lancer le chrono associe avec la fenetre
   SetTimer(Wnd, TypeChrono_P, Duree_P, NULL);

   // Cas particulier du diaporama
   if (TypeChrono_P == TIMER_REVEIL)
   {
      FenetreTimer = Fenetre_P;
   }
}

/*****************************************************************************
* But de la fonction : Arreter le timer
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ArreterChrono(unsigned int TypeChrono_P, T_FenetreVirtuelle* Fenetre_P)
{
   if (TypeChrono_P >= TIMER_BASE_ANIME && Fenetre_P != NULL)
   {
      for (int i = 0; i < NB_TIMER_ANIME; i++)
      {
         if (ChronosAnimation[i] == Fenetre_P)
         {
            TypeChrono_P = TIMER_BASE_ANIME + i;
            ChronosAnimation[i] = NULL;
            break;
         }
      }
   }

   KillTimer(Wnd, TypeChrono_P);
}

void T_GestionnaireFenetre::LogMonitorsConfiguration()
{
   Tracer("Configuration materielle de l'affichage: \n");
   for (unsigned int index = 0; index < 8; ++index)
   {
      if (monitorsInfos_.IsPresent(index))
      {
         RECT rect = monitorsInfos_.Rect(index);
         Tracer("  - ecran %d : width=%d, height=%d, pos={%d,%d}\n",
            index,
            rect.right - rect.left,
            rect.bottom - rect.top,
            rect.top,
            rect.left
         );
      }
   }
   Tracer("  - ecran principale: %d\n", monitorsInfos_.MainMonitorId());
}

/*****************************************************************************
* But de la fonction : Redémarrer la tempo d'inaction avec la dernière valeur utilisée.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::RedemarrageTempoInaction()
{
   if (TempoDemarree == VRAI)
   {
      KillTimer(Wnd, TIMER_INACTION);
      SetTimer(Wnd, TIMER_INACTION, ValeurTempo, NULL);
   }
}

void T_GestionnaireFenetre::DemarrageTempoToolTip()
{
   if (TempoDemarree == VRAI)
   {
      ValeurTempoToolTip = TEMPO_TOOLTIP;
      SetTimer(Wnd, TIMER_TOOLTIP, ValeurTempoToolTip, NULL);
      _statetoolTip = true;
   }
}



/*****************************************************************************
* But de la fonction : Redémarrer la tempo d'inaction avec une valeur donnée.
* --------------------
*
* Parametres :
* ------------
*
* Entree : Tempo_P, valeur de la tempo d'inaction.
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::RedemarrageTempoInaction(unsigned int Tempo_P)
{
   KillTimer(Wnd, TIMER_INACTION);
   SetTimer(Wnd, TIMER_INACTION, Tempo_P, NULL);

   TempoDemarree = VRAI;

   ValeurTempo = Tempo_P;
}

/*****************************************************************************
* But de la fonction : Temporiser les clics sur l'écran pour bloquer les interactions
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::TemporiserClic(unsigned int Temps_P)
{
   /*  Si temporisation correcte */
   if (Temps_P > 0)
   {
      /* Armer le timer */
      SetTimer(Wnd, TIMER_AFFICHAGE, Temps_P, NULL);
   }
   else
   {
      /* Débloquer les interactions */
      DebloquerInteractions();
   }
}
/*****************************************************************************
* But de la fonction : Arreter la tempo d'inaction
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ArretTempoInaction()
{
   KillTimer(Wnd, TIMER_INACTION);
   TempoDemarree = FAUX;
}

/*****************************************************************************
* But de la fonction :
* --------------------
*
* Parametres :
* ------------
*
* Entree : Zone à rafraichir
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::AjouterZoneARafraichir(long Gauche_P, long Haut_P, long Droite_P, long Bas_P)
{
   if (ZoneARafraichirValide == VRAI)
   {
      ZoneARafraichir.left = min(ZoneARafraichir.left, Gauche_P);
      ZoneARafraichir.top = min(ZoneARafraichir.top, Haut_P);
      ZoneARafraichir.right = max(ZoneARafraichir.right, Droite_P);
      ZoneARafraichir.bottom = max(ZoneARafraichir.bottom, Bas_P);
   }
   else
   {
      ZoneARafraichirValide = VRAI;
      ZoneARafraichir.left = Gauche_P;
      ZoneARafraichir.top = Haut_P;
      ZoneARafraichir.right = Droite_P;
      ZoneARafraichir.bottom = Bas_P;
   }
}

/*****************************************************************************
* But de la fonction :
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::ReinitialiserZoneARafraichir(void)
{
   ZoneARafraichirValide = FAUX;
}

void T_GestionnaireFenetre::InitialiserRouleau(void)
{
}

/*****************************************************************************
* But de la fonction :
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::InitialiserImages()
{
   I_Renderer * Rendu_L = GestionnaireRendu.DonnerRendu();

   char CheminImage_L[32];
   char CheminOrigine_L[32];
   char CheminDestination_L[32];

   // Raz des structures
   memset(CheminImage_L, 0, sizeof CheminImage_L);
   memset(CheminOrigine_L, 0, sizeof CheminOrigine_L);
   memset(CheminDestination_L, 0, sizeof CheminDestination_L);

   if (DonneTypeRhm() == RHM_TYPE_3)
   {
      // Fond vente conducteur spécifique pour RHM3
      sprintf((char*)CheminImage_L, "..\\bmp\\FondVente3C.jpg");

      if (RechercherSiFichierExiste(CheminImage_L) == VRAI)
      {
         BitmapFondVenteConducteur.LibererRessources();
         BitmapFondVenteConducteur.LoadFile(CheminImage_L, &TraceLayout);
      }
      else
      {
         if (strcmp(GestionnaireFenetreVente->LayoutSelection()->BitmapFond.Bitmap, "") != 0)
         {
            // Vente
            BitmapFondVenteConducteur.LibererRessources();
            BitmapFondVenteConducteur.LoadFile(GestionnaireFenetreVente->LayoutSelection()->BitmapFond.Bitmap, &TraceLayout);
         }
      }
   }
   else
   {
      // Si non RHM3 on prend le fond de vente sélection
      if (strcmp(GestionnaireFenetreVente->LayoutSelection()->BitmapFond.Bitmap, "") != 0)
      {
         // Vente
         BitmapFondVenteConducteur.LibererRessources();
         BitmapFondVenteConducteur.LoadFile(GestionnaireFenetreVente->LayoutSelection()->BitmapFond.Bitmap, &TraceLayout);
      }
   }

   if (FondVente != NULL)
   {
      Rendu_L->LibererFond(FondVente);
   }
   FondVente = Rendu_L->PreparerFond(BitmapFondVente.DonneHBitmap());

   if (FondVenteMalvoyant != NULL)
   {
      Rendu_L->LibererFond(FondVenteMalvoyant);
   }
   FondVenteMalvoyant = Rendu_L->PreparerFond(BitmapFondVenteMalvoyants.DonneHBitmap());

   if (FondVenteConducteur != NULL)
   {
      Rendu_L->LibererFond(FondVenteConducteur);
   }
   if (BitmapFondVenteConducteur.DonneHBitmap() != NULL)
   {

      FondVenteConducteur = Rendu_L->PreparerFond(BitmapFondVenteConducteur.DonneHBitmap());
   }
   else
   {
      FondVenteConducteur = NULL;
   }

   // Copie des Zones
   DeplacerFichier("..\\par\\ZoneNonSelectionnees.bmp", "..\\bmp\\ZoneNonSelectionnees.bmp");
   DeplacerFichier("..\\par\\ZoneSelectionnable.bmp", "..\\bmp\\ZoneSelectionnable.bmp");
   DeplacerFichier("..\\par\\ZoneSelectionnee.bmp", "..\\bmp\\ZoneSelectionnee.bmp");
   DeplacerFichier("..\\par\\ZonesMasque.bmp", "..\\bmp\\ZonesMasque.bmp");

   ////////////////////
   // Selection de Zones

   /// Chargement de la bitmap des zones selectionnees
   DeleteResources(&SelectionZoneSelectionnees);
   SelectionZoneSelectionnees = new T_Bitmap();
   if (RechercherSiFichierExiste("..\\bmp\\ZoneSelectionnee.bmp") == VRAI)
   {
      SelectionZoneSelectionnees->LoadFile("..\\bmp\\ZoneSelectionnee.bmp");
   }

   /// Chargement de la bitmap des zones selectionnables
   DeleteResources(&SelectionZoneSelectionnables);
   SelectionZoneSelectionnables = new T_Bitmap();
   if (RechercherSiFichierExiste("..\\bmp\\ZoneSelectionnable.bmp") == VRAI)
   {
      SelectionZoneSelectionnables->LoadFile("..\\bmp\\ZoneSelectionnable.bmp");
   }

   /// Chargement de la bitmap des zones non selectionnees
   DeleteResources(&SelectionZoneNonSelectionnees);
   SelectionZoneNonSelectionnees = new T_Bitmap();
   if (RechercherSiFichierExiste("..\\bmp\\ZoneNonSelectionnees.bmp") == VRAI)
   {
      SelectionZoneNonSelectionnees->LoadFile("..\\bmp\\ZoneNonSelectionnees.bmp");
   }

   /// Chargement de la bitmap de masque des zones
   DeleteResources(&SelectionZoneMasque);
   SelectionZoneMasque = new T_Bitmap();
   if (RechercherSiFichierExiste("..\\bmp\\ZonesMasque.bmp") == VRAI)
   {
      SelectionZoneMasque->LoadFile("..\\bmp\\ZonesMasque.bmp");
   }

   Tracer("Initialisation des fenêtres dynamiques\n");
}

/*****************************************************************************
* But de la fonction :
* --------------------
*
* Parametres :
* ------------
*
* Entree : Aucune
*
* Sortie : Aucune
*
* return : Aucune
*
* Fonctions externes utilisees :
*
* Points particuliers :
*
*****************************************************************************/
void T_GestionnaireFenetre::AutoriserSequenceSecrete(unsigned int TypeEcran_P)
{
   SequenceSecreteAutorisee = FAUX;
   if (DonneTypeRhm() == RHM_TYPE_1)
   {
      if (TypeEcran_P == VEN_ECRAN_ACCUEIL || TypeEcran_P == VEN_ECRAN_SEL_LANGUE || TypeEcran_P == VEN_ECRAN_HORSSERVICE || TypeEcran_P == VEN_ECRAN_OFFLINE)
      {
         SequenceSecreteAutorisee = VRAI;
         KillTimer(Wnd, TIMER_RAZ_SEQUENCE_SECRETE);
         SetTimer(Wnd, TIMER_RAZ_SEQUENCE_SECRETE, 5000, NULL); // 5 sec pour saisir sequence
      }
      else
      {
         InitialiserSequenceSecrete();
         KillTimer(Wnd, TIMER_RAZ_SEQUENCE_SECRETE);
      }
   }
}

void T_GestionnaireFenetre::InitialiserSequenceSecrete()
{
   IndiceSecretSequence = 0;
   for (int i = 0; i < MAX_TOUCHE_SEQUENCE; i++)
   {
      SequenceSecreteSaisie[i] = NULL;
   }
}

unsigned int T_GestionnaireFenetre::EstCeSequenceSecreteSaisie(void)
{
   unsigned int Retour_L = FAUX;
   if (SequenceSecreteAutorisee)
   {
      if (memcmp(SequenceSecreteSaisie, SEQUENCE_SECRETE, sizeof SEQUENCE_SECRETE) == 0)
      {
         Retour_L = VRAI;
         // RAZ pour nouvelle sequence
         InitialiserSequenceSecrete();
      }
   }
   return Retour_L;
}

void T_GestionnaireFenetre::AjouterToucheDansSequence(unsigned int Touche_P)
{
   bool TouchePriseEnCompte = VRAI;

   if (SequenceSecreteAutorisee)
   {
      if (IndiceSecretSequence < MAX_TOUCHE_SEQUENCE)
      {
         // Si rouleau, on accepte plusieurs déplacement !!! (le rouleau n'est pas du tout précis)
         if (DonneTypeRhm() == RHM_TYPE_1
            && ((IndiceSecretSequence > 0 && SequenceSecreteSaisie[IndiceSecretSequence - 1] == WM_DEPLACEMENT_ROULEAU_BAS && Touche_P == WM_DEPLACEMENT_ROULEAU_BAS)
               || (IndiceSecretSequence > 0 && SequenceSecreteSaisie[IndiceSecretSequence - 1] == WM_DEPLACEMENT_ROULEAU_HAUT || Touche_P == WM_DEPLACEMENT_ROULEAU_HAUT)
               )
            )
         {
            TouchePriseEnCompte = FAUX;
         }

         if (TouchePriseEnCompte)
         {
            SequenceSecreteSaisie[IndiceSecretSequence++] = (unsigned int)Touche_P;
         }
      }
      else
      {
         InitialiserSequenceSecrete();
      }
   }
}

void T_GestionnaireFenetre::Retaillage()
{
   switch (m_TypeRhm)
   {
   case RHM_TYPE_1:       /**< RHM avec écran format paysage 640x480 pixels, rouleau de sélection, boutons annuler et valider */
   case RHM_TYPE_2:       /**< RHM avec écran format paysage 640x480 pixels, 8 boutons de sélection et boutons annuler */
      T_FenetreBase::Retaillage(640, 480);
      Tracer("Retaillage écran : 640x480\n");
      break;
   case RHM_TYPE_3:       /**< RHM avec écran tactile format portrait 768x1024 pixels */
      T_FenetreBase::Retaillage(768, 1024);
      Tracer("Retaillage écran : 768x1024\n");
      break;
   case RHM_TYPE_4:       /**< RHM avec écran tactile format portrait 1024x768 pixels */
      T_FenetreBase::Retaillage(1024, 768);
      Tracer("Retaillage écran : 1024x768\n");
      break;
   case RHM_TYPE_5:       /**< RHM avec écran tactile format portrait 1920x1080 pixels */
      T_FenetreBase::Retaillage(1920, 1080);
      Tracer("Retaillage écran : 1920x1080\n");
      break;
   }
}

void T_GestionnaireFenetre::ShowInterventionWindows(const bool showWindow)
{
   delete interventionInProgressWnd;
   interventionInProgressWnd = NULL;

   if (showWindow)
   {
      interventionInProgressWnd = new InterventionInProgressWindow(Wnd,
         optionalDisplayRect_.left,
         optionalDisplayRect_.top,
         optionalDisplayRect_.right - optionalDisplayRect_.left,
         optionalDisplayRect_.bottom - optionalDisplayRect_.top);
   }
}

void T_GestionnaireFenetre::AfficherSortieEcran(const bool saleDisplay, unsigned int outputId, unsigned int optionalOutputID)
{
   RECT newDisplayRect;
   memset(&newDisplayRect, 0, sizeof newDisplayRect);
   // Identify the display to use, and its coordinates.
   newDisplayRect = monitorsInfos_.Rect(1);
   if (monitorsInfos_.IsPresent(outputId))
   {
      newDisplayRect = monitorsInfos_.Rect(outputId);
   }
#ifndef DEBUG
   else
   {
      _HALT(outputId, "Numéro d'écran  principal non supporté");
   }
#endif

   optionalDisplayRect_ = newDisplayRect;
   if (optionalOutputID != 0)
   {
      if (monitorsInfos_.IsPresent(optionalOutputID))
      {
         optionalDisplayRect_ = monitorsInfos_.Rect(optionalOutputID);
      }
#ifndef _DEBUG
      else
      {
         _HALT(optionalOutputID, "Numéro d'écran secondaire non supporté");
      }
#else
      else
      {
         // if there is only one display, display both screen on the same display, but shit a little bit to avoir display overlap... 
         optionalDisplayRect_.left += 800;
      }
#endif   
   }
   else
   {
   }

   if (saleDisplay)
   {
      // -------------------------------------------
      // Display the sale screen
      // -------------------------------------------
      WaitForDoorOpen = false;

      // Release agent menu resources
      Liberer(FAUX);

      // Display sale screen
      m_TypeRhm = m_TypeRhmVente;

      Initialiser(VRAI);

      Initialiser(FAUX);
      I_Renderer * renderer = GestionnaireRendu.DonnerRendu();
      FondAgent = renderer->PreparerFond(BitmapFondAgent.DonneHBitmap());
      FondAgentFormatVente = renderer->PreparerFond(BitmapFondAgentFormatVente.DonneHBitmap());

      InitDialog();
      InitialiserImages();
   }
   else
   {
      // -------------------------------------------
      // Display the agent menu.
      // -------------------------------------------

      // Manage the intervention information window
      // If configured, the "Intervention in progress" screen is displayed on optional display
      if (optionalOutputID != 0)
      {
         ShowInterventionWindows(true);
      }

      // Display maintenance screen
      m_TypeRhm = m_TypeRhmMaintenance;

      if (firstInit || refreshOnRhmTypeChange)
      {
         refreshOnRhmTypeChange = false;

         // Release agent menu resources
         Liberer(FAUX);
         Initialiser(FAUX);

         I_Renderer * renderer = GestionnaireRendu.DonnerRendu();
         FondAgent = renderer->PreparerFond(BitmapFondAgent.DonneHBitmap());
         FondAgentFormatVente = renderer->PreparerFond(BitmapFondAgentFormatVente.DonneHBitmap());
         if (!firstInit)
         {
            InitDialog();
         }
         InitialiserImages();

         firstInit = false;
      }
   }

   GestionnaireRendu.DonnerRendu()->Effacer(&RectAffichage);
   RectAffichage = newDisplayRect;

   // The display is moved to the correct screen.
   MoveWindow(RectAffichage.left, RectAffichage.top, Largeur, Hauteur, TRUE);

   if (optionalOutputID == 0)
   {
      ShowInterventionWindows(false);
   }

   SetFocus(HandleFenetre());
}

void T_GestionnaireFenetre::SetDoorHandle()
{
   WaitForDoorOpen = false;
}

void T_GestionnaireFenetre::ActivateValidationOnPresentation()
{
   RedemarrageTempoInaction();
   FenetreVenteAccueilMalvoyants.Valider();
}