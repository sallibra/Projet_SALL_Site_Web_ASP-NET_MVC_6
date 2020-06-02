
#include <string.h>
#include <stdio.h>
#include <util.h>

#include <es.h>
#include <salr.h>
#include <ctd.h>
#include <apitlb.h>
#include <csc.h>
#include <rhm.h>
#include <parics.h>
#include <ics.h>
#include <ap5000.h>
#include <TSsec.h>
#include <mrt.h>
#include <parmny.h>
#include <mny.h>
#include <chs.h>
#include <nri.h>
#include <pp.h>
#include <I_BankCardPayment.h>
#include <bci.h>
#include <config.h>
#include <par.h>
#include <mib.h>
#include <bdd.h>
#include <act.h>
#include <ir.h>
#include <tvi.h>
#include <dc.h>
#include <liste.h>
#include <abb.h>
#include <pb.h>
#include <bv.h>
#include <bnr.h>
#include <socket.h>
#include <rendu.h>
#include <I_OpticalReader.h>
#include <etat.h>
#include <ConfigDat.h>
#include <sstream>

#ifdef _AVANT_ESFP_SUPERVISION
#pragma message ("!!! AVANT ESFP SUPERVISION")
#else
#pragma message ("!!! APRES ESFP SUPERVISION")
#endif

/***************************************************************************
*
* Codes sous module du module ICS
*
****************************************************************************/
#define ALA_IMPRIMANTE_IC              0
#define NB_ALA_IMPRIMANTE_IC         100
#define ALA_IC_BOBINEAU              300 
#define NB_ALA_IC_BOBINEAU            10
#define ALA_TOUS_LES_BOBINEAUX_VIDES 333

/***************************************************************************
*
* DC : Dépileur codeur
*
****************************************************************************/
#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_DC_OoO 1  // DC001 - Card Issuer: Out-of-Order
#define ALA_DC_AE  22 // DC022 - Card Issuer: Almost emppty
#define ALA_DC_E   23 // DC023 - Card Issuer: Empty
#define ALA_DC_J   24 // DC024 - Card Issuer: Jam
#define ALA_DC_FtU 26 // DC026 - Card Issuer: Failed to unstack
#define ALA_DC_FtB 27 // DC027 - Card Issuer: Failed to bin
#define ALA_DC_FtI 28 // DC028 - Card Issuer: Failed to issue
#define ALA_DC_UA  70 // DC070 - Card Issuer: Unauthorized access
#else
#define ALA_DC_OoO 1   // DC001 - Card Issuer: Out-of-Order
#define ALA_DC_CE  2   // DC002 - Card Issuer: Card error
#define ALA_DC_J   3   // DC003 - Card Issuer: Jam
#define ALA_DC_FtU 4   // DC004 - Card Issuer: Failed to unstack
#define ALA_DC_FtI 5   // DC005 - Card Issuer: Failed to issue
#define ALA_DC_FtB 6   // DC006 - Card Issuer: Failed to bin
#define ALA_DC_UA  7   // DC007 - Card Issuer: Unauthorized access
#define ALA_DC_AE  101 // DC101 - Card Issuer: Almost emppty
#define ALA_DC_E   102 // DC102 - Card Issuer: Empty
#endif

/***************************************************************************
*
* Codes sous module du module CHS (MOS20)
*
****************************************************************************/
#define ALA_DISPENSATEUR        0
#define NB_ALA_DISPENSATEUR   100
#define ALA_MODULE_TRI        400
#define NB_ALA_MODULE_TRI     100
#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_RESERVE           240
#elif
#define ALA_RESERVE           200
#endif
#define NB_ALA_RESERVE         20  //avec CHS : 4 rsvs max., 20 anomalies max.  par rsv => code en 201->216, 221->236, 241->256, 261->276
#define ALA_RECYCLEUR         300
#define NB_ALA_RECYCLEUR       20  //avec CHS : 4 rcys max., 20 anomalies max.  par rcy => code en 301->316, 321->336, 341->356, 361->376
#define ALA_CAISSE            500
#define NB_ALA_CAISSE          45
#define ALA_DEFAUT_RENDU_MONNAIE 551

#ifdef DG_TODO_MISSING
#define NB_ALA_RENDU_MONNAIE     10
#define ALA_CONNEXION_CHS        600
#define NB_ALA_CONNEXION_CHS     100
#endif

//***************************************************************************
//
// Codes sous module du module REC (imprimante reçu)
//
//***************************************************************************
#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_REC_ROULEAU_PRESQUE_VIDE 1
#define ALA_REC_PORTE_OUVERTE        2 
#define ALA_REC_ROULEAU_VIDE         3
#define ALA_REC_BOURRAGE             4
#define ALA_REC_PB_DEFILEMENT        5
#define ALA_REC_PB_TEMPERATURE       6
#define ALA_REC_PB_TENSION           7
#endif

/***************************************************************************
*
* Codes sous module du module MRT
*
****************************************************************************/
#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_MRT                0
#define NB_ALA_MRT            30
#endif

/****************************************************************************
*
* Codes sous module du module MNY (RS22)
*
****************************************************************************/
#define ALA_RS22_DISPENSATEUR                  0
#define ALA_RS22_NB_DISPENSATEUR               100
#define ALA_RS22_MODULE_TRI                    100
#define ALA_RS22_NB_MODULE_TRI                 100
#define ALA_RS22_BOL                           200
#define ALA_RS22_NB_BOL                        50
#define ALA_RS22_TAMBOUR                       300
#define ALA_RS22_NB_TAMBOUR                    20
#define ALA_RS22_CAISSE                        500
#define ALA_RS22_NB_CAISSE                     50

#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_CONNEXION_CHS        600
#define NB_ALA_CONNEXION_CHS     100
#endif


/****************************************************************************
*
* COD de connexion
*
****************************************************************************/
#ifdef _AVANT_ESFP_SUPERVISION
#define ALA_CONNECTE   0
#define ALA_DECONNECTE 1
#endif

/****************************************************************************
*
* Listes des codes pannes specifiques a chacun des types d'accepteur de billets
*
****************************************************************************/
#define ALA_NB_CODES_PANNES_ABB  90

// Liste des clés SAM

#define SAM_TYPE_CLE_CSC  2
#define SAM_TYPE_CLE_TSC  5

/*****************************************************************************
*
*  Définition du numéro de réserve utilisée sur Toulouse
*
******************************************************************************/
#define ETAT_NO_RESERVE_UTILISE 2

/****************************************************************************
*
* Trace->Tracer la présence des alarmes
*
****************************************************************************/
static T_ClassTrace Trace_S("ala.trc", 512 * 1024, 30, ".\\trc");

/*****************************************************************************
* but de la fonction : Donner les parametres d'une alarme
* --------------------
* parametres :
* ------------
*
* entree : module
*          code alarme
*          pointeur sur les parametres de l'alarme
*
* sortie : les parametres de l'alarme
*
* return : VRAI, FAUX
*
* points particuliers :
*
*****************************************************************************/
T_ParAla *T_Etat::DonnerParAla(char *CodeComposant_P, unsigned int CodeAlarme_P)

{
   static T_ParAla Return_L;

   char *CodeComposant_L = CodeComposant_P;

   // BDD_ALA_MNY ?
   if (CodeComposant_P[0] == 'X')
   {
      if (ConfigDat->LirePP().Type == PP_CHS)
      {
         CodeComposant_L = "XA";
      }
      else // if (ConfigDat->LirePP ().Type == PP_MNY)
      {
         CodeComposant_L = "XB";
      }
   }

   // BDD_ALA_ABB ?
   else if (CodeComposant_P[0] == 'Y')
   {
      if (ConfigDat->LirePB().Type == PB_BNA57_MEI)
      {
         CodeComposant_L = "YA";
      }
      else if (ConfigDat->LirePB().Type == PB_BNR)
      {
         CodeComposant_L = "YB";
      }
      else //if (ConfigDat->LirePB ().Type == PB_BV)
      {
         CodeComposant_L = "YC";
      }
   }

   // BDD_ALA_ICS ?
   else if (CodeComposant_P[0] == 'Z')
   {
      CodeComposant_L = "ZA";
   }
   return Par->DonnerParAla(CodeComposant_L, CodeAlarme_P);

}

/*****************************************************************************
* But de la fonction : Enregistrer une Alarmes
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::EnregistrerAlarme(T_ParAla *ParAla_P)

{
   unsigned int Annee_L;
   unsigned int AlarmeDetaillee_L = FAUX;
   T_Date Date_L;
   T_BddESE ESE_L;
   T_ClassListe *ListeAlarmes_L = NULL;
   T_ParAla *ParAla_L;                     // Paramètre d'une alarme        
   T_BddEtatEquipement Etat_L;
   char ModeExploitation_L;
   T_RhmLibelle LibelleDebutFin;

   /* Acquerir la rubrique etat du sous ensemble en alarme */
   Bdd->LireESE(ESE_L);

   // La disparition des évenements n'est pas enregistrée
#ifdef _AVANT_ESFP_SUPERVISION
   if (!(ESE_L.Disparition && ParAla_P->NivTraitement <= BDD_ALA_NIV_EVENEMENT))
#endif // _AVANT_ESFP_SUPERVISION
   {
      /* Allouer memoire */
      char *Str_L = (char *)AllouerMemoire(1024);

      /* Acquerir la date et l'heure courante */
      DonnerDateHeure(&Date_L);
      DonnerMillesime(&Date_L, &Annee_L);

      /* Evaluation de l'etat du distributeur en fonction des alarmes présentes */
      Bdd->DonnerEtatEquipement(Etat_L, ModeExploitation_L);

#ifdef _AVANT_ESFP_SUPERVISION
      {
         /* PP : Paiement par pièces de monnaie */
         if (strcmp(ParAla_P->CodeComposant, BDD_ALA_PP) == 0)
         {
            if (ConfigDat->LirePP().Type == PP_CHS)
            {
               ListeAlarmes_L = DernieresAlarmesPPDetaillees;
            }
         }

         /* Monnayeur RS22 */
         else if (stricmp(ParAla_P->CodeComposant, BDD_ALA_RS22) == 0)
         {
            AlarmeDetaillee_L = VRAI;
            ListeAlarmes_L = DernieresAlarmesPPDetaillees;
            ParAla_L = DonnerParAla(BDD_ALA_PP, ParAla_P->COD);
         }

         /* Accepteur de billet de banque BNA57, BNR ou BV */
         else if (stricmp(ParAla_P->CodeComposant, BDD_ALA_ABB) == 0 ||
            stricmp(ParAla_P->CodeComposant, BDD_ALA_BV) == 0 ||
            stricmp(ParAla_P->CodeComposant, BDD_ALA_BNR) == 0)
         {
            AlarmeDetaillee_L = VRAI;
            ListeAlarmes_L = DernieresAlarmesPBDetaillees;
            ParAla_L = DonnerParAla(BDD_ALA_PB, ParAla_P->COD);
         }

         /* IC : Imprimante codeuse */
         else if (strcmp(ParAla_P->CodeComposant, BDD_ALA_IC) == 0)
         {
            ListeAlarmes_L = DernieresAlarmesICDetaillees;
         }
         else
         {
            ListeAlarmes_L = NULL;
         }
      }
#else //  _AVANT_ESFP_SUPERVISION
      {
         // BDD_ALA_MNY ?
         if (ParAla_P->CodeComposant[0] == 'X')
         {
            AlarmeDetaillee_L = VRAI;
            ListeAlarmes_L = DernieresAlarmesPPDetaillees;
            ParAla_L = DonnerParAla(BDD_ALA_PP, ParAla_P->COD);
         }

         // BDD_ALA_ABB ?
         else if (ParAla_P->CodeComposant[0] == 'Y')
         {
            AlarmeDetaillee_L = VRAI;
            ListeAlarmes_L = DernieresAlarmesPBDetaillees;
            ParAla_L = DonnerParAla(BDD_ALA_PB, ParAla_P->COD);
         }

         // BCS400 ?
         else if (stricmp(ParAla_P->CodeComposant, BDD_ALA_ICS) == 0)
         {
            AlarmeDetaillee_L = VRAI;
            ListeAlarmes_L = DernieresAlarmesICDetaillees;
            ParAla_L = DonnerParAla(BDD_ALA_IC, ParAla_P->COD);
         }
         else
         {
            ListeAlarmes_L = NULL;
         }
      }
#endif //  _AVANT_ESFP_SUPERVISION

      // Les alarmes detaillees ne remontent pas au système.
      if (!AlarmeDetaillee_L)
      {
         if (ParAla_P->EmettreVersCtreAlarmes && ConfigDat->LireDAT().SALRConnected)
         {
            /* Allouer memoire */
            T_SalrTrap *Trap_L = (T_SalrTrap *)AllouerMemoire(sizeof(T_SalrTrap));

            /* Acquerir la rubrique etat du distributeur */
            T_BddETA ETA_L = Bdd->LireETA();

            /* Preparer la structure du trap */
            strcpy(Trap_L->CEB, ETA_L.CEB);
            Trap_L->ETAT = Etat_L;
            Trap_L->Indic = ESE_L.Disparition;
            Trap_L->NOC = ESE_L.NoOrdre;
            Trap_L->EXP = ModeExploitation_L;
            Trap_L->MOD = ETA_L.ModeVente;
            Trap_L->TIN = ESE_L.TIN;
            memcpy(Trap_L->SSE, ESE_L.SSE, sizeof Trap_L->SSE);
            Trap_L->COD = ESE_L.COD;
            Trap_L->CLP = ESE_L.Gravite;
            Trap_L->CLM = ETA_L.CLM;
            sprintf(Trap_L->Libelle, "%-20.20s", ParAla_P->LibelleCourt);

            /* Emettre le trap */
            Salr->EmettreTrap(*Trap_L);

            /* Liberer memoire */
            LibererMemoire(Trap_L);
         }

         if (ParAla_P->EnregistrerDansFichierAct == VRAI)
         {
            /* Enregistrer l'alarme dans le fichier activite */
//#ifdef _AVANT_ESFP_SUPERVISION
            Act->EnregistrerEA();
            //#else
            //            Act->Enregistrerea ();
            //#endif
         }

         // Pour envoye act fil de l'eau
         NbEvtsEnregistres++;
      }

      T_ParLangue  Langue_L = Par->DonnerLangueAgentParDefaut();

#ifdef _AVANT_ESFP_SUPERVISION
      if (ESE_L.NivTraitement > BDD_ALA_NIV_EVENEMENT)
      {
         Par->DonnerLibelleAgent(LibelleDebutFin, (ESE_L.Disparition == VRAI) ? "L1516" : "L1515", Langue_L);
      }
      else
      {
         strcpy(LibelleDebutFin, "     ");
      }
#else // _AVANT_ESFP_SUPERVISION
      Par->DonnerLibelleAgent(LibelleDebutFin, (ESE_L.Disparition == VRAI) ? "L1516" : "L1515", Langue_L);
#endif // _AVANT_ESFP_SUPERVISION

      if (!AlarmeDetaillee_L)
      {
         /* "DEB JJ/MM/AAAA HH:MM:SS PPOO1 3 Libelle" */
         sprintf(Str_L,
            "%s %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u %2.2s%3.3u %1.1u %s",
            LibelleDebutFin,
            Date_L.Jour, Date_L.Mois, Annee_L,
            Date_L.Heure, Date_L.Minute, Date_L.Seconde,
            ESE_L.SSE,
            ESE_L.COD,
            ESE_L.NivTraitement,
            Langue_L == PAR_LANGUE_FR ? ParAla_P->FR : ParAla_P->EN);

         /* Ajouter l'alarme dans la liste des dernieres alarmes */
         DernieresAlarmes->Ajouter(Str_L);
      }
      else
      {
         /* "DEB JJ/MM/AAAA HH:MM:SS PPOO1 3 Libelle" */
         sprintf(Str_L,
            "%s %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u %2.2s%3.3u %1.1u %s",
            LibelleDebutFin,
            Date_L.Jour, Date_L.Mois, Annee_L,
            Date_L.Heure, Date_L.Minute, Date_L.Seconde,
            ParAla_L->CodeComposant,
            ParAla_L->CodeAlarme,
            ParAla_L->NivTraitement,
            Langue_L == PAR_LANGUE_FR ? ParAla_P->FR : ParAla_P->EN);
      }

      if (ListeAlarmes_L != NULL)
      {
         ListeAlarmes_L->Ajouter(Str_L);
      }

      if (!AlarmeDetaillee_L)
      {
#ifdef _AVANT_ESFP_SUPERVISION
         if (ESE_L.NivTraitement > BDD_ALA_NIV_EVENEMENT)
         {
            strcpy(LibelleDebutFin, (ESE_L.Disparition == VRAI) ? "---" : "+++");
         }
         else
         {
            strcpy(LibelleDebutFin, "   ");
         }
#else // _AVANT_ESFP_SUPERVISION
         strcpy(LibelleDebutFin, (ESE_L.Disparition == VRAI) ? "---" : "+++");
#endif // _AVANT_ESFP_SUPERVISION

         /* Trace->Tracer l'alarme */
         sprintf(Str_L,
            "%3.3s;%2.2s%3.3u;%1.1u;%s\n",
            LibelleDebutFin,
            ESE_L.SSE,
            ESE_L.COD,
            ESE_L.NivTraitement,
            Langue_L == PAR_LANGUE_FR ? ParAla_P->FR : ParAla_P->EN);

         Trace_S.Tracer(Str_L);

         /* Tracer l'alarme si l'usager est actif */
         if (UtilisationEnCours)
         {
            Usager.Tracer("*** ALARME : %s", Str_L);
         }
      }

      /* Liberer memoire */
      LibererMemoire(Str_L);
   }

}

