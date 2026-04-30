# ⚡ EDF CSV Tool

Outil en C++ permettant de consulter, analyser et rechercher facilement dans des fichiers CSV de facturation EDF, directement depuis le terminal.

---

## 📌 Description

Ce logiciel a été développé pour simplifier la lecture et l’exploitation de fichiers CSV complexes (factures EDF), sans passer par Excel.

Il permet notamment de :

* Charger un fichier CSV via une interface Windows
* Explorer les données rapidement
* Rechercher des sites ou factures
* Obtenir des statistiques globales

---

## 🚀 Fonctionnalités

* 📂 Ouverture de fichier CSV via fenêtre Windows
* 🔍 Recherche par :

  * Nom de site
  * Adresse
  * Référence d’acheminement
* 📊 Dashboard global :

  * Nombre total de lignes
  * Nombre de sites uniques
  * Période des factures (min → max)
* 🧠 Analyse des données directement dans le terminal
* 🖥️ Interface simple et rapide (console)

---

## 🗂️ Format attendu du CSV

Le fichier doit contenir les colonnes suivantes :

* `date_facture`
* `num_facture`
* `ref_acheminement`
* `nom_site`
* `adresse_site`

---

## 🛠️ Installation

### 1. Cloner le projet

```bash
git clone https://github.com/Yonieva/edf-csv-tool.git
cd edf-csv-tool
```

### 2. Compiler (Windows - MinGW)

```bash
g++ main.cpp -o edf_tool
```

### 3. Lancer

```bash
edf_tool.exe
```

---

## ▶️ Utilisation

1. Lancer le programme
2. Sélectionner un fichier CSV
3. Naviguer dans le menu :

   * Rechercher
   * Afficher les statistiques
   * Explorer les données

---

## 🧱 Structure du projet

```text
edf-csv-tool/
├── main.cpp
├── README.md
├── LICENSE
```

---

## 📸 Aperçu

*(Ajoute ici un screenshot de ton terminal pour faire pro 🔥)*

---

## ⚠️ Limitations

* Fonctionne principalement sur Windows
* Nécessite un format CSV conforme
* Interface uniquement en ligne de commande

---

## 📜 Licence

Ce projet est distribué sous licence.

Voir le fichier `LICENSE` pour plus d’informations.

---

## 👤 Auteur

Yoan ONIEVA
Projet personnel – outil de traitement de données EDF

---

## 💡 Objectif

Ce projet a été réalisé dans le cadre d’un stage à la communauté de commune Albères, Côte Vermeille, Illibèris et l'apprentissage en développement C/C++ et manipulation de données.

---
