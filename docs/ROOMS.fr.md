# Fichiers de description des Rooms

Ce document décrit le format déclaratif utilisé pour définir les Rooms
du jeu.

Une description de Room remplace les anciens fichiers `room_<name>.c`
pour les données propres à la Room : images affichées, hotspots,
conditions, interactions, utilisation des objets de l’inventaire et
sorties.

Le moteur reste responsable de l’analyse du fichier et de l’exécution du
comportement déclaré.

## 1. Emplacement et structure générale

Une Room nommée `livingroom` est chargée depuis :

``` text
romfs:/rooms/livingroom/room
```

Ses ressources graphiques sont chargées depuis le jeu de ressources
correspondant à la Room.

Une Room contient généralement trois sections :

``` text
# Images

IMAGE bg 0 0 0
END_IMAGE

# Hotspots

HOTSPOT EXAMPLE_OBJECT 10 20 40 30
    MESSAGE EXAMPLE_OBJECT_EXAMINE
END_HOTSPOT

# Sorties

PATH SOUTH
    ACTION
        WAIT_SFX door_open
        ROOM corridor
    END_ACTION
END_PATH
```

Les lignes vides sont ignorées. Une ligne dont le premier caractère non
blanc est `#` est un commentaire. Les commentaires doivent être placés
sur leur propre ligne ; les commentaires en fin de ligne ne font pas
partie du format.

Les éléments sont séparés par des espaces. Les identifiants tels que les
noms d’états, d’objets, de messages, de Rooms, de sons et d’images ne
contiennent donc pas d’espaces et ne sont pas placés entre guillemets.

L’indentation sert uniquement à améliorer la lisibilité ; la structure
des blocs est déterminée par les directives `END_*`.

## 2. Conditions

Les conditions sont introduites par `WHEN` :

``` text
WHEN STATE_IS <state> <true|false>
WHEN INVENTORY_HAS <item> <true|false>
```

Exemples :

``` text
WHEN STATE_IS underground_dug true
WHEN INVENTORY_HAS SHOVEL false
```

Plusieurs directives `WHEN` dans un même bloc sont combinées avec un
**ET** logique : toutes les conditions doivent être satisfaites.

Une condition s’applique au bloc dans lequel elle apparaît. Elle peut
donc contrôler un `IMAGE`, un `HOTSPOT`, un `ACTION`, un `USE` ou un
`PATH`.

Les identifiants d’états et d’objets doivent correspondre à des
identifiants connus respectivement par les systèmes de gestion des états
du jeu et de l’inventaire.

## 3. Images

Syntaxe :

``` text
IMAGE <image> <x> <y> <z>
    [WHEN ...]
END_IMAGE
```

Exemple :

``` text
IMAGE hole_dug 261 174 0.3
    WHEN STATE_IS underground_dug true
    WHEN STATE_IS underground_card_taken false
END_IMAGE
```

`image` est le nom du fichier image sans son extension. Dans l’exemple
précédent, `hole_dug` désigne l’image `hole_dug.png`. Les noms d’images
peuvent contenir des lettres, des chiffres et des underscores (`_`) ;
les tirets (`-`) ne doivent pas être utilisés.

`x` (de 0 à 320) et `y` (de 0 à 240) sont les coordonnées d’affichage.
`z` (de -1.0 à 1.0) contrôle la profondeur d’affichage.

L’image n’est affichée que lorsque toutes ses conditions sont vraies.
Sans `WHEN`, l’image est toujours affichée.

Les images conditionnelles sont indépendantes. Si les conditions de deux
blocs `IMAGE` sont vraies, les deux images sont affichées. Lors de la
conversion de code C contenant un `if ... else if ...`, les conditions
doivent donc rendre explicitement les alternatives mutuellement
exclusives.

Par exemple, ce code C :

``` c
if (gamestate_get("card_taken")) {
    draw(empty);
} else {
    draw(full);
}
```

doit devenir quelque chose d’équivalent à :

``` text
IMAGE full ...
    WHEN STATE_IS card_taken false
END_IMAGE

IMAGE empty ...
    WHEN STATE_IS card_taken true
END_IMAGE
```

## 4. Hotspots

Syntaxe :

``` text
HOTSPOT <id> <x> <y> <width> <height>
    [WHEN ...]
    [MESSAGE <message_id>]
    [ACTION ... END_ACTION]
    [USE ... END_USE]
END_HOTSPOT
```

