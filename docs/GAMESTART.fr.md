# Fichier de configuration du début de partie

Ce document décrit le fichier `autorun.inf`, qui définit comment démarre
une nouvelle partie : la première salle, la musique de fond et les objets
déjà présents dans l'inventaire.

## 1. Emplacement et chargement

Le fichier se trouve dans :

```text
resources/game/autorun.inf
```

La compilation copie tous les fichiers de `resources/game/` dans la
RomFS, et le jeu le lit depuis :

```text
romfs:/game/autorun.inf
```

Le fichier est lu **une seule fois**, au lancement du jeu, avant
l'initialisation de l'inventaire et du HUD. Si le fichier est absent ou
invalide, le jeu ne démarre pas.

Son contenu est ensuite appliqué **à chaque nouvelle partie** lancée
depuis l'écran titre :

1. l'inventaire, les états de jeu et le HUD sont réinitialisés ;
2. la musique est lancée, si `music` est renseigné ;
3. les objets listés dans `items` sont ajoutés à l'inventaire ;
4. le joueur entre dans la salle indiquée par `open`.

## 2. Syntaxe générale

Le fichier suit une syntaxe INI simplifiée :

```ini
[AutoRun]
open=hall
music=romfs:/audio/background.ogg
items=
```

- Chaque paramètre est une ligne `clé=valeur`.
- Les espaces autour de la clé, autour du `=` et en fin de valeur sont
  ignorés.
- Les clés sont sensibles à la casse : `open` est valide, `Open` ne
  l'est pas.
- Les lignes vides sont ignorées.
- Une ligne dont le premier caractère non blanc est `#` ou `;` est un
  commentaire. Les commentaires doivent être sur leur propre ligne : les
  commentaires en fin de ligne ne sont pas pris en charge et font partie
  de la valeur.
- Une ligne commençant par `[` est un en-tête de section. Les en-têtes
  sont ignorés : `[AutoRun]` n'est là que pour la lisibilité, et les
  clés sont lues où qu'elles soient dans le fichier.
- Les lignes sans `=` et les clés inconnues sont ignorées sans message.
- Une ligne ne doit pas dépasser 511 caractères.

## 3. Clés

| Clé     | Obligatoire | Description                                          |
|---------|-------------|------------------------------------------------------|
| `open`  | oui         | Salle dans laquelle démarre une nouvelle partie.     |
| `music` | non         | Musique de fond jouée pendant la partie.             |
| `items` | non         | Objets déjà dans l'inventaire au début de la partie. |

Chaque clé ne peut apparaître qu'une seule fois. Une clé en double fait
échouer le chargement.

### 3.1. open

```ini
open=hall
```

Nom de la salle de départ, c'est-à-dire le nom de son répertoire dans
`resources/rooms/` (chargée depuis `romfs:/rooms/<nom>`).

Cette clé est obligatoire : sans elle, le chargement échoue avec
`Missing open in game configuration`.

La salle n'est chargée qu'au démarrage d'une partie, pas à la lecture du
fichier. Une faute de frappe dans le nom de la salle n'est donc signalée
qu'à ce moment-là, par `Cannot enter room: <nom>`.

### 3.2. music

```ini
music=romfs:/audio/background.ogg
```

Chemin RomFS complet d'un fichier Ogg Vorbis (mono ou stéréo). La musique
est lancée au début d'une partie, puis relancée quand le joueur quitte un
mini-jeu.

Sans cette clé, le jeu est silencieux : aucune musique n'est jouée, ni au
début de la partie ni après un mini-jeu.

### 3.3. items

```ini
items=FLASHLIGHT, SCREWDRIVER
```

Liste d'identifiants d'objets séparés par des virgules, tels que déclarés
par les directives `ITEM` de `resources/inventory/inventory`. Ils sont
ajoutés à l'inventaire dans l'ordre de la liste.

- Les espaces autour de chaque identifiant sont ignorés, et les entrées
  vides sont sautées.
- Seuls les 16 premiers objets sont pris en compte ; les suivants sont
  ignorés.
- Un identifiant inconnu n'empêche pas la partie de démarrer : le jeu
  affiche `Unknown inventory item: <id>` et passe à l'objet suivant.
- Un objet listé deux fois n'est ajouté qu'une seule fois.
- Une valeur vide (`items=`) ou une clé absente signifie que l'inventaire
  démarre vide.

## 4. Exemple complet

```ini
# Configuration d'une nouvelle partie
[AutoRun]

# Première salle
open=hall

# Musique de fond
music=romfs:/audio/background.ogg

# Inventaire de départ
items=FLASHLIGHT, MAGNIFYING_GLASS
```

## 5. Erreurs fréquentes

### Mettre un commentaire en fin de ligne

```ini
open=hall # salle de départ
```

Le nom de la salle devient `hall # salle de départ`. Placez le
commentaire sur sa propre ligne.

### Donner un chemin relatif pour la musique

```ini
music=audio/background.ogg
```

Le chemin est utilisé tel quel : indiquez toujours le chemin complet
`romfs:/...`.

### Utiliser le nom de l'objet au lieu de son identifiant

`items` attend l'identifiant déclaré après `ITEM` (par exemple
`FLASHLIGHT`), ni la clé de traduction ni le nom de l'image.

### Répartir les objets sur plusieurs lignes

```ini
items=FLASHLIGHT
items=SCREWDRIVER
```

La seconde ligne est une clé en double et fait échouer le chargement.
Listez tous les objets sur une seule ligne.
