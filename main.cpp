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
//Bibliotheques C
#include <string.h>
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

void test(char* operation_sur_base, char* nom_base, bool success)
{
    if (success==false)
    {
        std::string texte = std::string(temps())
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
        std::string texte = std::string(temps())
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
    MYSQL_ROW row;

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
                                       "172.16.0.1", "iris2", "iris2fluidart",
                                       "jaipaslenomdelabase",1236,NULL,0);
    MYSQL* connect_base_fluidart = mysql_real_connect(base_fluidart,
                                   "172.16.126.150", "root", "xxx",
                                   "bddfluidarttest",0,NULL,0);




    //Gestion des erreurs sur les deux bases
    if(!connect_base_meteo)
    {
        test("connexion", "base_meteo", false);
    }
    else
    {
        /*if (!connect_base_temperatures)
        {
            test(connexion, base_temperatures, false);
        }
        else
        { */
        test("connexion", "base_meteo", true);

        //Instructions meteo
        char commande_meteo[] = "select * from meteo_valeurs_brutes;";
        mysql_query(base_meteo, commande_meteo);
        MYSQL_RES *resultat_meteo = mysql_store_result(base_meteo);
        char * chaine_meteo = (char *)malloc(500);
        while (row = mysql_fetch_row(resultat_meteo))
        {
            sprintf(chaine_meteo, "Date : %s "
                    "Temperature : %s "
                    "Rayonnement solaire : %s "
                    "Rayonnement UV : %s "
                    "Hygrometrie : %s "
                    "Pluviometrie : %s "
                    "Vitesse du vent : %s "
                    "Direction du vent : %s\n",
                    row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
            fputs(chaine_meteo, fichier);
        }
        mysql_free_result(resultat_meteo);

        // Instructions solaires
        char commande_solaire[] = "select * from solaire_valeurs_brutes;";
        mysql_query(base_meteo, commande_solaire);
        MYSQL_RES *resultat_solaire = mysql_store_result(base_meteo);
        char * chaine_solaire = (char *)malloc(500);
        while (row = mysql_fetch_row(resultat_solaire))
        {
            sprintf(chaine_solaire, "Date : %s "
                    "Puissance fournie : %s "
                    "Eclairement : %s "
                    "Energie totale : %s "
                    "Energie du jour : %s\n",
                    row[1],row[2],row[3],row[4],row[5]);
            fputs(chaine_solaire, fichier);
        }
        mysql_free_result(resultat_solaire);

        //Liberation de la base
        mysql_close(base_meteo);

        //Instructions temperatures
        //const char * commande_temperatures = "show tables;";
        //mysql_query(base_temperatures, commande_temperatures);
        //MYSQL_RES *resultat_temperatures = mysql_store_result(base_temperatures);
        //printf ("Nb de colonnes: %lu\n",
        //        (unsigned long) mysql_num_rows(resultat_temperatures));
        //Liberation du resultat et de la base
        //mysql_free_result(resultat_temperatures);
        //mysql_close(base_temperatures);
        //}
    }

    /*if (!connect_base_fluidart)
    {
        test("connexion", "base_fluidart", false);
    }
    else
    {
        test("connexion", "base_fluidart", true);
    }
    mysql_close(base_fluidart);*/

    fclose(fichier);

    return 0;

}