Exemple :

``` text
HOTSPOT UNDERGROUND_MAGNETIC_CARD 275 182 24 14
    WHEN STATE_IS underground_card_taken false
    WHEN STATE_IS underground_dug true
    ACTION
        SET underground_card_taken
        INVENTORY_ADD MAGNETIC_CARD
    END_ACTION
END_HOTSPOT
```

Le rectangle est défini par `x`, `y`, `width` et `height`.

Les directives `WHEN` placées au niveau du hotspot déterminent si
celui-ci est actif du point de vue du joueur. Un hotspot inactif est
ignoré lors de la détection de la zone sélectionnée.

### L’ordre des hotspots est important

Les hotspots sont testés dans leur ordre de déclaration. Le premier
hotspot actif dont le rectangle contient le point sélectionné est
retenu.

Ce comportement est notamment utilisé lorsque des hotspots se
chevauchent. Un grand hotspot générique doit donc normalement être
déclaré **après** les hotspots plus petits et plus spécifiques qu’il
recouvre.

Par exemple :

``` text
HOTSPOT UNDERGROUND_X_FORM 141 190 17 16
    ...
END_HOTSPOT

HOTSPOT UNDERGROUND_GROUND 45 171 275 68
    ...
END_HOTSPOT
```

Déclarer le grand hotspot du sol en premier masquerait le petit hotspot
en forme de X.

## 5. Messages d’examen

Un `MESSAGE` placé directement dans un `HOTSPOT`, en dehors d’un
`ACTION` ou d’un `USE`, définit le message affiché lorsque le joueur
examine l’objet :

``` text
HOTSPOT LIVINGROOM_FIREPLACE 163 72 26 17
    MESSAGE LIVINGROOM_FIREPLACE_EXAMINE
END_HOTSPOT
```

C’est différent d’un `MESSAGE` utilisé comme action :

``` text
ACTION
    MESSAGE CELLAR_DISABLE_ALARM_BOX
END_ACTION
```

Dans ce cas, le message est affiché lorsque le bloc d’actions est
exécuté.

## 6. Blocs ACTION

Un bloc `ACTION` décrit ce qui se produit lorsque le joueur effectue
l’action normale sur un hotspot :

``` text
HOTSPOT UNDERGROUND_SHOVEL 18 89 37 100
    WHEN INVENTORY_HAS SHOVEL false
    ACTION
        INVENTORY_ADD SHOVEL
    END_ACTION
END_HOTSPOT
```

Un `ACTION` peut lui-même posséder des conditions :

``` text
ACTION
    WHEN STATE_IS cellar_alarm_box_unscrewed true
    SET cellar_alarm_box_opened
END_ACTION
```

### Plusieurs blocs ACTION sont séquentiels

Les blocs `ACTION` ne sont **pas** des alternatives de type
`if / else if`.

Tous les blocs d’actions dont les conditions correspondent sont évalués
dans leur ordre de déclaration. Les conditions sont réévaluées lorsque
chaque bloc est atteint : un bloc précédent peut donc modifier l’état du
jeu et influencer les conditions d’un bloc suivant.

Ce comportement est intentionnel et utile pour les transitions d’état :

``` text
ACTION
    SET diningroom_lasers_disabled
END_ACTION

ACTION
    WHEN STATE_IS diningroom_lasers_disabled true
    MESSAGE CELLAR_DISABLE_ALARM_BOX
END_ACTION

ACTION
    WHEN STATE_IS diningroom_lasers_disabled false
    MESSAGE CELLAR_ENABLE_ALARM_BOX
END_ACTION
```

Si `diningroom_lasers_disabled` est un état de type `TOGGLE`, le premier
bloc modifie sa valeur. Les blocs suivants examinent ensuite cette
**nouvelle** valeur.

Il ne faut donc pas interpréter ou réécrire ce type de construction
comme « seul le premier bloc correspondant est exécuté ».

## 7. Blocs USE

`USE` décrit l’utilisation d’un objet de l’inventaire sur un hotspot.

Syntaxe :

``` text
USE <item>
    [WHEN ...]
    <actions>
END_USE
```

Exemple :

