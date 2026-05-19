#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <windows.h>
#include <commdlg.h>
#include <limits>

using namespace std;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readIntSafe(const string& messageErreur = "Saisie invalide. Veuillez entrer un nombre : ") {
    int value;

    while (true) {
        if (cin >> value) {
            clearInput();
            return value;
        }

        setColor(12);
        cout << messageErreur;
        setColor(7);
        clearInput();
    }
}

string ouvrirFenetreFichier() {
    char filename[MAX_PATH] = "";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Fichiers CSV (*.csv)\0*.csv\0Tous les fichiers (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "csv";

    if (GetOpenFileName(&ofn)) {
        return string(filename);
    }

    return "";
}

void pauseMenuTerminal() {
    setColor(2);
    cout << "\n                        Appuyez sur la touche 'ENTREE' pour choisir un fichier Excel EDF";
    setColor(7);
    cin.get();
}

void pauseTerminal() {
    cout << "\n                                Appuyez sur la touche 'ENTREE' pour continuer...";
    cin.get();
}

void clearScreen() {
    system("cls");
}

struct Facture {
    string date_facture;
    string num_facture;
    string ref_acheminement;
    string nom_site;
    string adresse_site;
    string date_de_debut_de_consommation;
    string date_de_fin_de_consommation;

    double conso_elec_facturee_kwh = 0;
    double montant_total_ttc_euros = 0;
};

struct Bilan {
    int nbFactures = 0;
    double consoKwh = 0;
    double montantTTC = 0;
};

string csvText(const string& value) {
    string result = value;

    for (char& c : result) {
        if (c == '"') c = '\'';
    }

    return "\"" + result + "\"";
}

vector<string> splitCSV(const string& ligne, char sep = ';') {
    vector<string> result;
    string cell;
    bool inQuotes = false;

    for (size_t i = 0; i < ligne.size(); i++) {
        char c = ligne[i];

        if (c == '"') {
            if (inQuotes && i + 1 < ligne.size() && ligne[i + 1] == '"') {
                cell += '"';
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        }
        else if (c == sep && !inQuotes) {
            result.push_back(cell);
            cell.clear();
        }
        else {
            cell += c;
        }
    }

    result.push_back(cell);
    return result;
}

string nettoyer(string txt) {
    while (!txt.empty() && (txt.back() == '\r' || txt.back() == '\n' || txt.back() == ' ')) {
        txt.pop_back();
    }

    while (!txt.empty() && txt.front() == ' ') {
        txt.erase(txt.begin());
    }

    return txt;
}

string lowerText(string txt) {
    transform(txt.begin(), txt.end(), txt.begin(), ::tolower);
    return txt;
}

double toDoubleSafe(string value) {
    value = nettoyer(value);

    if (value.empty()) return 0;

    replace(value.begin(), value.end(), ',', '.');

    try {
        return stod(value);
    } catch (...) {
        return 0;
    }
}

int getIndex(const vector<string>& headers, const string& name) {
    for (int i = 0; i < headers.size(); i++) {
        if (nettoyer(headers[i]) == name) return i;
    }
    return -1;
}

vector<string> splitWords(const string& input) {
    vector<string> words;
    string word;
    stringstream ss(lowerText(input));

    while (ss >> word) {
        words.push_back(word);
    }

    return words;
}

bool matchRecherche(const Facture& f, const vector<string>& mots) {
    string cible = lowerText(
        f.nom_site + " " +
        f.adresse_site + " " +
        f.ref_acheminement
    );

    for (const string& mot : mots) {
        if (cible.find(mot) == string::npos) {
            return false;
        }
    }

    return true;
}

string getMois(const string& date) {
    if (date.size() >= 7) return date.substr(0, 7);
    return "INCONNU";
}

string getAnnee(const string& date) {
    if (date.size() >= 4) return date.substr(0, 4);
    return "INCONNU";
}

vector<Facture> chargerCSV(const string& chemin) {
    ifstream fichier(chemin);

    if (!fichier.is_open()) {
        throw runtime_error("Impossible d'ouvrir le fichier CSV.");
    }

    string ligne;
    getline(fichier, ligne);

    if (ligne.size() >= 3 && ligne[0] == '\xEF') {
        ligne = ligne.substr(3);
    }

    vector<string> headers = splitCSV(ligne);

    int idxDate = getIndex(headers, "date_facture");
    int idxNum = getIndex(headers, "num_facture");
    int idxRef = getIndex(headers, "ref_acheminement");
    int idxNom = getIndex(headers, "nom_site");
    int idxAdresse = getIndex(headers, "adresse_site");
    int idxDebutConso = getIndex(headers, "date_de_debut_de_consommation");
    int idxFinConso = getIndex(headers, "date_de_fin_de_consommation");
    int idxConsoElec = getIndex(headers, "conso_elec_facturee_kwh");
    int idxMontantTTC = getIndex(headers, "montant_total_ttc_euros");

    if (
        idxDate == -1 || idxNum == -1 || idxRef == -1 ||
        idxNom == -1 || idxAdresse == -1 ||
        idxDebutConso == -1 || idxFinConso == -1 ||
        idxConsoElec == -1 || idxMontantTTC == -1
    ) {
        throw runtime_error("Colonnes obligatoires introuvables dans le CSV.");
    }

    vector<Facture> factures;

    while (getline(fichier, ligne)) {
        vector<string> cols = splitCSV(ligne);

        if (cols.size() <= max({
            idxDate, idxNum, idxRef, idxNom, idxAdresse,
            idxDebutConso, idxFinConso, idxConsoElec, idxMontantTTC
        })) {
            continue;
        }

        Facture f;
        f.date_facture = nettoyer(cols[idxDate]);
        f.num_facture = nettoyer(cols[idxNum]);
        f.ref_acheminement = nettoyer(cols[idxRef]);
        f.nom_site = nettoyer(cols[idxNom]);
        f.adresse_site = nettoyer(cols[idxAdresse]);
        static int debugAdresse = 0;/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (debugAdresse < 10) {
            cout << "DEBUG adresse_site = [" << f.adresse_site << "]" << endl;
            debugAdresse++;
        }////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        f.date_de_debut_de_consommation = nettoyer(cols[idxDebutConso]);
        f.date_de_fin_de_consommation = nettoyer(cols[idxFinConso]);
        f.conso_elec_facturee_kwh = toDoubleSafe(cols[idxConsoElec]);
        f.montant_total_ttc_euros = toDoubleSafe(cols[idxMontantTTC]);

        if (!f.ref_acheminement.empty()) {
            factures.push_back(f);
        }
    }

    return factures;
}

vector<Facture> sitesUniques(const vector<Facture>& factures) {
    map<string, Facture> uniques;

    for (const auto& f : factures) {
        if (uniques.find(f.ref_acheminement) == uniques.end()) {
            uniques[f.ref_acheminement] = f;
        }
    }

    vector<Facture> result;
    for (auto& item : uniques) {
        result.push_back(item.second);
    }

    return result;
}


string extraireVillageDepuisAdresse(const string& adresse) {

    if (adresse.find("66700") != string::npos) return "ARGELES SUR MER";
    if (adresse.find("66650") != string::npos) return "BANYULS SUR MER";
    if (adresse.find("66290") != string::npos) return "CERBERE";
    if (adresse.find("66190") != string::npos) return "COLLIOURE";
    if (adresse.find("66200") != string::npos) return "ELNE";
    if (adresse.find("66560") != string::npos) return "ORTAFFA";
    if (adresse.find("66660") != string::npos) return "PORT VENDRES";
    if (adresse.find("66620") != string::npos) return "BROUILLA";
    if (adresse.find("66670") != string::npos) return "BAGES";

    if (adresse.find("66690") != string::npos) return "SECTEUR 66690";
    if (adresse.find("66740") != string::npos) return "SECTEUR 66740";

    return "INCONNU";
}

void afficherTableauDeBord(const vector<Facture>& factures) {
    set<string> sites;
    map<string, double> totalTtcParSite;
    map<string, double> totalTtcParVillage;

    string dateMin = "9999-99-99";
    string dateMax = "0000-00-00";

    double totalConso = 0;
    double totalTTC = 0;

    for (const auto& f : factures) {
        sites.insert(f.ref_acheminement);
        totalTtcParSite[f.ref_acheminement] += f.montant_total_ttc_euros;
        string village = extraireVillageDepuisAdresse(f.adresse_site);
        totalTtcParVillage[village] += f.montant_total_ttc_euros;
        if (!f.date_facture.empty()) {
            dateMin = min(dateMin, f.date_facture);
            dateMax = max(dateMax, f.date_facture);
        }

        totalConso += f.conso_elec_facturee_kwh;
        totalTTC += f.montant_total_ttc_euros;
    }

    vector<pair<string, double>> topSites(totalTtcParSite.begin(), totalTtcParSite.end());

    sort(topSites.begin(), topSites.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    setColor(13);
    cout << "                                    &&&&                                                        \n";
    setColor(9);
    cout << "                          &&";                                                                  
    setColor(13);
    cout << "    &&&&&&&&&&                                                      \n";
    setColor(9);
    cout << "                       &&& &&";
    setColor(13);
    cout << " &&&&&&&&&&&&&                                                     \n";
    setColor(9);
    cout << "                     &&";
    setColor(13);
    cout << "     &&&&&&&&&&&&&&&                                                     \n";
    setColor(9);
    cout << "                       &&";
    setColor(13);
    cout << "    &&&&&&&&&&&&&&&                                                    \n";
    setColor(9);
    cout << "                        &&";
    setColor(13);
    cout << "    &&&&&&&&&&&";
    setColor(9);
    cout << "   &&&&&&&&&                                                                                    \n";
    cout << "                          &&";
    setColor(13); 
    cout << "   &&&&&&&&";
    setColor(9);
    cout << "            &&             COMMUNAUTE DES COMMUNES       \n";
    cout << "                           &&                      &&             ALBERES                       \n";
    cout << "                           &&                      &&             COTE VERMEILLE                \n";
    cout << "                         &&&                        &             ILLIBERIS                     \n";
    cout << "                     &&&&                            &                                          \n";
    cout << "                     &                                 &&                                       \n";
    cout << "                     &";
    setColor(14);
    cout << "                  &&";
    setColor(9);
    cout << "                &&&&&                                                                           \n";
    cout << "                     &";
    setColor(14);
    cout << "              &&&&&&&&&&&";
    setColor(11);
    cout << "                  &&                                                                            \n";
    setColor(9);
    cout << "                     &&&";
    setColor(14);
    cout << "          &&&&&&&&&&&&&";
    setColor(11);
    cout << "             &&&&&&&&&&&                                                                        \n";
    setColor(14);
    cout << "                            &&&&&&&&&&&&&&&&&&&&";
    setColor(11);
    cout << "         &&&&&&&&&&&&&&&                                                                        \n";
    setColor(14);
    cout << "                                  &&&&&&&&&&&&&&&&";
    setColor(11);
    cout << "         &&&&&&&&&&&&&&&                                                                        \n";
    setColor(14);
    cout << "                                    &&&&&&&&&&&";
    setColor(9);
    cout << "    &&&";
    setColor(11);
    cout << "      &&&&&&&&&&&&&                                                                             \n";
    setColor(14);
    cout << "                                      &&&&&&&&";
    setColor(9);
    cout << "        &&";
    setColor(11);
    cout << "      &&&&&&&&&&                        \n";
    setColor(9);
    cout << "                                                        &&";
    setColor(11);
    cout << "      &&&&&&&                         \n\n\n";
    setColor(10);
    cout << "================================================================================\n";
    cout << fixed << setprecision(2);
    cout << "|| Nombre total de lignes/factures : " << factures.size() <<"                                     ||"<< endl;
    cout << "|| Nombre de sites uniques         : " << sites.size() <<"                                      ||"<< endl;
    cout << "|| Periode                         : " << dateMin << " -> " << dateMax <<"                 ||"<< endl;
    cout << "|| Consommation totale             : " << totalConso << " kWh" <<"                          ||"<< endl;
    cout << "|| Montant total TTC               : " << totalTTC << " euros" <<"                         ||"<< endl;
    cout << "================================================================================\n\n";
    setColor(14);
    cout << "================================================================================\n\n";
    cout << "                              Top sites par montant TTC\n\n";

    setColor(7);

    int count = 0;

    for (auto& item : topSites) {

        if (count >= 5) break;

        string ref = item.first;

        auto it = find_if(factures.begin(), factures.end(), [&](const Facture& f) {
            return f.ref_acheminement == ref;
        });

        if (it != factures.end()) {

            setColor(10);
            cout << count + 1 << ". ";
            setColor(7);

            cout << it->nom_site
                << " | "
                << fixed << setprecision(2)
                << item.second
                << " euros TTC\n";
        }

        count++;
    }

    vector<pair<string, double>> topVillages(
        totalTtcParVillage.begin(),
        totalTtcParVillage.end()
    );

    sort(topVillages.begin(), topVillages.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    setColor(14);

    cout << "\n================================================================================\n\n";
    cout << "                           Top villages par montant TTC\n\n";

    setColor(7);

    int countVillage = 0;

    for (auto& item : topVillages) {

        if (countVillage >= 5) break;

        setColor(10);
        cout << countVillage + 1 << ". ";
        setColor(7);

        cout << item.first
            << " | "
            << fixed << setprecision(2)
            << item.second
            << " euros TTC\n";

        countVillage++;
    }

    setColor(14);
    cout << "\n=================================================================================\n\n";
    setColor(7);
    }

    string nomFichierSafe(string txt) {
        for (char& c : txt) {
            if (
                c == ' ' || c == '/' || c == '\\' || c == ':' ||
                c == '*' || c == '?' || c == '"' || c == '<' ||
                c == '>' || c == '|'
            ) {
                c = '_';
            }
        }

        return txt;
    }

void exporterFacturesSite(const vector<Facture>& factures, const string& ref) {
    vector<Facture> result;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            result.push_back(f);
            site = f;
        }
    }

    if (result.empty()) {
        setColor(12);
        cout << "\nAucune facture a exporter.\n";
        setColor(7);
        return;
    }

    sort(result.begin(), result.end(), [](const Facture& a, const Facture& b) {
        return a.date_facture < b.date_facture;
    });

    string filename = "factures_" + nomFichierSafe(site.nom_site) + ".csv";
    ofstream out(filename);

    if (!out.is_open()) {
        setColor(12);
        cout << "\nErreur : impossible de creer le fichier d'export.\n";
        setColor(7);
        return;
    }

    out << fixed << setprecision(2);

    out << "Site selectionne:;" << csvText(site.nom_site) << "\n";
    out << "Adresse:;" << csvText(site.adresse_site) << "\n";
    out << "Ref acheminement:;" << csvText(site.ref_acheminement) << "\n\n";

    out << "Factures :\n";
    out << "Date;Numero facture;Conso kWh;Montant TTC\n";

    for (const auto& f : result) {
        out << f.date_facture << ";"
            << f.num_facture << ";"
            << f.conso_elec_facturee_kwh << ";"
            << f.montant_total_ttc_euros
            << "\n";
    }

    out.close();

    setColor(10);
    cout << "\nExport cree : " << filename << endl;
    setColor(7);
    pauseTerminal();
}

void exporterBilanMensuelSite(const vector<Facture>& factures, const string& ref) {
    map<string, Bilan> bilans;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            site = f;
            string mois = getMois(f.date_facture);

            bilans[mois].nbFactures++;
            bilans[mois].consoKwh += f.conso_elec_facturee_kwh;
            bilans[mois].montantTTC += f.montant_total_ttc_euros;
        }
    }

    if (bilans.empty()) {
        setColor(12);
        cout << "\nAucun bilan mensuel a exporter.\n";
        setColor(7);
        return;
    }

    string filename = "bilan_mensuel_" + nomFichierSafe(site.nom_site) + ".csv";
    ofstream out(filename);

    if (!out.is_open()) {
        setColor(12);
        cout << "\nErreur : impossible de creer le fichier d'export.\n";
        setColor(7);
        return;
    }

    out << fixed << setprecision(2);

    out << "BILAN MENSUEL DU SITE\n";
    out << "Site:;" << csvText(site.nom_site) << "\n";
    out << "Adresse:;" << csvText(site.adresse_site) << "\n";
    out << "Ref acheminement:;" << csvText(site.ref_acheminement) << "\n\n";

    out << "Mois;Nb factures;Conso kWh;Montant TTC\n";

    for (const auto& item : bilans) {
        out << item.first << ";"
            << item.second.nbFactures << ";"
            << item.second.consoKwh << ";"
            << item.second.montantTTC
            << "\n";
    }

    out.close();

    setColor(10);
    cout << "\nExport cree : " << filename << endl;
    setColor(7);
    pauseTerminal();
}

