/*====================================Projet Fluid'Art====================================
===================================== Marco Aouzoulene ===================================
================================Importation des BDD distantes ============================
===================================== IDE : Code::Blocks =================================
==================================== Language utilise : C++ ==============================
========================================================================================*/

//Bibliotheques C++
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <ctime>
//Bibliotheques C
#include <windows.h>
#include <time.h>
#include <winsock.h>
#include <MYSQL/mysql.h>

//Espace de nom
using namespace std;

//Initialisation du log
FILE* fichier = fopen("importer_bdd.log", "a");

//Fonction "temps" a appeller avec les messages d'erreur pour les logs.
char* temps()
{
    time_t current_time = time(NULL);
    char* c_time_string = ctime(&current_time);
    return c_time_string; //type string
}

//Les messages d'erreur (ou de bon fonctionnement)
void test(char* operation_sur_base, char* nom_base, bool success)
{
    if (success==false)
    {
        string texte = std::string(temps())
                            + "Erreur avec "
                            + operation_sur_base
                            + " sur "
                            + nom_base
                            + "\n";
        cout << texte << endl;
        fputs(texte.c_str(), fichier);
    }
    else
    {
        //si cela marche
        string texte = std::string(temps())
                            + operation_sur_base
                            + " fonctionne avec "
                            + nom_base
                            + "\n";
        cout << texte << endl;
        fputs(texte.c_str(), fichier);
    }
}

int main(void)
{
//Declarations des bases
    MYSQL* base_meteo = mysql_init(NULL);
    MYSQL* base_temperatures = mysql_init(NULL);
    MYSQL* base_fluidart = mysql_init(NULL);
    MYSQL_ROW row_resultat_solaire;
    MYSQL_ROW row_resultat_meteo;

//Tests d'init sur base meteo
    if(base_meteo==NULL)
    {
        test("mysql_init", "base_meteo", false);
    }
    else
    {
        test("mysql_init", "base_meteo", true);
    }

//Tests d'init sur base temperatures
    if(base_temperatures==NULL)
    {
        test("mysql_init", "base_temperatures", false);
    }
    else
    {
        test("mysql_init", "base_temperatures", true);
    }

//Tests d'init sur base fluidart
    if(base_fluidart==NULL)
    {
        test("mysql_init", "base_fluidart", false);
    }
    else
    {
        test("mysql_init", "base_fluidart", true);
    }

//Options de connexion
    int options_meteo=mysql_options(base_meteo,
                                    MYSQL_SECURE_AUTH, "false");
    int options_temperatures=mysql_options(base_temperatures,
                                           MYSQL_SECURE_AUTH, "false");

//Connexion
    MYSQL* connect_base_meteo = mysql_real_connect(base_meteo,
                                "172.16.0.1", "iris2", "iris2fluidart",
                                "solaireLGM",1235,NULL,0);
    MYSQL* connect_base_temperatures = mysql_real_connect(base_temperatures,
                                       "lgm.ac-grenoble.fr", "iris2", "iris2fluidart",
                                       "sdcbat",1236,NULL,0);
    MYSQL* connect_base_fluidart = mysql_real_connect(base_fluidart,
                                   "172.16.126.150", "writerFluidArt", "xxx",
                                   "fluidart",0,NULL,0);

    
    
//Prelever les données toutes les 30 minutes
    while (1)
    {
        //Gestion des erreurs sur les trois bases avant execution du programme
        if(!connect_base_meteo)
        {
            test("connexion", "base_meteo", false);
        }
        else
        {
            if (!connect_base_temperatures)
            {
                test("connexion", "base_temperatures", false);
            }
            else
            {
                if (!connect_base_fluidart)
                {
                    test("connexion", "base_fluidart", false);
                }
                else
                {
                    test("connexion", "base_meteo", true);
                    test("connexion", "base_temperatures", true);
                    test("connexion", "base_fluidart", true);

                    // Recupération des valeurs

                    char commande_solaire[] = "select * from solaire_valeurs_brutes;";
                    mysql_query(base_meteo, commande_solaire);
                    MYSQL_RES *resultat_solaire = mysql_store_result(base_meteo);
                    char * chaine_solaire = (char *)malloc(2000);
                    while (row_resultat_solaire = mysql_fetch_row(resultat_solaire))
                    {
                        sprintf(chaine_solaire, "Date : %s "
                                "Puissance fournie : %s "
                                "Eclairement : %s "
                                "Energie totale : %s "
                                "Energie du jour : %s\n",
                                row_resultat_solaire[1],
                                row_resultat_solaire[2],
                                row_resultat_solaire[3],
                                row_resultat_solaire[4],
                                row_resultat_solaire[5]);
                        fputs(chaine_solaire, fichier);

                        /* Envoi des données à notre BDD
                        char commande_envoi_vers_base_fluidart[] = "INSERT INTO ??? VALUES ";
                        mysql_query(base_fluidart, commande_envoi_vers_base_fluidart);
                        //
                        */
                    }
                    mysql_free_result(resultat_solaire);
                }
            }
        }
    //"Dormir" pendant 1/2 heure
        Sleep(1000*60*30);
    }

//Fermeture des bases
    mysql_close(base_meteo);
    mysql_close(base_temperatures);
    mysql_close(base_fluidart);
    fclose(fichier);
    return 0;
    
    
}