``` text
USE SCREWDRIVER
    WHEN STATE_IS cellar_alarm_box_unscrewed false
    SET cellar_alarm_box_unscrewed
    MESSAGE CELLAR_OPEN_ALARM_BOX
END_USE
```

Un bloc `USE` contient directement des actions. Il n’y a **pas de bloc
`ACTION` imbriqué** dans un `USE`.

Incorrect :

``` text
USE SCREWDRIVER
    ACTION
        SET cellar_alarm_box_unscrewed
    END_ACTION
END_USE
```

Correct :

``` text
USE SCREWDRIVER
    SET cellar_alarm_box_unscrewed
END_USE
```

Cette distinction est importante : `ACTION` appartient à la grammaire du
hotspot ou du chemin englobant, et non à celle de `USE`.

### Plusieurs blocs USE

Plusieurs blocs `USE` peuvent faire référence au même objet et utiliser
des conditions pour sélectionner le comportement approprié :

``` text
USE SCREWDRIVER
    WHEN STATE_IS cellar_alarm_box_unscrewed false
    SET cellar_alarm_box_unscrewed
    MESSAGE CELLAR_OPEN_ALARM_BOX
END_USE

USE SCREWDRIVER
    WHEN STATE_IS cellar_alarm_box_unscrewed true
    MESSAGE CELLAR_ALARM_BOX_ALREADY_OPENED
END_USE
```

Contrairement aux blocs `ACTION` d’un hotspot, les blocs `USE` sont des
alternatives : le premier bloc correspondant à l’objet et dont toutes
les conditions sont satisfaites est exécuté, puis le traitement de
l’utilisation de l’objet s’arrête.

### USE générique

`USE *` correspond à n’importe quel objet de l’inventaire :

``` text
HOTSPOT STUDY_DARK 0 0 320 240
    WHEN STATE_IS study_lights_on false
    MESSAGE STUDY_MESSAGE_NO_LIGHT
    USE *
        MESSAGE STUDY_USE_NO_LIGHT
    END_USE
END_HOTSPOT
```

Les blocs `USE` étant testés dans leur ordre de déclaration, un `USE *`
doit être placé après les traitements d’objets plus spécifiques lorsque
les deux sont présents.

## 8. Chemins et sorties

Un `PATH` déclare une direction de déplacement disponible.

Les directions prises en charge sont :

``` text
NORTH
NORTHEAST
EAST
SOUTHEAST
SOUTH
SOUTHWEST
WEST
NORTHWEST
```

Exemple simple :

``` text
PATH EAST
    ACTION
        ROOM secondunderground
    END_ACTION
END_PATH
```

Un chemin peut posséder des conditions déterminant si la direction est
disponible :

``` text
PATH NORTH
    WHEN STATE_IS livingroom_secret_passage_opened true

    ACTION
        WHEN STATE_IS livingroom_rope_in_hearth_bound false
        WAIT_SFX falling_down
        TIMELINE gameover_falldown
    END_ACTION

    ACTION
        WHEN STATE_IS livingroom_rope_in_hearth_bound true
        WAIT_SFX rope_climbing
        ROOM cryoroom
    END_ACTION
END_PATH
```

Les conditions placées au niveau du `PATH` déterminent si le joueur peut
utiliser cette sortie.

Les conditions placées à l’intérieur des blocs `ACTION` du chemin
déterminent ce qui se produit lorsque le chemin est emprunté.

Les blocs `ACTION` d’un `PATH` suivent la même sémantique séquentielle
que ceux d’un hotspot.

## 9. Actions disponibles

Les actions sont valides à l’intérieur des blocs `ACTION` et `USE`.

| Directive                 | Effet                                                                                                  |
|---------------------------|--------------------------------------------------------------------------------------------------------|
| `SET <state>`             | Modifie l’état selon son type déclaré. Pour un `TOGGLE`, inverse la valeur courante.                   |
| `INVENTORY_ADD <item>`    | Ajoute un objet à l’inventaire.                                                                        |
| `INVENTORY_REMOVE <item>` | Retire un objet de l’inventaire.                                                                       |
| `MESSAGE <message_id>`    | Affiche un message localisé du jeu.                                                                    |
| `SFX <name>`              | Lance un effet sonore et continue immédiatement.                                                       |
| `WAIT_SFX <name>`         | Lance un effet sonore et attend sa fin avant de continuer.                                             |
| `ROOM <room>`             | Change de Room. **Termine le flux d’actions courant et doit être la dernière action de son bloc.**     |
| `TIMELINE <name>`         | Lance une timeline. **Termine le flux d’actions courant et doit être la dernière action de son bloc.** |
| `MINIGAME <name>`         | Lance un mini-jeu. **Termine le flux d’actions courant et doit être la dernière action de son bloc.**  |

