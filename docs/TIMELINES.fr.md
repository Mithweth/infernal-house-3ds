# Fichiers de description des timelines

Ce document décrit le format déclaratif utilisé pour définir les
timelines du jeu.

Une timeline est une séquence scriptée de textes localisés, d’images, de
pauses, de musique, d’effets sonores et de compositions plein écran. Les
timelines sont utilisées pour des séquences telles que l’introduction,
les fins du jeu et les scènes de game over.

Le moteur reste responsable de l’analyse du fichier et de l’exécution
des événements déclarés, dans l’ordre.

## 1. Emplacement et structure générale

Une timeline nommée `intro` est chargée depuis :

``` text
romfs:/timelines/intro/timeline
```

Ses ressources graphiques sont chargées depuis l’ensemble de ressources
correspondant à cette timeline.

Une timeline simple peut ressembler à ceci :

``` text
MUSIC_START intro

IMAGE_LEFT scene1
TEXT RED TITLE_INTRO_SCENE_1
PAUSE 2000
END_SCENE

MUSIC_STOP
END
```

Les lignes vides sont ignorées. Une ligne dont le premier caractère non
blanc est `#` est un commentaire. Les commentaires doivent être placés
sur leur propre ligne ; les commentaires en fin de ligne ne font pas
partie du format.

Les éléments sont séparés par des espaces. Les noms d’images, les
identifiants de localisation et les chemins de ressources ne contiennent
donc pas d’espaces et ne sont pas placés entre guillemets.

Les événements sont exécutés séquentiellement, dans leur ordre de
déclaration.

Chaque timeline doit se terminer par :

``` text
END
```

`END` termine la timeline. Il ne faut pas le confondre avec `END_SCENE`,
qui efface uniquement la scène visuelle courante et permet à la timeline
de continuer.

## 2. Affichage standard d’une scène

En dehors d’un bloc `FULL_SCREEN`, une timeline utilise l’affichage
standard :

- l’écran supérieur peut contenir jusqu’à trois images : gauche, centre
  et droite ;
- l’écran inférieur contient le texte de la timeline.

Les images et le texte courants restent affichés pendant l’exécution des
événements suivants. Ils ne sont pas automatiquement effacés par
`PAUSE`, les événements audio ou l’ajout d’un nouveau texte.

`END_SCENE` efface la scène courante :

``` text
END_SCENE
```

Il supprime :

- les images gauche, centre et droite ;
- le texte accumulé ;
- toute composition plein écran active.

Il **n’arrête pas** la musique en cours. Utilisez explicitement
`MUSIC_STOP` lorsque la musique doit s’arrêter.

## 3. Texte

Syntaxe :

``` text
TEXT <color> <message_id>
```

Exemple :

``` text
TEXT BLUE TITLE_INTRO_SCENE_1
```

`message_id` est une clé de localisation. La chaîne affichée est obtenue
par le système de langues habituel.

Les couleurs prises en charge sont :

``` text
WHITE
RED
BLUE
YELLOW
GREEN
```

Le texte est affiché progressivement, caractère par caractère.

Plusieurs événements `TEXT` appartenant à la même scène sont cumulatifs.
Un nouveau `TEXT` continue après le texte déjà affiché ; il n’efface pas
le texte précédent.

Par exemple :

``` text
TEXT RED TITLE_INTRO_SCENE_3
IMAGE_CENTER scene4
PAUSE 100
TEXT RED TITLE_INTRO_SCENE_4
PAUSE 2000
END_SCENE
```

Les deux événements `TEXT` appartiennent à la même scène. `END_SCENE`
efface le texte avant le début de la scène suivante.

## 4. Images standard

Trois directives contrôlent les emplacements d’images sur l’écran
supérieur :

``` text
IMAGE_LEFT <image>
IMAGE_CENTER <image>
IMAGE_RIGHT <image>
```

Exemple :

``` text
IMAGE_LEFT scene1
IMAGE_CENTER scene2
IMAGE_RIGHT scene3
```

`image` est le nom de l’image dans l’ensemble de ressources graphiques
de la timeline courante. Il correspond au nom du fichier PNG sans
l’extension `.png`. Par exemple, `scene1` fait référence à `scene1.png`.

Une image reste dans son emplacement jusqu’à ce qu’elle soit remplacée,
explicitement supprimée ou que la scène se termine.