/*****************************************************************************
* But de la fonction : Traiter signalisation des Alarmes
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterSignalisationAlarmes(void)

{
   unsigned int i;
   unsigned int j;
   unsigned int k = 0;
   unsigned int l;
   unsigned int m;
   char *PChar_L;
   unsigned int Millesime_L;
   char ActionnerSirene_L = FAUX;               // Actionner la sirène effraction ?
   char ActionnerTor1_L = FAUX;                 // Actionner la TOR 1 ?
   char ActionnerTor2_L = FAUX;                 // Actionner la TOR 2 ?
   char ActionnerTor3_L = FAUX;                 // Actionner la TOR 3 ?
   char ActionnerTor4_L = FAUX;                 // Actionner la TOR 4 ?
   char ActionnerTor5_L = FAUX;                 // Actionner la TOR 5 ?
   unsigned int ListeTraitement_L[BDD_NB_LAL];  // Liste des niveaux de traitements des alarmes
   unsigned int TraitementMaxA_L = 0;           // Traitement Max Alarmes automatiques
   unsigned int TraitementMaxM_L = 0;           // Traitement Max Alarmes maintenances
   unsigned int TraitementMaxE_L = 0;           // Traitement Max Alarmes exploitations
   unsigned int TraitementMaxR_L = 0;           // Traitement Max Alarmes recettes
   T_BddLAL LAL_L;                              // Liste des 10 dernières alarmes (TRAP) = tableau de 10 chaines de 59 caracteres
   T_BddETA ETA_L;                              // Etat général des composants
   T_BddALC *ALC_L;                             // Liste des alarmes en cours
   T_ParAla *ParAla_L;                     // Paramètre d'une alarme        
   T_RhmLigneService LigneService_L;            // Ligne de service de l'entête de l'écran
   T_BddEtatTechnique EtatTechnique_L = BDD_FONCTIONNEL; // Etat technique par défaut
   T_ParCategorieAla GTC_L;

   /* Initialiser Mise Hors Service */
   EquipementHorsService = false;

   /* Gravité et niveau de traitement max. */
   unsigned int GraviteMax_L = 0;
   unsigned int NiveauMax_L = BDD_ALA_NIV_EVENEMENT;

   /* Initialiser la liste des Alarmes en cours */
   memset(LAL_L, 0, sizeof LAL_L);

   /* Acquerir la rubrique des Alarmes en cours */
   Bdd->LireALC(&ALC_L);

   /* raz ecran des alarmes en cours */
   memset(ALC_L->Ecran, '\0', sizeof ALC_L->Ecran);

   memset(&GTC_L, 0, sizeof GTC_L);

   /* pointer sur ecran des alarmes en cours */
   PChar_L = ALC_L->Ecran;

   /* Initialiser le nombre de sous-ensembles d'alarmes */
   ALC_L->NSA = 0;

   /* Initialiser les elements de la ligne de service */
   memset(Automatique, 0, sizeof Automatique);
   memset(Maintenance, 0, sizeof Maintenance);
   memset(Exploitation, 0, sizeof Exploitation);
   memset(Recette, 0, sizeof Recette);

   /* Parcourir la table des modules */
   for (i = 0; i < BDD_NB_MODULES; i++)
   {
      /* Si au moins une alarme est presente ET que ce n'est pas un module qui donne les alarmes detaillees
         (les alarmes detaillees sont listees dans les "Dernieres alarmes" des modules correspondants mais
         pas dans les alarmes en cours)*/
#ifdef _AVANT_ESFP_SUPERVISION
      if (ALC_L->Module[i].NAP != 0 &&
         stricmp(ALC_L->Module[i].SSE, BDD_ALA_RS22) != 0 &&
         stricmp(ALC_L->Module[i].SSE, BDD_ALA_ABB) != 0)
#else
      if (ALC_L->Module[i].NAP != 0 &&
         stricmp(ALC_L->Module[i].SSE, BDD_ALA_MNY) != 0 &&
         stricmp(ALC_L->Module[i].SSE, BDD_ALA_ABB) != 0 &&
         stricmp(ALC_L->Module[i].SSE, BDD_ALA_ICS) != 0)
#endif
      {
         /* Incrementer le nombre de sous-ensembles d'alarmes */
         ALC_L->NSA += 1;

         /* Parcourir la table des alarmes du module */
         for (j = 1; j < BDD_NB_ALARMES; j++)
         {
            /* Si une alarme du module est presente */
            if (ALC_L->Module[i].COD[j] != BDD_ALA_ABSENTE)
            {
               /* Acquerir les parametres de l'alarme */
               ParAla_L = DonnerParAla(ALC_L->Module[i].SSE, j);

               // 0: Alarme événementielle, l’alarme n’a pas de durée. Il n’y a donc pas de disparition d’alarme.
               //    L’état du composant n’est pas impacté.
#ifdef _AVANT_ESFP_SUPERVISION
               if (ParAla_L->NivTraitement > BDD_ALA_NIV_EVENEMENT)
#endif // _AVANT_ESFP_SUPERVISION
               {
                  /* Récupérer l'année entière */
                  DonnerMillesime(&ALC_L->Module[i].Date[j], &Millesime_L);

#ifdef TOULOUSE8NOT_SUPPORTED
                  /* Imprimer l'alarme dans l'ecran des alarmes en cours */
                  PChar_L += sprintf(PChar_L,
                     "%2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u %2.2s%3.3u %1.1u %s\n",
                     ALC_L->Module[i].Date[j].Jour,
                     ALC_L->Module[i].Date[j].Mois,
                     Millesime_L,
                     ALC_L->Module[i].Date[j].Heure,
                     ALC_L->Module[i].Date[j].Minute,
                     ALC_L->Module[i].Date[j].Seconde,
                     ALC_L->Module[i].SSE,
                     j,
                     ParAla_L->NivTraitement,
                     // Dans la langue en cours (si elle existe !)
                     LangueAgent == PAR_LANGUE_FR ? ParAla_L->FR : ParAla_L->EN);
#else
                  /* Imprimer l'alarme dans l'ecran des alarmes en cours */
                  PChar_L += sprintf(PChar_L,
                     "%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d %c %s %1.1d %1.1d %3.3d %s\n",
                     ALC_L->Module[i].Date[j].Jour,
                     ALC_L->Module[i].Date[j].Mois,
                     Millesime_L,
                     ALC_L->Module[i].Date[j].Heure,
                     ALC_L->Module[i].Date[j].Minute,
                     ALC_L->Module[i].Date[j].Seconde,
                     ParAla_L->CategorieIntervention,
                     ALC_L->Module[i].SSE,
                     ParAla_L->GraviteAlarme,
                     ParAla_L->NivTraitement,
                     j,
                     // Dans la langue en cours (si elle existe !)
                     LangueAgent == PAR_LANGUE_FR ? ParAla_L->FR : ParAla_L->EN);
#endif
                  /* Mettre a jour le niveau de gravité maximale en cours */
                  GraviteMax_L = (ParAla_L->GraviteAlarme > GraviteMax_L) ? ParAla_L->GraviteAlarme : GraviteMax_L;

                  /* Mettre a jour le niveau de traitement maximale en cours */
                  NiveauMax_L = (ParAla_L->NivTraitement > NiveauMax_L) ? ParAla_L->NivTraitement : NiveauMax_L;

                  // 4 : L’état de l’équipement est hors service.
                  // 5 : Une effraction a été détectée sur l’équipement ayant généré.
                  //     L’état de l’équipement est hors service.
                  if (!EquipementHorsService && ParAla_L->NivTraitement >= BDD_ALA_NIV_DAT_HS)
                  {
                     /* Si le sous-ensemble est un equipement */
                     if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_EQ) == 0)
                     {
                        if (j != COD_EN_SERVICE_VENTE &&
                           j != COD_HORS_SERVICE_EXPLOITATION &&
                           j != COD_EN_SERVICE_SOMMEIL &&
                           j != COD_HORS_SERVICE_SOMMEIL &&
                           j != COD_INTERVENTION_EXPLOITATION &&
                           j != COD_INTERVENTION_MAINTENANCE &&
                           j != COD_INTERVENTION_RECETTE)
                        {
                           EquipementHorsService = true;
                        }
                     }

                     /* Si le sous-ensemble est une porte */
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_PT) == 0)
                     {
                        /* Si on n'est pas en intervention */
                        if (!InterventionEnCours())
                        {
                           EquipementHorsService = true;
                        }

                        /* Si ce n'est pas une ouverture porte avant ou arrière */
                        /* Pour autoriser la vente en mode test portes ouvertes */
                        else if (j != COD_OUVERTURE_PORTE_AVANT && j != COD_OUVERTURE_PORTES_ARRIERES && j != COD_CLE_ACTIONNEE)
                        {
                           EquipementHorsService = true;
                        }
                     }

                     /* Par defaut */
                     else
                     {
                        EquipementHorsService = true;
                     }
                  }

                  /* Ajouter l'alarme à la liste si elle n'est pas un événement ET qu'elle doit etre
                  transmise au systeme (du coup, les alarmes detaillees ne sont pas imprimees) */
                  // if (ParAla_L->NivTraitement != BDD_ALA_NIV_EVENEMENT && ParAla_L->EmettreVersCtreAlarmes)
#ifdef _AVANT_ESFP_SUPERVISION
                  if (ParAla_L->NivTraitement != BDD_ALA_NIV_EVENEMENT)
#endif // _AVANT_ESFP_SUPERVISION
                  {
                     char *PChar_L = NULL;

                     /* si la liste des alarmes n'est pas rempli */
                     if (k < BDD_NB_LAL)
                     {
                        /* pointer sur l'alarme a imprimer */
                        PChar_L = LAL_L[k];

                        /* memoriser le niveau de traitement de l'alarme */
                        ListeTraitement_L[k++] = ParAla_L->NivTraitement;
                     }
                     else
                     {
                        /* Atteindre le niveau de traitement de l'alarme en cours */
                        for (l = 1; l < ParAla_L->NivTraitement && PChar_L == NULL; l++)
                        {
                           /* Pour chaque alarme dans la liste */
                           for (m = 0; m < BDD_NB_LAL && PChar_L == NULL; m++)
                           {
                              /* Si le niveau de traitement correspond à l'alarme en cours */
                              if (ListeTraitement_L[m] == l)
                              {
                                 /* memoriser le niveau de traitement de l'alarme */
                                 ListeTraitement_L[m] = ParAla_L->NivTraitement;

                                 /* pointer sur l'alarme a imprimer */
                                 PChar_L = LAL_L[m];
                              }
                           }
                        }
                     }

                     if (PChar_L != NULL)
                     {
                        /* RAZ chaine de caracteres */
                        memset(PChar_L, '\0', sizeof LAL_L[0]);

                        /* Récupérer l'année entière */
                        Millesime_L = 0;
                        DonnerMillesime(&ALC_L->Module[i].Date[j], &Millesime_L);

                        /* Imprimer l'alarme dans la liste des alarmes en cours */
                        _snprintf(PChar_L,
                           sizeof LAL_L[0] - 1,
                           "%4.4d-%2.2d-%2.2d;%2.2d:%2.2d:%2.2d;%c%2.2s%1.1d%3.3d;%-30.30s",
                           Millesime_L,
                           ALC_L->Module[i].Date[j].Mois,
                           ALC_L->Module[i].Date[j].Jour,
                           ALC_L->Module[i].Date[j].Heure,
                           ALC_L->Module[i].Date[j].Minute,
                           ALC_L->Module[i].Date[j].Seconde,
                           ParAla_L->CategorieIntervention,
                           ALC_L->Module[i].SSE,
                           ParAla_L->GraviteAlarme,
                           ParAla_L->CodeAlarme,
                           ParAla_L->LibelleCourt);
                     }
                  }

                  /* Mettre a jour la ligne de service */
                  if (ParAla_L->MajLigneDeService)
                  {
                     switch (ParAla_L->CategorieIntervention)
                     {
                        /* Alarmes automatiques */
                     case 'A':
                        if (ParAla_L->NivTraitement > TraitementMaxA_L || Automatique[0] == '\0')
                        {
                           TraitementMaxA_L = ParAla_L->NivTraitement;

                           /* Afficher un message d'alarmes automatique */
                           sprintf(Automatique, "A%s%3.3d", ALC_L->Module[i].SSE, j);
                        }
                        break;

                        /* Alarmes a l'intention d'un agent de maintenance */
                     case 'M':
                        if (ParAla_L->NivTraitement > TraitementMaxM_L || Maintenance[0] == '\0')
                        {
                           TraitementMaxM_L = ParAla_L->NivTraitement;

                           /* Afficher un message d'alarmes d'agent de maintenance */
                           sprintf(Maintenance, "M%s%3.3d", ALC_L->Module[i].SSE, j);
                        }
                        break;

                        /* Alarmes a l'intention d'un agent d'exploitation */
                        /* Si l'alarme est bobineau vide elle est prioritaire sur les alarmes de même niveau */
                     case 'E':
                        if ((ParAla_L->NivTraitement > TraitementMaxE_L || Exploitation[0] == '\0') ||
                           (ParAla_L->NivTraitement == TraitementMaxE_L && strcmp(ALC_L->Module[i].SSE, "IC") == 0 &&
                           (j == 302 || j == 312 || j == 322 || j == 332)))
                        {
                           TraitementMaxE_L = ParAla_L->NivTraitement;

                           /* Afficher un message d'alarmes d'agent d'exploitation */
                           sprintf(Exploitation, "E%s%3.3d", ALC_L->Module[i].SSE, j);
                        }
                        break;

                        /* Alarmes a l'intention d'un agent de recette */
                     case 'R':
                        if (ParAla_L->NivTraitement > TraitementMaxR_L || Recette[0] == '\0')
                        {
                           TraitementMaxR_L = ParAla_L->NivTraitement;

                           /* Afficher un message d'alarmes d'agent de recette */
                           sprintf(Recette, "R%s%3.3d", ALC_L->Module[i].SSE, j);
                        }
                        break;

                     default:
                        break;
                     }
                  }

                  // Si une action sirene est associe a l'alarme
                  if (ParAla_L->ActionnerSirene)
                  {
                     // Declencher la sirene
                     ActionnerSirene_L = VRAI;
                  }

                  // Si une activation du TOR 1 est associée a l'alarme
                  if (ParAla_L->ActionnerTor1)
                  {
                     ActionnerTor1_L = VRAI;
                  }

                  // Si une activation du TOR 2 est associée a l'alarme
                  if (ParAla_L->ActionnerTor2)
                  {
                     ActionnerTor2_L = VRAI;
                  }

                  // Si une activation du TOR 3 est associée a l'alarme
                  if (ParAla_L->ActionnerTor3)
                  {
                     ActionnerTor3_L = VRAI;
                  }

                  // Si une activation du TOR 4 est associée a l'alarme
                  if (ParAla_L->ActionnerTor4)
                  {
                     ActionnerTor4_L = VRAI;
                  }

                  // Est-ce un défaut technique ?
                  if (ParAla_L->DefautTechnique)
                  {
                     // Le distributeur est-il en panne aka Hors-Service ?
                     if (ParAla_L->NivTraitement >= BDD_ALA_NIV_DAT_HS)
                     {
                        EtatTechnique_L = BDD_EN_PANNE;
                     }

                     // Le distributeur est-il dégradé ?
                     else if (ParAla_L->NivTraitement >= BDD_ALA_NIV_SSE_DEGRADE)
                     {
                        if (EtatTechnique_L != BDD_EN_PANNE)
                        {
                           EtatTechnique_L = BDD_DEGRADE;
                        }
                     }
                  }

                  /* Mémoriser l'état des catégories de la GTC */
                  AssocierAvecGTC(GTC_L.Effraction, ParAla_L->GTC.Effraction);
                  AssocierAvecGTC(GTC_L.BobinotDVide, ParAla_L->GTC.BobinotDVide);
                  AssocierAvecGTC(GTC_L.CaisseMonnaieVide, ParAla_L->GTC.CaisseMonnaieVide);
                  AssocierAvecGTC(GTC_L.ReserveMonnaieVide, ParAla_L->GTC.ReserveMonnaieVide);
                  AssocierAvecGTC(GTC_L.CarteBancaireHS, ParAla_L->GTC.CarteBancaireHS);
                  AssocierAvecGTC(GTC_L.ImprimanteCodeuseHS, ParAla_L->GTC.ImprimanteCodeuseHS);
                  AssocierAvecGTC(GTC_L.ModuleTelebillettiqueHS, ParAla_L->GTC.ModuleTelebillettiqueHS);
                  AssocierAvecGTC(GTC_L.MonnayeurHS, ParAla_L->GTC.MonnayeurHS);
                  AssocierAvecGTC(GTC_L.EquipementHS, ParAla_L->GTC.EquipementHS);
                  AssocierAvecGTC(GTC_L.BobinotBVide, ParAla_L->GTC.BobinotBVide);
                  AssocierAvecGTC(GTC_L.EquipementHorsExploitation, ParAla_L->GTC.EquipementHorsExploitation);
                  AssocierAvecGTC(GTC_L.RenduMonnaieHS, ParAla_L->GTC.RenduMonnaieHS);
                  AssocierAvecGTC(GTC_L.BobinotCPresqueVide, ParAla_L->GTC.BobinotCPresqueVide);
                  AssocierAvecGTC(GTC_L.BobinotCVide, ParAla_L->GTC.BobinotCVide);
                  AssocierAvecGTC(GTC_L.PorteOuverte, ParAla_L->GTC.PorteOuverte);

               }
            }
         }
      }
   }

   /* Mettre à jour les catégories des alarmes pour la GTC */
   Bdd->EcrireGTC(GTC_L);

   /* Mettre a jour la liste des Alarmes */
   Bdd->EcrireLAL(LAL_L);

   /* Acquerir la liste des Alarmes */
   Bdd->LireETA(ETA_L);

   /* Si le niveau de gravite en cours est different de la BDD */
   if (ETA_L.CLM != GraviteMax_L)
   {
      /* Acquerir le niveau de gravite en cours */
      ETA_L.CLM = GraviteMax_L;

      /* Mettre a jour la liste des Alarmes dans la Bdd */
      Bdd->EcrireETA(ETA_L);
   }

   /* Si le niveau de traitement max en cours est different de la BDD */
   if (ETA_L.NiveauMaxAlarme != NiveauMax_L)
   {
      /* Acquerir le niveau de gravite en cours */
      ETA_L.NiveauMaxAlarme = NiveauMax_L;

      /* Mettre a jour la liste des Alarmes dans la Bdd */
      Bdd->EcrireETA(ETA_L);
   }

   /* Si on doit actionner la sirène en dehors des interventions agents */
   if (ActionnerSirene_L && TypeIntervention == '0')
   {
      /* Activer la sirene */
      Es->ActiverSirene();
   }

   else
   {
      /* Arreter la sirene */
     // Es->DesactiverSirene ();
   }

   /* TOR1 : Défaut Exploitation */
   if (ActionnerTor1_L)
   {
      /* Activer la TOR 1 */
      Es->ActiverTor1();
   }
   else
   {
      /* Désactiver la TOR 1 */
      Es->DesactiverTor1();
   }

   /* TOR2 : Défaut Technique */
   if (ActionnerTor2_L)
   {
      /* Activer la TOR 2 */
      Es->ActiverTor2();
   }
   else
   {
      /* Désactiver la TOR 2 */
      Es->DesactiverTor2();
   }

   /* TOR3 : Hors Service */
   if (ActionnerTor3_L)
   {
      /* Activer la TOR 3 */
      Es->ActiverTor3();
   }
   else
   {
      /* Désactiver la TOR 3 */
      Es->DesactiverTor3();
   }

   // TOR4 : Boucle sèche Effraction
   // En principe levée sur porte ouverte effraction ou retrait caisse hors procédure
   if (ActionnerTor4_L)
   {
      // Activer la TOR 4
      Es->ActiverTor4();
   }
   else
   {
      // Désactiver la TOR 4
      Es->DesactiverTor4();
   }


   // Etat technique
   Bdd->EcrireEtatTechnique(EtatTechnique_L);

   /* Construire la ligne de service */
   ConstruireLigneService(&LigneService_L);

   /* Afficher la ligne de service */
   Rhm->AfficherLigneService(LigneService_L);

}

/*****************************************************************************
* But de la fonction : Traiter presence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::DonnerIdentifiantModuleEnAlarme(T_BddESE& ESE_P)

{
   unsigned int IndiceModule_L;

   memset(&ESE_P.IdentifiantModule, 0, sizeof ESE_P.IdentifiantModule);

   // AG : Agent
   if (strcmp(ESE_P.SSE, BDD_ALA_AG) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_AG);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // CR : Connexion réseau
   else if (strcmp(ESE_P.SSE, BDD_ALA_CR) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_CR);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // DC : Dépileur codeur
   else if (strcmp(ESE_P.SSE, BDD_ALA_DC) == 0)
   {
      T_BddCompDC DC_L;

      /* Lire le composant DC */
      Bdd->LireCompDC(DC_L);

      ESE_P.IdentifiantModule.NumOrdre = DC_L.DC.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_DC);
      strncpy(ESE_P.IdentifiantModule.NumSerie, DC_L.DC.NumSerie, 34);
   }

   // EQ : Equipement
   else if (strcmp(ESE_P.SSE, BDD_ALA_EQ) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_EQ);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // FA : Fichier d'activité
   else if (strcmp(ESE_P.SSE, BDD_ALA_FA) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_FA);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // FP : Fichier paramètre
   else if (strcmp(ESE_P.SSE, BDD_ALA_FP) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_FP);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // IC : Imprimante codeuse
   else if (strcmp(ESE_P.SSE, BDD_ALA_IC) == 0)
   {
      T_BddCompIC IC_L;

      /* Lire le composant IC */
      Bdd->LireCompIC(IC_L);

      ESE_P.IdentifiantModule.NumOrdre = IC_L.IC.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_IC);
      strncpy(ESE_P.IdentifiantModule.NumSerie, IC_L.IC.NumSerie, 34);
   }

   // IR : Imprimante reçu
   else if (strcmp(ESE_P.SSE, BDD_ALA_IR) == 0)
   {
      T_BddCompIR IR_L;

      /* Lire le composant Ir */
      Bdd->LireCompIR(IR_L);

      ESE_P.IdentifiantModule.NumOrdre = IR_L.IR.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_IR);
      strncpy(ESE_P.IdentifiantModule.NumSerie, IR_L.IR.NumSerie, 21);
   }

   // MB : Paiement carte bancaire
   else if (strcmp(ESE_P.SSE, BDD_ALA_MB) == 0)
   {
      T_BddCompMB MB_L;

      /* Lire le composant MB */
      Bdd->LireCompMB(MB_L);

      ESE_P.IdentifiantModule.NumOrdre = MB_L.MB.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_MB);
      strncpy(ESE_P.IdentifiantModule.NumSerie, MB_L.MB.NumSerie, 21);
   }

   // PB : Paiement par billets de banque
   else if (strcmp(ESE_P.SSE, BDD_ALA_PB) == 0)
   {
      T_BddCompPB PB_L;

      /* Lire le composant PB */
      Bdd->LireCompPB(PB_L);

      ESE_P.IdentifiantModule.NumOrdre = PB_L.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_PB);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // PP : Paiement par pièces de monnaie
   else if (strcmp(ESE_P.SSE, BDD_ALA_PP) == 0)
   {
      T_BddCompPP PP_L;

      /* Lire le composant PP */
      Bdd->LireCompPP(PP_L);

      // Cas du sélecteur Paiement par Pièce (module PP)
      if ((ESE_P.COD >= ALA_MODULE_TRI) && (ESE_P.COD < (ALA_MODULE_TRI + NB_ALA_MODULE_TRI)))
      {
         ESE_P.IdentifiantModule.NumOrdre = PP_L.PP.NumOrdre;
         strcpy(ESE_P.IdentifiantModule.Type, "PP");
         strncpy(ESE_P.IdentifiantModule.NumSerie, PP_L.PP.NumSerie, 34);
      }

      // Cas de la Réserve Complémentaire (module RC)
      if ((ESE_P.COD >= ALA_RESERVE) && (ESE_P.COD < (ALA_RESERVE + (BDD_MAX_RESERVES * NB_ALA_RESERVE))))
      {
         IndiceModule_L = (ESE_P.COD % ALA_RESERVE) / NB_ALA_RESERVE;

         if (IndiceModule_L < BDD_MAX_RESERVES)
         {
            ESE_P.IdentifiantModule.NumOrdre = PP_L.RC.Reserves[IndiceModule_L].Identifiant.Position + 1;
            strcpy(ESE_P.IdentifiantModule.Type, "RC");
            strncpy(ESE_P.IdentifiantModule.NumSerie, PP_L.RC.Reserves[IndiceModule_L].NumSerie, 34);
         }
      }

      // Cas du REcycleur (module RE)
      if ((ESE_P.COD >= ALA_RECYCLEUR) && (ESE_P.COD < (ALA_RECYCLEUR + (BDD_MAX_RECYCLEUR * NB_ALA_RECYCLEUR))))
      {
         IndiceModule_L = (ESE_P.COD%ALA_RECYCLEUR) / NB_ALA_RECYCLEUR;

         if (IndiceModule_L < BDD_MAX_RECYCLEUR)
         {
            ESE_P.IdentifiantModule.NumOrdre = PP_L.RE.Recycleurs[IndiceModule_L].Identifiant.Position + 1;
            strcpy(ESE_P.IdentifiantModule.Type, "RE");
            strncpy(ESE_P.IdentifiantModule.NumSerie, PP_L.RE.Recycleurs[IndiceModule_L].NumSerie, 34);
         }
      }
   }

   // PT : Portes du distributeur
   else if (strcmp(ESE_P.SSE, BDD_ALA_PT) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_PT);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // RT : Rechargement télébillettique
   else if (strcmp(ESE_P.SSE, BDD_ALA_RT) == 0)
   {
      T_BddCompRT RT_L;

      /* Lire le composant RT */
      Bdd->LireCompRT(RT_L);

      ESE_P.IdentifiantModule.NumOrdre = RT_L.RT.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_RT);
      strncpy(ESE_P.IdentifiantModule.NumSerie, RT_L.RT.NumSerie, 21);
   }

   // VD : VDE
   else if (strcmp(ESE_P.SSE, BDD_ALA_VD) == 0)
   {
      T_BddCompVD VD_L;

      /* Lire le composant Tvi (VDE) */
      Bdd->LireCompVD(VD_L);

      ESE_P.IdentifiantModule.NumOrdre = VD_L.VD.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_VD);
      strncpy(ESE_P.IdentifiantModule.NumSerie, VD_L.VD.NumSerie, 21);
   }
#ifdef _APRES_ESFP_SUPERVISION
   // OR : Optical Reader (Lecteur code Barre)
   else if (strcmp(ESE_P.SSE, BDD_ALA_OR) == 0)
   {
      T_BddCompOR OR_L;

      /* Lire le composant Tvi (VDE) */
      Bdd->LireCompOR(OR_L);

      ESE_P.IdentifiantModule.NumOrdre = OR_L.OR.NumOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_OR);
      strcpy(ESE_P.IdentifiantModule.NumSerie, OR_L.OR.NumSerie);
   }

   // XA : alarmes detaillées du monnayeur RS2x
   else if (strcmp(ESE_P.SSE, BDD_ALA_RS22) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_RS22);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // YA : alarmes detaillées de l'accepteur de billets BNA57 de MEI
   else if (strcmp(ESE_P.SSE, BDD_ALA_ABB) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_ABB);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // YB : alarmes detaillées de l'accepteur de billets BNR/BNA
   else if (strcmp(ESE_P.SSE, BDD_ALA_BNR) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_BNR);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // YC : alarmes detaillées BV
   else if (strcmp(ESE_P.SSE, BDD_ALA_BV) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_BV);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // ZA : alarmes detaillées BCS400
   else if (strcmp(ESE_P.SSE, BDD_ALA_ICS) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_ICS);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // X- : alarmes detaillées du monnayeur
   else if (strcmp(ESE_P.SSE, BDD_ALA_MNY) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_MNY);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }
#else
   // Y- : alarmes detaillées de l'accepteur de billets
   else if (strcmp(ESE_P.SSE, BDD_ALA_ABB) == 0)
   {
      ESE_P.IdentifiantModule.NumOrdre = ESE_P.NoOrdre;
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_ABB);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }
#endif

   // default
   else
   {
      strcpy(ESE_P.IdentifiantModule.Type, BDD_ALA_NONE);
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

   // Verification du numéro de série : Si le numéro de série est vide, il faut mettre la valeur à '0'
   if (strlen(ESE_P.IdentifiantModule.NumSerie) == 0)
   {
      strcpy(ESE_P.IdentifiantModule.NumSerie, ACT_NO_SERIE_DEFAUT);
   }

}