void exporterBilanAnnuelSite(const vector<Facture>& factures, const string& ref) {
    map<string, Bilan> bilans;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            site = f;
            string annee = getAnnee(f.date_facture);

            bilans[annee].nbFactures++;
            bilans[annee].consoKwh += f.conso_elec_facturee_kwh;
            bilans[annee].montantTTC += f.montant_total_ttc_euros;
        }
    }

    if (bilans.empty()) {
        setColor(12);
        cout << "\nAucun bilan annuel a exporter.\n";
        setColor(7);
        return;
    }

    string filename = "bilan_annuel_" + nomFichierSafe(site.nom_site) + ".csv";
    ofstream out(filename);

    if (!out.is_open()) {
        setColor(12);
        cout << "\nErreur : impossible de creer le fichier d'export.\n";
        setColor(7);
        return;
    }

    out << fixed << setprecision(2);

    out << "BILAN ANNUEL DU SITE\n";
    out << "Site:;" << csvText(site.nom_site) << "\n";
    out << "Adresse:;" << csvText(site.adresse_site) << "\n";
    out << "Ref acheminement:;" << csvText(site.ref_acheminement) << "\n\n";

    out << "Annee;Nb factures;Conso kWh;Montant TTC\n";

    for (const auto& item : bilans) {
        out << item.first << ";"
            << item.second.nbFactures << ";"
            << item.second.consoKwh << ";"
            << item.second.montantTTC
            << "\n";
    }

    out.close();

    setColor(10);
    cout << "\nExport cree : " << filename << endl;
    setColor(7);
    pauseTerminal();
}