Les noms des sons sont indiqués sans l’extension `.raw` :

``` text
SFX closet_open
WAIT_SFX metal_ladder
```

Le moteur les résout à partir du répertoire de la room.

### SET ne signifie pas nécessairement « passer à true »

L’effet de `SET` dépend du type de l’état.

Pour un état déclaré comme `TOGGLE`, `SET` **inverse sa valeur
courante** :

- `false` devient `true` ;
- `true` devient `false`.

Par exemple :

``` text
ACTION
    SET livingroom_piano_opened
END_ACTION
```

ouvre un piano fermé si `livingroom_piano_opened` vaut actuellement
`false`, et le ferme si l’état vaut actuellement `true`.

Il ne faut donc pas interpréter `SET foo` comme l’équivalent de
`foo = true` sans vérifier la définition de l’état.

## 10. Effets sonores et flux d’actions

`SFX` lance un effet sonore puis passe immédiatement à l’action suivante
:

``` text
SFX closet_open
SET closet_opened
```

`WAIT_SFX` lance un effet sonore et attend qu’il soit terminé avant de
poursuivre avec l’action suivante **du même bloc** :

``` text
ACTION
    WAIT_SFX metal_ladder
    ROOM cellar
END_ACTION
```

Il ne faut pas compter sur l’exécution de blocs `ACTION` ultérieurs
après un `WAIT_SFX`. Toutes les actions qui doivent suivre le son
doivent être placées après `WAIT_SFX` dans le même bloc.

### Actions terminales

`ROOM`, `TIMELINE` et `MINIGAME` terminent le flux d’actions courant.
Elles doivent donc toujours être la **dernière action de leur bloc**.

Correct :

``` text
ACTION
    WAIT_SFX door_open
    ROOM corridor
END_ACTION
```

Incorrect :

``` text
ACTION
    ROOM corridor
    MESSAGE UNREACHABLE_MESSAGE
END_ACTION
```

Le `MESSAGE` ne sera jamais exécuté, car `ROOM` termine le flux
d’actions.

**Un `MESSAGE` ne doit pas être suivi de `WAIT_SFX`, `ROOM`, `TIMELINE` ou `MINIGAME`.**
Ces actions changent immédiatement le mode du jeu et remplacent le message avant qu'il puisse être affiché. Les actions telles que `SET`, `INVENTORY_ADD`, `INVENTORY_REMOVE` ou `SFX` peuvent en revanche précéder un `MESSAGE`.

## 11. Exemple complet

``` text
# ---------------------------------------------------------------------------
# Images
# ---------------------------------------------------------------------------

IMAGE bg 0 0 0
END_IMAGE

IMAGE hole_dug 261 174 0.3
    WHEN STATE_IS underground_dug true
    WHEN STATE_IS underground_card_taken false
END_IMAGE

IMAGE hole_empty 265 175 0.3
    WHEN STATE_IS underground_dug true
    WHEN STATE_IS underground_card_taken true
END_IMAGE

# ---------------------------------------------------------------------------
# Hotspots
# ---------------------------------------------------------------------------

HOTSPOT UNDERGROUND_MAGNETIC_CARD 275 182 24 14
    WHEN STATE_IS underground_card_taken false
    WHEN STATE_IS underground_dug true
    ACTION
        SET underground_card_taken
        INVENTORY_ADD MAGNETIC_CARD
    END_ACTION
END_HOTSPOT

HOTSPOT UNDERGROUND_GROUND 284 176 12 12
    WHEN STATE_IS underground_dug false
    USE SHOVEL
        SET underground_dug
        MESSAGE UNDERGROUND_DIG
    END_USE
END_HOTSPOT

HOTSPOT UNDERGROUND_X_FORM 141 190 17 16
    MESSAGE UNDERGROUND_X_FORM_EXAMINE
    USE MEASURING_TAPE
        MINIGAME measure
    END_USE
END_HOTSPOT

# Hotspot générique volontairement déclaré après les hotspots plus spécifiques.
HOTSPOT UNDERGROUND_GROUND 45 171 275 68
    USE SHOVEL
        MESSAGE UNDERGROUND_DIG_ANYWHERE
    END_USE
END_HOTSPOT

# ---------------------------------------------------------------------------
# Sorties
# ---------------------------------------------------------------------------

PATH EAST
    ACTION
        ROOM secondunderground
    END_ACTION
END_PATH

PATH NORTH
    ACTION
        WAIT_SFX metal_ladder
        ROOM cellar
    END_ACTION
END_PATH
```