Utilisez `NONE` pour effacer un seul emplacement sans effacer le reste
de la scène :

``` text
IMAGE_CENTER NONE
```

Par exemple :

``` text
IMAGE_LEFT scene10
IMAGE_CENTER scene11
IMAGE_RIGHT scene12
TEXT BLUE TITLE_INTRO_SCENE_16

IMAGE_CENTER NONE
TEXT BLUE TITLE_INTRO_SCENE_17

PAUSE 2000
END_SCENE
```

Seule l’image centrale disparaît ; les images gauche et droite ainsi que
le texte restent actifs.

## 5. Pauses

Syntaxe :

``` text
PAUSE <milliseconds>
```

Exemple :

``` text
PAUSE 2000
```

La timeline attend pendant la durée indiquée avant de poursuivre avec
l’événement suivant.

Une durée de `0` crée une pause indéfinie :

``` text
PAUSE 0
```

Une pause indéfinie ne se termine pas automatiquement. Le joueur doit
appuyer sur **B** pour poursuivre la timeline.

Une pause n’efface ni ne modifie la scène courante. Les images, le texte
et les compositions plein écran restent donc visibles pendant la pause.

Maintenir **A** accélère la lecture de la timeline. Cela accélère à la
fois l’affichage progressif du texte et les pauses temporisées.

## 6. Musique et effets sonores

### MUSIC_START

Syntaxe :

``` text
MUSIC_START <path>
```

Exemple :

``` text
MUSIC_START intro
MUSIC_START romfs:/audio/ending.ogg
```

Démarre la musique indiquée et poursuit immédiatement avec l’événement
suivant de la timeline.

La musique peut être indiquée soit par un nom relatif au répertoire de
la timeline, soit par un chemin absolu `romfs:/`. Les noms relatifs sont
résolus depuis le répertoire de la timeline et reçoivent automatiquement
l’extension `.ogg`. Les chemins absolus sont utilisés tels quels.

La musique continue indépendamment des limites de scènes. `END_SCENE` ne
l’arrête pas.

### MUSIC_STOP

Syntaxe :

``` text
MUSIC_STOP
```

Arrête la musique en cours et poursuit immédiatement.

Exemple :

``` text
MUSIC_STOP
FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
END_FULL_SCREEN
```

### SFX

Syntaxe :

``` text
SFX <path>
```

Exemple :

``` text
SFX footsteps
SFX romfs:/audio/title_choice.raw
```

Démarre un effet sonore et poursuit immédiatement avec l’événement
suivant. `SFX` n’attend pas la fin du son.

Les effets sonores peuvent être indiqués soit par un nom relatif au
répertoire de la timeline, soit par un chemin absolu `romfs:/`. Les noms
relatifs sont résolus depuis le répertoire de la timeline et reçoivent
automatiquement l’extension `.raw`. Les chemins absolus sont utilisés
tels quels.

Ajoutez un `PAUSE` lorsque la timeline doit rester sur la scène courante
pendant une durée déterminée :

``` text
SFX footsteps
PAUSE 2000
```

## 7. Compositions plein écran

`FULL_SCREEN` décrit une composition pouvant utiliser les deux écrans de
la 3DS.

Syntaxe :

``` text
FULL_SCREEN
    SPRITE <image> <x> <y>
    [SPRITE <image> <x> <y> ...]
END_FULL_SCREEN
```

Exemple :

``` text
FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE far_man 94 240
END_FULL_SCREEN

PAUSE 1500
```

Une composition `FULL_SCREEN` prend en charge jusqu’à **16 sprites** par
défaut.

Une composition plein écran utilise un espace de coordonnées logique de
**320 × 480** :

``` text
y =   0 .. 239   écran supérieur
y = 240 .. 479   écran inférieur
```

L’écran supérieur mesure physiquement 400 pixels de large. La
composition logique de 320 pixels y est centrée, ce qui permet
d’utiliser la même largeur de coordonnées de 320 pixels pour les deux
écrans.

Par exemple :

``` text
SPRITE top_background    0   0
SPRITE bottom_background 0 240
```

place un arrière-plan sur chacun des deux écrans.

Les sprites sont déclarés de l’arrière-plan vers le premier plan. Les
sprites déclarés plus tard sont dessinés au-dessus des précédents.