void demanderExportFactures(const vector<Facture>& factures, const string& ref) {
    setColor(13);
    cout << "\n1 - Expoter la liste en CSV\n";
    setColor(7);
    cout << "2 - Retour\n";

    setColor(10);
    cout << "\nVotre choix : ";
    setColor(7);

    int choix = readIntSafe();

     while (choix < 1 || choix > 2) {
        setColor(12);
        cout << "Choix invalide. Veuillez entrer 1 ou 2 : ";
        setColor(7);
        choix = readIntSafe();
    }

    if (choix == 1)
        exporterFacturesSite(factures, ref);

    if (choix == 2)
        return;
}

void demanderExportBilanMensuel(const vector<Facture>& factures, const string& ref) {
    setColor(13);
    cout << "\n1 - Expoter le bilan mensuel en CSV\n";
    setColor(7);
    cout << "2 - Retour\n";

    setColor(10);
    cout << "\nVotre choix : ";
    setColor(7);

    int choix = readIntSafe();

    while (choix < 1 || choix > 2) {
        setColor(12);
        cout << "Choix invalide. Veuillez entrer 1 ou 2 : ";
        setColor(7);
        choix = readIntSafe();
    }
    
    if (choix == 1)
        exporterBilanMensuelSite(factures, ref);

    if (choix == 2)
        return;
}

