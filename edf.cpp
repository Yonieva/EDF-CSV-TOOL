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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Couleurs terminal Windows
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Nettoyer les erreurs de saisie
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Lire un nombre entier de manière sécurisée
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Ouvrir fenetre
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Demander appuie touche ENTREE
void pauseTerminal() {
    cout << "\n                         Appuyez sur la touche 'ENTREE' pour continuer...";
    cin.get();
}

struct Facture {
    string date_facture;
    string num_facture;
    string ref_acheminement;
    string nom_site;
    string adresse_site;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Clear l 'écran
void clearScreen() {
    system("cls");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

vector<string> splitCSV(const string& ligne, char sep = ';') {
    vector<string> result;
    string cell;
    stringstream ss(ligne);

    while (getline(ss, cell, sep)) {
        result.push_back(cell);
    }

    return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

string lowerText(string txt) {
    transform(txt.begin(), txt.end(), txt.begin(), ::tolower);
    return txt;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int getIndex(const vector<string>& headers, const string& name) {
    for (int i = 0; i < headers.size(); i++) {
        if (headers[i] == name) return i;
    }
    return -1;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

vector<string> splitWords(const string& input) {
    vector<string> words;
    string word;
    stringstream ss(lowerText(input));

    while (ss >> word) {
        words.push_back(word);
    }

    return words;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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

    if (idxDate == -1 || idxNum == -1 || idxRef == -1 || idxNom == -1 || idxAdresse == -1) {
        throw runtime_error("Colonnes obligatoires introuvables dans le CSV.");
    }

    vector<Facture> factures;

    while (getline(fichier, ligne)) {
        vector<string> cols = splitCSV(ligne);

        if (cols.size() <= max({idxDate, idxNum, idxRef, idxNom, idxAdresse})) {
            continue;
        }

        Facture f;
        f.date_facture = cols[idxDate];
        f.num_facture = cols[idxNum];
        f.ref_acheminement = cols[idxRef];
        f.nom_site = cols[idxNom];
        f.adresse_site = cols[idxAdresse];

        if (!f.ref_acheminement.empty()) {
            factures.push_back(f);
        }
    }

    return factures;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void afficherTableauDeBord(const vector<Facture>& factures) {
    set<string> sites;
    set<string> communes;
    map<string, int> compteurSites;

    string dateMin = "9999-99-99";
    string dateMax = "0000-00-00";

    for (const auto& f : factures) {
        sites.insert(f.ref_acheminement);
        compteurSites[f.ref_acheminement]++;

        if (!f.date_facture.empty()) {
            dateMin = min(dateMin, f.date_facture);
            dateMax = max(dateMax, f.date_facture);
        }

        communes.insert(f.adresse_site);
    }

    vector<pair<string, int>> topSites(compteurSites.begin(), compteurSites.end());

    sort(topSites.begin(), topSites.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    setColor(14);
    cout << "                                    &&&&                                                        \n";
    cout << "                          &&    &&&&&&&&&&                                                      \n";
    cout << "                       &&& && &&&&&&&&&&&&&                                                     \n";
    cout << "                     &&     :&&&&&&&&&&&&&&                                                     \n";
    cout << "                       &&    &&&&&&&&&&&&&&&                 COMMUNAUTE DES COMMUNES            \n";
    cout << "                        &&    &&&&&&&&&&&   &&&&&&&&&                                           \n";
    cout << "                          &&   &&&&&&&&            &&                                           \n";
    cout << "                           &&                      &&             ALBERES                       \n";
    cout << "                           &&                      x&             COTE VERMEILLE                \n";
    cout << "                         &&&                        &             ILLIBERIS                     \n";
    cout << "                     &&&&                            &                                          \n";
    cout << "                     &                                 &&                                       \n";
    cout << "                     &                 &&                ;&&&&                                  \n";
    cout << "                     &             &&&&&&&&&&;                  &&                              \n";
    cout << "                     &&&          &&&&&&&&&&&&&             +&&&&&&&&&&                         \n";
    cout << "                            &&&&&&&&&&&&&&&&&&&&           &&&&&&&&&&&&&&&                      \n";
    cout << "                                  &&&&&&&&&&&&&&&&         &&&&&&&&&&&&&&&                      \n";
    cout << "                                    &&&&&&&&&&&    &&&      &&&&&&&&&&&&&                       \n";
    cout << "                                      &&&&&&&&        &&      &&&&&&&&&&                        \n";
    cout << "                                                        &X      :&&&&&&                         \n";
    setColor(14);
    cout << "\n\n==========================================================================================\n";
    cout << " _____  _    ____  _     _____    _   _   _   ____  _____   ____   ___  ____  ____        \n";
    cout << "|_   _|/ \\  | __ )| |   | ____|  / \\ | | | | |  _ \\| ____| | __ ) / _ \\|  _ \\|  _ \\ \n";
    cout << "  | | / _ \\ |  _ \\| |   |  _|   / _ \\| | | | | | | |  _|   |  _ \\| | | | |_) | | | |  \n";
    cout << "  | |/ ___ \\| |_) | |___| |___ / ___ \\ |_| | | |_| | |___  | |_) | |_| |  _ <| |_| |    \n";
    cout << "  |_/_/   \\_\\____/|_____|_____/_/   \\_\\___/  |____/|_____| |____/ \\___/|_| \\_\\____/\n\n";
    cout << "==========================================================================================\n";
    setColor(10);
    cout << "Nombre total de lignes/factures : " << factures.size() << endl;
    cout << "Nombre de sites uniques         : " << sites.size() << endl;
    cout << "Periode                         : " << dateMin << " -> " << dateMax << endl;
    setColor(14);
    cout << "==========================================================================================\n\n";
    cout << "                          Top sites par nombre de factures\n\n";
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
                 << " | " << it->adresse_site
                 << " | " << item.second << " facture(s)\n";
        }

        count++;
    }

    setColor(14);
    cout << "==========================================================================================\n\n";
    setColor(7);
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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
    cout << left << setw(15) << "Date" << setw(20) << "Numero facture" << endl;
    cout << "-----------------------------------\n";
    setColor(7);

    for (const auto& f : result) {
        cout << left << setw(15) << f.date_facture
             << setw(20) << f.num_facture
             << endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void rechercherSite(const vector<Facture>& factures) {
        setColor(14);
    cout << "                                    &&&&                                                        \n";
    cout << "                          &&    &&&&&&&&&&                                                      \n";
    cout << "                       &&& && &&&&&&&&&&&&&                                                     \n";
    cout << "                     &&     :&&&&&&&&&&&&&&                                                     \n";
    cout << "                       &&    &&&&&&&&&&&&&&&                 COMMUNAUTE DES COMMUNES            \n";
    cout << "                        &&    &&&&&&&&&&&   &&&&&&&&&                                           \n";
    cout << "                          &&   &&&&&&&&            &&                                           \n";
    cout << "                           &&                      &&             ALBERES                       \n";
    cout << "                           &&                      x&             COTE VERMEILLE                \n";
    cout << "                         &&&                        &             ILLIBERIS                     \n";
    cout << "                     &&&&                            &                                          \n";
    cout << "                     &                                 &&                                       \n";
    cout << "                     &                 &&                ;&&&&                                  \n";
    cout << "                     &             &&&&&&&&&&;                  &&                              \n";
    cout << "                     &&&          &&&&&&&&&&&&&             +&&&&&&&&&&                         \n";
    cout << "                            &&&&&&&&&&&&&&&&&&&&           &&&&&&&&&&&&&&&                      \n";
    cout << "                                  &&&&&&&&&&&&&&&&         &&&&&&&&&&&&&&&                      \n";
    cout << "                                    &&&&&&&&&&&    &&&      &&&&&&&&&&&&&                       \n";
    cout << "                                      &&&&&&&&        &&      &&&&&&&&&&                        \n";
    cout << "                                                        &X      :&&&&&&                         \n";
    setColor(14);
    cout << " ____  _____ ____ _   _ _____ ____   ____ _   _ _____   ____  _____   ____ ___ _____ _____ \n";
    cout << "|  _ \\| ____/ ___| | | | ____|  _ \\ / ___| | | | ____| |  _ \\| ____| / ___|_ _|_   _| ____|\n";
    cout << "| |_) |  _|| |   | |_| |  _| | |_) | |   | |_| |  _|   | | | |  _|   \\___ \\| |  | | |  _|  \n";
    cout << "|  _ <| |__| |___|  _  | |___|  _ <| |___|  _  | |___  | |_| | |___   ___) | |  | | | |___ \n";
    cout << "|_| \\_\\_____\\____|_| |_|_____|_|  \\_\\____|_| |_|_____| |____/|_____| |____/___| |_| |_____|\n\n";
    setColor(7);
    cout << "\nExemples : forage, step, 66200, elne, pompage... (0 pour retour)\n";
    setColor(10);
    cout << "Mot-cle : ";
    setColor(7);

    string input;
    getline(cin, input);

    while (input.empty()) {
        setColor(12);
        cout << "Recherche vide. Veuillez saisir un mot-cle ou 0 pour retour : ";
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
        return;
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

    afficherFacturesSite(factures, resultats[choix - 1].ref_acheminement);
    pauseTerminal();
    clearScreen();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void draw_logo(){
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
    setColor(7);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int main() {
    draw_logo();
    pauseTerminal();

    string chemin;
    chemin = ouvrirFenetreFichier();

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

    clearScreen();
    afficherTableauDeBord(factures);

    int choix;

    do {
        setColor(14);
        cout <<"                              __  __ _____ _   _ _   _        \n";
        cout <<"                             |  \\/  | ____| \\ | | | | |       \n";
        cout <<"                             | |\\/| |  _| |  \\| | | | |       \n";
        cout <<"                             | |  | | |___| |\\  | |_| |       \n";
        cout <<"                             |_|  |_|_____|_| \\_|\\___/        \n\n";
        setColor(7);
        cout << "                          1 - Rechercher un site\n";
        cout << "                          2 - Lister tous les sites\n";
        cout << "                          3 - Afficher le tableau de bord\n";
        cout << "                          4 - Quitter\n";
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
                rechercherSite(factures);
                break;
            case 2:
                listerSites(factures);
                pauseTerminal();
                clearScreen();
                break;
            case 3:
                afficherTableauDeBord(factures);
                break;
            case 4:
                setColor(10);
                cout << "Fermeture.\n";
                setColor(7);
                break;
            default:
                setColor(12);
                cout << "Choix invalide.\n";
                setColor(7);
        }

    } while (choix != 4);

    return 0;
}





















/*#include <iostream>
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Ouvrir fenetre
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Demander appuie touche
void pauseTerminal() {
    cout << "\nAppuyez sur la touche 'ENTREE' pour continuer...";
    //cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

struct Facture {
    string date_facture;
    string num_facture;
    string ref_acheminement;
    string nom_site;
    string adresse_site;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//Clear l 'écran
void clearScreen() {
    system("cls");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

vector<string> splitCSV(const string& ligne, char sep = ';') {
    vector<string> result;
    string cell;
    stringstream ss(ligne);

    while (getline(ss, cell, sep)) {
        result.push_back(cell);
    }

    return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

string lowerText(string txt) {
    transform(txt.begin(), txt.end(), txt.begin(), ::tolower);
    return txt;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int getIndex(const vector<string>& headers, const string& name) {
    for (int i = 0; i < headers.size(); i++) {
        if (headers[i] == name) return i;
    }
    return -1;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

vector<string> splitWords(const string& input) {
    vector<string> words;
    string word;
    stringstream ss(lowerText(input));

    while (ss >> word) {
        words.push_back(word);
    }

    return words;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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

    if (idxDate == -1 || idxNum == -1 || idxRef == -1 || idxNom == -1 || idxAdresse == -1) {
        throw runtime_error("Colonnes obligatoires introuvables dans le CSV.");
    }

    vector<Facture> factures;

    while (getline(fichier, ligne)) {
        vector<string> cols = splitCSV(ligne);

        if (cols.size() <= max({idxDate, idxNum, idxRef, idxNom, idxAdresse})) {
            continue;
        }

        Facture f;
        f.date_facture = cols[idxDate];
        f.num_facture = cols[idxNum];
        f.ref_acheminement = cols[idxRef];
        f.nom_site = cols[idxNom];
        f.adresse_site = cols[idxAdresse];

        if (!f.ref_acheminement.empty()) {
            factures.push_back(f);
        }
    }

    return factures;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void afficherTableauDeBord(const vector<Facture>& factures) {
    set<string> sites;
    set<string> communes;
    map<string, int> compteurSites;

    string dateMin = "9999-99-99";
    string dateMax = "0000-00-00";

    for (const auto& f : factures) {
        sites.insert(f.ref_acheminement);
        compteurSites[f.ref_acheminement]++;

        if (!f.date_facture.empty()) {
            dateMin = min(dateMin, f.date_facture);
            dateMax = max(dateMax, f.date_facture);
        }

        communes.insert(f.adresse_site);
    }

    vector<pair<string, int>> topSites(compteurSites.begin(), compteurSites.end());

    sort(topSites.begin(), topSites.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    cout << "                                    &&&&                                                        \n";
    cout << "                          &&    &&&&&&&&&&                                                      \n";
    cout << "                       &&& && &&&&&&&&&&&&&                                                     \n";
    cout << "                     &&     :&&&&&&&&&&&&&&                                                     \n";
    cout << "                       &&    &&&&&&&&&&&&&&&                 COMMUNAUTE DES COMMUNES            \n";
    cout << "                        &&    &&&&&&&&&&&   &&&&&&&&&                                           \n";
    cout << "                          &&   &&&&&&&&            &&                                           \n";
    cout << "                           &&                      &&             ALBERES                       \n";                      
    cout << "                           &&                      x&             COTE VERMEILLE                \n";
    cout << "                         &&&                        &             ILLIBERIS                     \n";  
    cout << "                     &&&&                            &                                          \n";
    cout << "                     &                                 &&                                       \n";
    cout << "                     &                 &&                ;&&&&                                  \n";
    cout << "                     &             &&&&&&&&&&;                  &&                              \n";
    cout << "                     &&&          &&&&&&&&&&&&&             +&&&&&&&&&&                         \n";
    cout << "                            &&&&&&&&&&&&&&&&&&&&           &&&&&&&&&&&&&&&                      \n";
    cout << "                                  &&&&&&&&&&&&&&&&         &&&&&&&&&&&&&&&                      \n";
    cout << "                                    &&&&&&&&&&&    &&&      &&&&&&&&&&&&&                       \n";
    cout << "                                      &&&&&&&&        &&      &&&&&&&&&&                        \n";
    cout << "                                                        &X      :&&&&&&                         \n";
    cout << "\n\n==========================================================================================\n";
    cout << " _____  _    ____  _     _____    _   _   _   ____  _____   ____   ___  ____  ____        \n"; 
    cout << "|_   _|/ \\  | __ )| |   | ____|  / \\ | | | | |  _ \\| ____| | __ ) / _ \\|  _ \\|  _ \\ \n";
    cout << "  | | / _ \\ |  _ \\| |   |  _|   / _ \\| | | | | | | |  _|   |  _ \\| | | | |_) | | | |  \n";
    cout << "  | |/ ___ \\| |_) | |___| |___ / ___ \\ |_| | | |_| | |___  | |_) | |_| |  _ <| |_| |    \n";
    cout << "  |_/_/   \\_\\____/|_____|_____/_/   \\_\\___/  |____/|_____| |____/ \\___/|_| \\_\\____/\n\n";
    cout << "==========================================================================================\n";
    cout << "Nombre total de lignes/factures : " << factures.size() << endl;
    cout << "Nombre de sites uniques         : " << sites.size() << endl;
    cout << "Periode                         : " << dateMin << " -> " << dateMax << endl;
    cout << "==========================================================================================\n\n";
    cout << "                          Top sites par nombre de factures\n\n";

    int count = 0;
    for (auto& item : topSites) {
        if (count >= 5) break;

        string ref = item.first;
        auto it = find_if(factures.begin(), factures.end(), [&](const Facture& f) {
            return f.ref_acheminement == ref;
        });

        if (it != factures.end()) {
            cout << count + 1 << ". "
                 << it->nom_site
                 << " | " << it->adresse_site
                 << " | " << item.second << " facture(s)\n";
        }

        count++;
    }

    cout << "==========================================================================================\n\n";;
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

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
        cout << "Aucune facture trouvee.\n";
        return;
    }

    cout << "\nSite selectionne :\n";
    cout << result[0].nom_site << endl;
    cout << result[0].adresse_site << endl;
    cout << "Ref acheminement : " << result[0].ref_acheminement << endl;

    cout << "\nFactures :\n";
    cout << left << setw(15) << "Date" << setw(20) << "Numero facture" << endl;
    cout << "-----------------------------------\n";

    for (const auto& f : result) {
        cout << left << setw(15) << f.date_facture
             << setw(20) << f.num_facture
             << endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void rechercherSite(const vector<Facture>& factures) {
    cin.ignore();
    cout << " ____  _____ ____ _   _ _____ ____   ____ _   _ _____   ____  _____   ____ ___ _____ _____ \n";
    cout << "|  _ \\| ____/ ___| | | | ____|  _ \\ / ___| | | | ____| |  _ \\| ____| / ___|_ _|_   _| ____|\n";
    cout << "| |_) |  _|| |   | |_| |  _| | |_) | |   | |_| |  _|   | | | |  _|   \\___ \\| |  | | |  _|  \n";
    cout << "|  _ <| |__| |___|  _  | |___|  _ <| |___|  _  | |___  | |_| | |___   ___) | |  | | | |___ \n";
    cout << "|_| \\_\\_____\\____|_| |_|_____|_|  \\_\\____|_| |_|_____| |____/|_____| |____/___| |_| |_____|\n\n";
    cout << "\nExemples : forage, step, 66200, elne, pompage... (0 pour retour)\n";
    cout << "Mot-cle : ";

    string input;
    getline(cin, input);

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
        cout << "\nAucun site trouve.\n";
        return;
    }

    cout << "\nSites trouves :\n\n";

    for (int i = 0; i < resultats.size(); i++) {
        cout << "[" << i + 1 << "] " << resultats[i].nom_site << endl;
        cout << "    Adresse : " << resultats[i].adresse_site << endl;
        cout << "    Ref EDF : " << resultats[i].ref_acheminement << endl << endl;
    }

    cout << "Choisir un site, 0 pour retour : ";
    int choix;
    cin >> choix;

    if (choix <= 0 || choix > resultats.size()) {
        clearScreen();
        return;
    }

    afficherFacturesSite(factures, resultats[choix - 1].ref_acheminement);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void listerSites(const vector<Facture>& factures) {
    vector<Facture> uniques = sitesUniques(factures);

    sort(uniques.begin(), uniques.end(), [](const Facture& a, const Facture& b) {
        return a.nom_site < b.nom_site;
    });

    cout << "\nListe des sites uniques :\n\n";

    for (int i = 0; i < uniques.size(); i++) {
        cout << "[" << i + 1 << "] " << uniques[i].nom_site << endl;
        cout << "    Adresse : " << uniques[i].adresse_site << endl;
        cout << "    Ref EDF : " << uniques[i].ref_acheminement << endl << endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void draw_logo(){
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
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


int main() {
    draw_logo();
    pauseTerminal();
    string chemin;
    chemin = ouvrirFenetreFichier();
    if (chemin.empty()) {
        cout << "Aucun fichier selectionne.\n";
        return 0;
    }
    cout << "Fichier selectionne : " << chemin << endl;



    vector<Facture> factures;

    try {
        factures = chargerCSV(chemin);
    } catch (const exception& e) {
        cerr << "\nErreur : " << e.what() << endl;
        return 1;
    }
    clearScreen();
    afficherTableauDeBord(factures);

    int choix;

    do {
        cout <<"                              __  __ _____ _   _ _   _        \n";
        cout <<"                             |  \\/  | ____| \\ | | | | |       \n";                      
        cout <<"                             | |\\/| |  _| |  \\| | | | |       \n";
        cout <<"                             | |  | | |___| |\\  | |_| |       \n";
        cout <<"                             |_|  |_|_____|_| \\_|\\___/        \n\n";
        cout << "                          1 - Rechercher un site\n";
        cout << "                          2 - Lister tous les sites\n";
        cout << "                          3 - Afficher le tableau de bord\n";
        cout << "                          4 - Quitter\n";
        cout << "\nVotre choix : ";
        cin >> choix;
        clearScreen();

        switch (choix) {
            case 1:
                rechercherSite(factures);
                break;
            case 2:
                listerSites(factures);
                break;
            case 3:
                afficherTableauDeBord(factures);
                break;
            case 4:
                cout << "Fermeture.\n";
                break;
            default:
                cout << "Choix invalide.\n";
        }

    } while (choix != 4);

    return 0;
}*/