# ⚡ EDF CSV Tool
<img width="1048" height="472" alt="image" src="https://github.com/user-attachments/assets/d819ed11-6464-4111-96e2-3fe0f5f039e3" />


<p align="center">


</p>

---

## 📌 Description

EDF CSV Tool est une application développée en C++ permettant d’explorer, analyser et exploiter rapidement des fichiers CSV de facturation EDF directement depuis un terminal Windows.

Le projet a progressivement évolué d’un simple lecteur CSV vers un véritable outil d’analyse et de navigation destiné à faciliter le traitement de données volumineuses sans dépendre d’Excel.

Développé dans le cadre d’un stage à la communauté de communes Albères, Côte Vermeille, Illibèris.

---

<img width="1201" alt="Dashboard EDF CSV Tool" src="https://github.com/user-attachments/assets/741d9acb-bfb0-4362-87d5-20272755d480" />

---

## 🚀 Fonctionnalités

### 📂 Gestion des fichiers CSV

* Ouverture de fichiers via fenêtre Windows
* Lecture rapide de fichiers CSV volumineux
* Parsing et structuration des données

### 🔍 Recherche et navigation

* Recherche par :

  * Nom de site
  * Adresse
  * Référence d’acheminement
* Navigation simplifiée entre les différents sites
* Menus dynamiques en console

### 📊 Analyse des données

* Dashboard global :

  * Nombre total de lignes
  * Nombre de sites uniques
  * Nombre de factures
  * Périodes disponibles
* Exploration détaillée des factures par site
* Bilans mensuels et annuels

### 🖥️ Interface console

* Interface terminal interactive
* Menus colorés
* Navigation rapide et légère

---

<img width="1035" alt="Analyse EDF CSV Tool" src="https://github.com/user-attachments/assets/5be52c02-aa24-4633-bd93-8f0313aa73d0" />

---

## 🧠 Évolution du projet

Le projet a progressivement intégré :

* Une architecture plus modulaire
* Des menus dynamiques par site
* Des statistiques globales avancées
* Des outils d’analyse de consommation et de facturation
* Une meilleure gestion des recherches et de l’affichage console

L’objectif est de continuer à faire évoluer l’outil vers une solution toujours plus rapide et pratique pour l’exploitation de données métier.

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

### 1️⃣ Cloner le projet

```bash
git clone https://github.com/Yonieva/edf-csv-tool.git
cd edf-csv-tool
```

### 2️⃣ Compiler (Windows - MinGW)

```bash
g++ main.cpp -o edf_tool
```

### 3️⃣ Lancer l'application

```bash
edf_tool.exe
```

---

<img width="1131" alt="Navigation EDF CSV Tool" src="https://github.com/user-attachments/assets/b3467e61-5640-46ac-be7b-c984e032669c" />

---

## ▶️ Utilisation

1. Lancer le programme
2. Sélectionner un fichier CSV
3. Naviguer dans les menus :

   * Rechercher un site
   * Consulter les factures
   * Voir les statistiques
   * Explorer les bilans

---

<img width="381" alt="Menu EDF CSV Tool" src="https://github.com/user-attachments/assets/edba3960-a593-49d3-9a6f-d16c40a54b96" />

---

## 🧱 Structure du projet

```text
edf-csv-tool/
├── main.cpp
├── README.md
├── LICENSE
```

---

## ⚠️ Limitations actuelles

* Compatible principalement Windows
* Interface console uniquement
* Dépend d’un format CSV précis
* Pas encore de version graphique

---

## 📜 Licence

Projet distribué sous licence GNU GPL v3.

Voir le fichier `LICENSE` pour plus d’informations.

---

## 👤 Auteur

**Yoan ONIEVA**

Projet personnel développé autour :

* du traitement de données
* du développement C/C++
* de l’analyse de fichiers métier
* de l’optimisation d’outils internes

---

## 💡 Objectif du projet

Créer un outil léger, rapide et pratique permettant d’exploiter efficacement des données de facturation EDF sans outils lourds ou solutions propriétaires.

---