void demanderExportBilanAnnuel(const vector<Facture>& factures, const string& ref) {
    setColor(13);
    cout << "\n1 - Exporter le bilan annuel en CSV\n";
    setColor(7);
    cout << "2 - Retour\n";

    setColor(10);
    cout << "\nVotre choix : ";
    setColor(7);

    int choix = readIntSafe();

    while (choix < 1 || choix > 2) {
        setColor(12);
        cout << "Choix invalide. Veuillez entrer 1 ou 2 : ";
        setColor(7);
        choix = readIntSafe();
    }

    if (choix == 1) {
        exporterBilanAnnuelSite(factures, ref);
    }
    if (choix == 2)
        return;
}

void afficherFacturesSite(const vector<Facture>& factures, const string& ref) {
    vector<Facture> result;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            result.push_back(f);
        }
    }

    sort(result.begin(), result.end(), [](const Facture& a, const Facture& b) {
        return a.date_facture < b.date_facture;
    });

    if (result.empty()) {
        setColor(12);
        cout << "Aucune facture trouvee.\n";
        setColor(7);
        return;
    }

    setColor(11);
    cout << "\nSite selectionne :\n";
    setColor(7);
    cout << result[0].nom_site << endl;
    cout << result[0].adresse_site << endl;
    cout << "Ref acheminement : " << result[0].ref_acheminement << endl;

    setColor(14);
    cout << "\nFactures :\n";
    cout << left
         << setw(15) << "Date"
         << setw(20) << "Numero facture"
         << setw(15) << "Conso kWh"
         << setw(15) << "Montant TTC"
         << endl;

    cout << "------------------------------------------------------------------\n";
    setColor(7);

    cout << fixed << setprecision(2);

    for (const auto& f : result) {
        cout << left
             << setw(15) << f.date_facture
             << setw(20) << f.num_facture
             << setw(15) << f.conso_elec_facturee_kwh
             << setw(15) << f.montant_total_ttc_euros
             << endl;
    }
}