/*****************************************************************************
* But de la fonction : Traiter presence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterPresenceAlarme(unsigned int Module_P,
   unsigned int SousModule_P,
   unsigned int COD_P,
   unsigned int NbAlarmes_P)

{
   unsigned int COD_L = SousModule_P + COD_P;
   unsigned int i;
   unsigned int AlarmeMonnayeurHS_L = FAUX;
   unsigned int AlarmeBancaireHS_L = FAUX;
   unsigned int AlarmeBilletHS_L = FAUX;
   unsigned int AlarmeDelivranceTitreTransportHS_L = FAUX;
   unsigned int AlarmeRechargementHS_L = FAUX;
   unsigned int AlarmeDelivranceRecuHS_L = FAUX;
   T_BddALC *ALC_L = NULL;
   T_ParAla *ParAla_L = NULL;
   T_ParAla *ParAlaOld_L = NULL;
   T_BddESE ESE_L;

   /* Acquerir la rubrique des Alarmes en cours */
   Bdd->LireALC(&ALC_L);

   /* Tester le numero de module */
   if (Module_P < BDD_NB_MODULES)
   {
      /* Tester le numero de l'alarme */
      if (COD_L < BDD_NB_ALARMES)
      {
         /* Si l'alarme est absente */
         if (ALC_L->Module[Module_P].COD[COD_L] == BDD_ALA_ABSENTE)
         {
            /* Acquerir les parametres de l'alarme */
            ParAla_L = DonnerParAla(ALC_L->Module[Module_P].SSE, COD_L);

            /* Parcourir la table des Alarmes du sous module */
            for (i = 1; i <= NbAlarmes_P; i++)
            {
               /* Si une alarme est presente */
               if (ALC_L->Module[Module_P].COD[SousModule_P + i] == BDD_ALA_PRESENTE)
               {
                  /* Si c'est une nouvelle alarme */
                  if (COD_L != SousModule_P + i)
                  {
                     /* Acquerir les parametres de l'alarme precedente */
                     ParAlaOld_L = DonnerParAla(ALC_L->Module[Module_P].SSE, SousModule_P + i);

                     /* Si le niveau de traitement de l'ancienne alarme
                     est inférieur à la nouvelle */
                     if (ParAlaOld_L->NivTraitement <= ParAla_L->NivTraitement)
                     {
                        /* L'ancienne alarme n'est plus mémorisée comme présente */
                        ALC_L->Module[Module_P].COD[SousModule_P + i] = BDD_ALA_DETECTEE_DISPARUE;
                     }
                  }
               }
            }

            /* Incrementer le nombre d'Alarmes presentes */
            ALC_L->Module[Module_P].NAP += 1;

            /* Acquerir la rubrique etat du sous-ensemble dans la Bdd */
            Bdd->LireESE(ESE_L);

            /* Mettre a jour l'etat du sous-ensemble */
            ESE_L.NoOrdre = 1;
            strcpy(ESE_L.SSE, ALC_L->Module[Module_P].SSE);
            ESE_L.COD = COD_L;
            ESE_L.TIN = ParAla_L->CategorieIntervention;
            ESE_L.Gravite = ParAla_L->GraviteAlarme;
            ESE_L.NivTraitement = ParAla_L->NivTraitement;
            ESE_L.Disparition = FAUX;
            memset(ESE_L.Libelle, '\0', sizeof ESE_L.Libelle);
            strncpy(ESE_L.Libelle, Par->DonnerLangueAgentParDefaut() == PAR_LANGUE_FR ? ParAla_L->FR : ParAla_L->EN, sizeof ESE_L.Libelle - 1);
            DonnerIdentifiantModuleEnAlarme(ESE_L);

            /* Mettre a jour la rubrique dans la Bdd */
            Bdd->EcrireESE(ESE_L);

            /* Memorisation alarme paiement par pièce pour ticket prejudice */
            if (strcmp(ESE_L.SSE, BDD_ALA_PP) == 0)
            {
               *EsePP = ESE_L;
            }
            /* Memorisation alarme rechargement télébillettique */
            else if (strcmp(ESE_L.SSE, BDD_ALA_RT) == 0
               || strcmp(ESE_L.SSE, BDD_ALA_IC) == 0
               || strcmp(ESE_L.SSE, BDD_ALA_VD) == 0)
            {
               *EseMaterialisation = ESE_L;
            }

            /* Mettre a jour la date et l'heure d'apparition de l'alarme */
            DonnerDateHeure(&ALC_L->Module[Module_P].Date[COD_L]);

            /* Memoriser la presence de l'alarme */
            ALC_L->Module[Module_P].COD[COD_L] = BDD_ALA_PRESENTE;

            /* Niveau de traitement de l'alarme */
            ALC_L->Module[Module_P].NivTraitement[COD_L] = ParAla_L->NivTraitement;

            /* Mettre a jour l'alarme en cours dans la Bdd */
            Bdd->EcrireALC();

            /* Mettre à jour l'état des composants avant d'enregistrer l'alarme */

            /* Si une alarme Module HS est detectee */
            if (ParAla_L->NivTraitement >= BDD_ALA_NIV_SSE_HS)
            {
               /* Incrementer le nombre de hors service du module */
               ALC_L->Module[Module_P].HS += 1;

               // DC : Dépileur codeur
               if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_DC) == 0)
               {
                  T_BddCompDC DC_L;

                  if (!ConfigDat->LireIR().TP || (ConfigDat->LireIR().TP && Bdd->LireCompIR().Etat != BDD_EC_EN_SERVICE))
                  {
                     // Positionner l'alarme equipement délivrance HS
                     AlarmeDelivranceTitreTransportHS_L = VRAI;
                  }

                  /* Acquerir la rubrique DC */
                  Bdd->LireCompDC(DC_L);

                  /* Memoriser l'etat Hors Service du module bancaire */
                  DC_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique DC */
                  Bdd->EcrireCompDC(DC_L);
               }

               // IC : Imprimante codeuse
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_IC) == 0)
               {
                  T_BddCompIC IC_L;

                  if (!ConfigDat->LireIR().TP || (ConfigDat->LireIR().TP && Bdd->LireCompIR().Etat != BDD_EC_EN_SERVICE))
                  {
                     // Positionner l'alarme equipement délivrance HS
                     AlarmeDelivranceTitreTransportHS_L = VRAI;
                  }

                  /* Acquerir la rubrique du composant IC */
                  Bdd->LireCompIC(IC_L);

                  if (ConfigDat->LireICS().Recu)
                  {
                     // Positionner l'alarme equipement délivrance recu HS
                     AlarmeDelivranceRecuHS_L = VRAI;
                  }

                  /* Memoriser l'etat Hors Service de l'ICS */
                  IC_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique ICS dans la Bdd */
                  Bdd->EcrireCompIC(IC_L);
               }

               // IR : Imprimante reçu
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_IR) == 0)
               {
                  T_BddCompIR IR_L;

                  if (ConfigDat->LireIR().TP)
                  {
                     if (!ConfigDat->LireICS().Present || (ConfigDat->LireICS().Present && Bdd->LireCompIC().Etat != BDD_EC_EN_SERVICE))
                     {
                        // Positionner l'alarme equipement délivrance HS
                        AlarmeDelivranceTitreTransportHS_L = VRAI;
                     }
                  }

                  if (ConfigDat->LireIR().Recu)
                  {
                     // Positionner l'alarme equipement délivrance recu HS
                     AlarmeDelivranceRecuHS_L = VRAI;
                  }

                  /* Acquerir la rubrique du composant IR */
                  Bdd->LireCompIR(IR_L);

                  /* Memoriser l'etat Hors Service de IR */
                  IR_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique  dans la Bdd */
                  Bdd->EcrireCompIR(IR_L);
               }

               // MB : Paiement carte bancaire
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_MB) == 0)
               {
                  T_BddCompMB MB_L;

                  // Positionner l'alarme equipement paiement carte bancaire HS
                  AlarmeBancaireHS_L = VRAI;

                  /* Acquerir la rubrique du composant module bancaire MB */
                  Bdd->LireCompMB(MB_L);

                  /* Memoriser l'etat Hors Service du module bancaire */
                  MB_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique MPC dans la Bdd */
                  Bdd->EcrireCompMB(MB_L);
               }

               // PB : Paiement par billets de banque
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_PB) == 0)
               {
                  T_BddCompPB PB_L;

                  // Positionner l'alarme equipement paiement billet HS
                  AlarmeBilletHS_L = VRAI;

                  /* Acquerir la rubrique du composant PB */
                  Bdd->LireCompPB(PB_L);

                  /* Memoriser l'etat Hors Service du composant */
                  PB_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique dans la Bdd */
                  Bdd->EcrireCompPB(PB_L);
               }

               // PP : Paiement par pièces de monnaie
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_PP) == 0)
               {
                  T_BddCompPP PP_L;

                  // Positionner l'alarme equipement paiement monnaie HS
                  AlarmeMonnayeurHS_L = VRAI;

                  /* Acquerir la rubrique du composant PP */
                  Bdd->LireCompPP(PP_L);

                  /* Memoriser l'etat Hors Service du composant */
                  PP_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique dans la Bdd */
                  Bdd->EcrireCompPP(PP_L);
               }

               // RT : Rechargement télébillettique
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_RT) == 0)
               {
                  T_BddCompRT RT_L;

                  // Positionner l'alarme equipement rechargement HS
                  AlarmeRechargementHS_L = VRAI;

                  /* Acquerir la rubrique du composant RT */
                  Bdd->LireCompRT(RT_L);

                  /* Memoriser l'etat Hors Service de RT */
                  RT_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique  dans la Bdd */
                  Bdd->EcrireCompRT(RT_L);
               }

               // VD : VDE
               else if (strcmp(ALC_L->Module[Module_P].SSE, BDD_ALA_VD) == 0)
               {
                  T_BddCompVD VD_L;

                  // Positionner l'alarme equipement délivrance HS
                  AlarmeDelivranceTitreTransportHS_L = VRAI;

                  /* Acquerir la rubrique du composant VD */
                  Bdd->LireCompVD(VD_L);

                  /* Memoriser l'etat Hors Service de VD */
                  VD_L.Etat = BDD_EC_HORS_SERVICE;

                  /* Mettre a jour la rubrique  dans la Bdd */
                  Bdd->EcrireCompVD(VD_L);
               }
            }

            /* Enregistrer les alarmes  */
            EnregistrerAlarme(ParAla_L);

            /* Traiter la signalisation des Alarmes */
            TraiterSignalisationAlarmes();

            /* Si une alarme CB HS est detectee */
            if (AlarmeBancaireHS_L == VRAI)
            {
               // Positionner l'alarme equipement paiement carte bancaire HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_CB_HS);
            }

            /* Si une alarme Monnayeur HS est detectee */
            if (AlarmeMonnayeurHS_L == VRAI)
            {
               // Positionner l'alarme equipement paiement monnaie HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_MONNAIE_HS);
            }

            /* Si une alarme billet HS est detectee */
            if (AlarmeBilletHS_L == VRAI)
            {
               // Positionner l'alarme equipement paiement billets HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_BILLET_HS);
            }

            /* Si une alarme delivrance HS est detectee */
            if (AlarmeDelivranceTitreTransportHS_L == VRAI)
            {
               // Positionner l'alarme equipement delivrance titre HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
            }

            /* Si une alarme rechargement HS est detectee */
            if (AlarmeRechargementHS_L == VRAI)
            {
               // Positionner l'alarme equipement rechargement HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
            }

            /* Si une alarme delivrance recu HS est detectee */
            if (AlarmeDelivranceRecuHS_L == VRAI)
            {
               // Positionner l'alarme equipement recu HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
            }

            // Si alarme effraction
            if (ParAla_L->NivTraitement == BDD_ALA_NIV_EFFRACTION)
            {
               // Positionner l'alarme equipement recu HS
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_EFFRACTION);
            }
         }
         else
         {
            if (ALC_L->Module[Module_P].COD[COD_L] != BDD_ALA_PRESENTE)
            {
               /* Memoriser le numero de l'alarme presente */
               ALC_L->Module[Module_P].COD[COD_L] = BDD_ALA_PRESENTE;

               /* Mettre a jour la base de donnees */
               Bdd->EcrireALC();
            }
         }

         // Si on vient de traiter une alarme détaillée du monnayeur ou de l'accepteur
         // il faut egalement lever l'alarme de synthese "PP" ou "PB" qui lui correspond.
         if (ParAla_L != NULL)
         {
#ifdef _AVANT_ESFP_SUPERVISION
            if (stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_RS22) == 0)
            {
               if (ParAla_L->COD != 0)
               {
                  TraiterPresenceAlarme(BDD_INDEX_PP, ParAla_L->COD);
               }
            }
            else if (stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_ABB) == 0 ||
               stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_BV) == 0 ||
               stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_BNR) == 0)
            {
               if (ParAla_L->COD != 0)
               {
                  TraiterPresenceAlarme(BDD_INDEX_PB, ParAla_L->COD);
               }
            }
#else
            if (stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_MNY) == 0)
            {
               if (ParAla_L->COD != 0)
               {
                  TraiterPresenceAlarme(BDD_INDEX_PP, ParAla_L->COD);
               }
            }
            else if (stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_ABB) == 0)
            {
               if (ParAla_L->COD != 0)
               {
                  TraiterPresenceAlarme(BDD_INDEX_PB, ParAla_L->COD);
               }
            }
            else if (stricmp(ALC_L->Module[Module_P].SSE, BDD_ALA_ICS) == 0)
            {
               if (ParAla_L->COD != 0)
               {
                  TraiterPresenceAlarme(BDD_INDEX_IC, ParAla_L->COD);
               }
            }
#endif
         }
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter association des alarmes périphériques avec les alarmes composants
* --------------------
* Parametres :
* ------------
*
* entree : Table des alarmes
*          Index du composant dans la table
*          Index du périphérique dans la table
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterDisparitionAlarme(T_BddALC *ALC_P, unsigned int IdComp_P, unsigned int IdPeriph_P)

{
   unsigned int i;
   T_ParAla *ParAla_L;

   // Pour toutes les alarmes du composant
   for (i = 1; i < BDD_NB_ALARMES; i++)
   {
      // Filtrer les alarmes liées aux télécommandes du SALR
      if (i != ALA_HS_PAR_SALR)
      {
         // Alarme du composant à priori absente :
         // 1 alarme du composant est présente si au moins une alarme du périphérique qui lui correspond est levée.
         ALC_P->Module[IdComp_P].COD[i] = BDD_ALA_ABSENTE;
      }
   }

   // Pour toutes les alarmes du périphérique
   for (i = 1; i < BDD_NB_ALARMES; i++)
   {
      // Alarme disparue ?
      if (ALC_P->Module[IdPeriph_P].COD[i] == BDD_ALA_DISPARUE)
      {
         // acquerir les parametres de l'alarme du périphérique
         ParAla_L = DonnerParAla(ALC_P->Module[IdPeriph_P].SSE, i);

         if (ParAla_L->COD != 0)
         {
            // mise a jour de l'alarme du composant associé au périphérique
            ALC_P->Module[IdComp_P].COD[ParAla_L->COD] = BDD_ALA_DISPARUE;
         }
      }
   }

   // Pour toutes les alarmes du périphérique
   for (i = 1; i < BDD_NB_ALARMES; i++)
   {
      // Alarme présente ou détectée disparue
      if (ALC_P->Module[IdPeriph_P].COD[i] == BDD_ALA_PRESENTE || ALC_P->Module[IdPeriph_P].COD[i] == BDD_ALA_DETECTEE_DISPARUE)
      {
         // acquerir les parametres de l'alarme du périphérique
         ParAla_L = DonnerParAla(ALC_P->Module[IdPeriph_P].SSE, i);

         if (ParAla_L->COD != 0)
         {
            // mise a jour de l'alarme du composant associé au périphérique
            ALC_P->Module[IdComp_P].COD[ParAla_L->COD] = BDD_ALA_PRESENTE;
         }
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter disparition des Alarmes
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterDisparitionAlarme(void)

{
   unsigned int i;
   unsigned int j;
   unsigned int k;
   T_BddALC *ALC_L;
   T_ParAla *ParAla_L;
   T_BddESE ESE_L;

   /* Acquerir la rubrique des Alarmes en cours */
   Bdd->LireALC(&ALC_L);

#ifdef _AVANT_ESFP_SUPERVISION
   if (ConfigDat->LirePP().Present == VRAI && ConfigDat->LirePP().Type == PP_MNY)
   {
      // Traiter association des alarmes RS2x avec les alarmes du composant PP
      TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PP, BDD_INDEX_RS22);
   }

   if (ConfigDat->LirePB().Present == VRAI)
   {
      if (ConfigDat->LirePB().Type == PB_BNA57_MEI)
      {
         // Traiter association des alarmes accepteur billets de banque avec les alarmes du composant PB
         TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PB, BDD_INDEX_ABB);
      }

      else if (ConfigDat->LirePB().Type == PB_BNR)
      {
         // Traiter association des alarmes accepteur billets de banque avec les alarmes du composant PB
         TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PB, BDD_INDEX_BNR);
      }

      else if (ConfigDat->LirePB().Type == PB_BV || ConfigDat->LirePB().Type == PB_MSM)
      {
         // Traiter association des alarmes accepteur billets de banque avec les alarmes du composant PB
         TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PB, BDD_INDEX_BV);
      }

   }
#else
   if (ConfigDat->LirePP().Present == VRAI)
   {
      // Traiter association des alarmes RS2x avec les alarmes du composant PP
      TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PP, BDD_INDEX_MNY);
   }

   if (ConfigDat->LirePB().Present == VRAI)
   {
      // Traiter association des alarmes accepteur billets de banque avec les alarmes du composant PB
      TraiterDisparitionAlarme(ALC_L, BDD_INDEX_PB, BDD_INDEX_ABB);
   }

   if (ConfigDat->LireICS().Present == VRAI)
   {
      // Traiter association des alarmes BCS400 de banque avec les alarmes du composant IC
      TraiterDisparitionAlarme(ALC_L, BDD_INDEX_IC, BDD_INDEX_ICS);
   }
#endif

   /* Parcourir la table des modules */
   for (i = 0; i < BDD_NB_MODULES; i++)
   {
      /* Si il y a au moins une alarme presente */
      if (ALC_L->Module[i].NAP != 0)
      {
         /* Parcourir la table des Alarmes du module */
         for (j = 1; j < BDD_NB_ALARMES; j++)
         {
            /* Si l'alarme est presente */
            if (ALC_L->Module[i].COD[j] != BDD_ALA_ABSENTE)
            {
               /* Acquerir les parametres de l'alarme */
               ParAla_L = DonnerParAla(ALC_L->Module[i].SSE, j);

               /* Si l'alarme a disparu */
               if (ALC_L->Module[i].COD[j] == BDD_ALA_DISPARUE)
               {
                  /* Memoriser l'etat de l'alarme "ABSENTE" */
                  ALC_L->Module[i].COD[j] = BDD_ALA_ABSENTE;

                  /* Acquerir la rubrique etat du sous-ensemble */
                  Bdd->LireESE(ESE_L);

                  /* Memoriser l'etat du sous-ensemble */
                  ESE_L.NoOrdre = 1; // 02/12/2016 : ajout !
                  strcpy(ESE_L.SSE, ALC_L->Module[i].SSE);
                  ESE_L.COD = j;
                  ESE_L.TIN = ParAla_L->CategorieIntervention;
                  ESE_L.Gravite = ParAla_L->GraviteAlarme;
                  ESE_L.NivTraitement = ParAla_L->NivTraitement;
                  ESE_L.Disparition = VRAI;
                  memset(ESE_L.Libelle, '\0', sizeof ESE_L.Libelle);
                  strncpy(ESE_L.Libelle, ParAla_L->LibelleCourt, sizeof ESE_L.Libelle - 1);
                  DonnerIdentifiantModuleEnAlarme(ESE_L);

                  /* Mettre a jour la rubrique etat du sous-ensemble */
                  Bdd->EcrireESE(ESE_L);

                  /* Initialiser le nombre d'alarme presente */
                  ALC_L->Module[i].NAP = 0;

                  /* Initialiser le nombre de Hors service du module */
                  ALC_L->Module[i].HS = 0;

                  /* Parcourir la table des Alarmes pour les compter */
                  for (k = 1; k < BDD_NB_ALARMES; k++)
                  {
                     /* Si une alarme est presente */
                     if (ALC_L->Module[i].COD[k] != BDD_ALA_ABSENTE)
                     {
                        T_ParAla *ParAla_L;

                        /* Acquerir les parametres de l'alarme */
                        ParAla_L = DonnerParAla(ALC_L->Module[i].SSE, k);

                        /* Incrementer le nombre d'alarme presente */
                        ALC_L->Module[i].NAP += 1;

                        /* Si une alarme module HS détectée */
                        if (ParAla_L->NivTraitement >= BDD_ALA_NIV_SSE_HS)
                        {
                           /* Decrementer le nombre de Hors service du module */
                           ALC_L->Module[i].HS += 1;
                        }
                     }
                  }

                  /* Si le module n'est plus Hors Service */
                  if (ALC_L->Module[i].HS == 0)
                  {
                     // DC : Dépileur codeur
                     if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_DC) == 0)
                     {
                        T_BddCompDC DC_L;

                        /* Acquerir la rubrique DC */
                        Bdd->LireCompDC(DC_L);

                        // Si le composant est absent on ne change pas son état
                        if (DC_L.Etat != BDD_EC_ABSENT)
                        {
                           /* Le module DCS est en service */
                           DC_L.Etat = BDD_EC_EN_SERVICE;

                           /* Mettre a jour la rubrique DC */
                           Bdd->EcrireCompDC(DC_L);

                           // Dépositionner l'alarme equipement delivrance HS
                           if (SAM_TLB_OK && PLAFOND_CSC_TLB_OK)
                           {
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
                           }
                        }
                     }

                     // IC : Imprimante codeuse
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_IC) == 0)
                     {
                        T_BddCompIC IC_L;

                        /* Acquerir la rubrique IC */
                        Bdd->LireCompIC(IC_L);

                        // Si le composant est absent on ne change pas son état
                        if (IC_L.Etat != BDD_EC_ABSENT)
                        {
                           /* Le module ICS est en service */
                           IC_L.Etat = BDD_EC_EN_SERVICE;

                           /* Mettre a jour la rubrique ICS dans la Bdd */
                           Bdd->EcrireCompIC(IC_L);

                           // Dépositionner l'alarme equipement delivrance HS
                           if (SAM_TLB_OK && PLAFOND_TSC_TLB_OK)
                           {
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
                           }

                           if (ConfigDat->LireICS().Recu && RecuDisponible())
                           {
                              // Dépositionner l'alarme equipement delivrance recu HS
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
                           }
                        }
                     }

                     // IR : Imprimante reçu
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_IR) == 0)
                     {
                        T_BddCompIR IR_L;

                        /* Acquerir la rubrique IR */
                        Bdd->LireCompIR(IR_L);

                        // Si le composant est absent on ne change pas son état
                        if (IR_L.Etat != BDD_EC_ABSENT)
                        {
                           /* L'imprimante recu est en service */
                           IR_L.Etat = BDD_EC_EN_SERVICE;

                           /* Mettre a jour la rubrique dans la Bdd */
                           Bdd->EcrireCompIR(IR_L);


                           if (ConfigDat->LireIR().TP)
                           {
                              // Dépositionner l'alarme equipement delivrance HS
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
                           }

                           if (ConfigDat->LireIR().Recu)
                           {
                              // Dépositionner l'alarme equipement delivrance recu HS
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
                           }
                        }
                     }

                     // MB : Paiement carte bancaire
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_MB) == 0)
                     {
                        T_BddCompMB MB_L;

                        // Positionner l'alarme equipement paiement carte HS
                        TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_CB_HS);

                        /* Acquerir la rubrique MPC */
                        Bdd->LireCompMB(MB_L);

                        // Si le composant est absent on ne change pas son état
                        if (MB_L.Etat != BDD_EC_ABSENT)
                        {
                           /* Si telecommande en service */
                           if (Bdd->LireMHC())
                           {
                              /* Le module MPC reste en Hors Service */
                              MB_L.Etat = BDD_EC_HORS_SERVICE;
                           }
                           else
                           {
                              /* Le module MPC est en service */
                              MB_L.Etat = BDD_EC_EN_SERVICE;
                           }

                           /* Mettre a jour la rubrique MPC dans la Bdd */
                           Bdd->EcrireCompMB(MB_L);
                        }
                     }

                     // PB : Paiement par billets de banque
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_PB) == 0)
                     {
                        T_BddCompPB PB_L;

                        // Positionner l'alarme equipement paiement monnaie HS
                        TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_BILLET_HS);

                        /* Acquerir la rubrique Composant PP */
                        Bdd->LireCompPB(PB_L);

                        // Si le composant est absent on ne change pas son état
                        if (PB_L.Etat != BDD_EC_ABSENT)
                        {
                           // Si telecommande OK  
                           if (Bdd->LireMHB())
                           {
                              // Le composant PB reste en Hors Service
                              PB_L.Etat = BDD_EC_HORS_SERVICE;
                           }
                           else
                           {
                              // Le composant PB est en service
                              PB_L.Etat = BDD_EC_EN_SERVICE;
                           }

                           /* Mettre a jour la rubrique dans la Bdd */
                           Bdd->EcrireCompPB(PB_L);
                        }
                     }

                     // PP : Paiement par pièces de monnaie
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_PP) == 0)
                     {
                        T_BddCompPP PP_L;

                        // Positionner l'alarme equipement paiement monnaie HS
                        TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_PAIEMENT_MONNAIE_HS);

                        /* Acquerir la rubrique Composant PP */
                        Bdd->LireCompPP(PP_L);

                        // Si le composant est absent on ne change pas son état
                        if (PP_L.Etat != BDD_EC_ABSENT)
                        {
                           // Si telecommande OK  
                           if (Bdd->LireMHP())
                           {
                              /* Le composant PP reste en Hors Service */
                              PP_L.Etat = BDD_EC_HORS_SERVICE;
                           }
                           else
                           {
                              /* Le composant PP est en service */
                              PP_L.Etat = BDD_EC_EN_SERVICE;
                           }

                           /* Mettre a jour la rubrique dans la Bdd */
                           Bdd->EcrireCompPP(PP_L);
                        }
                     }

                     // RT : Rechargement télébillettique
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_RT) == 0)
                     {
                        T_BddCompRT RT_L;

                        /* Acquerir la rubrique RT */
                        Bdd->LireCompRT(RT_L);

                        // Si le composant est absent on ne change pas son état
                        if (RT_L.Etat != BDD_EC_ABSENT)
                        {
                           /* Le module MRT est en service */
                           RT_L.Etat = BDD_EC_EN_SERVICE;

                           /* Mettre a jour la rubrique dans la Bdd */
                           Bdd->EcrireCompRT(RT_L);

                           // Dépositionner l'alarme equipement rechargement HS
                           if (SAM_TLB_OK)
                           {
                              TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
                           }
                        }
                     }

                     // VD : VDE
                     else if (strcmp(ALC_L->Module[i].SSE, BDD_ALA_VD) == 0)
                     {
                        T_BddCompVD VD_L;

                        /* Acquerir la rubrique VD */
                        Bdd->LireCompVD(VD_L);

                        // Si le composant est absent on ne change pas son état
                        if (VD_L.Etat != BDD_EC_ABSENT)
                        {
                           /* Le module VD est en service */
                           VD_L.Etat = BDD_EC_EN_SERVICE;

                           /* Mettre a jour la rubrique VD */
                           Bdd->EcrireCompVD(VD_L);
                        }
                     }
                  }

                  if (ParAla_L->NivTraitement == BDD_ALA_NIV_EFFRACTION)
                  {
                     // Depositionner l'alarme equipement recu HS
                     TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EFFRACTION);
                  }

                  /* Mettre a jour la base de donnees */
                  Bdd->EcrireALC();

                  /* Enregistrer les parametres de l'alarme  */
                  EnregistrerAlarme(ParAla_L);

                  /* Mettre a jour la signalisation des Alarmes */
                  TraiterSignalisationAlarmes();
               }

               /* Si l'alarme detectee a disparue */
               else if (ALC_L->Module[i].COD[j] == BDD_ALA_DETECTEE_DISPARUE)
               {
                  /* Memoriser l'etat de l'alarme  */
                  ALC_L->Module[i].COD[j] = BDD_ALA_DISPARUE;

                  /* Mettre a jour la base de donnees */
                  Bdd->EcrireALC();
               }
            }
         }
      }
   }

   // Evaluer disponibilité du rendu monnaie
   TraiterDispoRendu();

}

