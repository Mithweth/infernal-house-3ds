# Compiler Infernal House 3DS

Ce document explique comment installer les outils nécessaires, compiler **Infernal House 3DS** et produire les fichiers destinés à la Nintendo 3DS.

Le projet est écrit en C et utilise la chaîne de compilation **devkitPro / devkitARM**, **libctru**, **Citro2D / Citro3D** et **libvorbisidec** pour la lecture des musiques Ogg/Vorbis. La compilation est pilotée par un `Makefile` GNU.

## 1. Prérequis

Il faut disposer de :

- **Git** et **GNU Make** ;
- **devkitPro**, avec le groupe de paquets `3ds-dev` (devkitARM, libctru, Citro2D, Citro3D, `tex3ds`, etc.) ;
- **`3ds-libvorbisidec`**, pour le décodage audio Ogg/Vorbis ;
- **`makerom`**, uniquement pour produire le paquet installable `.cia` ;
- **`bannertool`**, uniquement si vous souhaitez régénérer la bannière du menu HOME.

Les outils de compilation 3DS sont disponibles sous Linux, macOS et Windows. Suivez la [procédure officielle de devkitPro](https://devkitpro.org/wiki/Getting_Started) pour installer son gestionnaire de paquets : `dkp-pacman` sous Linux/macOS, ou l'environnement fourni par devkitPro sous Windows.

### Linux : attention à la version de GLIBC

Les outils Linux récents de devkitPro nécessitent une **GLIBC 2.38 ou plus récente**. Sur une distribution plus ancienne, certains exécutables peuvent refuser de démarrer, même si les paquets devkitPro sont correctement installés.

Pour connaître la version de GLIBC de votre système :

```sh
ldd --version
```

Si votre distribution ne fournit pas GLIBC 2.38, **inutile de mettre à jour GLIBC manuellement** : vous pouvez compiler avec l'image Docker officielle [`devkitpro/devkitarm:latest`](https://hub.docker.com/r/devkitpro/devkitarm).

Depuis la racine du projet :

```sh
docker run --rm -v "$(pwd):/work" -w /work devkitpro/devkitarm:latest make
```

Le répertoire du projet est monté dans le conteneur : les fichiers compilés sont donc produits directement sur votre machine. Cette commande construit le `.3dsx` ; la création d'un `.cia` nécessite également `makerom` (voir section 4).

### Linux (Debian/Ubuntu) : installation automatisée

Le dépôt fournit un script d'installation de l'environnement de développement, [`scripts/install-env.sh`](../scripts/install-env.sh). Il configure le dépôt de paquets devkitPro, installe les outils et bibliothèques nécessaires ainsi que `makerom` et `bannertool`, puis prépare les variables d'environnement.

Après avoir récupéré les sources (section 2), depuis la racine du projet :

```sh
sudo bash scripts/install-env.sh
source /etc/profile.d/devkit-env.sh
```

Ce script utilise `apt` et nécessite les privilèges administrateur. Il ne supprime **pas** la contrainte de GLIBC : si votre distribution est trop ancienne, utilisez plutôt Docker.

### Linux et macOS : installation manuelle

Une fois le gestionnaire de paquets devkitPro installé :

```sh
sudo dkp-pacman -S 3ds-dev 3ds-libvorbisidec
```

Sous Windows, installez les mêmes paquets depuis le terminal MSYS2/devkitPro, avec `pacman` (sans `sudo`) :

```sh
pacman -S 3ds-dev 3ds-libvorbisidec
```

Sur Linux, une installation standard utilise les variables suivantes :

```sh
export DEVKITPRO=/opt/devkitpro
export DEVKITARM="$DEVKITPRO/devkitARM"
export PATH="$DEVKITPRO/tools/bin:$PATH"
```

L'installation de devkitPro configure normalement ces variables. Si elles sont absentes, ouvrez une nouvelle session ou vérifiez la configuration de votre environnement. Le `Makefile` exige notamment que `DEVKITARM` soit défini.

Pour vérifier les outils :

```sh
echo "$DEVKITPRO"
echo "$DEVKITARM"
command -v arm-none-eabi-gcc
command -v tex3ds
```

## 2. Récupérer les sources

Clonez le dépôt, puis placez-vous à sa racine :

```sh
git clone https://github.com/Mithweth/infernal-house-3ds.git
cd infernal-house-3ds
```

Toutes les commandes `make` présentées ci-dessous doivent être exécutées depuis ce répertoire, où se trouve le `Makefile`.

## 3. Compiler le jeu (`.3dsx`)

La compilation standard tient en une commande :

```sh
make
```

Elle produit notamment :

| Fichier | Utilisation |
| --- | --- |
| `infernal-house.3dsx` | Exécutable pour le **Homebrew Launcher** et les émulateurs compatibles |
| `infernal-house.smdh` | Métadonnées et icône du programme |
| `infernal-house.elf` | Exécutable intermédiaire, utile notamment au débogage |

Le `Makefile` s'occupe également de préparer les ressources du jeu. Il génère les descriptions de spritesheets `gfx.t3s`, convertit les images PNG en textures `gfx.t3x` avec `tex3ds`, puis construit le répertoire `romfs/` contenant les données nécessaires au jeu : pièces, séquences, langues, sons, inventaire, etc.

**Il n'est pas nécessaire de construire la RomFS à la main** : elle est embarquée dans le `.3dsx` pendant la compilation.

### Nettoyer et recompiler

```sh
make clean
make
```

`make clean` supprime les fichiers de compilation et les ressources générées, sans supprimer les fichiers sources de `resources/`.

### Compilation avec `DEBUG`

Le `Makefile` permet d'activer la macro C `DEBUG` :

```sh
make clean
make DEBUG=1
```

Une cible d'analyse statique est également disponible :

```sh
make lint
```

Elle utilise l'analyseur `-fanalyzer` de GCC ; elle ne remplace pas un test du jeu sur console ou émulateur.

## 4. Produire un paquet installable (`.cia`)

Le format `.cia` permet d'installer Infernal House directement dans le menu HOME d'une Nintendo 3DS disposant d'un environnement homebrew adapté.

La génération du paquet nécessite **`makerom`**, qui doit être accessible dans le `PATH`. Cet outil peut être obtenu à partir du projet [Project_CTR](https://github.com/3DSGuy/Project_CTR). Il n'est pas nécessaire pour compiler le `.3dsx`.

Compilez d'abord le jeu, puis générez le paquet :

```sh
make
make cia
```

Le résultat est :

```text
infernal-house.cia
```

La bannière du menu HOME est conservée dans `resources/cia/banner.bnr`. Le `Makefile` l'utilise telle quelle lors de la création du `.cia` : **`bannertool` n'est donc pas requis pour une compilation normale**, tant que ce fichier est présent.

Si vous modifiez `resources/cia/banner.png` ou `resources/cia/banner.wav`, vous pouvez régénérer la bannière avec :

```sh
make banner
```

Cette commande nécessite `bannertool` dans le `PATH`. Le fichier `resources/cia/banner.bnr` ainsi produit pourra ensuite être utilisé par `make cia`.

### Version de l'application

La version est définie par la variable `APP_VERSION` du `Makefile` (par défaut `1.0.0`). Pour construire explicitement une version :

```sh
make clean
make APP_VERSION=1.0.0
make cia APP_VERSION=1.0.0
```

La même version est utilisée pour l'affichage dans le jeu et pour les informations de version du paquet CIA.

## 5. Tester le jeu

### Avec un émulateur

Le fichier `.3dsx` peut être ouvert dans un émulateur Nintendo 3DS compatible, comme **Azahar**.

Sous Linux, si Azahar est installé via Flatpak, vous pouvez le lancer depuis la racine du projet avec :

```sh
flatpak run --filesystem="$(pwd):ro" org.azahar_emu.Azahar "$(pwd)/infernal-house.3dsx"
```

L'option `--filesystem` autorise l'émulateur à lire le répertoire du projet pour cette exécution, sans modifier ses permissions Flatpak de manière permanente.

### Sur une Nintendo 3DS

Deux possibilités :

- **`.3dsx`** : copiez `infernal-house.3dsx` sur la carte SD, dans un répertoire accessible depuis le **Homebrew Launcher** (par exemple `/3ds/infernal-house/`), puis lancez-le depuis celui-ci.
- **`.cia`** : copiez `infernal-house.cia` sur la carte SD, installez-le avec un gestionnaire de titres adapté, tel que **FBI**, puis lancez-le depuis le menu HOME.

Le `.3dsx` et le `.cia` contiennent les ressources RomFS nécessaires au fonctionnement du jeu ; il n'est pas nécessaire de copier séparément le répertoire `resources/` sur la carte SD.

## 6. Problèmes courants

| Symptôme | Vérification |
| --- | --- |
| `Please set DEVKITARM in your environment` | Vérifiez `DEVKITPRO` et `DEVKITARM`, puis rouvrez votre terminal après l'installation de devkitPro. |
| `arm-none-eabi-gcc: command not found` | Vérifiez l'installation du groupe `3ds-dev` et l'environnement devkitPro. |
| `tex3ds: command not found` | Vérifiez que le groupe `3ds-dev` est installé et que les outils devkitPro sont dans le `PATH`. |
| `GLIBC_2.38 not found` (ou erreur similaire) | Votre distribution Linux est trop ancienne pour les outils devkitPro installés. Utilisez l’image Docker `devkitpro/devkitarm:latest`. |
| Erreur de liaison concernant `vorbisidec` ou `ogg` | Vérifiez l'installation de `3ds-libvorbisidec` et de ses dépendances. |
| `makerom not found in PATH` | Installez `makerom` pour générer le `.cia`, ou compilez seulement le `.3dsx` avec `make`. |
| `Missing resources/cia/banner.bnr` | Régénérez la bannière avec `make banner` (nécessite `bannertool`). |
| Les images ou ressources semblent obsolètes | Exécutez `make clean`, puis recompilez. |

## 7. Pour aller plus loin

La compilation et le fonctionnement interne du moteur sont deux sujets distincts. Pour comprendre l’architecture du projet et apprendre à modifier les pièces, les interactions, les séquences ou l’interface, consultez [docs/DEVELOPING.fr.md](./DEVELOPING.fr.md), qui présente les autres documents techniques.

Pour les commandes de jeu et les mécaniques de l'aventure, voir [HOWTOPLAY.fr.md](./HOWTOPLAY.fr.md).