void afficherBilanMensuelSite(const vector<Facture>& factures, const string& ref) {
    map<string, Bilan> bilans;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            site = f;
            string mois = getMois(f.date_facture);
            bilans[mois].nbFactures++;
            bilans[mois].consoKwh += f.conso_elec_facturee_kwh;
            bilans[mois].montantTTC += f.montant_total_ttc_euros;
        }
    }

    if (bilans.empty()) {
        setColor(12);
        cout << "Aucun bilan mensuel disponible.\n";
        setColor(7);
        return;
    }

    setColor(11);
    cout << "\nBILAN MENSUEL DU SITE\n";
    setColor(7);
    cout << site.nom_site << endl;
    cout << site.adresse_site << endl;
    cout << "Ref acheminement : " << site.ref_acheminement << endl << endl;

    setColor(14);
    cout << left
         << setw(12) << "Mois"
         << setw(15) << "Nb factures"
         << setw(18) << "Conso kWh"
         << setw(18) << "Montant TTC"
         << endl;
    cout << "---------------------------------------------------------------\n";
    setColor(7);

    cout << fixed << setprecision(2);

    for (const auto& item : bilans) {
        cout << left
             << setw(12) << item.first
             << setw(15) << item.second.nbFactures
             << setw(18) << item.second.consoKwh
             << setw(18) << item.second.montantTTC
             << endl;
    }
}