/*****************************************************************************
* But de la fonction : Traiter l'absence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAbsenceAlarme(unsigned int Module_P,
   unsigned int SousModule_P,
   unsigned int NbAlarmes_P)

{
   unsigned int i;
   T_BddALC *ALC_L;

   /* Acquerir la rubrique Alarmes en cours */
   Bdd->LireALC(&ALC_L);

   /* Si le nombre de module */
   if (Module_P < BDD_NB_MODULES)
   {
      /* Si il y a une alarme presente */
      if (ALC_L->Module[Module_P].NAP != 0)
      {
         /* Parcourir la table des Alarmes du sous module */
         for (i = 0; i < NbAlarmes_P; i++)
         {
            /* Si une alarme est presente */
            if (ALC_L->Module[Module_P].COD[SousModule_P + i] == BDD_ALA_PRESENTE)
            {
               /* Memoriser l'etat de l'alarme */
               ALC_L->Module[Module_P].COD[SousModule_P + i] = BDD_ALA_DETECTEE_DISPARUE;

               /* Mettre a jour la base de donnees */
               Bdd->EcrireALC();
            }
         }
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter l'absence de x alarmes
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarme(unsigned int Module_P,
   unsigned int SousModule_P,
   unsigned int COD_P,
   unsigned int NbAlarmes_P)

{
   // Si une alarme est presente
   if (COD_P != 0)
   {
      // Traiter la presence de l'alarme
      TraiterPresenceAlarme(Module_P, SousModule_P, COD_P, NbAlarmes_P);
   }
   else
   {
      /* Traiter l'absence de l'alarme */
      TraiterAbsenceAlarme(Module_P, SousModule_P, NbAlarmes_P);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la présence si positionné ou l'absence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::Traiter1Alarme(unsigned int Module_P,
   unsigned int SousModule_P,
   unsigned int AlarmePresente_P)

{
   // Si une alarme est presente
   if (AlarmePresente_P != 0)
   {
      // Traiter la presence de l'alarme
      TraiterPresenceAlarme(Module_P, SousModule_P);
   }
   else
   {
      /* Traiter l'absence de l'alarme */
      TraiterAbsenceAlarme(Module_P, SousModule_P);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la presence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterPresenceAlarme(unsigned int Module_P,
   unsigned int COD_P)
{

   if (COD_P)
   {
      TraiterPresenceAlarme(Module_P, COD_P - 1, 1, 1);
   }

}

/*****************************************************************************
* But de la fonction : Traiter l'absence d'une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAbsenceAlarme(unsigned int Module_P,
   unsigned int SousModule_P)

{

   if (SousModule_P)
   {
      TraiterAbsenceAlarme(Module_P, SousModule_P, 1);
   }

}

/*****************************************************************************
* But de la fonction : Indique la presence d'une alarme specifiquement
*****************************************************************************/
unsigned int T_Etat::TesterPresenceAlarme(unsigned int Module_P, unsigned int Alarme_P)

{
   unsigned int Retour_L = 0;
   T_BddALC *ALC_L;
   T_ParAla *ParAla_L;

   /* Acquerir la rubrique des Alarmes en cours */
   Bdd->LireALC(&ALC_L);

   // tester le numero de module
   if (Module_P < BDD_NB_MODULES)
   {
      // tester le numero de l'alarme
      if (Alarme_P < BDD_NB_ALARMES)
      {
         /* Acquerir les parametres de l'alarme */
         ParAla_L = DonnerParAla(ALC_L->Module[Module_P].SSE, Alarme_P);

         if (ALC_L->Module[Module_P].COD[Alarme_P] == BDD_ALA_PRESENTE)
         {
            Retour_L = 1;
         }
         else
         {
            Retour_L = 0;
         }
      }
   }

   return Retour_L;
}


/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module paiement carte
* Parametres :
* ------------
*
* entree : Etat du module paiement carte
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesMpc(BcpDeviceState *Etat_P)

{
   T_BddETA BddEta_L;
   T_BddCompMB MB_L;

   /*****************************************************************************
   * Etape 1 : Mémoriser l'état du composant dans la base de données
   ******************************************************************************/

   /* Acquerir la rubrique ETA */
   Bdd->LireETA(BddEta_L);

   /* Si l'état de la transaction est discordant (Enregistrement reussi, annule ou echoue) */
   if (BddEta_L.TransactionEnAttenteEnregistrement == VRAI &&
      Etat_P->isTransactionPending == false)
   {
      /* Raz du détail du paiement bancaire */
      UpdateBcpInfoBDD();
   }

   /* Memoriser l'état de la transaction bancaire */
   BddEta_L.TransactionEnAttenteEnregistrement = Etat_P->isTransactionPending ? VRAI : FAUX;

   /* Enregistrer les donnees dans la rubrique */
   Bdd->EcrireETA(BddEta_L);

   /*****************************************************************************
   * Etape 2 : Traiter les alarmes concernées par le composant
   ******************************************************************************/

   // Traiter Alarme Transaction en attente d'enregistrement
   // Attention, celle ci peut être émise à cause d'une déconnexion !!!
   if (Etat_P->isTransactionPending)
   {
      TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_TRANSACTION_EN_ATTENTE);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_MB, COD_MB_TRANSACTION_EN_ATTENTE);
   }

   if (Etat_P->deviceStatus != deviceStatus_ko)
   {
      if (cardPayment)
      {
         /* recuperer information device sur changement d'etat */
         cardPayment->GetDeviceInformation();
      }

      TraiterAbsenceAlarme(BDD_INDEX_MB, ALA_DEFAUT_TELETRANS);

      /* Traiter Alarme Insertion carte */
      /*
      if (Etat_P->CartePresente)
      {
      TraiterPresenceAlarme (BDD_INDEX_MB, COD_MB_INSERTION_CARTE);
      }
      else
      {
      TraiterAbsenceAlarme (BDD_INDEX_MB, COD_MB_INSERTION_CARTE);
      }*/

      /* Traiter Alarme Carte bancaire oubliée */
      if (Etat_P->cardState == card_forgotten)
      {
         TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_CARTE_OUBLIEE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_MB, COD_MB_CARTE_OUBLIEE);
      }

      if (Etat_P->componentsState.reader != componentState_ok
         || ((Etat_P->componentsState.pinpad != componentState_ok) & (Etat_P->componentsState.pinpad != componentState_notUsed))
         || (Etat_P->noAppAvailable == bcp_yes) /* should be initialized with bcp_not_appliable when sim used*/
         || (Etat_P->isOfflineTransactionMemoryFull)
         || (Etat_P->firmwareUpToDate == bcp_no))
      {
         TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_PROBLEME_MODULE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_MB, COD_MB_PROBLEME_MODULE);
      }

      /* Traiter Alarme Défaut de connexion au système bancaire */
      if (Etat_P->isServerConnected == bcp_yes)
      {
         TraiterAbsenceAlarme(BDD_INDEX_MB, COD_MB_CONNEXION_SERVEUR);
      }
      else
      {
         TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_CONNEXION_SERVEUR);
      }

      // Alarme problème module bancaire sans contact
      if ((Etat_P->componentsState.contactlessTarget != componentState_ok) & (Etat_P->componentsState.contactlessTarget != componentState_notUsed))
      {
         TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_PB_LECTEUR_SANS_CONTACT);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_MB, COD_MB_PB_LECTEUR_SANS_CONTACT);
      }

      // On enregistre le numéro de série du module bancaire
      if (Etat_P->serialNumber[0] != '\0')
      {
         Bdd->LireCompMB(MB_L);

         strcpy(MB_L.MB.NumSerie, Etat_P->serialNumber);

         // Module externe : on ne récupère pas les numéros de version du module dans la BDD. On le force donc a 0;
         MB_L.MB.NbVersionLogicielles = 0;

         Bdd->EcrireCompMB(MB_L);
      }
   }
   else
   {
#ifdef _AVANT_ESFP_SUPERVISION
      // DG todo : Toulouse project does not support MB601. Connexion problem is mapped on device problem...
      TraiterPresenceAlarme(BDD_INDEX_MB, COD_MB_PROBLEME_MODULE);
#else
      TraiterPresenceAlarme(BDD_INDEX_MB, ALA_DEFAUT_TELETRANS);
#endif
   }
}

/*****************************************************************************
* But de la fonction : Traiter les alarmes fichiers
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesFichiers(T_ParEtat *Etat_P)

{
   unsigned int i;
   unsigned int NbFichiersAbsent_L = 0;
   unsigned int AlarmeFichiersIncomplets_L = FAUX;
   unsigned int AlarmeDefautSignature_L = FAUX;
   unsigned int AlarmeFichiersIncoherents_L = FAUX;

   /* Tant qu'il existe des fichiers paramètres */
   for (i = 0; i < Etat_P->NbFichiers; i++)
   {
      /* Selon le code panne du fichier en cours */
      switch (Etat_P->Fichier[i].CodePanne)
      {
         /* Fichier correcte */
      case CONFIG_OK:
         break;

         /* Fichier absent ou vide */
      case CONFIG_FICHIER_ABSENT:
         /* Incrémenter le nombre de fichiers absents */
         NbFichiersAbsent_L++;
         AlarmeFichiersIncomplets_L = VRAI;
         break;

      case CONFIG_FICHIER_VIDE:
         /* Mémoriser Alarme fichier paramètre incomplet */
         AlarmeFichiersIncomplets_L = VRAI;
         break;

         /* Erreur signature fichier */
      case CONFIG_ERR_SIGNATURE:
         /* Mémoriser Alarme erreur signature fichier paramètre */
         AlarmeDefautSignature_L = VRAI;
         break;

      default:
         /* Mémoriser Alarme erreur fichier incohérent */
         AlarmeFichiersIncoherents_L = VRAI;
         break;
      }
   }

   /* Si tous les fichiers sont absents */
   if (NbFichiersAbsent_L == Etat_P->NbFichiers)
   {
      /* Traiter alarme configuration absente */
      TraiterPresenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_ABSENTS);
   }
   else
   {
      /* Traiter absence alarme configuration absente */
      TraiterAbsenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_ABSENTS);

      /* Traiter Alarme fichier paramètre incomplet */
      if (AlarmeFichiersIncomplets_L == VRAI)
      {
         TraiterPresenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_INCOMPLETS);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_INCOMPLETS);
      }

      /* Traiter Alarme erreur signature fichier paramètre */
      if (AlarmeDefautSignature_L == VRAI)
      {
         TraiterPresenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_DEFAUT_SIGNATURE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_DEFAUT_SIGNATURE);
      }

      /* Traiter Alarme erreur fichier incohérent */
      if (AlarmeFichiersIncoherents_L == VRAI)
      {
         TraiterPresenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_INCOHERENT);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_FP, ALA_FP_PARAMETRES_INCOHERENT);
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter l'alarme du module impression / codage
* --------------------
* Parametres :
* ------------
*
* entree : Etat du module impression / codage
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesIcs(T_IcsEtat *Etat_P)

{
   unsigned int i;
   unsigned int j;
   unsigned int TousLesBobineauxVides_L = VRAI;

#ifdef _AVANT_ESFP_SUPERVISION
   unsigned int Index_L = BDD_INDEX_IC;
#else
   unsigned int Index_L = BDD_INDEX_ICS;
#endif

   /* Si le module ICS est connecte */
   if (Etat_P->Connectee)
   {
      bool SamEchange_L = false;
      bool DefautSAM_L = false;
      bool Plafond_Atteint_L = false;
      bool PlafondCSC_Atteint_L = false;
      bool PlafondTSC_Atteint_L = false;

      /* Acquerir la rubrique etat de l'imprimante */
      T_BddCompIC IC_L = Bdd->LireCompIC();

      /*****************************************************************************
      * Etape 1 : Mémoriser l'état du composant dans la base de données
      ******************************************************************************/

      /* Initialisation du numéro de série IC */
      memset(IC_L.IC.NumSerie, 0, sizeof IC_L.IC.NumSerie);

      /* Mettre a jour l'identifiant de l'imprimante */
      _snprintf(IC_L.IC.NumSerie, sizeof(IC_L.IC.NumSerie) - 1, "%u", Etat_P->Identification.NoSerie);

      /* Memoriser les version de l'Appli de Transport geres par l'API */
      IC_L.IC.NbVersionLogicielles = Etat_P->Identification.NbSoftwares;

      for (i = 0; i < IC_L.IC.NbVersionLogicielles; i++)
      {
         /* Numéro, version et amendement du logiciel */
         // On les classe pour que l'appli soit en 1er logiciel (on gère ICS et CCS)
         // 1er logiciel = l'appli ; 2ème logiciel boot ; 3ème logiciel FPGA
         if (Etat_P->Identification.Software[i].Type == 1)
         {
            IC_L.IC.Logiciel[0].NumRefLA = Etat_P->Identification.Software[i].Number;
            IC_L.IC.Logiciel[0].VersionLA = Etat_P->Identification.Software[i].Version[0];
            IC_L.IC.Logiciel[0].AmendementLA = Etat_P->Identification.Software[i].Version[1];
         }
         else if (Etat_P->Identification.Software[i].Type == 0)
         {
            IC_L.IC.Logiciel[1].NumRefLA = Etat_P->Identification.Software[i].Number;
            IC_L.IC.Logiciel[1].VersionLA = Etat_P->Identification.Software[i].Version[0];
            IC_L.IC.Logiciel[1].AmendementLA = Etat_P->Identification.Software[i].Version[1];
         }
         else if (Etat_P->Identification.Software[i].Type == 2)
         {
            IC_L.IC.Logiciel[2].NumRefLA = Etat_P->Identification.Software[i].Number;
            IC_L.IC.Logiciel[2].VersionLA = Etat_P->Identification.Software[i].Version[0];
            IC_L.IC.Logiciel[2].AmendementLA = Etat_P->Identification.Software[i].Version[1];
         }
      }

      /* Memoriser le numero de serie de l'ICS */
      NoIcs = Etat_P->Identification.NoSerie;

      /* Memoriser le dernier compte-rendu de fabrication */
      IC_L.CrFabrication.CompteRendu = Etat_P->CrFabrication.CompteRendu;
      IC_L.CrFabrication.NbBillets = Etat_P->CrFabrication.NbBillets;

      for (i = 0; i < BDD_NB_MAX_BILLETS; i++)
      {
         IC_L.CrFabrication.Billet[i] = Etat_P->CrFabrication.Billet[i];
      }

      /* Traiter etat des bobibeaux */
      for (i = 0; i < ConfigDat->LireICS().NbStocks; i++)
      {
         switch (Etat_P->Bobineau[i])
         {
         case ICS_BOBINEAU_CORRECT:
            IC_L.STOCK[i].EtatStock = BDD_STOCK_NORMAL;
            break;

         case ICS_BOBINEAU_PRESQUE_VIDE:
            IC_L.STOCK[i].EtatStock = BDD_STOCK_PRESQUE_VIDE;
            break;

         case ICS_BOBINEAU_VIDE:
            IC_L.STOCK[i].EtatStock = BDD_STOCK_VIDE;
            break;
         }
      }

      // Traitement des SAMs
      T_ApiTlbEtatSAMs *Etat_L = &Etat_P->EtatSAMs;

      // Mémoriser les SAM du module Mrt */
      for (i = 0, IC_L.NbSAMs = 0; i < Etat_L->NbSAMs && i < BDD_NB_SAM; i++)
      {
         // Numéro de série du SAM
         unsigned __int64 NoSAM_L = Etat_L->Sam[i].NumeroSerieSam[11];
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[10] * (unsigned __int64)0x100;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[9] * (unsigned __int64)0x10000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[8] * (unsigned __int64)0x1000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[7] * (unsigned __int64)0x100000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[6] * (unsigned __int64)0x10000000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[5] * (unsigned __int64)0x1000000000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[4] * (unsigned __int64)0x100000000000000;

         if (IC_L.SAM[i].NumSerie != NoSAM_L)
         {
            SamEchange_L = true;
         }

         IC_L.SAM[i].NumSerie = NoSAM_L;

         // Code unicite : identifiant type de SAM
         IC_L.SAM[i].NumUnicite = Etat_L->Sam[i].CodeUnicite;

         // Type Altlas du SAM
         IC_L.SAM[i].NumTechno = Etat_L->Sam[i].TypeSAM;

         // Identifiant (partner Calypso) = T2 Provider
         IC_L.SAM[i].Provider[0] = Etat_L->Sam[i].Provider[0];
         IC_L.SAM[i].Provider[1] = Etat_L->Sam[i].Provider[1];

         // VRAI si sélection du SAM OK (y compris si SAM verrouillé)
         IC_L.SAM[i].DialogueOK = Etat_L->Sam[i].DialogueOK ? true : false;

         // VRAI indique que le SAM est dévérouillé ou non vérouillé
         IC_L.SAM[i].Deverouille = Etat_L->Sam[i].Deverouille ? true : false;

         // Pour toutes les clés du SAM
         for (j = 0, IC_L.SAM[i].NbCles = 0; j < APITLB_NB_CLES_PLAFONNEMENT_SAM && IC_L.SAM[i].NbCles < BDD_NB_CLE_SAM; j++)
         {
            // Présence de la clé dans le SAM (TRUE/FALSE)
            if (Etat_L->Sam[i].Plafonnement[j].PresenceCle)
            {
               // Type de clé : ATLAS_SAM_CLE_*
               IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Type = Etat_L->Sam[i].Plafonnement[j].TypeCle;

               // Plafond courant (0 pour non plafonné)
               IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Plafond = Etat_L->Sam[i].Plafonnement[j].Plafond;

               // Compteur courant
               IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Compteur = Etat_L->Sam[i].Plafonnement[j].Compteur;

               if (IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Plafond != 0)
               {
                  if (IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Compteur >= IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Plafond)
                  {
                     Plafond_Atteint_L = true;
                     if (IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Type == SAM_TYPE_CLE_CSC)
                     {
                        PlafondCSC_Atteint_L = true;
                     }
                     else if (IC_L.SAM[i].Cle[IC_L.SAM[i].NbCles].Type == SAM_TYPE_CLE_TSC)
                     {
                        PlafondTSC_Atteint_L = true;
                     }
                  }
               }

               // Nombre de clés
               IC_L.SAM[i].NbCles += 1;
            }
         }

         // Nombre de SAMs
         IC_L.NbSAMs += 1;

         if (!IC_L.SAM[i].DialogueOK || !IC_L.SAM[i].Deverouille || IC_L.SAM[i].NbCles == 0)
         {
            DefautSAM_L = true;
         }
      }

      /* Mettre a jour la rubrique etat de l'imprimante */
      Bdd->EcrireCompIC(IC_L);

      // En ICS simu on veut pouvoir vendre ! (bug du SIM à corriger mais pas la place sur mon PC d'avoir le code du simu...)
      if (DefautSAM_L && ConfigDat->LireICS().Type == T_ConfigICS::SIM)
      {
         DefautSAM_L = false;
      }

      // Etat du SAM
      SAM_TLB_OK = (IC_L.NbSAMs != 0 && !DefautSAM_L);
      PLAFOND_CSC_TLB_OK = !PlafondCSC_Atteint_L;
      PLAFOND_TSC_TLB_OK = !PlafondTSC_Atteint_L;

      /*****************************************************************************
      * Etape 2 : Traiter les alarmes concernées par le composant
      ******************************************************************************/

      /* Supprimer l'alarme défaut télétransmission */
      TraiterAbsenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);

      // Si l'ICS fait rechargement TLB (par ex BCS400)
      if (ConfigDat->LireICS().CSC)
      {
         // On ne modifie les alarmes que si on a initialisé TLB
         if (TLB_Initialise)
         {
            // Periph de rechargement TLB ?
            if (SAM_TLB_OK && (PLAFOND_CSC_TLB_OK || PLAFOND_TSC_TLB_OK))
            {
               // Rechargement TLB OK
               TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
            }
            else
            {
               // Rechargement TLB KO
               TraiterPresenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
            }

            // SAM RKL - Periph de délivrance ?
            if (SAM_TLB_OK && PLAFOND_TSC_TLB_OK)
            {
               // Delivrance OK
               TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
            }
            else
            {
               // Delivrance KO
               if (!ConfigDat->LireIR().TP || (ConfigDat->LireIR().TP && Bdd->LireCompIR().Etat != BDD_EC_EN_SERVICE))
               {
                  TraiterPresenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
               }
            }

            // Si nombre de SAM ok et 
            if (IC_L.NbSAMs != 0)
            {
               TraiterAbsenceAlarme(Index_L, COD_ICS_SAM_ABSENT);

               /* Si un SAM a été echangé */
               if (SamEchange_L == VRAI)
               {
                  /* Alarme informative SAM échangé */
                  TraiterPresenceAlarme(Index_L, COD_ICS_SAM_ECHANGE);
                  TraiterAbsenceAlarme(Index_L, COD_ICS_SAM_ECHANGE);

                  /* Enregistrer Activité "Inventaire des SAMs d'un equipement" */
#ifdef _AVANT_ESFP_SUPERVISION
                  Act->Enregistreres();
#else
                  Act->Enregistreres();
#endif
               }

               // Defaut SAM ?
               if (!DefautSAM_L)
               {
                  // SAM OK
                  TraiterAbsenceAlarme(Index_L, COD_ICS_DEFAUT_SAM);
               }
               else
               {
                  // SAM NOK
                  TraiterPresenceAlarme(Index_L, COD_ICS_DEFAUT_SAM);
               }
            }
            else
            {
               TraiterPresenceAlarme(Index_L, COD_ICS_SAM_ABSENT);
            }

            /* Traiter Alarme Défaut liaison serveur de replafonnement des SAMs */
            if (Etat_P->EtatRePlafonnement.DialogueOK == FAUX)
            {
               /* Traiter l'apparition alarme associée */
               TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
            }
            else
            {
               /* Traiter la disparition de l'alarme */
               TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
            }

            /* Traiter Alarme replafonnement des SAMs refusé */
            if (Etat_P->EtatRePlafonnement.Refuse == VRAI)
            {
               /* Traiter l'apparition alarme associée */
               TraiterPresenceAlarme(Index_L, COD_ICS_REPLAFONNEMENT_REFUSE);
            }
            else
            {
               /* Traiter la disparition de l'alarme */
               TraiterAbsenceAlarme(Index_L, COD_ICS_REPLAFONNEMENT_REFUSE);
            }

            /* Traiter alarme détection plafond sam atteint */
            if (Etat_P->EtatRePlafonnement.PlafondAtteint == VRAI || Plafond_Atteint_L)
            {
               /* Traiter l'alarme */
               TraiterPresenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_ATTEINT);
            }
            else
            {
               /* Traiter l'absence de l'alarme */
               TraiterAbsenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_ATTEINT);
            }

            /* Traiter alarme détection plafond sam presque atteint */
            if (Etat_P->EtatRePlafonnement.PlafondPresqueAtteint == VRAI)
            {
               /* Traiter l'alarme */
               TraiterPresenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_PRESQUE_ATTEINT);
            }
            else
            {
               /* Traiter l'absence de l'alarme */
               TraiterAbsenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_PRESQUE_ATTEINT);
            }
         }

      }
      else
      {
         // L'ICS est sans CSC, il ne peut y avoir d'alarme liées à la partie CSC !!!
         TraiterAbsenceAlarme(Index_L, COD_ICS_SAM_ABSENT);
         TraiterAbsenceAlarme(Index_L, COD_ICS_DEFAUT_SAM);
         TraiterAbsenceAlarme(Index_L, COD_ICS_REPLAFONNEMENT_REFUSE);
         TraiterAbsenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_ATTEINT);
         TraiterAbsenceAlarme(Index_L, COD_ICS_PLAFOND_SAM_PRESQUE_ATTEINT);
         // Pas à gérer ici (ex MRT + IC CCS)
                  //TraiterAbsenceAlarme (BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
      }

      if (ConfigDat->LireICS().Recu)
      {
         if (RecuDisponible())
         {
            // Dépositionner l'alarme equipement delivrance recu HS
            TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
         }
         else
         {
            // Positionner l'alarme equipement recu HS
            TraiterPresenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
         }
      }
   }
   else
   {
      /* Apparition l'alarme défaut télétransmission */
      TraiterPresenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);
   }

   if (Etat_P->NbAlarmes != 0)
   {
      /* Traiter les alarmes ICS */
      for (i = 0; i < Etat_P->NbAlarmes; i++)
      {
         /* Traiter apparition alarme */
         TraiterAlarme(Index_L, ALA_IMPRIMANTE_IC, Etat_P->CodeAlarme[i], NB_ALA_IMPRIMANTE_IC);
      }
   }
   else
   {
      /* Traiter disparition des alarmes ICS */
      TraiterAlarme(Index_L, ALA_IMPRIMANTE_IC, 0, NB_ALA_IMPRIMANTE_IC);
   }

   /* Traiter alarmes des bobineaux */
   for (i = 0; i < PARICS_NB_MAX_BOBINEAUX; i++)
   {
      if (i < ConfigDat->LireICS().NbStocks)
      {
         switch (Etat_P->Bobineau[i])
         {
         case ICS_BOBINEAU_PRESQUE_VIDE:
            TraiterPresenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
            TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
            TousLesBobineauxVides_L = FAUX;
            break;

         case ICS_BOBINEAU_VIDE:
            TraiterPresenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
            TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
            break;

         default:
            TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
            TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
            TousLesBobineauxVides_L = FAUX;
            break;
         }
      }
      else
      {
         // Pas d'alarme sur les bobineaux non présents
         TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
         TraiterAbsenceAlarme(Index_L, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
      }
   }

   /* Traiter alarme bobineaux vides */
   if (TousLesBobineauxVides_L)
   {
      TraiterPresenceAlarme(Index_L, ALA_TOUS_LES_BOBINEAUX_VIDES);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, ALA_TOUS_LES_BOBINEAUX_VIDES);
   }

}