### FULL_SCREEN décrit une composition complète

Un événement `FULL_SCREEN` remplace la composition plein écran
précédente. Les sprites ne sont pas hérités du `FULL_SCREEN` précédent.

Les éléments persistants, tels que les arrière-plans, doivent donc être
répétés :

``` text
FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE far_man 94 240
END_FULL_SCREEN
PAUSE 1500

FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE close_man 144 264
END_FULL_SCREEN
PAUSE 1500
```

`END_FULL_SCREEN` ferme uniquement la déclaration `FULL_SCREEN` dans le
fichier de timeline. Il **n’efface pas** la composition de l’affichage.
La composition reste visible jusqu’à ce qu’un autre `FULL_SCREEN` la
remplace ou que `END_SCENE` l’efface.

Seules les directives `SPRITE` sont valides entre `FULL_SCREEN` et
`END_FULL_SCREEN`.

## 8. Limites de scènes

`END_SCENE` constitue la limite habituelle entre deux scènes visuelles :

``` text
TEXT BLUE TITLE_INTRO_SCENE_1
PAUSE 2000
END_SCENE

TEXT RED TITLE_INTRO_SCENE_2
PAUSE 1500
END_SCENE
```

Sans `END_SCENE`, le second `TEXT` continuerait dans la même scène et
serait ajouté au texte déjà affiché.

La même règle s’applique aux images standard : elles restent présentes
jusqu’à ce qu’elles soient remplacées, supprimées avec `NONE` ou
effacées par `END_SCENE`.

Pour les compositions plein écran, `END_SCENE` efface également la
composition active et ramène l’affichage à une scène standard vide.

Là encore, `END_SCENE` n’affecte que la scène visuelle. La musique
continue jusqu’à `MUSIC_STOP` ou jusqu’à la fermeture de la timeline
elle-même.

## 9. Contrôles du joueur

Les timelines disposent de deux contrôles de lecture.

### A — lecture rapide

Maintenir **A** accélère :

- l’affichage progressif du texte ;
- la durée des `PAUSE`.

La timeline suit sinon exactement la même séquence d’événements.

### B — passer

Appuyer sur **B** fait avancer la timeline.

Lorsque l’événement courant est un `TEXT`, le reste du texte de cet
événement est affiché immédiatement avant de passer à la suite.

Si l’événement suivant immédiatement celui qui est passé est un `PAUSE`,
cette pause est également passée.

Cela permet au joueur de faire avancer les dialogues sans laisser une
phrase partiellement affichée à l’écran.

## 10. Directives disponibles

| Directive                   | Effet                                                   |
|-----------------------------|---------------------------------------------------------|
| `TEXT <color> <message_id>` | Affiche progressivement un texte localisé.              |
| `PAUSE <milliseconds>`      | Attend avant de poursuivre.                             |
| `MUSIC_START <path>`        | Démarre la musique et poursuit immédiatement.           |
| `MUSIC_STOP`                | Arrête la musique en cours.                             |
| `SFX <path>`                | Démarre un effet sonore et poursuit immédiatement.      |
| `IMAGE_LEFT <image>`        | Définit l’image gauche de l’écran supérieur.            |
| `IMAGE_CENTER <image>`      | Définit l’image centrale de l’écran supérieur.          |
| `IMAGE_RIGHT <image>`       | Définit l’image droite de l’écran supérieur.            |
| `IMAGE_* NONE`              | Efface l’emplacement d’image standard correspondant.    |
| `FULL_SCREEN`               | Commence la déclaration d’une composition plein écran.  |
| `SPRITE <image> <x> <y>`    | Ajoute un sprite à la composition plein écran courante. |
| `END_FULL_SCREEN`           | Termine la déclaration de la composition plein écran.   |
| `END_SCENE`                 | Efface le texte et la scène visuelle courants.          |
| `END`                       | Termine la timeline.                                    |

## 11. Exemple complet