void afficherBilanAnnuelSite(const vector<Facture>& factures, const string& ref) {
    map<string, Bilan> bilans;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            site = f;
            string annee = getAnnee(f.date_facture);
            bilans[annee].nbFactures++;
            bilans[annee].consoKwh += f.conso_elec_facturee_kwh;
            bilans[annee].montantTTC += f.montant_total_ttc_euros;
        }
    }

    if (bilans.empty()) {
        setColor(12);
        cout << "Aucun bilan annuel disponible.\n";
        setColor(7);
        return;
    }

    setColor(11);
    cout << "\nBILAN ANNUEL DU SITE\n";
    setColor(7);
    cout << site.nom_site << endl;
    cout << site.adresse_site << endl;
    cout << "Ref acheminement : " << site.ref_acheminement << endl << endl;

    setColor(14);
    cout << left
         << setw(12) << "Annee"
         << setw(15) << "Nb factures"
         << setw(18) << "Conso kWh"
         << setw(18) << "Montant TTC"
         << endl;
    cout << "---------------------------------------------------------------\n";
    setColor(7);

    cout << fixed << setprecision(2);

    for (const auto& item : bilans) {
        cout << left
             << setw(12) << item.first
             << setw(15) << item.second.nbFactures
             << setw(18) << item.second.consoKwh
             << setw(18) << item.second.montantTTC
             << endl;
    }
}

void menuSite(const vector<Facture>& factures, const string& ref) {
    int choix;
    Facture site;

    for (const auto& f : factures) {
        if (f.ref_acheminement == ref) {
            site = f;
            break;
        }
    }

    do {
        clearScreen();

        setColor(14);
        cout << "==============================\n";
        cout << "        MENU DU SITE\n";
        cout << "==============================\n";
        setColor(10);
        cout << site.nom_site << endl;
        cout << site.adresse_site << endl;
        cout << "Ref : " << site.ref_acheminement << endl;
        cout << "==============================\n\n";
        setColor(7);

        cout << "1 - Voir les factures\n";
        cout << "2 - Voir le bilan mensuel\n";
        cout << "3 - Voir le bilan annuel\n";
        cout << "4 - Retour\n";

        setColor(10);
        cout << "\nVotre choix : ";
        setColor(7);

        choix = readIntSafe();

        while (choix < 1 || choix > 4) {
            setColor(12);
            cout << "Choix invalide. Veuillez entrer 1, 2, 3 ou 4 : ";
            setColor(7);
            choix = readIntSafe();
        }

        clearScreen();

        switch (choix) {
            case 1:
                afficherFacturesSite(factures, ref);
                demanderExportFactures(factures, ref);
                break;

            case 2:
                afficherBilanMensuelSite(factures, ref);
                demanderExportBilanMensuel(factures, ref);
                break;

            case 3:
                afficherBilanAnnuelSite(factures, ref);
                demanderExportBilanAnnuel(factures, ref);
                break;

            case 4:
                clearScreen();
                break;
        }

    } while (choix != 4);
}