/*****************************************************************************
* But de la fonction : Traiter les alarmes du module MRT
* --------------------
* Parametres :
* ------------
*
* entree : Etat du module Mrt
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesMrt(T_MrtEtat *Etat_P)

{
   unsigned int i;
   unsigned int j;

   /* Si le module MRT est connecte */
   if (Etat_P->Connecte)
   {
      bool SamEchange_L = false;
      bool DefautSAM_L = false;
      bool Plafond_Atteint_L = false;
      bool PlafondCSC_Atteint_L = false;
      bool PlafondTSC_Atteint_L = false;

      /* Acquerir la rubrique etat du module rechargement telebillettique */
      T_BddCompRT RT_L = Bdd->LireCompRT();

      /*****************************************************************************
      * Etape 1 : Mémoriser l'état du composant dans la base de données
      ******************************************************************************/

      /* Numéro de série */
      sprintf(RT_L.RT.NumSerie, "%u", Etat_P->CscIdentification.CscSerialNumber);

      /* Nombre de logiciels */
      RT_L.RT.NbVersionLogicielles = Etat_P->CscIdentification.NbSoftwares;

      for (i = 0; i < Etat_P->CscIdentification.NbSoftwares; i++)
      {
         /* Numéro, version et amendement du logiciel */
         // On les classe pour que, le CSC420 ait l'appli en 1er logiciel et n'a pas de FPGA
         // 1er logiciel = l'appli ; 2ème logiciel boot ; 3ème logiciel FPGA
         if (Etat_P->CscIdentification.Software[i].Type == 1) // appli
         {
            RT_L.RT.Logiciel[0].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            RT_L.RT.Logiciel[0].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            RT_L.RT.Logiciel[0].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
         else if (Etat_P->CscIdentification.Software[i].Type == 0) // boot
         {
            RT_L.RT.Logiciel[1].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            RT_L.RT.Logiciel[1].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            RT_L.RT.Logiciel[1].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
         else if (Etat_P->CscIdentification.Software[i].Type == 2) // FPGA
         {
            RT_L.RT.Logiciel[2].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            RT_L.RT.Logiciel[2].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            RT_L.RT.Logiciel[2].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
      }

      // Traitement des SAMs
      T_ApiTlbEtatSAMs *EtatSAM_L = &Etat_P->EtatSAMs;

      // Mémoriser les SAM du module Mrt */
      for (i = 0, RT_L.NbSAMs = 0; i < EtatSAM_L->NbSAMs && i < BDD_NB_SAM; i++)
      {
         // Numéro de série du SAM
         unsigned __int64 NoSAM_L = EtatSAM_L->Sam[i].NumeroSerieSam[11];
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[10] * (unsigned __int64)0x100;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[9] * (unsigned __int64)0x10000;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[8] * (unsigned __int64)0x1000000;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[7] * (unsigned __int64)0x100000000;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[6] * (unsigned __int64)0x10000000000;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[5] * (unsigned __int64)0x1000000000000;
         NoSAM_L += EtatSAM_L->Sam[i].NumeroSerieSam[4] * (unsigned __int64)0x100000000000000;

         if (RT_L.SAM[i].NumSerie != NoSAM_L)
         {
            SamEchange_L = true;
         }

         RT_L.SAM[i].NumSerie = NoSAM_L;

         // Code unicite : identifiant type de SAM
         RT_L.SAM[i].NumUnicite = EtatSAM_L->Sam[i].CodeUnicite;

         // Type Altlas du SAM
         RT_L.SAM[i].NumTechno = EtatSAM_L->Sam[i].TypeSAM;

         // Identifiant (partner Calypso) = T2 Provider
         RT_L.SAM[i].Provider[0] = EtatSAM_L->Sam[i].Provider[0];
         RT_L.SAM[i].Provider[1] = EtatSAM_L->Sam[i].Provider[1];

         // VRAI si sélection du SAM OK (y compris si SAM verrouillé)
         RT_L.SAM[i].DialogueOK = EtatSAM_L->Sam[i].DialogueOK ? true : false;

         // VRAI indique que le SAM est dévérouillé ou non vérouillé
         RT_L.SAM[i].Deverouille = EtatSAM_L->Sam[i].Deverouille ? true : false;

         // Initialisation nb clés (il sera incrémenté dans la boucle ci-dessous)
         RT_L.SAM[i].NbCles = 0;

         // Pour toutes les clés du SAM
         for (j = 0, RT_L.SAM[i].NbCles = 0; j < APITLB_NB_CLES_PLAFONNEMENT_SAM && RT_L.SAM[i].NbCles < BDD_NB_CLE_SAM; j++)
         {
            // Présence de la clé dans le SAM (TRUE/FALSE)
            if (EtatSAM_L->Sam[i].Plafonnement[j].PresenceCle)
            {
               // Type de clé : ATLAS_SAM_CLE_*
               RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Type = EtatSAM_L->Sam[i].Plafonnement[j].TypeCle;

               // Plafond courant (0 pour non plafonné)
               RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Plafond = EtatSAM_L->Sam[i].Plafonnement[j].Plafond;

               // Compteur courant
               RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Compteur = EtatSAM_L->Sam[i].Plafonnement[j].Compteur;

               if (RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Plafond != 0)
               {
                  /*if (RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Compteur >= RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Plafond)
                  {
                     PlafondAtteint_L = true;
                  }*/
                  if (RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Compteur >= RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Plafond)
                  {
                     Plafond_Atteint_L = true;
                     if (RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Type == SAM_TYPE_CLE_CSC)
                     {
                        PlafondCSC_Atteint_L = true;
                     }
                     else if (RT_L.SAM[i].Cle[RT_L.SAM[i].NbCles].Type == SAM_TYPE_CLE_TSC)
                     {
                        PlafondTSC_Atteint_L = true;
                     }
                  }
               }

               // Nombre de clés
               RT_L.SAM[i].NbCles += 1;
            }
         }

         // Nombre de SAMs
         RT_L.NbSAMs += 1;

         if (!RT_L.SAM[i].DialogueOK || !RT_L.SAM[i].Deverouille || RT_L.SAM[i].NbCles == 0)
         {
            DefautSAM_L = true;
         }
      }

      /* Mettre a jour la rubrique etat du module rechargement telebillettique */
      Bdd->EcrireCompRT(RT_L);

      // Etat du SAM
      SAM_TLB_OK = (RT_L.NbSAMs != 0 && !DefautSAM_L);
      PLAFOND_CSC_TLB_OK = !PlafondCSC_Atteint_L;
      PLAFOND_TSC_TLB_OK = !PlafondTSC_Atteint_L;

      /*****************************************************************************
      * Etape 2 : Traiter les alarmes concernées par le composant
      ******************************************************************************/

      /* Supprimer l'alarme défaut télétransmission */
      TraiterAbsenceAlarme(BDD_INDEX_RT, ALA_DEFAUT_TELETRANS);

      // Pendant l'init, les infos ne sont pas pertinantes
      if (Etat_P->InitTlbEnCours == FAUX)
      {
         // TODO : cela sert il ? sachant que cette alarme est gérée dans tlb.cpp
                  /* Traiter Alarme Carte oubliée */
         if (Etat_P->CartePresente == FAUX)
         {
            /* Traiter la disparition alarme associee a oublie CSC */
            TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_CARTE_OUBLIEE);
         }

         // Periph de rechargement TLB ?
         if (SAM_TLB_OK && (PLAFOND_CSC_TLB_OK || PLAFOND_TSC_TLB_OK))
         {
            // Rechargement TLB OK
            TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
         }
         else
         {
            // Rechargement TLB KO
            TraiterPresenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
         }

         // Periph de délivrance ?
         if (ConfigDat->LireICS().Present || ConfigDat->LireDC().Present)
         {
            if (SAM_TLB_OK && PLAFOND_TSC_TLB_OK)
            {
               // Delivrance OK
               TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
            }
            else
            {
               // Delivrance KO
               if (!ConfigDat->LireIR().TP || (ConfigDat->LireIR().TP && Bdd->LireCompIR().Etat != BDD_EC_EN_SERVICE))
               {
                  TraiterPresenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
               }
            }
         }
         else
         {
            TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
         }

         /* Si nombre de SAM ok */
         if (RT_L.NbSAMs != 0)
         {
            TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_SAM_ABSENT);

            if (SamEchange_L)
            {
               /* Alarme informative SAM échangé */
               TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_SAM_ECHANGE);
               TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_SAM_ECHANGE);

               /* Enregistrer l'inventaire des SAMs d'un equipement */
               Act->Enregistreres();
            }

            if (!DefautSAM_L)
            {
               // SAM OK
               TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_DEFAUT_SAM);
            }
            else
            {
               // SAM NOK
               TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_DEFAUT_SAM);
            }
         }
         else
         {
            TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_SAM_ABSENT);
         }

         /* Traiter Alarme Défaut liaison serveur de replafonnement des SAMs */
         if (Etat_P->EtatRePlafonnement.DialogueOK == FAUX)
         {
            /* Traiter l'apparition alarme associée */
            TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
         }
         else
         {
            /* Traiter la disparition de l'alarme */
            TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
         }

         /* Traiter Alarme replafonnement des SAMs refusé */
         if (Etat_P->EtatRePlafonnement.Refuse == VRAI)
         {
            /* Traiter l'apparition alarme associée */
            TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_REPLAFONNEMENT_REFUSE);
         }
         else
         {
            /* Traiter la disparition de l'alarme */
            TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_REPLAFONNEMENT_REFUSE);
         }

         /* Traiter alarme détection plafond sam atteint */
         if (Etat_P->EtatRePlafonnement.PlafondAtteint == VRAI || Plafond_Atteint_L)
         {
            /* Traiter l'alarme */
            TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_PLAFOND_SAM_ATTEINT);
         }
         else
         {
            /* Traiter l'absence de l'alarme */
            TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_PLAFOND_SAM_ATTEINT);
         }

         /* Traiter alarme détection plafond sam presque atteint */
         if (Etat_P->EtatRePlafonnement.PlafondPresqueAtteint == VRAI)
         {
            /* Traiter l'alarme */
            TraiterPresenceAlarme(BDD_INDEX_RT, COD_MRT_PLAFOND_SAM_PRESQUE_ATTEINT);
            PlafondPresqueAtteint = true;
         }
         else
         {
            /* Traiter l'absence de l'alarme */
            TraiterAbsenceAlarme(BDD_INDEX_RT, COD_MRT_PLAFOND_SAM_PRESQUE_ATTEINT);
            PlafondPresqueAtteint = false;
         }

         // Devrait etre a 0, voir s'il faut faire un traitement si ce n'est pas le cas
         if (Etat_P->CrInit != 1)
         {
            Trace->Tracer("ATTENTION CrInit=%d\n", Etat_P->CrInit);
         }
      }
   }
   else
   {
      /* Traiter l'apparition alarme associee */
      TraiterPresenceAlarme(BDD_INDEX_RT, ALA_DEFAUT_TELETRANS);
   }


}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du monnayeur
* Parametres :
* ------------
*
* entree : Etat du monnayeur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesChs(T_ChsState *Etat_P)

{
   unsigned int i;
   T_BddCompPP PP_L;

#ifdef _AVANT_ESFP_SUPERVISION
   unsigned int Index_L = BDD_INDEX_PP;
#else
   unsigned int Index_L = BDD_INDEX_MNY;
#endif

   /*****************************************************************************
   * Etape 1 : Mémoriser l'état du composant dans la base de données
   ******************************************************************************/

   /*Récupérer la rubrique Paiement par pièce */
   Bdd->LireCompPP(PP_L);

   if (!ConfigDat->LirePB().BNR)
   {
      /* Selon l'état du rendu monnaie */
      if (Etat_P->Change == CHS_GIVES_CHANGE)
      {
         // MAJ Etat rendu monnaie
         PP_L.EtatRendLaMonnaie = 'O';
      }
      else
      {
         // MAJ Etat non rendu monnaie
         // dans tous les autres cas (CHS_DOES_NOT_GIVE_CHANGE et CHS_RETURN_CHANGE_SNAG)
         PP_L.EtatRendLaMonnaie = 'N';
      }
   }

   /* Mémoriser les données dans la base */
   Bdd->EcrireCompPP(PP_L);

   /* Ecrire les états alarmes du CHS */
   Bdd->EcrireChsState(Etat_P);

   /*****************************************************************************
   * Etape 2 : Traiter les alarmes concernées par le composant
   ******************************************************************************/

   /* Traiter télétransmission Monnayeur */
   if (Etat_P->Connected)
   {
#ifdef _AVANT_ESFP_SUPERVISION

      TraiterAlarme(Index_L, ALA_CONNEXION_CHS, ALA_CONNECTE, NB_ALA_CONNEXION_CHS);
#elif
      /* Monnayeur connecté : descendre alarme defaut transmission */
      TraiterAbsenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);
#endif
   }
   else
   {
#ifdef _AVANT_ESFP_SUPERVISION
      TraiterAlarme(Index_L, ALA_CONNEXION_CHS, ALA_DECONNECTE, NB_ALA_CONNEXION_CHS);
#elif
      /* Monnayeur déconnecté : monter alarme defaut transmission */
      TraiterPresenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);
#endif
   }

   /* Selon l'état du rendu monnaie */
   switch (Etat_P->Change)
   {
      /* MAJ Etat rendu monnaie */
   case CHS_GIVES_CHANGE:
      /* Traiter alarme rendu monnaie */
      if (!ConfigDat->LirePB().BNR)
      {
#ifdef _AVANT_ESFP_SUPERVISION
         {
            TraiterAbsenceAlarme(Index_L, ALA_RENDU_MONNAIE);
         }
#else
         {
            TraiterAbsenceAlarme(BDD_INDEX_EQ, ALA_RENDU_MONNAIE);
         }
#endif
      }

      TraiterAbsenceAlarme(Index_L, ALA_DEFAUT_RENDU_MONNAIE);
      break;

      /* MAJ Etat non rendu monnaie */
   case CHS_DOES_NOT_GIVE_CHANGE:
      /* Traiter alarme rendu monnaie */
      if (!ConfigDat->LirePB().BNR)
      {
#ifdef _AVANT_ESFP_SUPERVISION
         {
            TraiterPresenceAlarme(Index_L, ALA_RENDU_MONNAIE);
         }
#else
         {
            TraiterPresenceAlarme(BDD_INDEX_EQ, ALA_RENDU_MONNAIE);
         }
#endif
      }

      TraiterAbsenceAlarme(Index_L, ALA_DEFAUT_RENDU_MONNAIE);
      break;

      /* défaut rendu monnaie */
   case CHS_RETURN_CHANGE_SNAG:
      /* Traiter alarme rendu monnaie */
      TraiterPresenceAlarme(Index_L, ALA_DEFAUT_RENDU_MONNAIE);
      break;

   default:
      break;
   }

   // Evaluer disponibilité du rendu monnaie (si BNR)
   TraiterDispoRendu();

   /* Pour chaque recycleur */
   for (i = 0; i < CHS_MAX_RCYS; i++)
   {
      /* Si le recycleur est déclaré */
      if (Etat_P->Rcys[i] != CHS_HOPPER_NOT_DECLARED)
      {
         /* Acquitter les anciennes alarmes */
         TraiterAbsenceAlarme(Index_L, ALA_RECYCLEUR + i * NB_ALA_RECYCLEUR, NB_ALA_RECYCLEUR);

         /* Traiter alarme recycleur */
         // On ignore les alarmes recycleurs vide, preque vide et plein
         if (Etat_P->Rcys[i] != CHS_HOPPER_FULL
            && Etat_P->Rcys[i] != CHS_HOPPER_NEARLY_EMPTY
            && Etat_P->Rcys[i] != CHS_HOPPER_EMPTY)
         {
            TraiterAlarme(Index_L, ALA_RECYCLEUR + i * NB_ALA_RECYCLEUR, Etat_P->Rcys[i], NB_ALA_RECYCLEUR);
         }
      }
   }

   for (i = 0; i < CHS_MAX_RSVS; i++)
   {
      /* Si la réserve est déclarée */
      if (Etat_P->Rsvs[i] != CHS_HOPPER_NOT_DECLARED)
      {
         /* Acquitter les anciennes alarmes */
         TraiterAbsenceAlarme(Index_L, ALA_RESERVE + i * NB_ALA_RESERVE, NB_ALA_RESERVE);

         /* Traiter alarme réserve */
         // On ignore les alarmes réserves pleines pour ne pas afficher le monnayeur en jaune
         if (Etat_P->Rsvs[i] != CHS_HOPPER_FULL)
         {
            TraiterAlarme(Index_L, ALA_RESERVE + i * NB_ALA_RESERVE, Etat_P->Rsvs[i], NB_ALA_RESERVE);
         }
      }
      else
      {
         // Si la reserve n'est pas déclarée, on descend l'alarme reserve absente
         TraiterAbsenceAlarme(Index_L, ALA_RESERVE + CHS_HOPPER_MISSING + i * NB_ALA_RESERVE);
      }
   }

#ifdef _AVANT_ESFP_SUPERVISION
   /* Pour chaque recycleur */
   for (i = 0; i < CHS_MAX_RCYS; i++)
   {
      /* Si le recycleur est déclaré */
      if (Etat_P->Rcys[i] != CHS_HOPPER_NOT_DECLARED)
      {
         /* Acquitter les anciennes alarmes */
         TraiterAbsenceAlarme(Index_L, ALA_RECYCLEUR + i * NB_ALA_RECYCLEUR, NB_ALA_RECYCLEUR);

         /* Traiter alarme recycleur */
         TraiterAlarme(Index_L, ALA_RECYCLEUR + i * NB_ALA_RECYCLEUR, Etat_P->Rcys[i], NB_ALA_RECYCLEUR);

      }
   }

   /* Si la réserve est déclarée sur la 3° position (Toulouse) */
   if (Etat_P->Rsvs[ETAT_NO_RESERVE_UTILISE] != CHS_HOPPER_NOT_DECLARED)
   {
      /* Acquitter les anciennes alarmes */
      TraiterAbsenceAlarme(Index_L, ALA_RESERVE, NB_ALA_RESERVE);

      /* Traiter alarme réserve 3* position */
      TraiterAlarme(Index_L, ALA_RESERVE, Etat_P->Rsvs[ETAT_NO_RESERVE_UTILISE], NB_ALA_RESERVE);
   }
#endif

   /* Si la caisse a été corectement changée */
   if (Etat_P->CashBox != CHS_CASH_BOX_WELL_CHANGED)
   {
      /* Traiter alarme caisse */
      TraiterAlarme(Index_L, ALA_CAISSE, Etat_P->CashBox, NB_ALA_CAISSE);
   }

   /* Traiter alarme sélecteur suivant son état */
   TraiterAlarme(Index_L, ALA_DISPENSATEUR, Etat_P->Selector, NB_ALA_DISPENSATEUR);

   /* Traiter alarme tri suivant le capteur */
   TraiterAlarme(Index_L, ALA_MODULE_TRI, Etat_P->Sensor, NB_ALA_MODULE_TRI);

}

/*****************************************************************************
* But de la fonction : Traiter les alarmes monnayeur
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesMny(T_MnyEtat *Etat_P)

{
   unsigned int i;
   T_BddCompPP PP_L;

#ifdef _AVANT_ESFP_SUPERVISION
   unsigned int Index_L = BDD_INDEX_RS22;
#else
   unsigned int Index_L = BDD_INDEX_MNY;
#endif

   /*****************************************************************************
   * Etape 1 : Mémoriser l'état du composant dans la base de données
   ******************************************************************************/

   /*Récupérer la rubrique Paiement par pièce */
   Bdd->LireCompPP(PP_L);

   if (!ConfigDat->LirePB().BNR)
   {
      /* Selon l'état du rendu monnaie */
      if (Etat_P->RendLaMonnaie)
      {
         PP_L.EtatRendLaMonnaie = 'O';
      }
      else
      {
         PP_L.EtatRendLaMonnaie = 'N';
      }
   }

   /* Selon l'état du rendu monnaie */
   /* Mémoriser les données dans la base */
   Bdd->EcrireCompPP(PP_L);

   /*****************************************************************************
   * Etape 2 : Traiter les alarmes concernées par le composant
   ******************************************************************************/

   // traiter alarme connexion avec mny
   if (Etat_P->Connecte)
   {
      TraiterAbsenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);
   }
   else
   {
      TraiterPresenceAlarme(Index_L, ALA_DEFAUT_TELETRANS);
   }

   // traiter alarme tambour
   for (i = 0; i < MNY_MAX_TAMBOUR; i++)
   {
      // traiter code alarme ascom monetel
      TraiterAlarme(Index_L,
         ALA_RS22_TAMBOUR + i * ALA_RS22_NB_TAMBOUR,
         Etat_P->Tambour[i],
         ALA_RS22_NB_TAMBOUR);
   }

   // traiter alarme bol
   for (i = 0; i < MNY_MAX_BOL; i++)
   {
      TraiterAlarme(Index_L,
         ALA_RS22_BOL + i * ALA_RS22_NB_BOL,
         Etat_P->Bol[i],
         ALA_RS22_NB_BOL);
   }

   // traiter alarme caisse
   TraiterAlarme(Index_L, ALA_RS22_CAISSE, Etat_P->Caisse, ALA_RS22_NB_CAISSE);

   // traiter alarme dispensateur
   TraiterAlarme(Index_L, ALA_RS22_DISPENSATEUR, Etat_P->Dispensateur, ALA_RS22_NB_DISPENSATEUR);

   // traiter alarme module tri
   TraiterAlarme(Index_L, ALA_RS22_MODULE_TRI, Etat_P->Tri, ALA_RS22_NB_MODULE_TRI);

   // alarme rendu monnaie hors-service
   if (!ConfigDat->LirePB().BNR)
   {
      if (Etat_P->RendLaMonnaie)
      {
#ifdef _AVANT_ESFP_SUPERVISION
         {
            TraiterAbsenceAlarme(Index_L, ALA_RENDU_MONNAIE);
         }
#else
         {
            TraiterAbsenceAlarme(BDD_INDEX_EQ, ALA_RENDU_MONNAIE);
         }
#endif
      }
      else
      {
#ifdef _AVANT_ESFP_SUPERVISION
         {
            TraiterPresenceAlarme(Index_L, ALA_RENDU_MONNAIE);
         }
#else
         {
            TraiterPresenceAlarme(BDD_INDEX_EQ, ALA_RENDU_MONNAIE);
         }
#endif
      }
   }

   // Evaluer disponibilité du rendu monnaie (si BNR)
   TraiterDispoRendu();

}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du monnayeur
* Parametres :
* ------------
*
* entree : Etat du monnayeur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesNri(T_NriState *Etat_P)