## 12. Résumé du modèle d’exécution

Lorsqu’une Room est active :

1.  Les images dont les conditions sont satisfaites sont affichées.
2.  La détection des hotspots parcourt les hotspots actifs dans leur
    ordre de déclaration et sélectionne le premier rectangle
    correspondant.
3.  L’examen d’un hotspot utilise son `MESSAGE` direct, s’il existe.
4.  L’action normale sur un hotspot parcourt tous ses blocs `ACTION`
    dans leur ordre de déclaration. Chaque bloc dont les conditions sont
    satisfaites est exécuté, sauf si le flux d’actions est suspendu par
    `WAIT_SFX` ou terminé par `ROOM`, `TIMELINE` ou `MINIGAME`.
5.  L’utilisation d’un objet parcourt les blocs `USE` dans leur ordre de
    déclaration et exécute le premier bloc dont l’objet et les
    conditions correspondent.
6.  Un chemin n’existe que lorsque ses conditions de niveau `PATH` sont
    satisfaites ; lorsqu’il est emprunté, ses blocs `ACTION` sont
    évalués dans leur ordre de déclaration.

Le format conserve volontairement un moteur simple : les fichiers de
Room sont supposés être correctement écrits. Les erreurs du parser ou
d’exécution sont signalées par les sorties de debug habituelles plutôt
que masquées derrière une importante couche de validation.

## 13. Erreurs fréquentes

### Imbriquer ACTION dans USE

Ne pas faire ceci :

``` text
USE KEY_ONE
    ACTION
        MESSAGE SOMETHING
    END_ACTION
END_USE
```

Les actions doivent être placées directement dans le bloc `USE`.

### Traiter les blocs ACTION comme des else-if

Plusieurs blocs `ACTION` peuvent être exécutés. Les changements d’état
effectués dans un bloc peuvent modifier les conditions des blocs
suivants.

### Oublier la priorité des hotspots

Un grand hotspot déclaré avant un hotspot plus petit qui le chevauche
peut rendre ce dernier inaccessible.

### Traduire littéralement des images provenant d’un else-if en C

Les images sont évaluées indépendamment. Il faut ajouter des conditions
complémentaires lorsque seule une variante doit être visible.

### Supposer que SET force un booléen à true

Il faut vérifier la définition de l’état. Sur un état `TOGGLE`, `SET`
inverse la valeur courante : `false` devient `true` et `true` devient
`false`.

### Séparer WAIT_SFX et sa suite dans plusieurs blocs ACTION

Les actions qui doivent se produire après un `WAIT_SFX` doivent être
placées dans le même bloc :

``` text
ACTION
    WAIT_SFX door_open
    ROOM hall
END_ACTION
```

### Placer des actions après ROOM, TIMELINE ou MINIGAME

`ROOM`, `TIMELINE` et `MINIGAME` terminent le flux d’actions courant.
Rien ne doit les suivre dans le même bloc.

## 14. Style recommandé

Pour faciliter la lecture, les fichiers de Room devraient normalement
être organisés dans cet ordre :

``` text
Images
Hotspots
Sorties
```

Utilisez des commentaires de section et indentez les directives
imbriquées de manière cohérente. Placez les hotspots spécifiques avant
les hotspots génériques qui les recouvrent. Regroupez dans un même bloc
les `WAIT_SFX` et les actions de transition qui leur sont associées.

Le format des Rooms est volontairement un petit DSL spécifique au jeu,
et non un langage de script généraliste. Si une Room nécessite un
comportement qui ne peut pas être exprimé proprement avec les primitives
existantes, il est préférable d’ajouter au moteur une petite primitive
réutilisable plutôt que de réintroduire des callbacks C spécifiques à
une Room.
