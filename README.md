# Infernal House 3DS

> 🇬🇧 [English](#-english) · 🇫🇷 [Français](#-français)

------------------------------------------------------------------------

## 🇬🇧 English

### The project

**Infernal House** is an independent remake for the Nintendo 3DS of the adventure game originally published by **Lankhor** for the Amstrad CPC in 1991.

The goal of this project is not simply to reproduce an old game screen by screen. It is an attempt to bring back the spirit of *Infernal House* on hardware that feels wonderfully suited to it: two screens, a touch screen, physical controls, and just enough constraints to make development fun. The rooms, puzzles, strange discoveries and unmistakable atmosphere of the original remain at the heart of the experience, while the interface, graphics and engine have been rebuilt for the 3DS.

It is also, quite simply, a project made out of affection for Lankhor, the Amstrad CPC era, and homebrew development on Nintendo's little handheld.

The game is fully playable in **English**, **French** and **Japanese**.

### Lankhor, in a nutshell

**Lankhor** was a French video game developer and publisher born in 1987 from the association of Béatrice and Jean-Luc Langlois with Bruno Gourier's Kyilkhor Création. The studio became one of the memorable names of French video games at the end of the 1980s and beginning of the 1990s.

Its catalogue ranged from adventure games to racing games, but Lankhor is especially remembered for titles such as *Le Manoir de Mortevielle*, *Maupiti Island* and *Vroom*. The company initially developed and published both its own games and works by independent authors. From 1994 onward it concentrated on development, later working with publishers such as Eidos and Microïds. Financial difficulties eventually led to the studio closing at the end of 2001.

Its games were ambitious, inventive and full of personality. This remake is, above all, a small love letter to that spirit.

### Infernal House: development and story

*Infernal House* began as the project of **Christophe Lajoux**, who decided to create it at the age of fourteen on his Amstrad CPC 464. His original idea was remarkably ambitious: build a mysterious house in which the player could interact with as many things as possible, with those actions visibly changing the world.

The project grew over several years. Once the technical foundations were largely in place, **Jérôme Marlier** built a story around them; **Thierry Port** and **Momo (Yvan Monari)** contributed heavily to the graphics, while **Laurent Hiriart**, credited with programming assistance, was an important technical catalyst. The game mixed Locomotive BASIC with Z80 machine code — some of it first written on paper and then entered directly in hexadecimal — while the graphics were created with Art Studio. Lajoux also composed the music. Lankhor published the finished game for the **Amstrad CPC in 1991**.

You play a private detective whose past comes back to haunt him. In 1985, **Rainer Gelehrtman** asked you to investigate his colleague, Professor **Tcherslawsky**, whom he suspected of conducting disturbing experiments. Gelehrtman then disappeared. Five years later, your journalist friend **Sophie** takes an interest in the old case — and disappears in turn. The trail leads you back to Tcherslawsky's strange house, where a missing-person investigation soon becomes something far more unsettling.

👉 The in-game **Introduction**, available from the title screen, tells the events leading to the investigation in much greater detail. Don't skip it!

### Built with devkitPro

The remake is written in **C** using the open-source Nintendo 3DS homebrew toolchain from [devkitPro](https://devkitpro.org/):

- **devkitARM** provides the ARM cross-compilation toolchain;
- **libctru** provides access to 3DS system services, input, RomFS and NDSP audio;
- **Citro2D / Citro3D** provide hardware-accelerated rendering through the 3DS GPU;
- **Tremor** (`libvorbisidec`) provides integer Ogg Vorbis decoding for music playback.

The build produces both a `.3dsx`, which can be launched from the Homebrew Launcher, and a `.cia`, which can be installed as a title with its own HOME Menu icon and banner.

📖 Full build instructions: [docs/BUILD.en.md](./docs/BUILD.en.md)

### The game engine

At the heart of the remake is a small custom **data-driven adventure engine**. The C code implements the reusable mechanics, while most of the game itself — rooms, images, hotspots, paths, conditions, interactions, scripted sequences, HUD layout and title screen — is described by compact text files stored in `resources/` and packed into the RomFS.

The formats deliberately remain small and specific to *Infernal House*: they describe the game rather than trying to become a general-purpose scripting language. Text is kept separately in `.lang` files for localization, while behaviours that need real code — such as the mini-games — can be implemented as C extensions.

Detailed documentation for the data formats is here: [docs/DEVELOPING.en.md](./docs/DEVELOPING.en.md)

### How to play

To play on a Nintendo 3DS:

1.  Get the `.cia` or `.3dsx` package from the [Releases page](https://github.com/Mithweth/infernal-house-3ds/releases).
2.  For the `.cia`, copy it to the SD card and install it with a title manager such as **FBI**. For the `.3dsx`, copy it into the `/3ds/` directory of the SD card and launch it from the **Homebrew Launcher**.
3.  Pick your language, watch the introduction, and enter the house.

The game can also be run with compatible Nintendo 3DS emulators.

Once inside, *Infernal House* plays as a point-and-click adventure: explore the house, examine anything suspicious, collect useful objects, experiment with them and solve the mechanisms and puzzles hidden throughout the building. Observation matters — sometimes a tiny detail is exactly what opens the way forward.

| Control      | Action                          |
|--------------|---------------------------------|
| Circle Pad   | Move between rooms              |
| Touch screen | Interact with your surroundings |
| D-Pad        | Browse the inventory            |
| A            | Use the selected item           |
| X            | Examine the selected item       |
| B            | Cancel / close                  |
| START        | Quit the game                   |

And one last piece of advice: **try things**. *Infernal House* was built around interaction, secrets and the pleasure of discovering that, yes, somebody actually thought about what would happen if you did that.

The clock is ticking. Good luck, detective… you'll need it. 🕰️

------------------------------------------------------------------------

## 🇫🇷 Français

### Le projet

**Infernal House** est un remake indépendant pour Nintendo 3DS du jeu d'aventure publié à l'origine par **Lankhor** sur Amstrad CPC en 1991.

Le but du projet n'est pas simplement de reproduire un vieux jeu écran par écran. Il s'agit de faire revivre l'esprit d'*Infernal House* sur une machine qui lui va étonnamment bien : deux écrans, un écran tactile, des contrôles physiques et juste assez de contraintes pour rendre le développement amusant. Les pièces, les énigmes, les découvertes étranges et l'atmosphère si particulière du jeu original restent au cœur de l'expérience, tandis que l'interface, les graphismes et le moteur ont été reconstruits pour la 3DS.

C'est aussi, tout simplement, un projet né de l'affection portée à Lankhor, à l'époque de l'Amstrad CPC et au plaisir de développer du homebrew sur la petite portable de Nintendo.

Le jeu est entièrement jouable en **anglais**, en **français** et en **japonais**.

### Lankhor, en quelques mots

**Lankhor** était un développeur et éditeur français de jeux vidéo né en 1987 de l'association de Béatrice et Jean-Luc Langlois avec Kyilkhor Création, la structure de Bruno Gourier. Le studio est devenu l'un des noms marquants du jeu vidéo français de la fin des années 1980 et du début des années 1990.

Son catalogue allait du jeu d'aventure à la course automobile, mais Lankhor reste notamment associé à des titres comme *Le Manoir de Mortevielle*, *Maupiti Island* et *Vroom*. La société a d'abord développé et édité ses propres jeux ainsi que ceux d'auteurs indépendants. À partir de 1994, elle s'est concentrée sur le développement, travaillant ensuite avec des éditeurs comme Eidos et Microïds. Des difficultés financières ont finalement conduit à sa fermeture à la fin de l'année 2001.

Ses jeux étaient ambitieux, inventifs et pleins de personnalité. Ce remake est avant tout une petite lettre d'amour à cet esprit.

### Infernal House : développement et histoire

*Infernal House* naît du projet de **Christophe Lajoux**, qui décide de le concevoir à quatorze ans sur son Amstrad CPC 464. Son idée de départ est remarquablement ambitieuse : créer une maison mystérieuse dans laquelle le joueur puisse agir sur un maximum d'éléments et où ses actions modifient visiblement le monde.

Le projet grandit pendant plusieurs années. Une fois les bases techniques largement en place, **Jérôme Marlier** construit une histoire autour d'elles ; **Thierry Port** et **Momo (Yvan Monari)** participent largement aux graphismes, tandis que **Laurent Hiriart**, crédité pour l'aide à la programmation, joue un rôle important de catalyseur technique. Le jeu mélange Locomotive BASIC et code machine Z80 — dont une partie est d'abord écrite sur papier puis saisie directement en hexadécimal — et les graphismes sont réalisés avec Art Studio. Lajoux compose également les musiques. Le jeu terminé est publié par **Lankhor sur Amstrad CPC en 1991**.

Vous incarnez un détective privé rattrapé par une ancienne affaire. En 1985, **Rainer Gelehrtman** vous demande d'enquêter sur son collègue, le professeur **Tcherslawsky**, qu'il soupçonne de mener d'inquiétantes expériences. Gelehrtman disparaît ensuite. Cinq ans plus tard, votre amie journaliste **Sophie** s'intéresse à son tour à cette vieille histoire — et disparaît elle aussi. La piste vous ramène jusqu'à l'étrange demeure de Tcherslawsky, où une enquête sur une disparition va rapidement révéler quelque chose de beaucoup plus inquiétant.

👉 L'**Introduction** du jeu, accessible depuis l'écran titre, raconte beaucoup plus précisément les événements qui conduisent à cette enquête. Ne la sautez pas !

### Développé avec devkitPro

Le remake est écrit en **C** avec la chaîne d'outils homebrew open source pour Nintendo 3DS de [devkitPro](https://devkitpro.org/) :

- **devkitARM** fournit la chaîne de compilation croisée ARM ;
- **libctru** donne accès aux services système de la 3DS, aux contrôles, à la RomFS et à l'audio NDSP ;
- **Citro2D / Citro3D** assurent le rendu accéléré par le GPU de la 3DS ;
- **Tremor** (`libvorbisidec`) assure le décodage Ogg Vorbis en arithmétique entière pour la musique.

La compilation produit à la fois un `.3dsx`, lançable depuis le Homebrew Launcher, et un `.cia`, installable comme un titre avec sa propre icône et sa bannière dans le menu HOME.

📖 Instructions de compilation complètes : [docs/BUILD.fr.md](./docs/BUILD.fr.md)

### Le moteur de jeu

Le cœur du remake est un petit moteur d'aventure maison **piloté par les données**. Le code C fournit les mécanismes réutilisables, tandis que l'essentiel du jeu — pièces, images, zones interactives, chemins, conditions, interactions, séquences scriptées, disposition du HUD et écran titre — est décrit dans de petits fichiers texte rangés dans `resources/` puis embarqués dans la RomFS.

Ces formats restent volontairement petits et spécifiques à *Infernal House* : ils décrivent le jeu sans chercher à devenir un langage de script généraliste. Les textes sont séparés dans des fichiers `.lang` pour la localisation, tandis que les comportements qui nécessitent réellement du code — comme les mini-jeux — peuvent être implémentés sous forme d'extensions C.

La documentation détaillée des formats se trouve ici: [docs/DEVELOPING.fr.md](./docs/DEVELOPING.fr.md)

### Comment jouer

Pour jouer sur une Nintendo 3DS :

1.  Récupérez le paquet `.cia` ou `.3dsx` sur la [page des Releases](https://github.com/Mithweth/infernal-house-3ds/releases).
2.  Pour le `.cia`, copiez-le sur la carte SD et installez-le avec un gestionnaire de titres comme **FBI**. Pour le `.3dsx`, copiez-le dans le répertoire `/3ds/` de la carte SD et lancez-le depuis le **Homebrew Launcher**.
3.  Choisissez votre langue, regardez l'introduction et entrez dans la maison.

Le jeu peut également être lancé avec les émulateurs Nintendo 3DS compatibles.

Une fois à l'intérieur, *Infernal House* se joue comme un point-and-click : explorez la maison, examinez tout ce qui paraît suspect, ramassez les objets utiles, expérimentez avec eux et résolvez les mécanismes et énigmes dissimulés dans le bâtiment. L'observation compte : un détail minuscule est parfois exactement ce qui permet d'avancer.

| Commande             | Action                       |
|----------------------|------------------------------|
| Stick circulaire     | Se déplacer entre les pièces |
| Écran tactile        | Interagir avec le décor      |
| Croix directionnelle | Parcourir l'inventaire       |
| A                    | Utiliser l'objet sélectionné |
| X                    | Examiner l'objet sélectionné |
| B                    | Annuler / fermer             |
| START                | Quitter le jeu               |

Et un dernier conseil : **essayez des choses**. *Infernal House* a été pensé autour de l'interaction, des secrets et du plaisir de découvrir que, oui, quelqu'un avait effectivement prévu ce qui se passerait si vous faisiez ça.

Le temps presse. Bonne chance, détective… vous allez en avoir besoin. 🕰️