{
   unsigned int i;
   unsigned int j;
   T_BddCompPP PP_L;

   // PP001 - Coin verifier: missing
   static unsigned char PP001_S;

   // PP004 - Coin verifier: unjam flap open
   static unsigned char PP004_S;

   // PP005 - Coin verifier: possible credited coin removal
   static unsigned char PP005_S;

   // PP051 - Coin verifier-sorter: out-of-order
   static unsigned char PP051_S;

   // PP061 - Coin recycler: missing
   static unsigned char PP061_S;

   // PP063 - Coin recycler: out-of-order
   static unsigned char PP063_S;

   static struct
   {
      unsigned char Code;
      char *EN;
      unsigned char *Alarme;
   }
   Status_S[] =
   {
      {3, "A coin was validated but did not get to the place in the system when credit is given", &PP051_S},
      {4, "Defective Tube Sensor", &PP063_S},
      {7, "Tube Jam", &PP063_S},
      {8, "ROM checksum error", &PP051_S},
      {9, "Coin Routing Error", &PP051_S},
      {12, "Coin Jam", &PP051_S},
      {13, "Possible Credited Coin Removal", &PP005_S},
      {0, 0, 0},
   };

   static struct
   {
      unsigned char Z1;
      unsigned char Z2;
      char *EN;
      unsigned char *Alarme;
   }
   Diagnostic_S[] =
   {
      {1, 0, "Powering up Changer powering up / initialization", &PP051_S},
      {2, 0, "Powering down Changer powering down", &PP051_S},
      {4, 0, "Keypad shifted MODE key pressed and held so that", &PP051_S},
      {5, 0x10, "Manual Fill / Payout active", &PP051_S},
      {5, 0x20, "New Inventory Information Available", &PP051_S},
      {6, 0, "Inhibited", &PP051_S},
      {0x10, 0, "General changer error", &PP051_S},
      {0x10, 1, "Check sum error #1", &PP051_S},
      {0x10, 2, "Check sum error #2", &PP051_S},
      {0x10, 3, "Low line voltage detected", &PP051_S},
      {0x11, 0, "Discriminator module error", &PP051_S},
      {0x11, 0x10, "Flight deck open", &PP004_S},
      {0x11, 0x11, "Escrow Return stuck open", &PP004_S},
      {0x11, 0x30, "Coin jam in sensor", &PP051_S},
      {0x11, 0x41, "Discrimination below specified standard", &PP051_S},
      {0x11, 0x50, "Validation sensor A out of range", &PP051_S},
      {0x11, 0x51, "Validation sensor B out of range", &PP051_S},
      {0x11, 0x52, "Validation sensor C out of range", &PP051_S},
      {0x11, 0x53, "Operating temperature exceeded", &PP051_S},
      {0x11, 0x54, "Sizing optics failure", &PP051_S},
      {0x12, 0, "Accept gate module error", &PP051_S},
      {0x12, 0x30, "Coins entered gate, but did not exit", &PP051_S},
      {0x12, 0x31, "Accept gate alarm active", &PP051_S},
      {0x12, 0x40, "Accept gate open, but no coin detected", &PP051_S},
      {0x12, 0x50, "Post gate sensor covered before gate opened", &PP051_S},
      {0x13, 0, "Separator module error", &PP051_S},
      {0x13, 0x10, "Sort sensor error", &PP051_S},
      {0x14, 0, "Dispenser module error", &PP051_S},
      {0x15, 0, "Coin Cassette / tube module error", &PP063_S},
      {0x15, 2, "Cassette removed", &PP061_S},
      {0x15, 3, "Cash box sensor error", &PP063_S},
      {0x15, 4, "Sunlight on tube sensors", &PP063_S},
      {0, 0, 0, 0},
   };

   // PP001 - Coin verifier: missing
   PP001_S = FAUX;

   // PP004 - Coin verifier: unjam flap open
   PP004_S = FAUX;

   // PP005 - Coin verifier: possible credited coin removal
   PP005_S = FAUX;

   // PP051 - Coin verifier-sorter: out-of-order
   PP051_S = FAUX;

   // PP061 - Coin recycler: missing
   PP061_S = FAUX;

   // PP063 - Coin recycler: out-of-order
   PP063_S = FAUX;

   if (Etat_P->Connected)
   {
      // PP601 - Coins payment: Communication error
      TraiterAbsenceAlarme(BDD_INDEX_PP, ALA_DEFAUT_TELETRANS);
   }
   else
   {
      // PP601 - Coins payment: Communication error
      TraiterPresenceAlarme(BDD_INDEX_PP, ALA_DEFAUT_TELETRANS);
   }

   /* Acquerir rubrique Paiement par pièce */
   Bdd->LireCompPP(PP_L);

   /* Selon l'état du rendu monnaie */
   if (Etat_P->ChangeReturnAvailable)
   {
      // PP550 - Coins payment: Use exact change
      TraiterAbsenceAlarme(BDD_INDEX_PP, 550);

      /* MAJ Etat rendu monnaie */
      PP_L.EtatRendLaMonnaie = 'O';
   }
   else
   {
      // PP550 - Coins payment: Use exact change
      TraiterPresenceAlarme(BDD_INDEX_PP, 550);

      /* MAJ Etat non rendu monnaie */
      PP_L.EtatRendLaMonnaie = 'N';
   }

   /* Ecrire rubrique Paiement par pièce */
   Bdd->EcrireCompPP(PP_L);

   /* Ecrire état du monnayeur */
   Bdd->EcrireNriState(*Etat_P);

   // PP552 - Coins payment: No refund
   //if (Etat_P->ChangeReturnAvailable)
   if (!Etat_P->Refund)
   {
      NoRefund = true;
      TraiterPresenceAlarme(BDD_INDEX_PP, 552);
   }
   else
   {
      NoRefund = false;
      TraiterAbsenceAlarme(BDD_INDEX_PP, 552);
   }

   // PP501 - Coin Box: almost full
   if (Etat_P->CashBoxNearlyFull)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 501);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 501);
   }

   // PP502 - Coin Box: full
   if (Etat_P->CashBoxFull)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 502);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 502);
   }

   // PP503 - Coin Box: missing
   if (Etat_P->CashBoxMissing)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 503);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 503);
   }

   // PP505 - Coin Box: unauthorized withdrawing
   if (Etat_P->WrongCashboxReplacement)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 505);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 505);
   }

   // PP062 - Coin recycler: unauthorized withdrawing
   if (Etat_P->WrongReplacement)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 62);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 62);
   }

   /*** Status */
   for (i = 0; Status_S[i].Alarme != 0; i++)
   {
      if (Etat_P->Status == Status_S[i].Code)
      {
         *Status_S[i].Alarme = VRAI;
      }
   }

   /*** Diagnostic informations */
   for (i = 0; i < Etat_P->NbDiagnostic; i++)
   {
      for (j = 0; Diagnostic_S[j].Alarme != 0; j++)
      {
         if (Etat_P->Diagnostic[i].Z1 == Diagnostic_S[j].Z1 && Etat_P->Diagnostic[i].Z2 == Diagnostic_S[j].Z2)
         {
            *Diagnostic_S[j].Alarme = VRAI;
         }
      }
   }

   // PP001 - Coin verifier: missing
   if (PP001_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 1);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 1);
   }

   // PP004 - Coin verifier: unjam flap open
   if (PP004_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 4);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 4);
   }

   // PP005 - Coin verifier: possible credited coin removal
   if (PP005_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 5);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 5);
   }

   // PP051 - Coin verifier-sorter: out-of-order
   if (PP051_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 51);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 51);
   }

   // PP061 - Coin recycler: missing
   if (PP061_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 61);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 61);
   }

   // PP063 - Coin recycler: out-of-order
   if (PP063_S)
   {
      TraiterPresenceAlarme(BDD_INDEX_PP, 63);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PP, 63);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module SC66 de MEI (ABB)
* Parametres :
* ------------
*
* entree : Etat de l'accepteur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesMeisc(T_PbAlarme *Etat_P)
{
   T_BddCompPB PB_L;

   //TODO SCHMITT traiter les alarmes spécifiques à MeiSc66
   // En fin de compte, on fait tous les traitements des alarmes dans la fonction de base
   // sans discriminer suivant le type d'ABB. Comme ça, on garde un semblant de généricité !
   // Caisse échangée hors procédure
   if (VRAI == Etat_P->CaisseEchangeeHorsProcedure)
   {
      TraiterPresenceAlarme(BDD_INDEX_PB, COD_PB_ECHANGE_CAISSE_HORS_PROCEDURE);

      // Acquerir la rubrique etat de l'accepteur billets de banque 
      Bdd->LireCompPB(PB_L);

      if (PB_L.Caisse.Presente)
      {
         PB_L.Caisse.Presente = FAUX;
      }

      // Sauvegarder dans la BDD
      Bdd->EcrireCompPB(PB_L);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PB, COD_PB_ECHANGE_CAISSE_HORS_PROCEDURE);
   }

   // Caisse absente
   if (VRAI == Etat_P->CaisseAbsente)
   {
      TraiterPresenceAlarme(BDD_INDEX_PB, COD_ABB_CAISSE_ABSENTE);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PB, COD_ABB_CAISSE_ABSENTE);
   }

   // Problèmes divers, triche, bourrage, callage etc.
   if (VRAI == Etat_P->Triche ||
      VRAI == Etat_P->Bourrage ||
      VRAI == Etat_P->Stalled ||
      VRAI == Etat_P->Failed)
   {
      TraiterPresenceAlarme(BDD_INDEX_PB, COD_ABB_PANNE_TRANSPORT);
   }
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_PB, COD_ABB_PANNE_TRANSPORT);
   }
}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module de paiement par billets
* Parametres :
* ------------
*
* entree : Etat du monnayeur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesPB(T_PbAlarme *Etat_P)

{
   unsigned int Index_L;
   T_BddCompPB PB_L;

   /*****************************************************************************
   * Etape 1 : Mémoriser l'état du composant dans la base de données
   ******************************************************************************/

   // Récupérer la rubrique Paiement par billets
   Bdd->LireCompPB(PB_L);

   // Pas de controle d'echange hors procedure
   // => Memo de l'identification(uniquement si non vide, sinon c'est que l'initialisation n'est pas encore terminee)
   if (Etat_P->Identification.IdAccepteur[0] != 0)
   {
      // Memoriser l'identification de l'accepteur de billets de banque
      strncpy(PB_L.IdAccepteur, Etat_P->Identification.IdAccepteur, sizeof(PB_L.IdAccepteur));

      strcpy(PB_L.Caisse.NoSerie, Etat_P->Identification.IdCaisse.NoSerieCaisse);
      PB_L.Caisse.IdNoType = Etat_P->Identification.IdCaisse.NoType;

      Bdd->EcrireCompPB(PB_L);
   }

   /*****************************************************************************
   * Etape 2a : Traiter les alarmes du composant, communes au 2 types d'accepteurs
   ******************************************************************************/
#ifdef _AVANT_ESFP_SUPERVISION
   if (Etat_P->ModeleModule == PB_BNA57_MEI)
   {
      Index_L = BDD_INDEX_ABB;
   }
   else
   {
      Index_L = BDD_INDEX_PB;
   }
#else
   Index_L = BDD_INDEX_ABB;
#endif

   // Si le PB est connecte
   if (Etat_P->Connecte)
   {
      TraiterAbsenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);
   }
   else
   {
      TraiterPresenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);
   }

   // Si la caisse est presque pleine
   if (Etat_P->CaissePresquePleine == VRAI)
   {
      TraiterPresenceAlarme(Index_L, COD_PB_CAISSE_PRESQUE_PLEINE);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, COD_PB_CAISSE_PRESQUE_PLEINE);
   }

   /*****************************************************************************
   * Etape 2 : Traiter les alarmes du composant, selon son type d'accepteur
   ******************************************************************************/
   if (Etat_P->ModeleModule == PB_BNA57_MEI)
   {
      TraiterAlarmesAbb(Etat_P);
   }
   else if (Etat_P->ModeleModule == PB_SC66_MEI)
   {
      TraiterAlarmesMeisc(Etat_P);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module de paiement par billets Bill Validator
* Parametres :
* ------------
*
* entree : Etat du monnayeur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesBV(T_BVAlarme *Etat_P)

{
   T_BddCompPB PB_L;

#ifdef _AVANT_ESFP_SUPERVISION
   unsigned int Index_L = BDD_INDEX_BV;
#else
   unsigned int Index_L = BDD_INDEX_ABB;
#endif

   // Récupérer la rubrique Paiement par billets
   PB_L = Bdd->LireCompPB();

   if (strcmp(PB_L.Caisse.NoSerie, Etat_P->NoSerieCaisse))
   {
      strcpy(PB_L.Caisse.NoSerie, Etat_P->NoSerieCaisse);
      Bdd->EcrireCompPB(PB_L);
   }

   //*** PB601: Banknotes Payment: Communication error
   if (Etat_P->Connecte)
   {
      TraiterAbsenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);
   }
   else
   {
      TraiterPresenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);
   }

   //*** PB111 : Banknotes Payment: Jamming
   if (Etat_P->BourrageAccepteur)
   {
      TraiterPresenceAlarme(Index_L, 311);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 311);
   }

   //*** PB113 : Banknotes Payment: Attempted fraud
   if (Etat_P->TentativeFraude)
   {
      TraiterPresenceAlarme(Index_L, 313);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 313);
   }

   //*** PB141 : Banknotes Vault: Missing
   if (Etat_P->CaisseAbsente)
   {
      TraiterPresenceAlarme(Index_L, 341);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 341);
   }

   //*** PB143 : Banknotes Vault: Almost full
   if (Etat_P->CaissePresquePleine)
   {
      TraiterPresenceAlarme(Index_L, 343);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 343);
   }

   //*** PB144 : Banknotes Vault: Full
   if (Etat_P->CaissePleine)
   {
      TraiterPresenceAlarme(Index_L, 344);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 344);
   }

   //*** PB146 : Banknotes Vault: Unauthorized withdrawing
   if (Etat_P->CaisseEchangee)
   {
      TraiterPresenceAlarme(Index_L, 346);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 346);
   }

   if (Etat_P->BilletOublie)
   {
      TraiterPresenceAlarme(Index_L, 513);
   }
   else
   {
      TraiterAbsenceAlarme(Index_L, 513);
   }

   for (unsigned int i = 1; i <= 255; i++)
   {
      if (Etat_P->GenericFailure.Presente && Etat_P->GenericFailure.Error.Code == i)
      {
         TraiterPresenceAlarme(Index_L, i);
      }
      else
      {
         TraiterAbsenceAlarme(Index_L, i);
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module BNA57 de MEI (ABB)
* Parametres :
* ------------
*
* entree : Etat de l'accepteur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesAbb(T_PbAlarme *Etat_P)

{
   unsigned int i;
   unsigned int j;
   unsigned int Presente_L;
   T_BddCompPB PB_L;
   static unsigned int PremierDemarrage_S = VRAI;
   static unsigned int Code_S[ALA_NB_CODES_PANNES_ABB] = { 0 };

   // Mise a jour de la table des codes alarmes possibles de l'ABB
   if (PremierDemarrage_S)
   {
      Pb->DonnerCodesPannesPossibles(&Code_S[0], ALA_NB_CODES_PANNES_ABB);
      PremierDemarrage_S = FAUX;
   }

   // Pour chaque code alarme possible, on regarde si ce code n'est pas dans la liste de pannes actuelles 
   for (i = 0; Code_S[i] != 0 && i < ALA_NB_CODES_PANNES_ABB; i++)
   {
      // Initialisation de la presence d'une alarme pour ce code panne
      Presente_L = FAUX;

      // Parcourir la liste des pannes actuelles du module ABB
      for (j = 0; j < PB_NB_PANNES_SIMULTANEES && !Presente_L; j++)
      {
         // Le code panne courant correspond a une alarme en cours
         if (Etat_P->ListePannes[j] == Code_S[i])
         {
            // Memoriser la presence de l'alarme
            Presente_L = VRAI;
         }
      }

      /* Traiter tous les couincements, manipulations pannes ou fraudes */
      if (Code_S[i] == COD_ABB_PANNE_TRANSPORT)
      {
         // Au cas ou le code COD_ABB_PANNE_TRANSPORT (correspondant a la panne "M1") ne serait pas 
         // dans la ListePannes, on verifie les autres cas de "transport HS" possibles.
         if (Etat_P->CodeHS == ABB_OC_CODE_HORS_SERVICE)
         {
            // Memoriser la presence de l'alarme
            Presente_L = VRAI;
         }
      }

      /* Traiter caisse pleine */
      else if (Code_S[i] == COD_ABB_CAISSE_PLEINE)
      {
         // Au cas ou le code COD_ABB_CAISSE_PLEINE (correspondant a la panne "C3") ne serait pas 
         // dans la ListePannes, on verifie les autres cas de "caisse pleine" possibles.
         if (Etat_P->CaissePleine == VRAI || Etat_P->CodeHS == ABB_OC_CODE_CAISSE_PLEINE)
         {
            // Memoriser la presence de l'alarme
            Presente_L = VRAI;
         }
      }

      /* Traiter caisse absente */
      else if (Code_S[i] == COD_ABB_CAISSE_ABSENTE)
      {
         // Au cas ou le code COD_ABB_CAISSE_ABSENTE (correspondant a la panne "C1") ne serait pas
         //  dans la ListePannes, on verifie les autres cas "d'absence caisse" possibles.
         if (Etat_P->CodeHS == ABB_OC_CODE_CAISSE_ABSENTE)
         {
            // Memoriser la presence de l'alarme
            Presente_L = VRAI;
         }
      }

      /* Si l'alarme est presente */
      if (Presente_L)
      {
         // On leve l'alarme detaillee de l'ABB
         TraiterPresenceAlarme(BDD_INDEX_ABB, Code_S[i]);

         // Traiter caisse absente
         if (Code_S[i] == COD_ABB_CAISSE_ABSENTE)
         {
            // Récupérer la rubrique Paiement par billets
            Bdd->LireCompPB(PB_L);

            // Si une caisse identifiee etait presente
            if (strcmp(PB_L.Caisse.NoSerie, "") != 0 || PB_L.Caisse.IdNoType != 0)
            {
               // Echange caisse hors procedure
               TraiterPresenceAlarme(BDD_INDEX_ABB, COD_PB_ECHANGE_CAISSE_HORS_PROCEDURE);
            }
         }
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ABB, Code_S[i]);
      }
   }

}

/*****************************************************************************
* But de la fonction : Traiter la presence ou l'absence d'une alarme
* -------------------- du module de paiement par billets BNR / BNA
* Parametres :
* ------------
*
* entree : Etat du monnayeur
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesBnr(T_BnrAlarme *Etat_P)

{
   T_BddCompPB PB_L;
   T_PbState PbState_L = { 0 };

#ifdef _AVANT_ESFP_SUPERVISION
   unsigned int Index_L = BDD_INDEX_BNR;
#else
   unsigned int Index_L = BDD_INDEX_ABB;
#endif

   /*****************************************************************************
   * Etape 1 : Mémoriser l'état du composant dans la base de données
   ******************************************************************************/

   if (Etat_P->Connecte)
   {

      // Récupérer la rubrique Paiement par billets
      Bdd->LireCompPB(PB_L);

      TraiterAbsenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);

      // Memoriser l'identification de l'accepteur de billets de banque
      strncpy(PB_L.IdAccepteur, Etat_P->Identification.IdAccepteur, sizeof(PB_L.IdAccepteur));
      strncpy(PB_L.IdSpine, Etat_P->Identification.IdPbSpine, sizeof(PB_L.IdSpine));

      // Memoriser l'identification de la caisse
      strcpy(PB_L.Caisse.NoSerie, Etat_P->Identification.IdCaisse.NoSerieCaisse);
      PB_L.Caisse.IdNoType = Etat_P->Identification.IdCaisse.NoType;

      // Memoriser l'identification du recycleur et max billets
      strcpy(PB_L.NoSerieRecycleur1, Etat_P->Identification.IdPbRecycleurs[0]);
      strcpy(PB_L.NoSerieRecycleur2, Etat_P->Identification.IdPbRecycleurs[1]);
      strcpy(PB_L.NoSerieReserve, Etat_P->Identification.IdPbLoader);

      // Memoriser le firmware de l'ABB
      if (strcmp(PB_L.VersionFirmware, Etat_P->Identification.VersionFirmware) != 0)
      {
         strcpy(PB_L.VersionFirmware, Etat_P->Identification.VersionFirmware);
         //DonnerDateHeure(&PB_L.Logiciel.DateChargementLA);
      }

      // Sauvegarde en base certains états
      if (Etat_P->CaisseAbsente == VRAI)
      {
         PB_L.Caisse.Presente = FAUX;
      }
      else
      {
         PB_L.Caisse.Presente = VRAI;
      }

      Bdd->EcrireCompPB(PB_L);

      // Sauvegarde en base certaines alarmes applicatives
      PbState_L.CaisseEchangeeHorsProcedure = Etat_P->CaisseEchangeeHorsProcedure;
      PbState_L.RecycleurEchangeHorsProcedure = Etat_P->RecycleurEchangeHorsProcedure;
      PbState_L.LoaderEchangeHorsProcedure = Etat_P->LoaderEchangeHorsProcedure;
      Bdd->EcrirePbState(PbState_L);


      // Attention, pour ce premier groupe, 1 = OK (contrairement aux autres)
      // Firmware
      Traiter1Alarme(Index_L, COD_BNR_FIRMWARE, !Etat_P->FirwmareUptoDate);

      // Billet oublié
      Traiter1Alarme(Index_L, COD_PB_BILLET_OUBLIE, Etat_P->BilletOublie);
      // Offline
      Traiter1Alarme(Index_L, COD_BNR_OFFLINE, Etat_P->Offline);

      // Autre pb sur le main module
      Traiter1Alarme(Index_L, COD_BNR_MAIN_FAULTY, Etat_P->MainModuleFaulty);
      Traiter1Alarme(Index_L, COD_BNR_MAIN_JAM, Etat_P->MainModuleTransportJam);
      Traiter1Alarme(Index_L, COD_BNR_MAIN_TRANSPORT_FAULTY, Etat_P->MainModuleTransportFaulty);

      // Pb bundler
      Traiter1Alarme(Index_L, COD_BNR_BUNDLER_FAULTY, Etat_P->BundlerFaulty);
      // Pb Spine
      Traiter1Alarme(Index_L, COD_BNR_SPINE_FAULTY, Etat_P->SpineFaulty);
      Traiter1Alarme(Index_L, COD_BNR_SPINE_JAM, Etat_P->SpineTransportJam);
      Traiter1Alarme(Index_L, COD_BNR_SPINE_TRANSPORT_FAULT, Etat_P->SpineTransportFaulty);

      // Doors
      Traiter1Alarme(Index_L, COD_BNR_SAFE_DOOR_OPEN, Etat_P->SafeDoorOpen);
      Traiter1Alarme(Index_L, COD_BNR_BILLINTAKE_COVER_OPEN, Etat_P->BillIntakeCoverOpen);
      Traiter1Alarme(Index_L, COD_BNR_RECO_SENSOR_COVER_OPEN, Etat_P->RecognitionSensorCoverOpen);
      Traiter1Alarme(Index_L, COD_BNR_SPINE_COVER_OPEN, Etat_P->SpineCoverOpen);

      // Caisse
#ifdef _APRES_ESFP_SUPERVISION
      Traiter1Alarme(Index_L, COD_PB_CAISSE_DEFAUT, Etat_P->CaisseProbleme);
#endif
      Traiter1Alarme(Index_L, COD_ABB_CAISSE_ABSENTE, Etat_P->CaisseAbsente);
      Traiter1Alarme(Index_L, COD_PB_CAISSE_PRESQUE_PLEINE, Etat_P->CaissePresquePleine);
      Traiter1Alarme(Index_L, COD_ABB_CAISSE_PLEINE, Etat_P->CaissePleine);
      Traiter1Alarme(Index_L, COD_PB_ECHANGE_CAISSE_HORS_PROCEDURE, Etat_P->CaisseEchangeeHorsProcedure);
#ifdef _APRES_ESFP_SUPERVISION
      Traiter1Alarme(Index_L, COD_PB_CAISSE_PRESENCE_FAUX_BILLETS, Etat_P->CaissePresenceBilletInconnu);
#endif

      // Recycleurs
      Traiter1Alarme(Index_L, COD_PB_RECYCLEUR_HORS_PROCEDURE, Etat_P->RecycleurEchangeHorsProcedure);
      // Pour chaque recycleur
      // TODO : gérer + que 2 recycleurs
      //for (unsigned int i = 0; i < PB_MAX_RCY; i++)
      for (unsigned int i = 0; i < 2; i++)
      {
         Traiter1Alarme(Index_L, COD_BNR_RCY_1_FAULTY + i * 10, Etat_P->RecycleurProbleme[i]);
         Traiter1Alarme(Index_L, COD_BNR_RCY_1_MISSING + i * 10, Etat_P->RecycleurAbsent[i]);
         Traiter1Alarme(Index_L, COD_BNR_RCY_1_TRANSPORT_JAM + i * 10, Etat_P->RecycleurTransportJam[i]);
         Traiter1Alarme(Index_L, COD_BNR_RCY_1_TRANSPORT_PB + i * 10, Etat_P->RecycleurTransportFaulty[i]);
      }

      // Loader 
#ifdef _APRES_ESFP_SUPERVISION
      Traiter1Alarme(Index_L, COD_PB_LOADER_HORS_PROCEDURE, Etat_P->LoaderEchangeHorsProcedure);
#endif
      // (1 seul module)
      Traiter1Alarme(Index_L, COD_BNR_LOADER_FAULTY, Etat_P->LoaderProbleme);
      Traiter1Alarme(Index_L, COD_BNR_LOADER_JAM, Etat_P->LoaderTransportJam);
      Traiter1Alarme(Index_L, COD_BNR_LOADER_TRANSP_FAULTY, Etat_P->LoaderTransportFaulty);
#ifdef _APRES_ESFP_SUPERVISION
      Traiter1Alarme(Index_L, COD_PB_LOADER_ABSENT, Etat_P->LoaderAbsent);
      Traiter1Alarme(Index_L, COD_PB_LOADER_PRESQUE_VIDE, Etat_P->LoaderPresqueVide);
      Traiter1Alarme(Index_L, COD_PB_LOADER_VIDE, Etat_P->LoaderVide);
#endif
   }
   else
   {
      TraiterPresenceAlarme(Index_L, COD_PB_DEFAUT_TELETRANS);
   }

   // Evaluer disponibilité du rendu monnaie (si BNR)
   TraiterDispoRendu();
}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- du module Salr
* Parametres :
* ------------
*
* entree : Etat du module Salr
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesSalr(unsigned int Etat_P)

{

   /* Si Module SALR KO */
   if (Etat_P)
   {
      TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SUPERVISION);
   }
   /* Si Module SALR OK */
   else
   {
      TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SUPERVISION);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- connexion concentrateur billettique
* Parametres :
* ------------
*
* entree : Etat du module Salr
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesCtd(unsigned int Etat_P)

{

   /* Si l'etat */
   if (Etat_P == 0)
   {
      TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_CONCENTRATEUR);
   }
   else
   {
      TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_CONCENTRATEUR);
   }

}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- connexion Serveur Open media
* Parametres :
* ------------
*
* entree : Etat du module SOM
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesSOM(bool Connecte_P)

{
#ifdef TOULOUSE_NOT_SUPPORTED
   // Connecté ?
   if (Connecte_P)
   {
      TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SOM);
   }
   else
   {
      TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SOM);
   }
#endif
}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- connexion Scar online
* Parametres :
* ------------
*
* entree : Etat du module Scar online
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesScarOnline(bool Connecte_P)
{
   if (Connecte_P)
   {
      ScarOnlineConnecte = true;

      TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SCAR_ONLINE);

      TempoServeurWSScar.ArreterTempo();
   }
   else
   {
      ScarOnlineConnecte = false;

      TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SCAR_ONLINE);

      // On arme une tempo pour retenter la connexion
      TempoServeurWSScar.Armer(TEMPO_TEST_WS);
   }
}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- connexion SDD online
* Parametres :
* ------------
*
* entree : Etat du module Scar online
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesSDDOnline(bool Connecte_P)
{
#ifdef TOULOUSE_NOT_SUPPORTED
   if (Connecte_P)
   {
      SDDOnlineConnecte = true;

      TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SDD_ONLINE);

      TempoServeurWSSDD.ArreterTempo();
   }
   else
   {
      SDDOnlineConnecte = false;

      TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SDD_ONLINE);

      // On arme une tempo pour retenter la connexion
      TempoServeurWSSDD.Armer(TEMPO_TEST_WS);
   }