``` text
# Démarre la musique de l'introduction.

MUSIC_START intro

# Scène standard : texte sur l'écran inférieur.

TEXT BLUE TITLE_INTRO_SCENE_1
PAUSE 2000
END_SCENE

# Scène standard avec trois images sur l'écran supérieur.

IMAGE_LEFT scene1
TEXT RED TITLE_INTRO_SCENE_3

IMAGE_CENTER scene4
PAUSE 100
IMAGE_CENTER scene5
PAUSE 100
IMAGE_CENTER scene6

TEXT RED TITLE_INTRO_SCENE_4
PAUSE 2000
END_SCENE

# Arrête la musique et passe aux compositions plein écran.

MUSIC_STOP
SFX footsteps

FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE far_man 94 240
END_FULL_SCREEN
PAUSE 1500

FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE close_man 144 264
END_FULL_SCREEN
PAUSE 1500

FULL_SCREEN
    SPRITE bottom_background 0 240
    SPRITE top_background 0 0
    SPRITE logo_infernal 15 62
    SPRITE logo_house 57 318
END_FULL_SCREEN
PAUSE 5000

END_SCENE
END
```

## 12. Résumé du modèle d’exécution

Le format des timelines est volontairement séquentiel.

Les règles principales sont :

1.  Les événements sont exécutés dans leur ordre de déclaration.
2.  `TEXT` ajoute du texte à la scène courante jusqu’à `END_SCENE`.
3.  Les images standard restent présentes jusqu’à ce qu’elles soient
    remplacées, supprimées avec `NONE` ou effacées par `END_SCENE`.
4.  `PAUSE` conserve l’affichage courant pendant l’attente.
5.  La musique est indépendante des limites de scènes.
6.  `SFX` n’attend pas la fin du son.
7.  Un bloc `FULL_SCREEN` décrit une composition complète.
8.  Une composition plein écran reste visible après `END_FULL_SCREEN`.
9.  Un nouveau `FULL_SCREEN` remplace la composition précédente.
10. `END_SCENE` efface l’état visuel courant mais poursuit la timeline.
11. `END` termine la timeline.

## 13. Erreurs fréquentes

### Oublier END_SCENE entre deux scènes de texte

Incorrect :

``` text
TEXT RED FIRST_SCENE
PAUSE 2000
TEXT BLUE SECOND_SCENE
```

`SECOND_SCENE` est ajouté au texte déjà affiché.

Correct :

``` text
TEXT RED FIRST_SCENE
PAUSE 2000
END_SCENE

TEXT BLUE SECOND_SCENE
```

### S’attendre à ce que END_FULL_SCREEN efface l’affichage

Supposition incorrecte :

``` text
FULL_SCREEN
    SPRITE background 0 0
END_FULL_SCREEN

PAUSE 2000
```

L’arrière-plan reste visible pendant la pause. `END_FULL_SCREEN` ferme
la déclaration ; ce n’est pas une opération d’effacement à l’exécution.

Utilisez `END_SCENE` lorsque la composition doit disparaître.

### Oublier les sprites persistants dans un nouveau FULL_SCREEN

Chaque `FULL_SCREEN` est une composition complète.

Si un arrière-plan doit rester visible pendant qu’un personnage change
de position, répétez l’arrière-plan dans chaque composition.

### Utiliser SPRITE en dehors de FULL_SCREEN

`SPRITE` ne peut être utilisé qu’à l’intérieur de :

``` text
FULL_SCREEN
    ...
END_FULL_SCREEN
```

### S’attendre à ce que SFX attende

Ceci :

``` text
SFX footsteps
FULL_SCREEN
    ...
END_FULL_SCREEN
```

démarre le son et passe immédiatement à l’événement plein écran.

Ajoutez explicitement un `PAUSE` lorsqu’une temporisation est
nécessaire.

### Oublier END

Chaque timeline valide doit se terminer par `END`.

## 14. Style recommandé

Utilisez des lignes vides pour séparer les scènes et regroupez les
événements visuels et temporels associés.

Pour les scènes standard, un ordre lisible est généralement :

``` text
images
text
pause
END_SCENE
```

Pour les séquences plein écran, décrivez chaque image comme une
composition complète et placez sa temporisation immédiatement après :

``` text
FULL_SCREEN
    sprites
END_FULL_SCREEN
PAUSE ...
```

Conservez les traductions dans les fichiers de langues en utilisant des
identifiants de messages avec `TEXT`, plutôt que d’intégrer directement
le texte affiché dans les descripteurs de timelines.

Le format des timelines est volontairement un petit DSL spécifique au
jeu. Il décrit **ce qui se passe et dans quel ordre** ; le rendu, la
localisation, la lecture audio et la gestion des entrées restent de la
responsabilité du moteur.