void rechercherSite(const vector<Facture>& factures) {
    setColor(9);
    cout << "                             ,,                                ,,               \n";
    cout << "`7MM'''Mq.                 `7MM                              `7MM               \n";
    cout << "  MM   `MM.                  MM                                MM               \n";
    cout << "  MM   ,M9  .gP'Ya   ,p6'bo  MMpMMMb.  .gP'Ya `7Mb,od8 ,p6'bo  MMpMMMb.  .gP'Ya \n";
    cout << "  MMmmdM9  ,M'   Yb 6M'  OO  MM    MM ,M'   Yb  MM' ''6M'  OO  MM    MM ,M'   Yb\n";
    cout << "  MM  YM.  8M'''''' 8M       MM    MM 8M''''''  MM    8M       MM    MM 8M''''''\n";
    cout << "  MM   `Mb.YM.    , YM.    , MM    MM YM.    ,  MM    YM.    , MM    MM YM.    ,\n";
    cout << ".JMML. .JMM.`Mbmmd   YMbmd. JMML  JMML.`Mbmmd .JMML.   YMbmd .JMML  JMML. Mbmmd  \n";
    setColor(7);

    cout << "\nExemples : forage, step, 66200, elne, pompage... (0 pour retour)\n";
    setColor(10);
    cout << "Mot-cle : ";
    setColor(7);

    string input;
    getline(cin, input);

    while (input.empty()) {
        setColor(12);
        cout << "Recherche vide. Veuillez saisir un mot-cle existant ou 0 pour retour : ";
        setColor(7);
        getline(cin, input);
    }

    if (input == "0") {
        clearScreen();
        return;
    }

    vector<string> mots = splitWords(input);
    vector<Facture> uniques = sitesUniques(factures);
    vector<Facture> resultats;

    for (const auto& site : uniques) {
        if (matchRecherche(site, mots)) {
            resultats.push_back(site);
        }
    }

    if (resultats.empty()) {
        setColor(12);
        cout << "\nAucun site trouve.\n";
        setColor(7);
        pauseTerminal();
        clearScreen();
        return(rechercherSite(factures));
    }

    cout << "\nSites trouves :\n\n";

    for (int i = 0; i < resultats.size(); i++) {
        setColor(10);
        cout << "[" << i + 1 << "] ";
        setColor(7);
        cout << resultats[i].nom_site << endl;
        cout << "    Adresse : " << resultats[i].adresse_site << endl;
        cout << "    Ref EDF : " << resultats[i].ref_acheminement << endl << endl;
    }

    cout << "Choisir un site, 0 pour retour : ";
    int choix = readIntSafe();

    while (choix < 0 || choix > resultats.size()) {
        setColor(12);
        cout << "Choix invalide. Choisissez un numero entre 0 et " << resultats.size() << " : ";
        setColor(7);
        choix = readIntSafe();
    }

    if (choix == 0) {
        clearScreen();
        return;
    }

    menuSite(factures, resultats[choix - 1].ref_acheminement);
}

void listerSites(const vector<Facture>& factures) {
    vector<Facture> uniques = sitesUniques(factures);

    sort(uniques.begin(), uniques.end(), [](const Facture& a, const Facture& b) {
        return a.nom_site < b.nom_site;
    });

    cout << "\nListe des sites uniques :\n\n";

    for (int i = 0; i < uniques.size(); i++) {
        setColor(10);
        cout << "[" << i + 1 << "] ";
        setColor(7);
        cout << uniques[i].nom_site << endl;
        cout << "    Adresse : " << uniques[i].adresse_site << endl;
        cout << "    Ref EDF : " << uniques[i].ref_acheminement << endl << endl;
    }
}