#endif
}

/*****************************************************************************
* But de la fonction : IR : Imprimante reçu
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesIR(I_IR::T_Etat *Etat_P)
{
   T_BddCompIR IR_L;

   if (Etat_P->Detail.Connecte)
   {
      Bdd->LireCompIR(IR_L);

      // Numéro de serie de l'imprimante
      strcpy(IR_L.IR.NumSerie, Etat_P->Identification.NoSerie);

      // Récupère (comme on peut) la version de l'imprimante
      if (strlen(Etat_P->Identification.VersionLogiciel) >= 11)
      {
         char version_L[10];
         strncpy(version_L, &Etat_P->Identification.VersionLogiciel[1], 9);
         version_L[9] = '\0';
         IR_L.IR.Logiciel[0].VersionLA = atoi(version_L);
         // celle ci est en hexa on dirait
         if (Etat_P->Identification.VersionLogiciel[10] > 'A' && Etat_P->Identification.VersionLogiciel[10] <= 'F')
         {
            IR_L.IR.Logiciel[0].AmendementLA = (10 + Etat_P->Identification.VersionLogiciel[10] - 'A');
         }
         else
         {
            IR_L.IR.Logiciel[0].AmendementLA = (Etat_P->Identification.VersionLogiciel[10] - '0');
         }
         IR_L.IR.Logiciel[0].NumRefLA = atoi(Etat_P->Identification.Reference);
         IR_L.IR.NbVersionLogicielles = 1;
      }

      // Rouleau papier vide ?
      if (Etat_P->Detail.Stock[0] == I_IR::T_Etat::STOCK_VIDE)
      {
         IR_L.EtatRouleau = BDD_STOCK_VIDE;
      }
      else
      {
         // Le rouleau n'est pas vide, il peut etre presque vide
         if (Etat_P->Detail.Stock[0] == I_IR::T_Etat::STOCK_PRESQUE_VIDE)
         {
            IR_L.EtatRouleau = BDD_STOCK_PRESQUE_VIDE;
         }
         else
         {
            IR_L.EtatRouleau = BDD_STOCK_NORMAL;
         }
      }

      Bdd->EcrireCompIR(IR_L);


      TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_DEFAUT_TELETRANS);
#ifdef _APRES_ESFP_SUPERVISION
      if (Etat_P->Detail.Erreur)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_IR_ERREUR);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_ERREUR);
      }
#endif
      if (Etat_P->Detail.CouvercleOuvert)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_IR_OUVERTE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_OUVERTE);
      }

      // Rouleau papier vide ?
      //if (Etat_P->Detail.Stock[0] == I_IR::T_Etat::STOCK_VIDE)
      if (IR_L.EtatRouleau == BDD_STOCK_VIDE)
      {
         // Plus de papier. On supprime l'alarme "presque vide"
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_IR_VIDE);
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_PRESQUE_VIDE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_VIDE);

         // Le rouleau n'est pas vide, il peut etre presque vide
         //if (Etat_P->Detail.Stock[0] == I_IR::T_Etat::STOCK_PRESQUE_VIDE)
         if (IR_L.EtatRouleau == BDD_STOCK_PRESQUE_VIDE)
         {
            TraiterPresenceAlarme(BDD_INDEX_IR, ALA_IR_PRESQUE_VIDE);
         }
         else
         {
            TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_PRESQUE_VIDE);
         }
      }

      if (Etat_P->Detail.Bourrage)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_IR_BOURRAGE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_IR_BOURRAGE);
      }

   }
   else
   {
      TraiterPresenceAlarme(BDD_INDEX_IR, ALA_DEFAUT_TELETRANS);
   }

}

/*****************************************************************************
* But de la fonction : VD : VDE
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesVD(T_TviStateFbMsg *Etat_P)

{
   T_BddCompVD VD_L;
   unsigned int i;
   unsigned int j;
   unsigned int Presente_L;
   static unsigned int Code_S[] =
   {
      10, // VD010: Ticket issuer: Uninitialized
      21, // VD021: Ticket issuer: Sensor CS faulty
      22, // VD022: Ticket issuer: Sensor CC faulty
      23, // VD023: Ticket issuer: Sensor CM faulty
      24, // VD024: Ticket issuer: Sensor CE faulty
      30, // VD030: Ticket issuer: Motor faulty
      31, // VD031: Ticket issuer: Flap open 
      32, // VD032: Ticket Issuer: Motor calibration faulty   
      //34, // VD034: Ticket issuer: Trash can full
      //35, // VD035: Ticket issuer: Trash can almost full
      42, // VD042: Ticket issuer: Encoding faulty 
      50, // VD050: Ticket issuer: Stucked
      //59, // VD059: Ticket Issuer: Almost empty
      //60, // VD060: Ticket Issuer: Empty
      61, // VD061: Ticket issuer: Supply stucked
      71, // VD071: Ticket issuer: Ticket not withdrawn
      0
   };

   if (Etat_P->Connected)
   {
      /* Acquerir la rubrique etat du composant */
      Bdd->LireCompVD(VD_L);

      /* Initialisation du numéro de série IC */
      memset(VD_L.VD.NumSerie, 0, sizeof VD_L.VD.NumSerie);

      /* Mettre a jour l'identifiant de l'imprimante */
      sprintf(VD_L.VD.NumSerie, "%u", Etat_P->Identification.HardwareId);

      /* Memoriser les version de l'Appli de Transport geres par l'API */
      VD_L.VD.NbVersionLogicielles = Etat_P->Identification.SoftwaresCount;

      for (i = 0; i < VD_L.VD.NbVersionLogicielles; i++)
      {
         /* Numéro de série logiciel */
         VD_L.VD.Logiciel[i].NumRefLA = Etat_P->Identification.Softwares[i].NumberId;

         /* N° version logiciel */
         VD_L.VD.Logiciel[i].VersionLA = Etat_P->Identification.Softwares[i].Version;

         /* N° amendement logiciel */
         VD_L.VD.Logiciel[i].AmendementLA = Etat_P->Identification.Softwares[i].Amendment;
      }

      /* Nombre de bobineaux en service */
      VD_L.NbStocks = 1;

      VD_L.STOCK[0].EtatStock = BDD_STOCK_NORMAL;

      if (Etat_P->StocksState == TVI_STOCKS_ONE_EMPTY)
      {
         VD_L.STOCK[0].EtatStock = BDD_STOCK_PRESQUE_VIDE;
      }
      else if (Etat_P->StocksState == TVI_STOCKS_TWO_EMPTY)
      {
         VD_L.STOCK[0].EtatStock = BDD_STOCK_VIDE;
      }
      else
      {
         VD_L.STOCK[0].EtatStock = BDD_STOCK_NORMAL;
      }

      /* Mettre a jour la rubrique etat du composant */
      Bdd->EcrireCompVD(VD_L);

      // VD059 et VD060 : Supply Almost Empty / Supply Empty
      if (Etat_P->StocksState == TVI_STOCKS_ONE_EMPTY)
      {
         TraiterPresenceAlarme(BDD_INDEX_VD, 59);
         TraiterAbsenceAlarme(BDD_INDEX_VD, 60);
      }
      else if (Etat_P->StocksState == TVI_STOCKS_TWO_EMPTY)
      {
         TraiterPresenceAlarme(BDD_INDEX_VD, 60);
         TraiterAbsenceAlarme(BDD_INDEX_VD, 59);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_VD, 60);
         TraiterAbsenceAlarme(BDD_INDEX_VD, 59);
      }

      // VD601: Ticket issuer: Communication faulty
      TraiterAbsenceAlarme(BDD_INDEX_VD, ALA_DEFAUT_TELETRANS);

      for (i = 0; Code_S[i] != 0; i++)
      {
         for (Presente_L = FAUX, j = 0; j < Etat_P->AlarmsCount && Presente_L == FAUX; j++)
         {
            if (Code_S[i] == Etat_P->AlarmCodes[j])
            {
               Presente_L = VRAI;
            }
         }

         if (Presente_L == VRAI)
         {
            TraiterPresenceAlarme(BDD_INDEX_VD, Code_S[i]);
         }
         else
         {
            TraiterAbsenceAlarme(BDD_INDEX_VD, Code_S[i]);
         }
      }

      // Optionnelle selon l'affaire
      if (ConfigDat->LireVD().PriseEnCompteChgtHorsProcedure && Etat_P->UnauthorizedStockWithdrawal)
      {
         // VD072 : Ticket Issuer: Unauthorized access
         TraiterPresenceAlarme(BDD_INDEX_VD, 72);
      }
      else
      {
         // VD072 : Ticket Issuer: Unauthorized access
         TraiterAbsenceAlarme(BDD_INDEX_VD, 72);
      }
   }
   else
   {
      // VD601: Ticket issuer: Communication faulty
      TraiterPresenceAlarme(BDD_INDEX_VD, ALA_DEFAUT_TELETRANS);
   }

}


/*****************************************************************************
* But de la fonction : DC : Dépileur codeur
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesDC(T_DcEtat *Etat_P)

{
   unsigned int i;
   unsigned int j;

   if (Etat_P->Connecte)
   {
      bool SamEchange_L = false;
      bool DefautSAM_L = false;
      bool PlafondAtteint_L = false;

      /* Acquerir la rubrique etat du module dépileur carte */
      T_BddCompDC DC_L = Bdd->LireCompDC();

      /*****************************************************************************
      * Etape 1 : Mémoriser l'état du composant dans la base de données
      ******************************************************************************/

      /* Memoriser l'identifiant du module DC*/
      sprintf(DC_L.DC.NumSerie, "%u", Etat_P->CscIdentification.CscSerialNumber);

      /* Memoriser les version de l'Appli de Transport geres par l'API */
      DC_L.DC.NbVersionLogicielles = Etat_P->CscIdentification.NbSoftwares;

      for (i = 0; i < Etat_P->CscIdentification.NbSoftwares; i++)
      {
         /* Numéro, version et amebdement dulogiciel */
         // 1er logiciel = l'appli ; 2ème logiciel boot ; 3ème logiciel FPGA
         if (Etat_P->CscIdentification.Software[i].Type == 1)
         {
            DC_L.DC.Logiciel[0].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            DC_L.DC.Logiciel[0].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            DC_L.DC.Logiciel[0].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
         else if (Etat_P->CscIdentification.Software[i].Type == 0)
         {
            DC_L.DC.Logiciel[1].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            DC_L.DC.Logiciel[1].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            DC_L.DC.Logiciel[1].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
         else if (Etat_P->CscIdentification.Software[i].Type == 2)
         {
            DC_L.DC.Logiciel[2].NumRefLA = Etat_P->CscIdentification.Software[i].Number;
            DC_L.DC.Logiciel[2].VersionLA = Etat_P->CscIdentification.Software[i].Version[0];
            DC_L.DC.Logiciel[2].AmendementLA = Etat_P->CscIdentification.Software[i].Version[1];
         }
      }

      // Traitement des SAMs
      T_ApiTlbEtatSAMs *Etat_L = &Etat_P->EtatSAMs;

      // Mémoriser les SAM du module Mrt */
      for (i = 0, DC_L.NbSAMs = 0; i < Etat_L->NbSAMs && i < BDD_NB_SAM; i++)
      {
         // Numéro de série du SAM
         unsigned __int64 NoSAM_L = Etat_L->Sam[i].NumeroSerieSam[11];
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[10] * (unsigned __int64)0x100;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[9] * (unsigned __int64)0x10000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[8] * (unsigned __int64)0x1000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[7] * (unsigned __int64)0x100000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[6] * (unsigned __int64)0x10000000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[5] * (unsigned __int64)0x1000000000000;
         NoSAM_L += Etat_L->Sam[i].NumeroSerieSam[4] * (unsigned __int64)0x100000000000000;

         if (DC_L.SAM[i].NumSerie != NoSAM_L)
         {
            SamEchange_L = true;
         }

         DC_L.SAM[i].NumSerie = NoSAM_L;

         // Code unicite : identifiant type de SAM
         DC_L.SAM[i].NumUnicite = Etat_L->Sam[i].CodeUnicite;

         // Type Altlas du SAM
         DC_L.SAM[i].NumTechno = Etat_L->Sam[i].TypeSAM;

         // Identifiant (partner Calypso) = T2 Provider
         DC_L.SAM[i].Provider[0] = Etat_L->Sam[i].Provider[0];
         DC_L.SAM[i].Provider[1] = Etat_L->Sam[i].Provider[1];

         // VRAI si sélection du SAM OK (y compris si SAM verrouillé)
         DC_L.SAM[i].DialogueOK = Etat_L->Sam[i].DialogueOK ? true : false;

         // VRAI indique que le SAM est dévérouillé ou non vérouillé
         DC_L.SAM[i].Deverouille = Etat_L->Sam[i].Deverouille ? true : false;

         // Pour toutes les clés du SAM
         for (j = 0, DC_L.SAM[i].NbCles = 0; j < APITLB_NB_CLES_PLAFONNEMENT_SAM && DC_L.SAM[i].NbCles < BDD_NB_CLE_SAM; j++)
         {
            // Présence de la clé dans le SAM (TRUE/FALSE)
            if (Etat_L->Sam[i].Plafonnement[j].PresenceCle)
            {
               // Type de clé : ATLAS_SAM_CLE_*
               DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Type = Etat_L->Sam[i].Plafonnement[j].TypeCle;

               // Plafond courant (0 pour non plafonné)
               DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Plafond = Etat_L->Sam[i].Plafonnement[j].Plafond;

               // Compteur courant
               DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Compteur = Etat_L->Sam[i].Plafonnement[j].Compteur;

               if (DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Plafond != 0)
               {
                  if (DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Compteur >= DC_L.SAM[i].Cle[DC_L.SAM[i].NbCles].Plafond)
                  {
                     PlafondAtteint_L = true;
                  }
               }

               // Nombre de clés
               DC_L.SAM[i].NbCles += 1;
            }
         }

         // Nombre de SAMs
         DC_L.NbSAMs += 1;

         if (!DC_L.SAM[i].DialogueOK || !DC_L.SAM[i].Deverouille || DC_L.SAM[i].NbCles == 0)
         {
            DefautSAM_L = true;
         }
      }

      /* Mettre a jour la rubrique etat du module dépileur carte */
      Bdd->EcrireCompDC(DC_L);

      /*****************************************************************************
      * Etape 2 : Traiter les alarmes concernées par le composant
      ******************************************************************************/

      // DC601 - Card Issuer: Communication error
      TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DEFAUT_TELETRANS);

      /* Si nombre de SAM ok */
      if (DC_L.NbSAMs != 0)
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ABSENT);

         /* Si un SAM a été echangé */
         if (SamEchange_L == VRAI)
         {
            /* Alarme informative SAM échangé */
            TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ECHANGE);
            TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ECHANGE);

            /* Enregistrer Activité "Inventaire des SAMs d'un equipement" */
            Act->Enregistreres();
         }

         if (!DefautSAM_L)
         {
            // SAM OK
            TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_DEFAUT_SAM);
         }
         else
         {
            // SAM NOK
            TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_DEFAUT_SAM);
         }
      }
      else
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ABSENT);
      }

      /* Si un SAM a été echangé */
      if (SamEchange_L == VRAI)
      {
         /* Alarme informative SAM échangé */
         TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ECHANGE);
         TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_SAM_ECHANGE);

         /* Enregistrer l'inventaire des SAMs d'un equipement */
         Act->Enregistreres();
      }

      /* Traiter Alarme Défaut liaison serveur de replafonnement des SAMs */
      if (Etat_P->EtatRePlafonnement.DialogueOK == FAUX)
      {
         /* Traiter l'apparition alarme associée */
         TraiterPresenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
      }
      else
      {
         /* Traiter la disparition de l'alarme */
         TraiterAbsenceAlarme(BDD_INDEX_CR, ALA_CONNEXION_SERVEUR_SAM);
      }

      /* Traiter Alarme replafonnement des SAMs refusé */
      if (Etat_P->EtatRePlafonnement.Refuse == VRAI)
      {
         /* Traiter l'apparition alarme associée */
         TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_REPLAFONNEMENT_REFUSE);
      }
      else
      {
         /* Traiter la disparition de l'alarme */
         TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_REPLAFONNEMENT_REFUSE);
      }

      /* Traiter alarme détection plafond sam atteint */
      if (Etat_P->EtatRePlafonnement.PlafondAtteint == VRAI || PlafondAtteint_L)
      {
         /* Traiter l'alarme */
         TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_PLAFOND_SAM_ATTEINT);
      }
      else
      {
         /* Traiter l'absence de l'alarme */
         TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_PLAFOND_SAM_ATTEINT);
      }

      /* Traiter alarme détection plafond sam presque atteint */
      if (Etat_P->EtatRePlafonnement.PlafondPresqueAtteint == VRAI)
      {
         /* Traiter l'alarme */
         TraiterPresenceAlarme(BDD_INDEX_DC, COD_MRT_PLAFOND_SAM_PRESQUE_ATTEINT);
      }
      else
      {
         /* Traiter l'absence de l'alarme */
         TraiterAbsenceAlarme(BDD_INDEX_DC, COD_MRT_PLAFOND_SAM_PRESQUE_ATTEINT);
      }

      // DC001 - Card Issuer: Out-of-service
      if (Etat_P->PbLotCartes)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_OoO);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_OoO);
      }

      // DC022 - Card Issuer: Almost emppty
      // DC023 - Card Issuer: Empty
      if (Etat_P->Empty || Etat_P->Missing)
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_AE);
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_E);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_E);

         if (Etat_P->AlmostEmpty)
         {
            TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_AE);
         }
         else
         {
            TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_AE);
         }
      }

      // DC024 - Card Issuer: Jam
      if (Etat_P->Jam)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_J);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_J);
      }

      // DC026 - Card Issuer: Failed to unstack
      if (Etat_P->FailedToUnstack)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_FtU);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_FtU);
      }

      // DC027 - Card Issuer: Failed to bin
      if (Etat_P->FailedToBin)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_FtB);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_FtB);
      }

      // DC028 - Card Issuer: Failed to issue
      if (Etat_P->FailedToIssue)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_FtI);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_FtI);
      }

      // DC070 - Card Issuer: Unauthorized access
      if (Etat_P->FraudeMagasin)
      {
         TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DC_UA);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_DC, ALA_DC_UA);
      }
   }
   else
   {
      // DC601 - Card Issuer: Communication error
      TraiterPresenceAlarme(BDD_INDEX_DC, ALA_DEFAUT_TELETRANS);
   }

}

/*****************************************************************************
* But de la fonction : Traiter les Alarmes du systeme
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesSysteme(unsigned int Code_P)

{

   switch (Code_P)
   {
#ifdef _AVANT_ESFP_SUPERVISION
   case COD_EN_SERVICE_VENTE:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      break;

   case COD_HORS_SERVICE_EXPLOITATION:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      break;

   case COD_EN_SERVICE_SOMMEIL:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      break;

   case COD_HORS_SERVICE_SOMMEIL:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      break;

   case COD_INTERVENTION_EXPLOITATION:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      break;

   case COD_INTERVENTION_RECETTE:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      break;

   case COD_INTERVENTION_MAINTENANCE:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_VENTE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_EN_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE_SOMMEIL);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_RECETTE);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_MAINTENANCE);
      break;

   case COD_CHANGEMENT_VERSION_PARAMETRES:
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_VERSION_PARAMETRES);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_VERSION_PARAMETRES);
      break;

   case COD_CHANGEMENT_VERSION_PROGRAMME:
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_VERSION_PROGRAMME);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_VERSION_PROGRAMME);
      break;

   case COD_CHANGEMENT_DATE_HEURE_SUP_120S:
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_DATE_HEURE_SUP_120S);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_CHANGEMENT_DATE_HEURE_SUP_120S);
      break;


#else // _AVANT_ESFP_SUPERVISION
   case COD_EN_SERVICE_VENTE:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EN_COURS);
      break;

   case COD_HORS_SERVICE_EXPLOITATION:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_EXPLOITATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EN_COURS);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE);
      break;

   case COD_EN_SERVICE_SOMMEIL:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EN_COURS);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_EXPLOITATION);
      break;

   case COD_HORS_SERVICE_SOMMEIL:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EN_COURS);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_EXPLOITATION);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE);
      break;

   case COD_INTERVENTION_EN_COURS:
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_SERVICE);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_HORS_EXPLOITATION);
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_INTERVENTION_EN_COURS);
      break;
#endif

   default:
      break;
   }

}

/*****************************************************************************
* But de la fonction : Traiter les Alarmes du lecteur code barre
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesOpticalReader(I_OpticalReader::T_ORState * State_P)
{
#ifdef _APRES_ESFP_SUPERVISION
   T_BddCompOR OR_L;
   Bdd->LireCompOR(OR_L);

   if (State_P->Connected)
   {
      OR_L.Etat = BDD_EC_EN_SERVICE;
      TraiterAbsenceAlarme(BDD_INDEX_OR, ALA_DEFAUT_TELETRANS);
   }
   else
   {
      OR_L.Etat = BDD_EC_HORS_SERVICE;
      TraiterPresenceAlarme(BDD_INDEX_OR, ALA_DEFAUT_TELETRANS);
   }

   // Extraction des version logiciel pour le LSR116
   if (ConfigDat->LireOpticalReader().Type == T_ConfigOpticalReader::LSR116)
   {
      sscanf(State_P->Firmwares.Version,
         "SB %d.%d.%d",
         &OR_L.OR.Logiciel[1].NumRefLA,
         &OR_L.OR.Logiciel[1].VersionLA,
         &OR_L.OR.Logiciel[1].AmendementLA);

      sscanf(State_P->Firmwares.ImagerVersion,
         "%d-%d",
         &OR_L.OR.Logiciel[2].NumRefLA,
         &OR_L.OR.Logiciel[2].VersionLA);
   }

   Bdd->EcrireCompOR(OR_L);
#endif
}

/*****************************************************************************
* But de la fonction :  Initialiser le traitement des Alarmes
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::InitialiserTrtAlarme(void)

{
   Trace->Tracer("Initialisation Traitement des alarmes\n");

   /* Armer la temporisation */
   TempoDisparitionAlarme.Armer(TEMPO_DISPARITION_ALARME);

   if (ConfigDat->LireICS().Present)
   {
      /* Acquerir les Alarmes de l'imprimante codeuse */
      if (ConfigDat->LireICS().Type == T_ConfigICS::CCS)
      {
         Ccs->DonnerAlarme();
      }
      else
      {
         Ics->DonnerAlarme();
      }
   }

   if (ConfigDat->LireMRT().Present)
   {
      // Acquerir les Alarmes du rechargement télébillettique
      Mrt->DonnerAlarme();
   }

   if (ConfigDat->LirePP().Present)
   {
      /* Acquerir les Alarmes du monnayeur */
      Pp->EngageAlarmsNotification();
   }

   if (ConfigDat->LirePB().Present)
   {
      /* Acquerir les Alarmes de l'accepteur de billets */
      Pb->DonnerAlarme();
   }

   if (ConfigDat->LireES().Present)
   {
      /* Demande de l'etat des E/S */
      Es->DonnerEvenements();

      /* Acquerir les Alarmes de la carte Entrées/Sorties */
      Es->DonnerAlarme();
   }

   if (cardPayment && ConfigDat->LireMPC().Present)
   {
      /* Acquerir les Alarmes du module paiement carte bancaire */
      cardPayment->GetAlarms();
   }

   if (ConfigDat->LireIR().Present)
   {
      /* Acquerir les Alarmes du module Imprimante reçu */
      Ir->DonnerAlarme();
   }

   if (ConfigDat->LireVD().Present)
   {
      /* Acquerir les Alarmes du module emetteur/valideur de ticket */
      Tvi->EngageAlarmsNotification();
   }

   if (ConfigDat->LireDC().Present)
   {
      /* Acquerir les Alarmes du module emetteur/valideur de ticket */
      DC->DonnerAlarme();
   }

   if (ConfigDat->LireOpticalReader().Present)
   {
      /* Acquerir les Alarmes du lecteur code barre */
      OpticalReader->GetAlarm();
   }

   //if (ConfigDat->LireDAT().Materiel == T_ConfigDAT::BCI)
   //{
   //   /* Acquerir les Alarmes de la BCI */
   //   Bci->DonnerAlarme ();
   //   Bci->DonnerAffichage ();
   //}

   /* Acquerir les Alarmes du Salr */
   Salr->DonnerAlarme();

   /* Enregister la disparition de l'alarme arret application */
   TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_ARRET_APPLICATION);

   if (!Bdd->Initialisee())
   {
      /* enregister apparition de l'alarme non initialise */
      TraiterPresenceAlarme(BDD_INDEX_EQ, COD_NON_INITIALISE);
   }

   /* Mettre a jour la signalisation des alarmes */
   TraiterSignalisationAlarmes();

}