void afficherAccueil(const vector<Facture>& factures) {
    int choix;

    do {
        clearScreen();

        afficherTableauDeBord(factures);

        setColor(9);
        cout << "                 `7MMM.     ,MMF'                             \n";
        cout << "                   MMMb    dPMM                               \n";
        cout << "                   M YM   ,M MM  .gP'Ya `7MMpMMMb.`7MM  `7MM  \n";
        cout << "                   M  Mb  M' MM ,M'   Yb  MM    MM  MM    MM  \n";
        cout << "                   M  YM.P'  MM 8M''''''  MM    MM  MM    MM  \n";
        cout << "                   M  `YM'   MM YM.    ,  MM    MM  MM    MM  \n";
        cout << "                 .JML. `'  .JMML.`Mbmmd'.JMML  JMML.`Mbod'YML.\n\n";
        setColor(7);
        cout << "                          1 - Rechercher un site\n";
        cout << "                          2 - Lister tous les sites\n";
        cout << "                          3 - Quitter\n";
        setColor(14);
        cout << "=================================================================================\n";
        setColor(7);

        setColor(10);
        cout << "\nVotre choix : ";
        setColor(7);

        choix = readIntSafe();

        while (choix < 1 || choix > 3) {
            setColor(12);
            cout << "Choix invalide. Veuillez entrer 1, 2 ou 3 : ";
            setColor(7);
            choix = readIntSafe();
        }

        clearScreen();

        switch (choix) {
            case 1:
                rechercherSite(factures);
                break;

            case 2:
                listerSites(factures);
                pauseTerminal();
                break;

            case 3:
                setColor(10);
                cout << "Fermeture.\n";
                setColor(7);
                break;
        }

    } while (choix != 3);
}

void draw_logo() {
    setColor(14);
    cout << "================================================================================================\n";
    cout << "                                CONSULTATION FACTURES CSV EDF                                   \n";
    cout << "================================================================================================\n";
    cout << "                            .=*****+.                                                           \n";
    cout << "                        ..*******.                                                              \n";
    cout << "                        .*******=.                          par Yoan onieva                     \n";
    cout << "                        .********..                                                             \n";
    cout << "                        .********.                                                              \n";
    cout << "                ..*..     ..-*****:.    ....... ..                                              \n";
    cout << "            ..***..      ..-+-...  ..+*******-...                                               \n";
    cout << "            .******=...... .     ..**************..  .%%%%%%%%%%.. :%%%%%%%%%%%.  %%%%%%%%%-.   \n";
    cout << "            +*************..     .****************. .%%%%....%%%%. :%%%-...-%%%%..%%%%......    \n";
    cout << "            .:************..     ... ......******:...%%%%----#%%%= :%%%-.  .*%%%:.%%%%-----.    \n";
    cout << "               .+*******..                ........ .-%%%%%%%%%%%%*.:%%%-.   -%%%-.%%%%%%%%%.    \n";
    cout << "               .......... ....   .......           ..%%%%.. ...... :%%%-. ..#%%%..%%%%.......   \n";
    cout << "                        ...*=.   =*****=..         ..%%%%%....*%+. :%%%-...%%%%#..%%%%.         \n";
    cout << "                      ...****.   ..-******-..        .*%%%%%%%%%%: :%%%%%%%%%%..  %%%%.         \n";
    cout << "                ........******:      .*******..                                                 \n";
    cout << "                ..************.       +*******-..                                               \n";
    cout << "                +**********-.       *********..                                                 \n";
    cout << "                .+*******=...       ...*****+.                                                  \n\n";
    setColor(9);
    cout << "\n                                        Veuillez agrandir cette fenetre\n";
    setColor(7);
}

int main() {
    draw_logo();
    pauseMenuTerminal();

    string chemin = ouvrirFenetreFichier();

    if (chemin.empty()) {
        setColor(12);
        cout << "Aucun fichier selectionne.\n";
        setColor(7);
        return 0;
    }

    cout << "Fichier selectionne : " << chemin << endl;

    vector<Facture> factures;

    try {
        factures = chargerCSV(chemin);
    } catch (const exception& e) {
        setColor(12);
        cerr << "\nErreur : " << e.what() << endl;
        setColor(7);
        return 1;
    }

    afficherAccueil(factures);

    return 0;
}