/*****************************************************************************
* But de la fonction :  Initialiser le traitement des Alarmes suivant la présence du composant
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAbsenceAlarmesComposants(void)

{

   /*** DC : Dépileur codeur */
   if (!ConfigDat->LireDC().Present)
   {
      T_BddCompDC DC_L;

      /* Le composant est absent */
      Bdd->LireCompDC(DC_L);
      if (DC_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_DC);
      }

      /* Déclarer le composant comme absent dans la BDD */
      DC_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompDC(DC_L);
   }

   /*** ES : Entrées / Sorties */
   if (!ConfigDat->LireES().AccesArriere)
   {
      /* Pas de portes arrières, acquitter les alarmes correspodantes */
      TraiterAbsenceAlarme(BDD_INDEX_PT, COD_OUVERTURE_PORTES_ARRIERES);
   }

   if (!ConfigDat->LireES().AccesAvant)
   {
      /* Pas de portes avant, acquitter les alarmes correspodantes */
      TraiterAbsenceAlarme(BDD_INDEX_PT, COD_OUVERTURE_PORTE_AVANT);
   }

   if (!ConfigDat->LireES().ClePsAgent)
   {
      /* Pas de bouton de maintenance, acquitter les alarmes correspodantes */
      TraiterAbsenceAlarme(BDD_INDEX_PT, COD_CLE_ACTIONNEE);
   }

   if (!ConfigDat->LireES().TrappeAccesCaisse)
   {
      /* Pas de portes avant, acquitter les alarmes correspodantes */
#ifdef TOULOUSE_NOT_SUPPORTED
      TraiterAbsenceAlarme(BDD_INDEX_PT, COD_OUVERTURE_TRAPPE_MONNAIE);
#else
      TraiterAbsenceAlarme(BDD_INDEX_PT, COD_OUVERTURE_TRAPPE_CAISSE);
#endif
   }

   /*** IC : Imprimante codeuse */
   if (!ConfigDat->LireICS().Present)
   {
      T_BddCompIC IC_L;

      /* Le composant est absent */
      Bdd->LireCompIC(IC_L);
      if (IC_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_IC);
#ifdef _APRES_ESFP_SUPERVISION
         AcquitterToutesAlarmes(BDD_INDEX_ICS);
#endif
      }

      /* Déclarer le composant comme absent dans la BDD */
      IC_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompIC(IC_L);
   }

   /*** IR : Imprimante reçu */
   if (!ConfigDat->LireIR().Present)
   {
      T_BddCompIR IR_L;

      /* Le composant est absent */
      Bdd->LireCompIR(IR_L);
      if (IR_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_IR);
      }

      /* Déclarer le composant comme absent dans la BDD */
      IR_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompIR(IR_L);
   }

   /**** MB : Paiement carte bancaire */
   if (!ConfigDat->LireMPC().Present)
   {
      T_BddCompMB MB_L;

      /* Le composant est absent */
      Bdd->LireCompMB(MB_L);

      if (MB_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_MB);
      }

      /* Déclarer le composant comme absent dans la BDD */
      MB_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompMB(MB_L);
   }

   /*** PB : Paiement par billets de banque */
   if (!ConfigDat->LirePB().Present)
   {
      T_BddCompPB PB_L;

      /* Le composant est absent */
      Bdd->LireCompPB(PB_L);

      if (PB_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_PB);
         AcquitterToutesAlarmes(BDD_INDEX_ABB);
      }

      /* Déclarer le composant comme absent dans la BDD */
      PB_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompPB(PB_L);
   }

   /*** PP : Paiement par pièces de monnaie */
   if (!ConfigDat->LirePP().Present)
   {
      T_BddCompPP PP_L;

      /* Le composant est absent */
      Bdd->LireCompPP(PP_L);

      if (PP_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_PP);
#ifdef _APRES_ESFP_SUPERVISION
         AcquitterToutesAlarmes(BDD_INDEX_MNY);
#endif
      }

      /* Déclarer le composant comme absent dans la BDD */
      PP_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompPP(PP_L);
   }

   /*** RT : Module de rechargement télébillettique */
   if (!ConfigDat->LireMRT().Present)
   {
      T_BddCompRT RT_L;
      /* Le composant est absent */

      Bdd->LireCompRT(RT_L);
      if (RT_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_RT);
      }

      /* Déclarer le composant comme absent dans la BDD */
      RT_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompRT(RT_L);
   }

   /*** UPS : Onduleur */
   if (!ConfigDat->LireUPS().Present)
   {
      /* Le composant est absent, acquitter ses alarmes */
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_COUPURE_SECTEUR);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_UPS_PERTE_COMMUNICATION);
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_UPS_DEFAUT_BATTERIE);
   }

   /*** VD : VDE */
   if (!ConfigDat->LireVD().Present)
   {
      T_BddCompVD VD_L;

      /* Le composant est absent */
      Bdd->LireCompVD(VD_L);
      if (VD_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_VD);
      }

      /* Déclarer le composant comme absent dans la BDD */
      VD_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompVD(VD_L);
   }

   /*** OR : Optical Reader */
#ifdef _APRES_ESFP_SUPERVISION
   if (!ConfigDat->LireOpticalReader().Present)
   {
      T_BddCompOR OR_L;

      /* Le composant est absent */
      Bdd->LireCompOR(OR_L);
      if (OR_L.Etat != BDD_EC_ABSENT)
      {
         /* Jusqu'à présent, le composant était présent. */
         /* (Cas d'un changement de config par exemple) */
         /* Dans ce cas, il faut acquitter les alarmes du composant. */
         AcquitterToutesAlarmes(BDD_INDEX_OR);
      }

      /* Déclarer le composant comme absent dans la BDD */
      OR_L.Etat = BDD_EC_ABSENT;
      Bdd->EcrireCompOR(OR_L);
   }
#endif
   // Periph de délivrance titre de transport ?
   if (!ConfigDat->LireICS().Present && !ConfigDat->LireIR().TP && !ConfigDat->LireDC().Present && !ConfigDat->LireVD().Present)
   {
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_HS);
   }

   // Periph de rechargement TLB ?
   if (!ConfigDat->LireMRT().Present && !ConfigDat->LireICS().CSC)
   {
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_RECHARGEMENT_HS);
   }

   // Periph de délivrance recu ?
   if (!ConfigDat->LireICS().Recu && !ConfigDat->LireIR().Recu)
   {
      TraiterAbsenceAlarme(BDD_INDEX_EQ, COD_DELIVRANCE_RECU_HS);
   }

   // En config autonome, on ne veut pas de ces alarmes
   if (ConfigDat->LireDAT().CTDConnected == FAUX || ConfigDat->LireDAT().ConfigStandalone == VRAI)
   {
      /* Suppression de l'alarme nombre de transactions depasse */
      TraiterAbsenceAlarme(BDD_INDEX_FA, ALA_FA_MAX_TRANSACTION);

      /* Suppression de l'alarme montont cumule des transactions depasse */
      TraiterAbsenceAlarme(BDD_INDEX_FA, ALA_FA_MAX_RECETTE);
   }

}

/*****************************************************************************
* But de la fonction : Est - ce une alarme
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
unsigned int T_Etat::EstCeAlarme(void)
{
   unsigned int Return_L = VRAI;
   union
   {
      T_ChsState     Chs;
      T_EsEtat      Es;

      T_IcsEtat Ics;
      T_MrtEtat Mrt;
      T_MnyEtat Mny;
      BcpDeviceState Mpc;
      T_NriState Nri;
      T_PbAlarme Pb;
      T_BVAlarme BV;
      T_BnrAlarme BNR;
      I_IR::T_Etat Ir;
      T_TviStateFbMsg Tvi;
      T_DcEtat Dc;
      T_EtatAP5000 Ap5000;
      unsigned int Salr;
      I_OpticalReader::T_ORState OpticalReader;
   }Etat_L = { 0 };

   /*** DC : Dépileur codeur */
   if (DC->EstCeAlarme(&Etat_L.Dc))
   {
      /* Donner les Alarmes du dépileur codeur */
      DC->DonnerAlarme();

      /* Traiter les Alarmes du dépileur codeur*/
      TraiterAlarmesDC(&Etat_L.Dc);
   }

   /*** ES : Entrées / Sorties  */
   else if (Es->EstCeAlarme(&Etat_L.Es))
   {
      /* Donner les Alarmes des entrées / sorties */
      Es->DonnerAlarme();

      /* Carte connexion connectée ? */
      if (Etat_L.Es.Connecte == FAUX)
      {
         TraiterPresenceAlarme(BDD_INDEX_PT, COD_CARTE_ES_HORS_SERVICE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_PT, COD_CARTE_ES_HORS_SERVICE);
      }
   }

   /*** IC : Imprimante codeuse */
   else if (Ics->EstCeAlarme(Etat_L.Ics))
   {
      /* Donner les Alarmes de l'ICS */
      Ics->DonnerAlarme();

      /* Traiter les Alarmes de L'ICS */
      TraiterAlarmesIcs(&Etat_L.Ics);
   }

   /*** Ccs : Imprimante codeuse ticket */
   else if (Ccs->EstCeAlarme(&Etat_L.Ap5000))
   {
      /* Donner les Alarmes de l'impirmante codeuse */
      Ccs->DonnerAlarme();

      /* Traiter les Alarmes de l'impirmante codeuse */
      TraiterAlarmesCcs(&Etat_L.Ap5000);
   }

   /*** IR : Imprimante reçu */
   else if (Ir->EstCeAlarme(Etat_L.Ir))
   {
      /* Donner les Alarmes de l'imprimante recu */
      Ir->DonnerAlarme();

      /* Traiter les Alarmes de l'imprimante recu */
      TraiterAlarmesIR(&Etat_L.Ir);
   }

   /*** MB : Paiement carte bancaire */
   else if (cardPayment && cardPayment->IsReportGetAlarm(Etat_L.Mpc))
   {
      /* Donner les Alarmes du MPC */
      cardPayment->GetAlarms();

      /* Traiter les Alarmes du MPC */
      TraiterAlarmesMpc(&Etat_L.Mpc);
   }

   /*** PB : Paiement par billets de banque */
   else if (Pb->EstCeAlarme(Etat_L.Pb))
   {
      // Donner les Alarmes du PB
      Pb->DonnerAlarme();

      // Traiter les Alarmes du PB
      TraiterAlarmesPB(&Etat_L.Pb);
   }

   /*** PB : Paiement par billets de banque */
   else if (Pb->EstCeAlarme(Etat_L.BV))
   {
      // Donner les Alarmes du PB
      Pb->DonnerAlarme();

      // Traiter les Alarmes du PB
      TraiterAlarmesBV(&Etat_L.BV);
   }

   /*** PB : Paiement par billets de banque */
   else if (Pb->EstCeAlarme(Etat_L.BNR))
   {
      // Donner les Alarmes du PB
      Pb->DonnerAlarme();

      // Traiter les Alarmes du PB
      TraiterAlarmesBnr(&Etat_L.BNR);
   }

   /*** PP : Paiement par pièces de monnaie */
   else if (Pp->IsAlarmNotified(Etat_L.Chs))
   {
      /* Donner les Alarmes du monnayeur */
      Pp->EngageAlarmsNotification();

      /* Traiter les Alarmes du monnayeur */
      TraiterAlarmesChs(&Etat_L.Chs);
   }

   /*** PP : Paiement par pièces de monnaie */
   else if (Pp->EstCeAlarme(Etat_L.Mny))
   {
      /* Donner les Alarmes du monnayeur */
      Pp->EngageAlarmsNotification();

      /* Traiter les Alarmes du monnayeur */
      TraiterAlarmesMny(&Etat_L.Mny);
   }

   /*** PP : Paiement par pièces de monnaie */
   else if (Pp->IsAlarmNotified(Etat_L.Nri))
   {
      /* Donner les Alarmes du monnayeur */
      Pp->EngageAlarmsNotification();

      /* Traiter les Alarmes du monnayeur */
      TraiterAlarmesNri(&Etat_L.Nri);
   }

   /*** RT : Rechargement télébillettique */
   else if (Mrt->EstCeAlarme(&Etat_L.Mrt))
   {
      /* Donner les Alarmes du MRT */
      Mrt->DonnerAlarme();

      /* Traiter les Alarmes du Mrt */
      TraiterAlarmesMrt(&Etat_L.Mrt);
   }

   /* Si c'est une alarme Imprimante recu */
   //else if (Rec->EstCeAlarme(Etat_L.Rec))
   //{
     // /* Donner les Alarmes de l'imprimante recu */
     // Rec->DonnerAlarme();

     // /* Traiter les Alarmes de l'imprimante recu */
     // TraiterAlarmesRec(&Etat_L.Rec);
   //}

   /*** SALR : Serveur d'alarmes */
   else if (Salr->EstCeAlarme(Etat_L.Salr))
   {
      /* Donner les Alarmes du module SALR */
      Salr->DonnerAlarme();

      /* Traiter les Alarmes du module SALR */
      TraiterAlarmesSalr(Etat_L.Salr);
   }

   /*** VD : VDE */
   else if (Tvi->IsAlarmNotified(Etat_L.Tvi))
   {
      /* Donner les Alarmes de l'imprimante recu */
      Tvi->EngageAlarmsNotification();

      /* Traiter les Alarmes de l'imprimante recu */
      TraiterAlarmesVD(&Etat_L.Tvi);
   }

   /*** OR : Lecteur code barre */
   else if (OpticalReader->IsAlarmReport(Etat_L.OpticalReader))
   {
      // Donner les Alarmes du OpticalReader
      OpticalReader->GetAlarm();

      // Traiter les Alarmes du OpticalReader
      TraiterAlarmesOpticalReader(&Etat_L.OpticalReader);
   }

   /*** Temporisation filtre disparitions alarmes */
   else if (TempoDisparitionAlarme.EstCeCrTempsEcoule())
   {

      TempoDisparitionAlarme.Armer(TEMPO_DISPARITION_ALARME);

      /* Traiter disparition alarme */
      TraiterDisparitionAlarme();
   }

   /*** Default */
   else
   {
      Return_L = FAUX;
   }

   /* Fin de la fonction */
   return Return_L;

}

/*****************************************************************************
* But de la fonction : Acquitter toutes les alarmes d'un composant
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::AcquitterToutesAlarmes(const unsigned int Module_P)
{
   TraiterAbsenceAlarme(Module_P, 0, BDD_NB_ALARMES);
}

/*****************************************************************************
* But de la fonction : CCS : Imprimante codeuse de ticket
* -------------------
* Parametres :
* ------------
*
* entree :
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::TraiterAlarmesCcs(T_EtatAP5000 *Etat_P)
{
   T_BddCompIC IC_L;
   unsigned int TousLesBobineauxVides_L = VRAI;

   /* Acquerir la rubrique etat de l'imprimante */
   Bdd->LireCompIC(IC_L);

   /* Mettre a jour l'identifiant de l'imprimante */
   strcpy_s(IC_L.IC.NumSerie, sizeof(IC_L.IC.NumSerie) - 1, Etat_P->EtatCCS.SerialNumber);

   /* Mettre a jour Les compteurs de l'imprimante */
   // A mettre a jour via fonction cpt si besoin
/*   IC_L.IC.CYL = Etat_P->EtatCCS.Compteurs.CYL;
   IC_L.IC.CPI = Etat_P->EtatCCS.Compteurs.CPI;
   IC_L.IC.CPM = Etat_P->EtatCCS.Compteurs.CPM;*/

   /* Mettre a jour la version du firmware */
   IC_L.IC.NbVersionLogicielles = 1;
   IC_L.IC.Logiciel[0].NumRefLA = atoi(Etat_P->EtatCCS.Infos.Hardware_Version);

   char* tmp = strtok(Etat_P->EtatCCS.Infos.FirmwareVersion, ".");
   if (tmp != NULL)
   {
      IC_L.IC.Logiciel[0].VersionLA = atoi(tmp);
   }
   else
   {
      IC_L.IC.Logiciel[0].VersionLA = 0;
   }

   tmp = strtok(NULL, ".");
   if (tmp != NULL)
   {
      IC_L.IC.Logiciel[0].AmendementLA = atoi(tmp);
   }
   else
   {
      IC_L.IC.Logiciel[0].AmendementLA = 0;
   }

   // Date de chargement du firmware
   if (IC_L.IC.Logiciel[0].VersionLA != 0 || IC_L.IC.Logiciel[0].AmendementLA != 0)
   {
      // On ne remplit la date de chargement que si une version a été touvée
      IC_L.IC.Logiciel[0].DateChargementLA.Annee = (Etat_P->EtatCCS.Infos.Firmware_Date[4] - '0') * 10 + (Etat_P->EtatCCS.Infos.Firmware_Date[5] - '0');
      IC_L.IC.Logiciel[0].DateChargementLA.Mois = (Etat_P->EtatCCS.Infos.Firmware_Date[2] - '0') * 10 + (Etat_P->EtatCCS.Infos.Firmware_Date[3] - '0');
      IC_L.IC.Logiciel[0].DateChargementLA.Jour = (Etat_P->EtatCCS.Infos.Firmware_Date[0] - '0') * 10 + (Etat_P->EtatCCS.Infos.Firmware_Date[1] - '0');
   }
   else
   {
      IC_L.IC.Logiciel[0].DateChargementLA.Annee = 0;
      IC_L.IC.Logiciel[0].DateChargementLA.Mois = 0;
      IC_L.IC.Logiciel[0].DateChargementLA.Jour = 0;
   }

   IC_L.IC.Logiciel[0].DateChargementLA.Heure = 0;
   IC_L.IC.Logiciel[0].DateChargementLA.Minute = 0;
   IC_L.IC.Logiciel[0].DateChargementLA.Seconde = 0;

   // Etat des stocks (Normal, vide, presque vide (si capteur presque vide présent !)
   for (unsigned int i = 0; i < NB_MAGASINS; i++)
   {
      switch (Etat_P->EtatCCS.Detail.Magasin[i])
      {
      case T_EtatCCS::MAGASIN_VIDE:
         IC_L.STOCK[i].EtatStock = BDD_STOCK_VIDE;
         break;

      case T_EtatCCS::MAGASIN_PRESQUE_VIDE:
         IC_L.STOCK[i].EtatStock = BDD_STOCK_PRESQUE_VIDE;
         break;

      case T_EtatCCS::MAGASIN_OK:
      default:
         IC_L.STOCK[i].EtatStock = BDD_STOCK_NORMAL;
         break;
      }

      // Si c'est l'appli qui compte les titres, c'est l'appli qui positionne le presque vide
      if (ConfigDat->LireICS().ComptageTitres && Etat_P->EtatCCS.Detail.Magasin[i] == T_EtatCCS::MAGASIN_OK)
      {
         if (
            IC_L.STOCK[i].NbTitresRestants
            && Par->DonnerNbStockBasCCS()
            && IC_L.STOCK[i].NbTitresRestants <= Par->DonnerNbStockBasCCS()
            )
         {
            IC_L.STOCK[i].EtatStock = BDD_STOCK_PRESQUE_VIDE;
         }
      }
   }

   // Pas de SAM dans le CCS !!!
   IC_L.NbSAMs = 0;
   memset(IC_L.SAM, 0, sizeof IC_L.SAM);

   /* Mettre a jour la rubrique etat de l'imprimante */
   Bdd->EcrireCompIC(IC_L);

#ifdef TOULOUSE_NOT_SUPPORTED

   /* Si le module est connecte */
   if (Etat_P->EtatCCS.Detail.Connecte)
   {
      /* Supprimer l'alarme défaut télétransmission */
      TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_DEFAUT_TELETRANS);

      if (Etat_P->EtatCCS.Detail.Erreur)
      {
         TraiterPresenceAlarme(BDD_INDEX_ICS, 1);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ICS, 1);
      }

      if (Etat_P->EtatCCS.Detail.ticketJam)
      {
         TraiterPresenceAlarme(BDD_INDEX_ICS, 32);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ICS, 32);
      }

      /* pourrait servir si pas de capteur presque vide (= fonctionnement Dubai)
            if (Etat_P->EtatCCS.Detail.Magasin[0] == T_EtatCCS::MAGASIN_VIDE ||
               Etat_P->EtatCCS.Detail.Magasin[1] == T_EtatCCS::MAGASIN_VIDE )
            {

               TraiterPresenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + ICS_BOBINEAU_PRESQUE_VIDE);
               TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + 33);
            }
            else
            {
               TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + ICS_BOBINEAU_PRESQUE_VIDE);
            }

            if (Etat_P->EtatCCS.Detail.Magasin[0] == T_EtatCCS::MAGASIN_VIDE &&
               Etat_P->EtatCCS.Detail.Magasin[1] == T_EtatCCS::MAGASIN_VIDE )
            {
               TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + ICS_BOBINEAU_PRESQUE_VIDE);
               TraiterPresenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + 33);
            }
            else
            {
               TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + 33);
            }
      */

      // Traiter alarmes des stocks
      for (unsigned int i = 0; i < PARICS_NB_MAX_BOBINEAUX; i++)
      {
         if (i < NB_MAGASINS)
         {
            switch (IC_L.STOCK[i].EtatStock)
            {
            case BDD_STOCK_PRESQUE_VIDE:
               TraiterPresenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
               TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
               TousLesBobineauxVides_L = FAUX;
               break;

            case BDD_STOCK_VIDE:
               TraiterPresenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
               TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
               break;

            default:
               TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
               TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
               TousLesBobineauxVides_L = FAUX;
               break;
            }
            /*
            switch ( Etat_P->EtatCCS.Detail.Magasin[i])
            {
               case T_EtatCCS::MAGASIN_PRESQUE_VIDE :
                  TraiterPresenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
                  TraiterAbsenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
                  TousLesBobineauxVides_L = FAUX;
               break;

               case T_EtatCCS::MAGASIN_VIDE :
                  TraiterPresenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
                  TraiterAbsenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
               break;

               default :
                  TraiterAbsenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
                  TraiterAbsenceAlarme (BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
                  TousLesBobineauxVides_L = FAUX;
               break;
            }
            */
         }
         else
         {
            // Pas d'alarme sur les bobineaux non présents
            TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_PRESQUE_VIDE);
            TraiterAbsenceAlarme(BDD_INDEX_ICS, (ALA_IC_BOBINEAU + i * NB_ALA_IC_BOBINEAU) + ICS_BOBINEAU_VIDE);
         }
      }

      // Traiter alarme bobineaux vides
      if (TousLesBobineauxVides_L)
      {
         TraiterPresenceAlarme(BDD_INDEX_ICS, ALA_TOUS_LES_BOBINEAUX_VIDES);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_TOUS_LES_BOBINEAUX_VIDES);
      }

      // Pb TLB sur le stock
      if (Etat_P->PbLotTickets)
      {
         TraiterPresenceAlarme(BDD_INDEX_ICS, 92);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ICS, 92);
      }

      // Alarme Fraude magasin (échange or procédure) inexsitante au niveau systèm actuellement (25/01/2013)
      if (Etat_P->FraudeMagasin)
      {
         TraiterPresenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + 40);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_ICS, ALA_IC_BOBINEAU + 40);
      }

   }
   else
   {
      // IC601 - Card Issuer: Communication error
      TraiterPresenceAlarme(BDD_INDEX_ICS, ALA_DEFAUT_TELETRANS);
   }
#endif
}

/*****************************************************************************
* But de la fonction : Traiter la Presence ou l'Absence d'une alarme
* -------------------- du module impression / codage
* Parametres :
* ------------
*
* entree : Etat du module impression / codage
*
* sortie : aucun
*
* return : aucun
*
* Fonctions externes utilisees : TraiterApparition
*
* Points particuliers :
*
*****************************************************************************/
#ifdef toto
void T_Etat::TraiterAlarmesRec(T_RecEtat *Etat_P)

{
   // traiter alarme teletrans
   if (Etat_P->Connecte)
   {
      TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_DEFAUT_TELETRANS);

      // L'imprimante ayant un comportement particulier (lorsqu'elle est deconnectee
      // toutes les autres alarmes ne sont plus significatives), on ne modifie les alarmes
      // presentes que si on est connecte.

      /* Alarme Rouleau Presque vide */
      if (Etat_P->PapierBas)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_ROULEAU_PRESQUE_VIDE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_ROULEAU_PRESQUE_VIDE);
      }

      /* Alarme Imprimante ouverte */
      if (Etat_P->PorteOuverte)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_PORTE_OUVERTE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_PORTE_OUVERTE);
      }

      /* Alarme rouleau vide */
      if (Etat_P->FinPapier)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_ROULEAU_VIDE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_ROULEAU_VIDE);
      }

      /* Alarme bourrage papier */
      if (Etat_P->Par3)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_BOURRAGE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_BOURRAGE);
      }

      /* Alarme problème défilement = PositionCheminHaut */
      if (Etat_P->PositionCheminHaut)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_PB_DEFILEMENT);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_PB_DEFILEMENT);
      }

      /* Alarme problème température */
      if (Etat_P->Temperature)
      {
         TraiterPresenceAlarme(BDD_INDEX_IR, ALA_REC_PB_TEMPERATURE);
      }
      else
      {
         TraiterAbsenceAlarme(BDD_INDEX_IR, ALA_REC_PB_TEMPERATURE);
      }
   }
   else
   {
      TraiterPresenceAlarme(BDD_INDEX_IR, ALA_DEFAUT_TELETRANS);
   }

}
#endif
/*****************************************************************************
* But de la fonction : Associer l'alarme à la catégorie GTC
* --------------------
* Parametres :
* ------------
*
* entree : aucun
*
* sortie : aucun
*
* return : aucun
*
* Points particuliers :
*
*****************************************************************************/
void T_Etat::AssocierAvecGTC(unsigned char &EtatGTC_P, unsigned char EtatAlarme_P)
{
   /* Si une action est associe a l'alarme */
   if (EtatAlarme_P && !EtatGTC_P)
   {
      /* Mémoriser l'état */
      EtatGTC_P = 1;
   }
}