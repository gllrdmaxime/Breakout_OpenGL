/**
 * @file breakout.cpp
 * @author G. Maxime
 * @version 2.7
 * @date 2025-05-06
 * @mainpage
 * @brief Projet S8 - Programmation des interfaces graphiques et interactives
 * 
 * Sujet : Break Out
 * 
 * Ce fichier contient l'implémentation du jeu Breakout avec les fonctionnalités suivantes :
 * - Mécaniques de la raquette et de la balle
 * - Détection de collision avec les briques et les murs
 * - Chargement et sauvegarde de niveaux 
 * - Un éditeur de niveaux avec contrôles à la souris et au clavier
 * 
 * Le jeu utilise OpenGL pour le rendu et GLFW pour la gestion de la fenêtre et des entrées.
 * Il inclut également le rendu de texte avec FreeType et le chargement de textures avec stb_image.
 * 
 * @details
 * Le jeu est divisé en plusieurs classes :
 * - `Paddle`: Représente la raquette contrôlée par le joueur.
 * - `Ball`: Représente la balle qui rebondit sur l'écran.
 * - `Brick`: Représente les briques individuelles dans le jeu.
 * - `Level`: Gère la grille de briques et s'occupe du chargement/sauvegarde des niveaux.
 * - `InputManager`: Gère les entrées clavier et souris.
 * - `TextRenderer`: Effectue le rendu du texte en utilisant FreeType.
 * - `Game`: La classe principale du jeu qui gère la boucle de jeu et les transitions d'état.
 * 
 * Le jeu comprend les états suivants :
 * - `MENU`: Le menu principal où le joueur peut commencer le jeu ou entrer dans l'éditeur.
 * - `GAME`: L'état principal du jeu.
 * - `EDITOR`: Un éditeur de niveaux où le joueur peut concevoir des niveaux personnalisés.
 * - `EXITING`: L'état lorsque le jeu est en train de se fermer.
 * 
 * 
 * @section ExternalLibraries Bibliothèques externes
 * FreeType pour le rendu de texte.
 * (Projet Github : @link https://github.com/freetype @endlink)
 * 
 * Utilisez cette commande pour l'installer sous Linux :
 * @code
 * sudo apt install libfreetype6-dev
 * @endcode
 * 
 * stb_image pour le chargement de textures.
 * Déjà installé dans ./include
 * 
 * (Projet Github : @link https://github.com/nothings/stb/blob/master/stb_image.h @endlink)
 * 
 * @section Compilation Instructions de compilation
 * Pour compiler le code, utilisez la commande suivante :
 * @code
 * g++ -Wall -O2 breakout.cpp -I/usr/include/freetype2 -lglfw -lGL -lfreetype -lX11 -pthread -o break_out
 * @endcode
 * 
 * @section Execution Instructions d'exécution
 * Pour exécuter le code compilé, utilisez la commande suivante :
 * @code
 * ./break_out
 * @endcode
 * 
 * @section Features Fonctionnalités
 * - Mécaniques de la raquette et de la balle avec détection de collision.
 * - Grille de briques avec des briques destructibles et indestructibles.
 * - Chargement et sauvegarde de niveaux dans un fichier level.txt.
 * - Éditeur de niveaux avec contrôles à la souris et au clavier.
 * - Rendu de texte pour l'HUD et les menus.
 * - Système de menu avec options pour jouer, éditer les niveaux ou quitter.
 * 
 * @section Controls Contrôles (Clavier AZERTY)
 * - **Mode Jeu**:
 *   - Mouvement de la souris : Déplacer la raquette.
 *   - Clic gauche : Lancer la balle.
 *   - `M` (ou `Virgule` sur QWERTY): Retourner au menu.
 * - **Mode Éditeur**:
 *   - Clic gauche : Sélectionner une brique.
 *   - Clic droit : Placer une brique (Maj + Clic droit pour une brique indestructible).
 *   - `Suppr` : Effacer la brique sélectionnée.
 *   - `M` (ou `Virgule` sur QWERTY): Retourner au menu.
 * - **Menu**:
 *   - Clic souris : Sélectionner les options du menu.
 * 
 * @section KnownIssues Problèmes connus
 * - Le jeu pourrait ne pas bien gérer les rapports d'aspect extrêmes.
 * - L'éditeur ne supporte pas la fonctionnalité d'annulation/rétablissement (undo/redo).
 * - Les collisions peuvent être un peu imprécises dans certaines situations.
 * 
 * @section FutureWork Travaux futurs
 * - Ajouter des power-ups et des mécaniques de jeu supplémentaires.
 * - Améliorer la détection de collision pour une physique plus réaliste.
 * - Ajouter le support pour la sauvegarde de plusieurs niveaux.
 * - Créer un menu pour visualiser les différents niveaux crées.
 * - Créer un leaderboard pour les scores de chaque niveaux.
 * - Ajouter des effets sonores et de la musique.
 * - Implémenter la fonctionnalité d'annulation/rétablissement dans l'éditeur.
 */

/**
 * @brief Bibliothèques incluses
*/
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <map>
#include <memory>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <ft2build.h>
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

/**
* @brief Constante pour le rendu des cercles.
*/
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Déclarations des classes.
*/
class Paddle;
class Ball;
class Brick;
class Level;
class TextRenderer;
class InputManager;
class Game;

/**
 * @namespace Config 
 * @brief Constantes de configuration.
*/
namespace Config {
    /**
    * @note Fenêtre & Taux de rafraîchissement 
    *       Cette section contien une taille de fenetre par défaut si on arrive pas à detecter la taille du moniteur de l'ordinateur qui éxécute ce programme.
    *       Elle contient aussi les deux variables WINDOW_WIDTH et WINDOW_HEIGHT qui sont initialisées à 0 et qui seront modifiées par la fonction initWindow() de la classe Game et quand on change les dimensions de notre fenetre.
    *       Pour finir on a un taux de rafraîchissement par défaut de 60 FPS.
    */
    constexpr int DEFAULT_WINDOW_WIDTH = 1024;
    constexpr int DEFAULT_WINDOW_HEIGHT = 768;
    int WINDOW_WIDTH = 0;
    int WINDOW_HEIGHT = 0;
    constexpr float TARGET_FPS = 60.0f;

    /**
    * @note Tailles des éléments de jeu.
    */
    constexpr float PADDLE_WIDTH = 130.0f;
    constexpr float PADDLE_HEIGHT = 20.0f;
    constexpr float BRICK_WIDTH = 75.0f;
    constexpr float BRICK_HEIGHT = 25.0f;
    constexpr float BRICK_SPACING = 3.0f;
    constexpr int GRID_COLS = 14;
    constexpr int GRID_ROWS = 8;
    constexpr float BALL_RADIUS = 10.0f;

    /**
     * @note Angle de rebond maximal pour la collision avec la raquette.
     */
    constexpr float PADDLE_MAX_BOUNCE_ANGLE_DEG = 75.0f;
    
    /**
    * @note Vitesse initiale de la balle (traverse la fenêtre en 4 secondes).
    */
    constexpr float BALL_INITIAL_SPEED_X_FACTOR = 0.25f; 
    constexpr float BALL_INITIAL_SPEED_Y_FACTOR = 0.25f; 
    

    /**
    * @note Points des briques et couleurs.
    * Score basé sur l'index de la rangée (indexé à partir de 0 en partant du haut, pour la logique interne, mais visuellement inversé pour l'affichage) :
    * Rangées logiques 0-1 (visuellement haut) : Rouge (7 pts)
    * Rangées logiques 2-3 : Orange (5 pts)
    * Rangées logiques 4-5 : Vert (3 pts)
    * Rangées logiques 6-7 (visuellement bas) : Jaune (1 pt)     
    * Indestructible : (-1 pt, type spécial)
    */
    constexpr int SCORE_YELLOW = 1;
    constexpr int SCORE_GREEN = 3;
    constexpr int SCORE_ORANGE = 5;
    constexpr int SCORE_RED = 7;
    constexpr int SCORE_INDESTRUCTIBLE = -1;

    /**
    * @note Règles du jeu.
    */
    constexpr int PLAYER_STARTING_LIVES = 3;

    /**
    * @note Fichier de niveau du jeu, police, logo et nom du copyright.
    */
    constexpr const char* LEVEL_FILENAME = "level.txt";
    constexpr const char* FONT_PATH = "font/montserrat.bold.ttf";
    constexpr int FONT_PIXEL_SIZE = 48;
    constexpr const char* LOGO_PATH = "images/copyright_logo.png";
    constexpr float LOGO_TARGET_HEIGHT = 30.0f;
    constexpr float LOGO_NAME_SPACING = 5.0f;
    constexpr const char* COPYRIGHT_NAME = "GAILLARD Maxime";

    /**
    * @struct Color
    * @brief Structure pour les couleurs.
    * @param r Composante rouge (0.0 à 1.0).
    * @param g Composante verte (0.0 à 1.0).
    * @param b Composante bleue (0.0 à 1.0).
    */
    struct Color { float r, g, b; };
    constexpr Color COLOR_PADDLE = {0.8f, 0.8f, 0.8f};
    constexpr Color COLOR_BALL = {0.0f, 0.0f, 1.0f};
    constexpr Color COLOR_BG = {0.2f, 0.2f, 0.2f};
    constexpr Color COLOR_BORDER = {0.0f, 0.0f, 0.0f};
    constexpr Color COLOR_TEXT_DEFAULT = {1.0f, 1.0f, 1.0f};
    constexpr Color COLOR_TEXT_ACCENT = {1.0f, 1.0f, 0.0f};
    constexpr Color COLOR_TEXT_ERROR = {1.0f, 0.2f, 0.2f};
    constexpr Color COLOR_BUTTON_PLAY = {0.2f, 0.8f, 0.2f};
    constexpr Color COLOR_BUTTON_EDITOR = {0.8f, 0.8f, 0.2f};
    constexpr Color COLOR_BUTTON_QUIT = {0.8f, 0.2f, 0.2f};
    constexpr Color COLOR_BUTTON_SAVE = {0.2f, 0.8f, 0.2f};
    constexpr Color COLOR_BUTTON_BACK = {0.8f, 0.2f, 0.2f};
    constexpr Color COLOR_BRICK_EMPTY = {0.2f, 0.2f, 0.2f};
    constexpr Color COLOR_HIGHLIGHT = {1.0f, 1.0f, 0.0f};
    constexpr Color COLOR_EDITOR_SELECT = {0.0f, 1.0f, 1.0f};

    /**
    * @note Couleurs des briques.
    */
    constexpr Color COLOR_BRICK_YELLOW = {0.9f, 0.9f, 0.2f};
    constexpr Color COLOR_BRICK_GREEN = {0.2f, 0.9f, 0.2f};
    constexpr Color COLOR_BRICK_ORANGE = {0.9f, 0.6f, 0.2f};
    constexpr Color COLOR_BRICK_RED = {1.0f, 0.0f, 0.0f};
    constexpr Color COLOR_BRICK_INDESTRUCTIBLE = {0.5f, 0.5f, 0.5f};

    /**
    * @brief Obtient la couleur de la brique en fonction de sa valeur de score.
    * @param score La valeur du score de la brique.
    * @return La couleur correspondante. Retourne `COLOR_BRICK_EMPTY` si le score n'est pas reconnu.
    */
    inline Color getBrickColorByScore(int score) {
        switch (score) {
            case SCORE_YELLOW: return COLOR_BRICK_YELLOW;
            case SCORE_GREEN:  return COLOR_BRICK_GREEN;
            case SCORE_ORANGE: return COLOR_BRICK_ORANGE;
            case SCORE_RED:   return COLOR_BRICK_RED;
            case SCORE_INDESTRUCTIBLE: return COLOR_BRICK_INDESTRUCTIBLE;
            default:           return COLOR_BRICK_EMPTY; 
        }
    }

    /**
    * @note Raccourcis clavier pour clavier AZERTY : 
    * 
    * Maj Gauche ou Maj Droite + Clic Droit pour une brique indestructible.
    * Suppr pour supprimer la brique sélectionnée.
    * M (Virgule sur un clavier QWERTY) pour le Menu.
    * Échap pour quitter le jeu.
    */
    constexpr int KEY_EDITOR_PLACE_INDEST = GLFW_KEY_LEFT_SHIFT; 
    constexpr int KEY_EDITOR_PLACE_INDEST_ALT = GLFW_KEY_RIGHT_SHIFT; 
    constexpr int KEY_EDITOR_SET_EMPTY = GLFW_KEY_DELETE;     
    constexpr int KEY_MENU_TOGGLE = GLFW_KEY_SEMICOLON;    
    constexpr int KEY_QUIT = GLFW_KEY_ESCAPE;              
}

/**
    * @enum GameState
    * @brief Déclaration des états du jeu.
*/
enum class GameState {
    MENU,
    GAME,
    EDITOR,
    EXITING
};

/**
 * @brief Structures utilitaires pour les vecteurs 2D et les rectangles.
 * 
 * @struct Vec2
 * @brief Structure pour un vecteur 2D.
 * @param x Coordonnée X du vecteur (par défaut : 0.0f).
 * @param y Coordonnée Y du vecteur (par défaut : 0.0f).
 * 
 * @struct Rect
 * @brief Structure pour un rectangle.
 * @param x Coordonnée X du coin supérieur gauche du rectangle (par défaut : 0.0f).
 * @param y Coordonnée Y du coin supérieur gauche du rectangle (par défaut : 0.0f).
 * @param width Largeur du rectangle (par défaut : 0.0f).
 * @param height Hauteur du rectangle (par défaut : 0.0f).
 */
struct Vec2 { float x = 0.0f; float y = 0.0f; };
struct Rect { float x = 0.0f, y = 0.0f; float width = 0.0f, height = 0.0f; };

/**
 * @namespace Utils
 * @brief Fonctions utilitaires pour dessiner des formes géométrique de base et gérer les textures.
 */
namespace Utils {
    /**
    * @brief Dessine un rectangle.
    * @param x Position X du coin supérieur gauche.
    * @param y Position Y du coin supérieur gauche.
    * @param width Largeur du rectangle.
    * @param height Hauteur du rectangle.
    * @param color Couleur du rectangle.
    */
    inline void drawRect(float x, float y, float width, float height, Config::Color color) {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }

    /**
    * @brief Dessine un rectangle en utilisant la structure Rect.
    * @param rect Structure Rect contenant la position et la taille.
    * @param color Couleur du rectangle.
    * @note Cette fonction est un wrapper autour de la fonction drawRect(float, float, float, float, Config::Color).
    */
    inline void drawRect(const Rect& rect, Config::Color color) { drawRect(rect.x, rect.y, rect.width, rect.height, color); }

    /**
    * @brief Dessine un cercle.
    * @param cx Position X du centre.
    * @param cy Position Y du centre.
    * @param radius Rayon du cercle.
    * @param segments Nombre de segments pour approximer le cercle.
    * @param color Couleur du cercle.
    */
    inline void drawCircle(float cx, float cy, float radius, int segments, Config::Color color) {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Center
        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * M_PI; // Use M_PI
            glVertex2f(cx + std::cos(angle) * radius, cy + std::sin(angle) * radius);
        }
        glEnd();
    }

    /**
    * @brief Charge une texture depuis un fichier.
    * @param filepath Chemin vers le fichier de texture.
    * @param outWidth Variable de sortie pour la largeur de la texture.
    * @param outHeight Variable de sortie pour la hauteur de la texture.
    * @return ID de la texture OpenGL. Retourne 0 en cas d'échec.
    * @note Supporte les textures avec 1 (alpha), 3 (RGB), ou 4 (RGBA) canaux.
    */
    inline GLuint loadTexture(const char* filepath, int& outWidth, int& outHeight) {
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "ERROR::TEXTURE: Failed to load texture '" << filepath << "': " << stbi_failure_reason() << std::endl;
            return 0;
        }
        GLenum format;
        if (nrChannels == 1) format = GL_ALPHA;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        else { stbi_image_free(data); return 0; }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, (nrChannels == 4 || nrChannels == 3) ? 4 : 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        outWidth = width; outHeight = height;
        std::cout << "Texture loaded: " << filepath << " (" << width << "x" << height << ", ID: " << textureID << ")" << std::endl;
        return textureID;
    }

    /**
    * @brief Dessine un rectangle texturé.
    * @param x Position X du coin supérieur gauche.
    * @param y Position Y du coin supérieur gauche.
    * @param width Largeur du rectangle.
    * @param height Hauteur du rectangle.
    * @param textureID ID de la texture OpenGL.
    */
    inline void drawTexturedRect(float x, float y, float width, float height, GLuint textureID) {
        if (textureID == 0) return;
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

/**
 * @class InputManager
 * @brief Gère les événements d'entrée du clavier, de la souris et les interactions du curseur.
 *
 * @param m_keyIsDown Une map stockant l'état actuel des touches (vrai si une touche est maintenue).
 * @param m_keyWasPressed Une map stockant les touches qui ont été pressées dans la trame actuelle.
 * @param m_keyWasReleased Une map stockant les touches qui ont été relâchées dans la trame actuelle.
 * @param m_mousePos Un objet Vec2 représentant la position actuelle de la souris.
 * @param m_mouseLeftDown Un booléen indiquant si le bouton gauche de la souris est actuellement maintenu.
 * @param m_mouseLeftClicked Un booléen indiquant si le bouton gauche de la souris a été cliqué dans la trame actuelle.
 * @param m_mouseRightDown Un booléen indiquant si le bouton droit de la souris est actuellement maintenu.
 * @param m_mouseRightClicked Un booléen indiquant si le bouton droit de la souris a été cliqué dans la trame actuelle.
 * @param m_windowHeight Un entier représentant la hauteur de la fenêtre, utilisé pour les calculs de position du curseur (Y inversé).
 */
class InputManager {
private:
    std::map<int, bool> m_keyIsDown;
    std::map<int, bool> m_keyWasPressed;
    std::map<int, bool> m_keyWasReleased;
    Vec2 m_mousePos = {0.0f, 0.0f};
    bool m_mouseLeftDown = false;
    bool m_mouseLeftClicked = false;
    bool m_mouseRightDown = false;
    bool m_mouseRightClicked = false;
    int m_windowHeight = Config::WINDOW_HEIGHT;

public:
    InputManager() = default;

    /** @brief Définit la hauteur de la fenêtre pour le calcul correct de la position Y de la souris. */
    void setWindowHeight(int height) { m_windowHeight = height > 0 ? height : 1; }
    
    /** @brief Réinitialise l'état des clics/pressions de touches pour la nouvelle trame. */
    void nextFrame() { m_keyWasPressed.clear(); m_keyWasReleased.clear(); m_mouseLeftClicked = false; m_mouseRightClicked = false; }
    
    /** @brief Fonction de Callback pour les événements clavier. */
    void keyCallback(int key, int action) {
        if (action == GLFW_PRESS) { m_keyIsDown[key] = true; m_keyWasPressed[key] = true; }
        else if (action == GLFW_RELEASE) { m_keyIsDown[key] = false; m_keyWasReleased[key] = true; }
    }

    /** @brief Fonction de Callback pour les événements des boutons de la souris. */
    void mouseButtonCallback(int button, int action) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            m_mouseLeftDown = (action == GLFW_PRESS);
            if (action == GLFW_PRESS) m_mouseLeftClicked = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            m_mouseRightDown = (action == GLFW_PRESS);
            if (action == GLFW_PRESS) m_mouseRightClicked = true;
        }
    }
    
    /** @brief Fonction de Callback pour la position du curseur. Inverse Y pour correspondre aux coordonnées OpenGL. */
    void cursorPosCallback(double xpos, double ypos) {
        m_mousePos.x = static_cast<float>(xpos);
        m_mousePos.y = static_cast<float>(m_windowHeight) - static_cast<float>(ypos);
    }
    
    /** @brief Vérifie si une touche est actuellement maintenue enfoncée. */
    bool isKeyDown(int key) const { auto it = m_keyIsDown.find(key); return (it != m_keyIsDown.end() && it->second); }
    
    /** @brief Vérifie si une touche a été pressée dans la trame actuelle. */
    bool wasKeyPressed(int key) const { return m_keyWasPressed.count(key); }
    
    /** @brief Vérifie si une touche a été relâchée dans la trame actuelle. */
    bool wasKeyReleased(int key) const { return m_keyWasReleased.count(key); }
    
    /** @brief Obtient la position actuelle de la souris. */
    const Vec2& getMousePos() const { return m_mousePos; }
    
    /** @brief Obtient la coordonnée X actuelle de la souris. */
    float getMouseX() const { return m_mousePos.x; }
    
    /** @brief Obtient la coordonnée Y actuelle de la souris (origine en bas à gauche). */
    float getMouseY() const { return m_mousePos.y; }
    
    /** @brief Vérifie si le bouton gauche de la souris est actuellement maintenu enfoncé. */
    bool isMouseLeftDown() const { return m_mouseLeftDown; }
    
    /** @brief Vérifie si le bouton gauche de la souris a été cliqué dans la trame actuelle. */
    bool wasMouseLeftClicked() const { return m_mouseLeftClicked; }
    
    /** @brief Vérifie si le bouton droit de la souris est actuellement maintenu enfoncé. */
    bool isMouseRightDown() const { return m_mouseRightDown; }
    
    /** @brief Vérifie si le bouton droit de la souris a été cliqué dans la trame actuelle. */
    bool wasMouseRightClicked() const { return m_mouseRightClicked; }
    
    /** @brief Vérifie si l'une des touches Maj (Shift) est maintenue enfoncée. */
    bool isShiftDown() const { return isKeyDown(GLFW_KEY_LEFT_SHIFT) || isKeyDown(GLFW_KEY_RIGHT_SHIFT); }
};

/**
* @class TextRenderer
* @brief Gère le rendu du texte à l'écran en utilisant FreeType.
*
* @param m_ft Instance de la bibliothèque FreeType.
* @param m_face Instance de la police FreeType (FT_Face).
* @param m_characters Map des caractères pré-rendus (ASCII vers la structure Character).
* @param m_initialized Indicateur booléen de l'état d'initialisation du moteur de rendu.
*/
class TextRenderer {
private:
    /**
    * @struct Character
    * @brief Structure pour stocker les propriétés d'un caractère de police.
    * @param textureID ID de la texture OpenGL pour le glyphe du caractère.
    * @param sizeX Largeur de la texture du caractère.
    * @param sizeY Hauteur de la texture du caractère.
    * @param bearingX Décalage X de la ligne de base au point le plus à gauche du caractère.
    * @param bearingY Décalage Y de la ligne de base au point le plus haut du caractère.
    * @param advance Décalage horizontal vers le caractère suivant.
    */
    struct Character {
        unsigned int textureID = 0; 
        int sizeX = 0; 
        int sizeY = 0;
        int bearingX = 0; 
        int bearingY = 0; 
        unsigned int advance = 0;
    };
    FT_Library m_ft = nullptr; 
    FT_Face m_face = nullptr;
    std::map<char, Character> m_characters; 
    bool m_initialized = false;
public:
    TextRenderer() = default; 
    ~TextRenderer() { cleanup(); }
    TextRenderer(const TextRenderer&) = delete; 
    TextRenderer& operator=(const TextRenderer&) = delete;

    /**
     * @brief Initialise le moteur de rendu de texte.
     * @param fontPath Chemin vers le fichier de police (.ttf).
     * @param pixelSize Taille de la police en pixels.
     * @return `true` si l'initialisation réussit, `false` sinon.
     */
    bool init(const char* fontPath, int pixelSize) {
        if (m_initialized) return true;
        if (FT_Init_FreeType(&m_ft)) {  return false; }
        if (FT_New_Face(m_ft, fontPath, 0, &m_face)) { FT_Done_FreeType(m_ft); m_ft = nullptr; return false; }
        FT_Set_Pixel_Sizes(m_face, 0, pixelSize); // Définit la largeur à 0 pour un calcul automatique
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Désactive la restriction d'alignement des octets
        for (unsigned char c = 32; c < 127; c++) { // Charge les caractères ASCII imprimables
            if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {  continue; }
            unsigned int texture; glGenTextures(1, &texture); glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Character character = { texture, static_cast<int>(m_face->glyph->bitmap.width), static_cast<int>(m_face->glyph->bitmap.rows),
                                    m_face->glyph->bitmap_left, m_face->glyph->bitmap_top, static_cast<unsigned int>(m_face->glyph->advance.x) };
            m_characters.insert({c, character});
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "Text rendering initialized with font: " << fontPath << std::endl;
        m_initialized = true; 
        return true;
    }

    /**
     * @brief Nettoie les ressources utilisées par le moteur de rendu de texte.
     */
    void cleanup() {
        if (m_initialized) {
            std::cout << "Cleaning up TextRenderer..." << std::endl;
            for (auto const& [key, val] : m_characters) { if (val.textureID != 0) glDeleteTextures(1, &val.textureID); }
            m_characters.clear();
            if (m_face) { FT_Done_Face(m_face); m_face = nullptr; }
            if (m_ft) { FT_Done_FreeType(m_ft); m_ft = nullptr; }
            m_initialized = false;
        }
    }

    /**
     * @brief Calcule la largeur d'une chaîne de texte rendue.
     * @param text Le texte à mesurer.
     * @param scale L'échelle à appliquer au texte.
     * @return La largeur du texte en pixels à l'échelle donnée.
     */
    float getTextWidth(const std::string& text, float scale) const {
        if (!m_initialized) return 0.0f; 
        float width = 0.0f;
        for (char c : text) { auto it = m_characters.find(c); if (it != m_characters.end()) width += (it->second.advance >> 6) * scale; }
        return width;
    }

    /**
     * @brief Calcule la hauteur d'une ligne de texte rendue (basée sur les métriques de la police).
     * @param scale L'échelle à appliquer au texte.
     * @return La hauteur du texte en pixels à l'échelle donnée.
     */
    float getTextHeight(float scale) const {
        if (!m_initialized || !m_face || !m_face->size) return 0.0f; 
        return (static_cast<float>(m_face->size->metrics.height >> 6) * scale);
    }

    /**
     * @brief Effectue le rendu d'une chaîne de texte à l'écran.
     * @param text Le texte à afficher. Gère les sauts de ligne '\n'.
     * @param x_center Coordonnée X du centre horizontal du bloc de texte.
     * @param y_baseline_start Coordonnée Y de la ligne de base de la première ligne de texte (le texte s'étend vers le bas).
     * @param scale L'échelle à appliquer au texte.
     * @param color La couleur du texte.
     */
    void render(const std::string& text, float x_center, float y_baseline_start, float scale, Config::Color color) const {
        if (!m_initialized) return;
        glEnable(GL_TEXTURE_2D); 
        glEnable(GL_BLEND); 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(color.r, color.g, color.b); 
        glActiveTexture(GL_TEXTURE0);
        
        float currentY = y_baseline_start;
        
        // Calcul de la hauteur de ligne
        float lineHeight = 0.0f;
        if (m_face && m_face->size && m_face->size->metrics.height) {
            lineHeight = (static_cast<float>(m_face->size->metrics.height >> 6) * scale);
        } else { // Solution de repli si les métriques de la police ne sont pas disponibles
            if (!m_characters.empty()) {
                auto it_char = m_characters.begin();
                if (m_characters.count('M')) it_char = m_characters.find('M');
                else if (m_characters.count('A')) it_char = m_characters.find('A'); // Utilise 'M' ou 'A' comme référence
                if (it_char != m_characters.end()) {
                    lineHeight = static_cast<float>(it_char->second.sizeY) * scale * 1.2f; // 1.2f pour un espacement
                }
            }
            if (lineHeight <= 0.001f) lineHeight = 20.0f * scale; // Valeur de secours absolue positive
        }
        if (lineHeight <= 0.001f) lineHeight = 20.0f * scale; // Assure une hauteur de ligne positive

        // Gestion des sauts de ligne
        std::vector<std::string> lines;
        std::string currentLineStr;
        for (char c : text) {
            if (c == '\n') {
                lines.push_back(currentLineStr);
                currentLineStr.clear();
            } else {
                currentLineStr += c;
            }
        }
        lines.push_back(currentLineStr); // Ajoute la dernière ligne

        for (const std::string& line : lines) {
            float lineWidth = getTextWidth(line, scale);
            float lineStartX = x_center - (lineWidth / 2.0f);
            float currentX = lineStartX;

            // Rendu de chaque ligne
            for (char c_char : line) {
                auto it = m_characters.find(c_char); 
                if (it == m_characters.end()) { // Caractère non trouvé
                    if (m_characters.count(' ')) { // Avance par la largeur d'un espace si disponible
                         currentX += (m_characters.at(' ').advance >> 6) * scale;
                    }
                    continue;
                }
                const Character& ch = it->second;
                
                // Si c'est un espace ou si le caractère n'a pas de texture/taille visible, on avance juste
                if (c_char == ' ' || ch.textureID == 0 || ch.sizeX == 0 || ch.sizeY == 0) { 
                    currentX += (ch.advance >> 6) * scale; 
                    continue; 
                }
                
                
                float xpos = currentX + ch.bearingX * scale;
                // La position Y est la ligne de base + (espaceY - hauteur_glyphe) * échelle
                // car bearingY est la distance de la ligne de base au sommet du glyphe,
                // et le quad est dessiné depuis son coin inférieur gauche.
                float ypos = currentY + (ch.bearingY - ch.sizeY) * scale; 
                float w = ch.sizeX * scale; 
                float h = ch.sizeY * scale;
                
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
                glBegin(GL_QUADS);
                // Attention : les coordonnées de texture Y sont souvent inversées par rapport à OpenGL.
                // FreeType rend avec Y=0 en haut du glyphe. Si on charge avec stbi_set_flip_vertically_on_load(true)
                // pour les textures normales, ici il faut être cohérent.
                // Les glyphes FreeType sont généralement générés avec l'origine en bas à gauche du bitmap.
                // Ici, le quad est défini avec (xpos, ypos) comme coin inférieur gauche.
                glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos, ypos + h);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos + h);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos, ypos);
                glEnd();
                
                currentX += (ch.advance >> 6) * scale; // Avance pour le prochain caractère
            }
            currentY -= lineHeight; // Déplace la ligne de base vers le bas pour la ligne suivante
        }
        glBindTexture(GL_TEXTURE_2D, 0); glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    }
};

/**
* @class Paddle
* @brief Représente la raquette contrôlée par le joueur.
* @param m_rect Rectangle définissant la position et la taille de la raquette.
* @param m_color Couleur de la raquette.
*/
class Paddle {
private:
    Rect m_rect; 
    Config::Color m_color;
public:
    Paddle() : m_color(Config::COLOR_PADDLE) { m_rect.width = Config::PADDLE_WIDTH; m_rect.height = Config::PADDLE_HEIGHT; }
    
    /** @brief Initialise la position de la raquette. */
    void init(float windowWidth, float windowHeight) { 
        m_rect.x = (windowWidth - m_rect.width) / 2.0f; 
        m_rect.y = windowHeight * 0.1f; 
    }

    /** @brief Réinitialise la position horizontale de la raquette au centre. */
    void resetPosition(float windowWidth) { 
        m_rect.x = (windowWidth - m_rect.width) / 2.0f; 
    }

    /** @brief Met à jour la position de la raquette en fonction des entrées et des limites de la fenêtre. */
    void update(float dt, const InputManager& input, float windowWidth) {
        float mouseX = input.getMouseX(); m_rect.x = mouseX - m_rect.width / 2.0f;
        m_rect.x = std::clamp(m_rect.x, 0.0f, windowWidth - m_rect.width);
    }

    /** @brief Dessine la raquette. */
    void render() const { Utils::drawRect(m_rect, m_color); }

    /** @brief Obtient la coordonnée X de la raquette. */
    float getX() const { return m_rect.x; } 
    
    /** @brief Obtient la coordonnée Y de la raquette. */
    float getY() const { return m_rect.y; }
    
    /** @brief Obtient la largeur de la raquette. */
    float getWidth() const { return m_rect.width; } 
    
    /** @breif Obtient la hauteur de la raquette. */
    float getHeight() const { return m_rect.height; }
    
    /** @brief Obtient le rectangle de la raquette. */
    const Rect& getRect() const { return m_rect; }
    
    /** @brief Définit la position de la raquette. */
    void setPosition(float x, float y) { m_rect.x = x; m_rect.y = y; } 
    
    /** @brief Définit la couleur de la raquette. */
    void setColor(const Config::Color& color) { m_color = color; }
};

/**
* @class Ball
* @brief Représente la balle du jeu.
* @param m_pos Position actuelle du centre de la balle.
* @param m_velocity Vitesse actuelle de la balle (déplacement par seconde).
* @param m_preservedvelocity Vitesse de la balle conservée après la perte d'une vie, pour le prochain lancement.
* @param m_nextCollisionPos Position du centre de la balle au moment de la prochaine collision anticipée.
* @param m_hasPendingCollision Booléen indiquant si une collision avec une brique est anticipée.
* @param m_radius Rayon de la balle.
* @param m_color Couleur de la balle.
* @param m_pendingCollisionNormal Normale de la surface de collision pour la prochaine collision anticipée.
* @param m_pendingHitBrickIndex Index de la brique qui sera touchée lors de la prochaine collision anticipée.
*/
class Ball {
    private:
        Vec2 m_pos; 
        Vec2 m_velocity;
        Vec2 m_preservedvelocity;
        Vec2 m_nextCollisionPos;
        bool m_hasPendingCollision = false;
        float m_radius; 
        Config::Color m_color;
        Vec2 m_pendingCollisionNormal; 
        int m_pendingHitBrickIndex = -1; 
    public:
        Ball() : m_radius(Config::BALL_RADIUS), m_color(Config::COLOR_BALL) {}
        
        /** 
         * @brief Initialise la position et la vitesse de la balle.
         * @param startX Position X initiale.
         * @param startY Position Y initiale.
         * @param initialVelX Composante X de la vitesse initiale.
         * @param initialVelY Composante Y de la vitesse initiale.
         */
        void init(float startX, float startY, float initialVelX, float initialVelY) { 
            m_pos = {startX, startY}; 
            m_velocity = {initialVelX, initialVelY}; 
            clearPendingCollision(); // Reset collision state
        }

        /** 
         * @brief Réinitialise la balle à une position et vitesse données.
         * @param startX Position X de départ.
         * @param startY Position Y de départ.
         * @param initialVelX Composante X de la vitesse initiale.
         * @param initialVelY Composante Y de la vitesse initiale.
         */
        void reset(float startX, float startY, float initialVelX, float initialVelY) { 
            init(startX, startY, initialVelX, initialVelY); 
        }
    
        /**
         * @brief Met à jour la position de la balle et gère les collisions avec les murs et les briques (via le mécanisme de collision en attente).
         * @param dt Delta-temps, temps écoulé depuis la dernière trame.
         * @param windowWidth Largeur de la fenêtre de jeu.
         * @param windowHeight Hauteur de la fenêtre de jeu.
         * @param gridMinX Coordonnée X minimale de la zone de jeu (bord gauche de la grille de briques).
         * @param gridMaxX Coordonnée X maximale de la zone de jeu (bord droit de la grille de briques).
         * @return L'index de la brique touchée si une collision en attente a été résolue, sinon -1.
         */
        int update(float dt, float windowWidth, float windowHeight, float gridMinX, float gridMaxX) {
            int brickHitThisFrame = -1;
            Vec2 oldPos = m_pos;

            m_pos.x += m_velocity.x * dt;
            m_pos.y += m_velocity.y * dt;
            
            // Si une collision est en attente
            if (m_hasPendingCollision) {
                Vec2 travelVec = {m_pos.x - oldPos.x, m_pos.y - oldPos.y};
                Vec2 targetVec = {m_nextCollisionPos.x - oldPos.x, m_nextCollisionPos.y - oldPos.y};
                float travelLenSq = travelVec.x*travelVec.x + travelVec.y*travelVec.y;
                float targetLenSq = targetVec.x*targetVec.x + targetVec.y*targetVec.y;

                // Petite tolérance pour les comparaisons flottantes
                float epsilonSq = 0.01f * 0.01f;
                // Vérifie si la distance parcourue est suffisante pour atteindre ou dépasser le point de collision 
                bool reachedTargetDistance = (travelLenSq >= targetLenSq - epsilonSq && targetLenSq > epsilonSq);
                
                Vec2 vecFromNewPosToCollisionPoint = {m_nextCollisionPos.x - m_pos.x, m_nextCollisionPos.y - m_pos.y};
                float dotWithVelocity = vecFromNewPosToCollisionPoint.x * m_velocity.x + vecFromNewPosToCollisionPoint.y * m_velocity.y;
                bool passedTargetPlane = (dotWithVelocity <= 0 && targetLenSq > epsilonSq);

                if (reachedTargetDistance || passedTargetPlane) {
                    m_pos = m_nextCollisionPos;

                    brickHitThisFrame = m_pendingHitBrickIndex;
                
                    float dot_product = m_velocity.x * m_pendingCollisionNormal.x + m_velocity.y * m_pendingCollisionNormal.y;
                    m_velocity.x -= 2 * dot_product * m_pendingCollisionNormal.x;
                    m_velocity.y -= 2 * dot_product * m_pendingCollisionNormal.y;
 
                    float safetyOffset = 0.01f;
                    m_pos.x += m_pendingCollisionNormal.x * safetyOffset;
                    m_pos.y += m_pendingCollisionNormal.y * safetyOffset;

                    clearPendingCollision();
                }
            }
        
            if (m_pos.y + m_radius > windowHeight) {
                m_pos.y = windowHeight - m_radius;
                if (m_velocity.y > 0) {
                    m_velocity.y *= -1.0f;
                    clearPendingCollision();
                }
            }
            if (m_pos.x - m_radius < gridMinX) {
                m_pos.x = gridMinX + m_radius;
                if (m_velocity.x < 0) {
                    m_velocity.x *= -1.0f;
                    clearPendingCollision();
                }
            } else if (m_pos.x + m_radius > gridMaxX) {
                m_pos.x = gridMaxX - m_radius;
                if (m_velocity.x > 0) {
                    m_velocity.x *= -1.0f;
                    clearPendingCollision();
                }
            }
            return brickHitThisFrame;
        }
    
        /**
         * @brief Définit les informations pour la prochaine collision anticipée avec une brique.
         * @param collisionPos Position du centre de la balle au moment de l'impact.
         * @param normal Normale de la surface de la brique au point d'impact.
         * @param brickIndex Index de la brique concernée par la collision.
         */
        void setNextCollision(const Vec2& collisionPos, const Vec2& normal, int brickIndex) {
            m_nextCollisionPos = collisionPos;
            m_pendingCollisionNormal = normal;
            m_pendingHitBrickIndex = brickIndex;
            m_hasPendingCollision = true;
        }
    
        /**
         * @brief Efface l'état de collision en attente.
         */
        void clearPendingCollision() {
            m_hasPendingCollision = false;
            m_pendingHitBrickIndex = -1;
        }

        /**
         * @brief Vérifie et gère la collision entre la balle et la raquette.
         * @param paddle La raquette avec laquelle vérifier la collision.
         * @param dt Delta-temps, utilisé pour prédire la position de la balle.
         * @return `true` si une collision avec la raquette s'est produite, `false` sinon.
         */
        bool checkCollisionPaddle(const Paddle& paddle, float dt) {
            const Rect& paddleRect = paddle.getRect();
            float paddleTopY = paddleRect.y + paddleRect.height;
            float paddleBottomY = paddleRect.y;

            float ballCurrentBottomY = m_pos.y - m_radius;
            float ballCurrentTopY = m_pos.y + m_radius;
            // Prédit la position du bas de la balle après le dt actuel
            float ballNextBottomY = (m_pos.y + m_velocity.y * dt) - m_radius;

            bool isMovingDown = m_velocity.y < 0;
           // Vérifie le chevauchement horizontal entre la balle et la raquette
            bool horizontalOverlap = (m_pos.x + m_radius > paddleRect.x && m_pos.x - m_radius < paddleRect.x + paddleRect.width);

            if (isMovingDown && horizontalOverlap) {
                bool collisionDetected = false;

                // Cas 1: La balle était au-dessus ou au niveau du haut de la raquette et va la croiser ou la toucher dans cette trame
                if (ballCurrentBottomY >= paddleTopY && ballNextBottomY < paddleTopY) {
                    collisionDetected = true;
                }
                // Cas 2: Le bas de la balle intersecte déjà l'épaisseur de la raquette par le haut
                else if (ballCurrentBottomY < paddleTopY && ballCurrentBottomY >= paddleBottomY && ballCurrentTopY > paddleTopY) {
                    collisionDetected = true;
                }

                if (collisionDetected) {
                    // Ajuste la position de la balle pour qu'elle soit juste au-dessus de la raquette
                    m_pos.y = paddleTopY + m_radius; 

                    float hitCenterOffset = m_pos.x - (paddleRect.x + paddleRect.width / 2.0f);
                    float normalizedOffset = std::clamp(hitCenterOffset / (paddleRect.width / 2.0f), -1.0f, 1.0f);
                    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
                    
                    float minSpeed = static_cast<float>(Config::WINDOW_WIDTH) / 4.0f; 
                    if (speed < minSpeed) speed = minSpeed;
                    
                    float maxAngleRad = Config::PADDLE_MAX_BOUNCE_ANGLE_DEG * (M_PI / 180.0f);
                    float bounceAngle = normalizedOffset * maxAngleRad;
                    m_velocity.x = speed * std::sin(bounceAngle);
                    m_velocity.y = speed * std::cos(bounceAngle);
                    m_velocity.y = std::abs(m_velocity.y); // Ensure velocity is upwards
                    
                    clearPendingCollision(); // Paddle hit overrides any pending brick collision
                    return true;
                }
            }
            return false;
        }
        
    /** @brief Arrête la balle (vitesse nulle). */
    void stop() { m_velocity.x = 0.0f; m_velocity.y = 0.0f; }
    
    /** @brief Dessine la balle. */
    void render() const { Utils::drawCircle(m_pos.x, m_pos.y, m_radius, 20, m_color); }
    
    /** @brief Obtient la coordonnée X de la balle. */
    float getX() const { return m_pos.x; } 
    
    /** @brief Obtient la coordonnée Y de la balle. */
    float getY() const { return m_pos.y; }
    
    /** @brief Obtient la position de la balle. */
    const Vec2& getPosition() const { return m_pos; } 
    
    /** @brief Obtient le rayon de la balle. */
    float getRadius() const { return m_radius; }
    
    /** @brief Obtient la composante X de la vitesse de la balle. */
    float getDx() const { return m_velocity.x; } 
    
    /** @brief Obtient la composante Y de la vitesse de la balle. */
    float getDy() const { return m_velocity.y; }
    
    /** @brief Obtient le vecteur vitesse de la balle. */
    const Vec2& getVelocity() const { return m_velocity; } 
    
    /** @brief Obtient le vecteur vitesse sauvegardé de la balle (utilisé après une perte de vie). */
    const Vec2& getPreservedVelocity() const { return m_preservedvelocity; }
    
    /** @brief Définit la vitesse sauvegardée de la balle. */
    void setPreservedVelocity(float dx, float dy) { m_preservedvelocity.x = dx; m_preservedvelocity.y = dy; }
    
    /** @brief Vérifie si la balle est en mouvement. */
    bool isMoving() const { return m_velocity.x != 0.0f || m_velocity.y != 0.0f; }
    
    /** @brief Définit la position de la balle. */
    void setPosition(float x, float y) { m_pos.x = x; m_pos.y = y; } 
    
    /** @brief Définit la vitesse de la balle. */
    void setVelocity(float dx, float dy) { m_velocity.x = dx; m_velocity.y = dy; }
    
    /** @brief Définit le rayon de la balle. */
    void setRadius(float r) { m_radius = r; } 
    
    /** @brief Définit la couleur de la balle. */
    void setColor(const Config::Color& color) { m_color = color; }
};

/**
* @class Brick
* @brief Représente une brique individuelle dans le jeu.
* @param m_rect Rectangle définissant la position et la taille de la brique.
* @param m_color Couleur de la brique, déterminée par sa valeur de score.
* @param m_active Indicateur booléen de l'état actif de la brique (visible et destructible).
* @param m_scoreValue Valeur du score de la brique. Peut être `Config::SCORE_INDESTRUCTIBLE`.
*/
class Brick {
private:
    Rect m_rect;
    Config::Color m_color;
    bool m_active;
    int m_scoreValue;

    /** @brief Met à jour la couleur de la brique en fonction de son état actif et de sa valeur de score. */
    void updateColor() {
        m_color = (m_active) ? Config::getBrickColorByScore(m_scoreValue) : Config::COLOR_BRICK_EMPTY;
    }

public:
    Brick() : m_active(false), m_scoreValue(0) {
        m_rect.width = Config::BRICK_WIDTH;
        m_rect.height = Config::BRICK_HEIGHT;
        updateColor();
    }

    /**
     * @brief Initialise une brique avec une position et une valeur de score.
     * @param x Position X du coin supérieur gauche de la brique.
     * @param y Position Y du coin supérieur gauche de la brique.
     * @param scoreValue Valeur du score de la brique. Si 0, la brique est inactive (vide).
     */
    void init(float x, float y, int scoreValue) {
        m_rect.x = x; m_rect.y = y;
        m_scoreValue = scoreValue;
        m_active = (m_scoreValue != 0);
        updateColor();
    }

    /** @brief Dessine la brique si elle est active. */
    void render() const {
        if (m_active) {
            Utils::drawRect(m_rect, m_color);
        }
    }

    /**
     * @brief Gère la collision avec la brique. La désactive si elle n'est pas indestructible.
     * @return Le score obtenu en touchant la brique. Retourne 0 si la brique est indestructible ou déjà inactive.
     */
    int hit() {
        if (!m_active) return 0;

        if (m_scoreValue == Config::SCORE_INDESTRUCTIBLE) {
            return 0;
        }

        int scoreAwarded = m_scoreValue;
        m_active = false;
        m_scoreValue = 0;
        updateColor();
        return scoreAwarded;
    }

    /** @brief Vérifie si la brique est active. */
    bool isActive() const { return m_active; }
    
    /** @brief Vérifie si la brique est indestructible. */
    bool isIndestructible() const { return m_scoreValue == Config::SCORE_INDESTRUCTIBLE; }
   
    /** @brief Obtient le rectangle de la brique. */
    const Rect& getRect() const { return m_rect; }
    
    /** @brief Obtient la coordonnée X de la brique. */
    float getX() const { return m_rect.x; }
    
    /** @brief Obtient la coordonnée Y de la brique. */
    float getY() const { return m_rect.y; }
    
    /** @brief Obtient la largeur de la brique. */
    float getWidth() const { return m_rect.width; }
    
    /** @brief Obtient la hauteur de la brique. */
    float getHeight() const { return m_rect.height; }
    
    /** @brief Obtient la valeur du score de la brique. */
    int getScoreValue() const { return m_scoreValue; }

    /**
     * @brief Définit l'état actif de la brique et son type (normal/indestructible).
     * @param active `true` pour activer la brique, `false` pour la désactiver.
     * @param indestructible `true` si la brique activée doit être indestructible.
     */
    void setActive(bool active, bool indestructible = false) {
         if (active && !m_active) {
             m_scoreValue = indestructible ? Config::SCORE_INDESTRUCTIBLE : 0;
             m_active = (m_scoreValue != 0);
         } else if (!active && m_active) {
             m_scoreValue = 0;
             m_active = false;
         }
         updateColor();
    }

    /**
     * @brief Définit la valeur de score de la brique. Met à jour son état actif et sa couleur.
     * @param score La nouvelle valeur de score. Doit être une des valeurs prédéfinies dans `Config` ou 0.
     */
    void setScoreValue(int score) {
         bool validScore = false;
         if (score == 0 || score == Config::SCORE_INDESTRUCTIBLE) validScore = true;
         else if (score == Config::SCORE_YELLOW || score == Config::SCORE_GREEN ||
                  score == Config::SCORE_ORANGE || score == Config::SCORE_RED) {
             validScore = true;
         }

         if (validScore) {
             m_scoreValue = score;
             m_active = (m_scoreValue != 0);
             updateColor();
         } else {
             std::cerr << "Warning: Attempted to set invalid score value: " << score << std::endl;
         }
    }
};

/**
* @class Level
* @brief Gère la grille de briques, le chargement et la sauvegarde des niveaux.
* @param m_bricks Vecteur d'objets Brick constituant le niveau.
* @param m_gridState Tableau 2D représentant l'état de la grille (valeurs de score des briques).
* @param m_activeBrickCount Nombre de briques actives (destructibles) dans le niveau.
* @param m_editorSelectedRow Ligne sélectionnée en mode éditeur (logique, 0 en haut).
* @param m_editorSelectedCol Colonne sélectionnée en mode éditeur (logique, 0 à gauche).
* @param m_gridTotalWidth Largeur totale de la grille de briques (avec espacements).
* @param m_gridTotalHeight Hauteur totale de la grille de briques (avec espacements).
* @param m_gridStartX Position X de départ (coin supérieur gauche) de la grille.
* @param m_gridStartY Position Y de départ (coin supérieur gauche) de la grille.
*/
class Level {
private:
    std::vector<Brick> m_bricks;
    int m_gridState[Config::GRID_ROWS][Config::GRID_COLS];
    int m_activeBrickCount = 0;
    int m_editorSelectedRow = -1;
    int m_editorSelectedCol = -1;


public:
    float m_gridTotalWidth = 0.0f;
    float m_gridTotalHeight = 0.0f;
    float m_gridStartX = 0.0f;
    float m_gridStartY = 0.0f;
    
    Level() {
        m_bricks.resize(Config::GRID_ROWS * Config::GRID_COLS);
        for (int r = 0; r < Config::GRID_ROWS; ++r) {
            for (int c = 0; c < Config::GRID_COLS; ++c) {
                m_gridState[r][c] = 0;
            }
        }
    }

    /**
     * @brief Charge la configuration du niveau depuis un fichier.
     * @param filename Le nom du fichier de niveau.
     * @return `true` si le chargement réussit ou si un niveau par défaut est créé, `false` si une erreur majeure se produit.
     * @note Si le fichier n'existe pas, un niveau par défaut est créé et sauvegardé.
     * @note Les valeurs de score lues sont validées. Les valeurs invalides sont remplacées par 0.
     */
    bool load(const std::string& filename) {
        std::ifstream levelFile(filename);
        if (!levelFile.is_open()) {
            std::cerr << "Warning: Could not load level file: " << filename << ". Creating default layout." << std::endl;
            for (int r = 0; r < Config::GRID_ROWS; ++r) {
                for (int c = 0; c < Config::GRID_COLS; ++c) {
                    if (r < 2) m_gridState[r][c] = Config::SCORE_RED;
                    else if (r < 4) m_gridState[r][c] = Config::SCORE_ORANGE;
                    else if (r < 6) m_gridState[r][c] = Config::SCORE_GREEN;
                    else if (r < 8) m_gridState[r][c] = Config::SCORE_YELLOW;
                    else m_gridState[r][c] = 0;
                }
            }
            save(filename);
            return true;
        }

        std::cout << "Loading level from " << filename << std::endl;
        int scoreValue;
        bool success = true;
        for (int r = 0; r < Config::GRID_ROWS; ++r) {
            for (int c = 0; c < Config::GRID_COLS; ++c) {
                if (!(levelFile >> scoreValue)) {
                    std::cerr << "Warning: Error reading level file format at r" << r << ", c" << c << ". Setting score to 0." << std::endl;
                    m_gridState[r][c] = 0;
                    success = false;
                } else {
                    bool valid = false;
                    if (scoreValue == 0 || scoreValue == Config::SCORE_INDESTRUCTIBLE ||
                        scoreValue == Config::SCORE_YELLOW || scoreValue == Config::SCORE_GREEN ||
                        scoreValue == Config::SCORE_ORANGE || scoreValue == Config::SCORE_RED) {
                        valid = true;
                    }
                    m_gridState[r][c] = valid ? scoreValue : 0;
                    if (!valid && scoreValue != 0) {
                         std::cerr << "Warning: Invalid score " << scoreValue << " read at r" << r << ", c" << c << ". Setting to 0." << std::endl;
                         success = false;
                    }
                }
            }
        }
        levelFile.close();
        m_editorSelectedRow = -1; 
        m_editorSelectedCol = -1;
        return success;
    }

    /**
     * @brief Sauvegarde la configuration actuelle du niveau (les valeurs de score de `m_gridState`) dans un fichier.
     * @param filename Le nom du fichier de niveau.
     * @return `true` si la sauvegarde réussit, `false` sinon.
     * @note Le fichier est écrasé s'il existe déjà.
     * @note Les valeurs de score sont écrites dans le même format que celui utilisé pour le chargement.
     * @note Les briques indestructibles sont représentées par `Config::SCORE_INDESTRUCTIBLE`.
     * @note Les briques vides/inactives sont représentées par 0.
     * @note Les briques actives sont représentées par leurs valeurs de score respectives.
     */
    bool save(const std::string& filename) const {
        std::ofstream levelFile(filename);
        if (!levelFile.is_open()) {
            std::cerr << "Error: Could not open level file for saving: " << filename << std::endl;
            return false;
        }
        std::cout << "Saving level (score values) to " << filename << std::endl;
        for (int r = 0; r < Config::GRID_ROWS; ++r) {
            for (int c = 0; c < Config::GRID_COLS; ++c) {
                levelFile << m_gridState[r][c] << (c == Config::GRID_COLS - 1 ? "" : " ");
            }
            levelFile << "\n";
        }
        levelFile.close();
        return true;
    }

    /**
     * @brief Calcule les dimensions et la position de départ de la grille de briques.
     * @param windowWidth Largeur de la fenêtre.
     * @param windowHeight Hauteur de la fenêtre.
     * @note La grille est centrée horizontalement. Sa position Y est calculée pour être en haut de l'écran,
     *       mais pas trop bas pour laisser de la place pour la raquette et la zone de jeu.
     */
    void calculateGridDimensions(float windowWidth, float windowHeight) {
        m_gridTotalWidth = Config::GRID_COLS * (Config::BRICK_WIDTH + Config::BRICK_SPACING) - Config::BRICK_SPACING;
        m_gridTotalHeight = Config::GRID_ROWS * (Config::BRICK_HEIGHT + Config::BRICK_SPACING) - Config::BRICK_SPACING;
        m_gridStartX = (windowWidth - m_gridTotalWidth) / 2.0f;
        m_gridStartY = windowHeight * 0.90f - m_gridTotalHeight;
        if (m_gridStartY < windowHeight * 0.25f) {
            m_gridStartY = windowHeight * 0.25f;
        }
    }

    /**
     * @brief Crée/Met à jour les objets Brick basés sur `m_gridState`.
     * @param windowWidth Largeur de la fenêtre.
     * @param windowHeight Hauteur de la fenêtre.
     * @note Cette fonction doit être appelée après `load()` et `calculateGridDimensions()`.
     *       Elle initialise chaque brique avec sa position et son type/score.
     */
    void setupBricks(float windowWidth, float windowHeight) {
        calculateGridDimensions(windowWidth, windowHeight);
        m_activeBrickCount = 0;
        int brickIndex = 0;
        for (int r = 0; r < Config::GRID_ROWS; ++r) {
            for (int c = 0; c < Config::GRID_COLS; ++c) {
                Brick& brick = m_bricks[brickIndex];
                int scoreValue = m_gridState[r][c];

                 int visualRow = Config::GRID_ROWS - 1 - r;
                 float brickX = m_gridStartX + c * (Config::BRICK_WIDTH + Config::BRICK_SPACING);
                 float brickY = m_gridStartY + visualRow * (Config::BRICK_HEIGHT + Config::BRICK_SPACING);

                brick.init(brickX, brickY, scoreValue);
                if (brick.isActive() && !brick.isIndestructible()) {
                    m_activeBrickCount++;
                }
                brickIndex++;
            }
        }
        std::cout << "Level setup complete. Active destructible bricks: " << m_activeBrickCount << std::endl;
    }


    /**
     * @brief Vérifie les collisions entre la balle et les briques du niveau en utilisant une méthode de balayage (swept collision).
     *        Si une collision est détectée comme la plus proche dans la trajectoire actuelle de la balle,
     *        elle informe la balle de cette collision imminente (position, normale, index de la brique).
     * @param ball La balle à vérifier.
     * @note Cette méthode ne modifie pas l'état des briques ni de la balle directement, elle prépare la balle à gérer la collision dans son propre `update()`.
     */
    void checkCollisions(Ball& ball) {
        Vec2 ballPos = ball.getPosition();
        float ballR = ball.getRadius();
        Vec2 ballVel = ball.getVelocity();
    
        float closestTime = 1.0f; 
        int closestBrickIndex = -1;
        Vec2 closestNormal = {0.0f, 0.0f};
        Vec2 collisionPos = {0.0f, 0.0f}; 
    
        for (size_t i = 0; i < m_bricks.size(); ++i) {
            Brick& brick = m_bricks[i];
            if (!brick.isActive()) continue;
            const Rect& bRect = brick.getRect();
    
            float left = bRect.x - ballR;
            float right = bRect.x + bRect.width + ballR;
            float bottom = bRect.y - ballR; 
            float top = bRect.y + bRect.height + ballR;
    
            float txMin = 0.0f, txMax = 1.0f;
            if (std::abs(ballVel.x) > 0.0001f) {
                float invVelX = 1.0f / ballVel.x;
                txMin = (left - ballPos.x) * invVelX;
                txMax = (right - ballPos.x) * invVelX;
                if (txMin > txMax) std::swap(txMin, txMax);
            } else if (ballPos.x < left || ballPos.x > right) {
                continue;
            }
    
            float tyMin = 0.0f, tyMax = 1.0f;
            if (std::abs(ballVel.y) > 0.0001f) {
                float invVelY = 1.0f / ballVel.y;
                tyMin = (bottom - ballPos.y) * invVelY; 
                tyMax = (top - ballPos.y) * invVelY;   
                if (tyMin > tyMax) std::swap(tyMin, tyMax);
            } else if (ballPos.y < bottom || ballPos.y > top) {
                continue;
            }
    
            float tMin = std::max(txMin, tyMin);
            float tMax = std::min(txMax, tyMax);
    
            if (tMin > tMax || tMax < 0.0f || tMin > 1.0f) {
                continue;
            }
    
            if (tMin < closestTime) {
                closestTime = tMin;
                closestBrickIndex = i;
    
                collisionPos = {
                    ballPos.x + ballVel.x * tMin,
                    ballPos.y + ballVel.y * tMin
                };
    
                if (txMin > tyMin) {
                    closestNormal = {(ballVel.x > 0.0f) ? -1.0f : 1.0f, 0.0f};
                } else {
                    closestNormal = {0.0f, (ballVel.y > 0.0f) ? -1.0f : 1.0f};
                }
            }
        }
    
        if (closestBrickIndex >= 0) {
            ball.setNextCollision(collisionPos, closestNormal, closestBrickIndex);
        }
    }

    /**
     * @brief Traite l'impact confirmé avec une brique (après que la balle a géré la collision).
     * @param brickIndex L'index de la brique qui a été touchée.
     * @return Le score obtenu de la brique. 0 si la brique est invalide, inactive ou indestructible.
     * @note Met à jour `m_activeBrickCount`.
     */
    int processBrickHit(int brickIndex) {
        if (brickIndex < 0 || static_cast<size_t>(brickIndex) >= m_bricks.size()) {
            return 0;
        }
        Brick& brick = m_bricks[brickIndex];
        if (!brick.isActive()) {
            return 0;
        }

        int scoreAwarded = brick.hit(); // This deactivates the brick and returns its score value

        if (scoreAwarded > 0 && !brick.isIndestructible()) { // Check if it was a destructible brick that gave score
            m_activeBrickCount = std::max(0, m_activeBrickCount - 1);
        }
        return scoreAwarded;
    }

    /** @brief Dessine toutes les briques actives du niveau. */
    void render() const {
        for (const Brick& brick : m_bricks) {
            brick.render();
        }
    }

    /**
     * @brief Dessine la grille de l'éditeur, montrant l'état de chaque cellule.
     * @param windowWidth Largeur de la fenêtre (non utilisée directement, mais implicite via m_gridStartX/Y).
     * @param windowHeight Hauteur de la fenêtre (non utilisée directement).
     * @note Utilise `m_gridState` pour déterminer la couleur de chaque cellule.
     */
    void renderEditorGrid(float windowWidth, float windowHeight) const {
        for (int r = 0; r < Config::GRID_ROWS; ++r) {
            for (int c = 0; c < Config::GRID_COLS; ++c) {
                 int visualRow = Config::GRID_ROWS - 1 - r;
                 float cellX = m_gridStartX + c * (Config::BRICK_WIDTH + Config::BRICK_SPACING);
                 float cellY = m_gridStartY + visualRow * (Config::BRICK_HEIGHT + Config::BRICK_SPACING);
                 int scoreValue = m_gridState[r][c];
                 Config::Color color = Config::getBrickColorByScore(scoreValue);
                 Utils::drawRect(cellX, cellY, Config::BRICK_WIDTH, Config::BRICK_HEIGHT, color);
            }
        }
    }

    /**
     * @brief Gère la sélection d'une cellule par clic gauche en mode éditeur.
     * @param mouseX Coordonnée X de la souris.
     * @param mouseY Coordonnée Y de la souris.
     * @return 1 si la sélection a changé (nouvelle cellule sélectionnée ou désélection), 0 sinon.
     * @note Met à jour `m_editorSelectedRow` et `m_editorSelectedCol`.
     */
    int handleEditorLeftClickSelection(float mouseX, float mouseY) {
        if (mouseX < m_gridStartX || mouseX > m_gridStartX + m_gridTotalWidth ||
            mouseY < m_gridStartY || mouseY > m_gridStartY + m_gridTotalHeight) {
             if (m_editorSelectedRow != -1 || m_editorSelectedCol != -1) {
                 m_editorSelectedRow = -1;
                 m_editorSelectedCol = -1;
                 return 1;
             }
             return 0;
        }

        float cellWidthWithSpacing = Config::BRICK_WIDTH + Config::BRICK_SPACING;
        float cellHeightWithSpacing = Config::BRICK_HEIGHT + Config::BRICK_SPACING;

        int visualCol = static_cast<int>((mouseX - m_gridStartX) / cellWidthWithSpacing);
        int visualRow = static_cast<int>((mouseY - m_gridStartY) / cellHeightWithSpacing);

        visualCol = std::clamp(visualCol, 0, Config::GRID_COLS - 1);
        visualRow = std::clamp(visualRow, 0, Config::GRID_ROWS - 1);

        // Convertit la rangée visuelle (0=bas) en rangée logique (0=haut) pour `m_gridState`
        int logicalRow = Config::GRID_ROWS - 1 - visualRow;
        int logicalCol = visualCol; 

        float brickX = m_gridStartX + logicalCol * cellWidthWithSpacing;
        float brickY = m_gridStartY + visualRow * cellHeightWithSpacing;

        if (mouseX >= brickX && mouseX <= brickX + Config::BRICK_WIDTH &&
            mouseY >= brickY && mouseY <= brickY + Config::BRICK_HEIGHT)
        {
            if (m_editorSelectedRow != logicalRow || m_editorSelectedCol != logicalCol) {
                m_editorSelectedRow = logicalRow;
                m_editorSelectedCol = logicalCol;
                return 1;
            }
            return 0;
        } else {
            if (m_editorSelectedRow != -1 || m_editorSelectedCol != -1) {
                m_editorSelectedRow = -1;
                m_editorSelectedCol = -1;
                return 1;
            }
            return 0;
        }
    }

    /**
     * @brief Gère le placement d'une brique par clic droit en mode éditeur sur la cellule sélectionnée.
     * @param isShiftDown `true` si la touche Maj est enfoncée (pour placer une brique indestructible).
     * @return `true` si une brique a été placée (s'il y avait une sélection), `false` sinon.
     * @note Modifie `m_gridState` pour la cellule sélectionnée.
     */
    bool handleEditorRightClickPlace(bool isShiftDown) {
        if (!hasEditorSelection()) {
            return false;
        }

        int row = m_editorSelectedRow;
        int col = m_editorSelectedCol;
        int scoreValue;

        if (isShiftDown) {
            scoreValue = Config::SCORE_INDESTRUCTIBLE;
        } else {
            // Détermine le score en fonction de la rangée logique (0=haut à 7=bas)
            if (row < 2) scoreValue = Config::SCORE_RED;      // Rows 0, 1
            else if (row < 4) scoreValue = Config::SCORE_ORANGE;  // Rows 2, 3
            else if (row < 6) scoreValue = Config::SCORE_GREEN; // Rows 4, 5
            else scoreValue = Config::SCORE_YELLOW;                 // Rows 6, 7
        }

        m_gridState[row][col] = scoreValue;
        return true;
    }

    /** @brief Efface la brique sélectionnée en mode éditeur (met sa valeur de score à 0 dans `m_gridState`). */
    void clearSelectedBrick() {
         if (hasEditorSelection()) {
             m_gridState[m_editorSelectedRow][m_editorSelectedCol] = 0;
         }
    }

    /** @brief Vérifie s'il y a une cellule sélectionnée en mode éditeur. */
    bool hasEditorSelection() const {
        return m_editorSelectedRow >= 0 && m_editorSelectedRow < Config::GRID_ROWS &&
               m_editorSelectedCol >= 0 && m_editorSelectedCol < Config::GRID_COLS;
    }

    /**
     * @brief Dessine un contour de surbrillance autour de la cellule sélectionnée en mode éditeur.
     * @param windowWidth Largeur de la fenêtre (non utilisée).
     * @param windowHeight Hauteur de la fenêtre (non utilisée).
     */
    void highlightEditorCell(float windowWidth, float windowHeight) const {
        if (!hasEditorSelection()) return;
        float cellWidthWithSpacing = Config::BRICK_WIDTH + Config::BRICK_SPACING;
        float cellHeightWithSpacing = Config::BRICK_HEIGHT + Config::BRICK_SPACING;
        int visualRow = Config::GRID_ROWS - 1 - m_editorSelectedRow;
        float cellX = m_gridStartX + m_editorSelectedCol * cellWidthWithSpacing;
        float cellY = m_gridStartY + visualRow * cellHeightWithSpacing;
        float outlinePadding = 2.0f;
        glColor3f(Config::COLOR_EDITOR_SELECT.r, Config::COLOR_EDITOR_SELECT.g, Config::COLOR_EDITOR_SELECT.b);
        glLineWidth(2.5f); glBegin(GL_LINE_LOOP);
        glVertex2f(cellX - outlinePadding, cellY - outlinePadding);
        glVertex2f(cellX + Config::BRICK_WIDTH + outlinePadding, cellY - outlinePadding);
        glVertex2f(cellX + Config::BRICK_WIDTH + outlinePadding, cellY + Config::BRICK_HEIGHT + outlinePadding);
        glVertex2f(cellX - outlinePadding, cellY + Config::BRICK_HEIGHT + outlinePadding);
        glEnd(); glLineWidth(1.0f);
    }

    /** @brief Obtient le nombre de briques destructibles actives. */
    int getActiveBrickCount() const { return m_activeBrickCount; }

    /** @brief Vérifie si toutes les briques destructibles ont été éliminées. */
    bool areAllBricksCleared() const { return m_activeBrickCount <= 0; }

    /** @brief Obtient la valeur de score de la brique actuellement sélectionnée dans l'éditeur. */
    int getSelectedBrickScore() const {
        if (hasEditorSelection()) return m_gridState[m_editorSelectedRow][m_editorSelectedCol];
        return 0;
    }

    /** @brief Obtient l'index de la ligne (logique) de la brique sélectionnée dans l'éditeur. */
    int getSelectedRow() const { return m_editorSelectedRow; }
    
    /** @brief Obtient l'index de la colonne (logique) de la brique sélectionnée dans l'éditeur. */
    int getSelectedCol() const { return m_editorSelectedCol; }
    
    /**
      * @brief Modifie directement l'état d'une cellule dans la grille `m_gridState`.
      * @param row Index de la rangée logique (0 en haut).
      * @param col Index de la colonne logique (0 à gauche).
      * @param scoreValue La nouvelle valeur de score pour la cellule. Doit être valide.
      */
    void setGridState(int row, int col, int scoreValue) {
        if (row >= 0 && row < Config::GRID_ROWS && col >= 0 && col < Config::GRID_COLS) {
            bool validScore = false;
            if (scoreValue == 0 || scoreValue == Config::SCORE_INDESTRUCTIBLE ||
                scoreValue == Config::SCORE_YELLOW || scoreValue == Config::SCORE_GREEN ||
                scoreValue == Config::SCORE_ORANGE || scoreValue == Config::SCORE_RED) {
                validScore = true;
            }
            if (validScore) {
               m_gridState[row][col] = scoreValue;
            } else {
                std::cerr << "Warning: Attempted to set invalid score " << scoreValue << " at [" << row << "," << col << "]" << std::endl;
                m_gridState[row][col] = 0;
            }
        }
    }
};

/**
* @class Game
* @brief Classe principale du jeu, gérant la boucle de jeu, les états et les objets principaux.
*
* @param m_window Pointeur vers la fenêtre GLFW.
* @param m_windowWidth Largeur actuelle de la fenêtre.
* @param m_windowHeight Hauteur actuelle de la fenêtre.
* @param m_currentState État actuel du jeu (Menu, Jeu, Éditeur, Sortie).
* @param m_inputManager Gestionnaire d'entrées.
* @param m_textRenderer Moteur de rendu de texte.
* @param m_paddle Objet raquette.
* @param m_ball Objet balle.
* @param m_level Objet niveau, gérant les briques.
* @param m_lastTime Temps de la dernière mise à jour, pour calculer le delta-temps.
* @param m_statusMessage Message à afficher à l'écran (ex: "Game Over").
* @param m_lives Nombre de vies restantes pour le joueur.
* @param m_score Score actuel du joueur.
* @param m_logoTextureID ID de la texture OpenGL pour le logo.
* @param m_logoWidth Largeur de la texture du logo.
* @param m_logoHeight Hauteur de la texture du logo.
* @param m_bricksHitInCurrentGame Compteur de briques touchées dans la partie en cours (pour les bonus de vitesse).
* @param m_firstOrangeBrickHit Indicateur si la première brique orange a été touchée (pour bonus de vitesse).
* @param m_firstRedBrickHit Indicateur si la première brique rouge a été touchée (pour bonus de vitesse).
* @param m_speedBoost4thBrickDone Indicateur si le bonus de vitesse pour la 4ème brique a été appliqué.
* @param m_speedBoost12thBrickDone Indicateur si le bonus de vitesse pour la 12ème brique a été appliqué.
*/
class Game {
private:
    GLFWwindow* m_window = nullptr;
    int m_windowWidth = Config::DEFAULT_WINDOW_WIDTH;
    int m_windowHeight = Config::DEFAULT_WINDOW_HEIGHT;
    GameState m_currentState = GameState::MENU;
    InputManager m_inputManager;
    TextRenderer m_textRenderer;
    Paddle m_paddle;
    Ball m_ball;
    Level m_level;
    double m_lastTime = 0.0;
    std::string m_statusMessage = "";
    int m_lives = Config::PLAYER_STARTING_LIVES;
    int m_score = 0;

    GLuint m_logoTextureID = 0;
    int m_logoWidth = 0;
    int m_logoHeight = 0;

    int m_bricksHitInCurrentGame = 0; 
    bool m_firstOrangeBrickHit = false;      
    bool m_firstRedBrickHit = false;            
    bool m_speedBoost4thBrickDone = false;
    bool m_speedBoost12thBrickDone = false;

public:
    Game() = default;
    ~Game() { cleanup(); }
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    /**
    * @brief Initialise le jeu.
    * @return `true` si l'initialisation est réussie, `false` sinon.
    * @note Configure GLFW, crée une fenêtre, initialise le contexte OpenGL et charge les ressources.
    * @note Définit la fonction de rappel d'erreur pour GLFW et initialise le moteur de rendu de texte.
    * @note Charge la texture du logo et configure la taille de la fenêtre en fonction du moniteur principal.
    * @note Configure le gestionnaire d'entrées et initialise l'état du jeu.
    * @note Définit la position initiale de la raquette et de la balle.
    * @note Définit la couleur de fond et active le mélange alpha.
    * @note Définit le titre de la fenêtre et active la V-Sync.
    * @note Définit le pointeur utilisateur de la fenêtre sur cette instance du jeu.
    * @note Définit les fonctions de rappel pour les touches, les boutons de la souris, la position du curseur et la taille du framebuffer pour GLFW.
    */
    bool init() {
        glfwSetErrorCallback(Game::errorCallback_GLFW);
        if (!glfwInit()) { return false; }

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        m_windowWidth = mode ? mode->width : Config::DEFAULT_WINDOW_WIDTH;
        m_windowHeight = mode ? mode->height : Config::DEFAULT_WINDOW_HEIGHT;
        Config::WINDOW_WIDTH = m_windowWidth;
        Config::WINDOW_HEIGHT = m_windowHeight;
        const char* windowTitle = "GLFW Breakout Game";
        if (mode) std::cout << "Detected primary monitor: " << mode->width << "x" << mode->height << ". Creating window." << std::endl;
        else std::cerr << "Warning: Could not get primary monitor info, using default size." << std::endl;

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, windowTitle, nullptr, nullptr);
        if (!m_window) { glfwTerminate(); return false; }
        if (mode && !glfwGetWindowMonitor(m_window)) {
             int xpos = (mode->width - m_windowWidth) / 2; int ypos = (mode->height - m_windowHeight) / 2;
             glfwSetWindowPos(m_window, xpos, ypos);
        }
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // V-Sync

        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, Game::keyCallback_GLFW);
        glfwSetMouseButtonCallback(m_window, Game::mouseButtonCallback_GLFW);
        glfwSetCursorPosCallback(m_window, Game::cursorPosCallback_GLFW);
        glfwSetFramebufferSizeCallback(m_window, Game::framebufferSizeCallback_GLFW);
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glClearColor(Config::COLOR_BG.r, Config::COLOR_BG.g, Config::COLOR_BG.b, 1.0f);

        if (!m_textRenderer.init(Config::FONT_PATH, Config::FONT_PIXEL_SIZE)) { cleanup(); return false; }
        m_logoTextureID = Utils::loadTexture(Config::LOGO_PATH, m_logoWidth, m_logoHeight);
        if (m_logoTextureID == 0) std::cerr << "Warning: Failed to load logo texture: " << Config::LOGO_PATH << ". Logo will not be displayed." << std::endl;

        glfwGetFramebufferSize(m_window, &m_windowWidth, &m_windowHeight);
        framebufferSizeCallback(m_windowWidth, m_windowHeight);

        m_inputManager.setWindowHeight(m_windowHeight);

        m_currentState = GameState::MENU;
        m_lastTime = glfwGetTime();
        std::cout << "Game initialized successfully." << std::endl;
        return true;
    }

    /**
    * @brief Exécute la boucle principale du jeu.
    * @note Gère la temporisation des trames, les événements, la mise à jour et le rendu.
    */
    void run() {
        const double targetFrameTime = 1.0 / Config::TARGET_FPS;
        while (m_currentState != GameState::EXITING && !glfwWindowShouldClose(m_window)) {
            double frameStartTime = glfwGetTime();
            double currentTime = glfwGetTime(); 
            double deltaTime = currentTime - m_lastTime; 
            m_lastTime = currentTime;
            m_inputManager.nextFrame(); 
            glfwPollEvents();
            update(static_cast<float>(deltaTime));
            render();
            glfwSwapBuffers(m_window);
            double frameEndTime = glfwGetTime(); 
            double frameDuration = frameEndTime - frameStartTime;
            if (frameDuration < targetFrameTime) {
                std::chrono::duration<double> sleepDuration(targetFrameTime - frameDuration);
                std::this_thread::sleep_for(sleepDuration);
            }
        }
        if (m_currentState != GameState::EXITING) m_currentState = GameState::EXITING;
    }
private:
   /**
     * @brief Met à jour la logique du jeu en fonction de l'état actuel.
     * @param dt Delta-temps.
     */
    void update(float dt) {
        if (m_inputManager.wasKeyPressed(Config::KEY_QUIT)) { glfwSetWindowShouldClose(m_window, GLFW_TRUE); return; }
        switch (m_currentState) {
            case GameState::MENU:   updateMenu(dt);   break;
            case GameState::GAME:   updateGame(dt);   break;
            case GameState::EDITOR: updateEditor(dt); break;
            case GameState::EXITING: break;
        }
    }

    /**
     * @brief Effectue le rendu du jeu en fonction de l'état actuel.
     * @note Configure la projection orthographique pour le rendu 2D.
     */
    void render() {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION); 
        glLoadIdentity();
        glOrtho(0.0, static_cast<double>(m_windowWidth), 0.0, static_cast<double>(m_windowHeight), -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW); 
        glLoadIdentity();

        switch (m_currentState) {
            case GameState::MENU:   renderMenu();   break;
            case GameState::GAME:   renderGame();   break;
            case GameState::EDITOR: renderEditor(); break;
            case GameState::EXITING: break;
        }
    }

    /**
     * @brief Nettoie les ressources du jeu avant de quitter.
     */
    void cleanup() {
        std::cout << "Cleaning up Game..." << std::endl;
        if (m_logoTextureID != 0) { glDeleteTextures(1, &m_logoTextureID); m_logoTextureID = 0; std::cout << "Logo texture deleted." << std::endl; }
        m_textRenderer.cleanup();
        if (m_window) { glfwDestroyWindow(m_window); m_window = nullptr; }
        glfwTerminate();
        std::cout << "Game cleanup complete." << std::endl;
    }

    /**
     * @brief Initialise ou réinitialise les objets pour une nouvelle partie.
     * @note Réinitialise les vies, le score, charge le niveau, et positionne la raquette et la balle.
     */
    void initGameObjects() {
        m_lives = Config::PLAYER_STARTING_LIVES;
        m_score = 0;
        m_firstOrangeBrickHit = false;
        m_firstRedBrickHit = false;
        m_bricksHitInCurrentGame = 0;
        m_speedBoost4thBrickDone = false;
        m_speedBoost12thBrickDone = false;

        m_paddle.init(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
        if (!m_level.load(Config::LEVEL_FILENAME)) {
            std::cerr << "Warning: Level file issues during game init. Check '" << Config::LEVEL_FILENAME << "'." << std::endl;
        }
        m_level.setupBricks(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
        resetBallAndPaddle();
        m_statusMessage = "Click to launch ball";
    }

    /**
     * @brief Réinitialise la position de la balle (sur la raquette) et de la raquette (au centre).
     * @note La balle est initialisée immobile, prête à être lancée.
     */
    void resetBallAndPaddle() {
        m_paddle.resetPosition(static_cast<float>(m_windowWidth));
        float ballStartX = m_paddle.getX() + m_paddle.getWidth() / 2.0f;
        float ballStartY = m_paddle.getY() + m_paddle.getHeight() + m_ball.getRadius() + 5.0f;
        float initialVelX = m_windowWidth * Config::BALL_INITIAL_SPEED_X_FACTOR * ((rand() % 2 == 0) ? 1.0f : -1.0f);
        float initialVelY = m_windowHeight * Config::BALL_INITIAL_SPEED_Y_FACTOR;
        m_ball.reset(ballStartX, ballStartY, initialVelX, std::abs(initialVelY));
        m_ball.stop();
        m_statusMessage = "Click to launch ball";
    }

    /**
     * @brief Réinitialise la balle après la perte d'une vie.
     * @note Tente de préserver la vitesse de la balle si elle était significative, sinon utilise la vitesse par défaut.
     *       La balle est replacée sur la raquette, immobile.
     */
    void resetAfterLifeLoss() {
        Vec2 lastVelocity = m_ball.getVelocity();
        float lastSpeed = std::sqrt(lastVelocity.x * lastVelocity.x + lastVelocity.y * lastVelocity.y);
        float speedPreservationThreshold = static_cast<float>(m_windowWidth) * Config::BALL_INITIAL_SPEED_X_FACTOR * 0.8f; 

        m_paddle.resetPosition(static_cast<float>(m_windowWidth));
        float ballStartX = m_paddle.getX() + m_paddle.getWidth() / 2.0f;
        float ballStartY = m_paddle.getY() + m_paddle.getHeight() + m_ball.getRadius() + 5.0f;

        float newVelX, newVelY;
        if (m_lives < Config::PLAYER_STARTING_LIVES && lastSpeed > speedPreservationThreshold && lastSpeed > 0) {
            float angleDegrees = 30.0f + static_cast<float>(rand() % 121);
            float angleRadians = angleDegrees * (M_PI / 180.0f);
            newVelX = lastSpeed * std::cos(angleRadians);
            newVelY = lastSpeed * std::sin(angleRadians);
            std::cout << "Life lost, attempting to preserve speed: " << lastSpeed << " at angle: " << angleDegrees << std::endl;
        } else {
            newVelX = static_cast<float>(m_windowWidth) * Config::BALL_INITIAL_SPEED_X_FACTOR * ((rand() % 2 == 0) ? 1.0f : -1.0f);
            newVelY = static_cast<float>(m_windowHeight) * Config::BALL_INITIAL_SPEED_Y_FACTOR;
            std::cout << "Using default initial speed after life loss." << std::endl;
        }

        m_ball.reset(ballStartX, ballStartY, newVelX, std::abs(newVelY));
        m_ball.setPreservedVelocity(newVelX, newVelY);
        m_ball.stop();
        m_statusMessage = (m_lives > 0) ? "Click to launch ball" : "";
    }

    /**
     * @brief Initialise l'état de l'éditeur de niveaux.
     * @note Charge le niveau actuel et calcule les dimensions de la grille.
     */
    void initEditorState() {
        if (!m_level.load(Config::LEVEL_FILENAME)) {
             std::cerr << "Warning: Level file issues during editor init. Check '" << Config::LEVEL_FILENAME << "'." << std::endl;
        }
        m_level.calculateGridDimensions(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
        m_statusMessage = "L-Click: Select | R-Click: Place (Shift+R-C: Indestructible) | Del: Clear";
    }

    /**
     * @brief Met à jour la logique du menu principal.
     * @param dt Delta-temps.
     * @note Gère les clics sur les boutons du menu pour changer d'état ou quitter.
     */
    void updateMenu(float dt) {
        float buttonW = m_windowWidth * 0.3f; float buttonH = m_windowHeight * 0.08f;
        float spacingY = m_windowHeight * 0.04f; float centerX = m_windowWidth / 2.0f;
        float topButtonY = m_windowHeight * 0.6f;
        Rect playButtonRect = { centerX - buttonW / 2.0f, topButtonY, buttonW, buttonH };
        Rect editorButtonRect = { centerX - buttonW / 2.0f, topButtonY - buttonH - spacingY, buttonW, buttonH };
        Rect quitButtonRect = { centerX - buttonW / 2.0f, editorButtonRect.y - buttonH - spacingY, buttonW, buttonH };

        if (m_inputManager.wasMouseLeftClicked()) {
            float mx = m_inputManager.getMouseX(); float my = m_inputManager.getMouseY();
            if (mx >= playButtonRect.x && mx <= playButtonRect.x + playButtonRect.width && my >= playButtonRect.y && my <= playButtonRect.y + playButtonRect.height) {
                std::cout << "Menu: Play button clicked. Switching to GAME state." << std::endl;
                initGameObjects(); m_currentState = GameState::GAME;
            } else if (mx >= editorButtonRect.x && mx <= editorButtonRect.x + editorButtonRect.width && my >= editorButtonRect.y && my <= editorButtonRect.y + editorButtonRect.height) {
                std::cout << "Menu: Editor button clicked. Switching to EDITOR state." << std::endl;
                initEditorState(); m_currentState = GameState::EDITOR;
            } else if (mx >= quitButtonRect.x && mx <= quitButtonRect.x + quitButtonRect.width && my >= quitButtonRect.y && my <= quitButtonRect.y + quitButtonRect.height) {
                std::cout << "Menu: Quit button clicked. Closing window." << std::endl;
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            }
        }
    }

    /**
     * @brief Effectue le rendu du menu principal.
     * @note Affiche le titre, les boutons et le logo/copyright.
     */
    void renderMenu() {
        const std::string title = "GLFW BREAKOUT";
        float titleScale = 1.5f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        float titleCenterX = m_windowWidth / 2.0f;
        float titleY = m_windowHeight * 0.85f;
        m_textRenderer.render(title, titleCenterX, titleY, titleScale, Config::COLOR_TEXT_ACCENT);

        float buttonW = m_windowWidth * 0.3f; float buttonH = m_windowHeight * 0.08f;
        float spacingY = m_windowHeight * 0.04f; float centerX = m_windowWidth / 2.0f;
        float topButtonY = m_windowHeight * 0.6f;
        float textScale = 0.9f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        float textHeight = m_textRenderer.getTextHeight(textScale);
        float textYOffset = (buttonH - textHeight) / 2.0f;

        Rect playButtonRect = { centerX - buttonW / 2.0f, topButtonY, buttonW, buttonH };
        Utils::drawRect(playButtonRect, Config::COLOR_BUTTON_PLAY);
        const std::string playText = "Play Game";
        float playTextCenterX = playButtonRect.x + playButtonRect.width / 2.0f;
        m_textRenderer.render(playText, playTextCenterX, playButtonRect.y - textYOffset, textScale, Config::COLOR_TEXT_DEFAULT);

        Rect editorButtonRect = { centerX - buttonW / 2.0f, topButtonY - buttonH - spacingY, buttonW, buttonH };
        Utils::drawRect(editorButtonRect, Config::COLOR_BUTTON_EDITOR);
        const std::string editorText = "Level Editor";
        float editorTextCenterX = editorButtonRect.x + editorButtonRect.width / 2.0f;
        m_textRenderer.render(editorText, editorTextCenterX, editorButtonRect.y - textYOffset, textScale, {0.1f, 0.1f, 0.1f});

        Rect quitButtonRect = { centerX - buttonW / 2.0f, editorButtonRect.y - buttonH - spacingY, buttonW, buttonH };
        Utils::drawRect(quitButtonRect, Config::COLOR_BUTTON_QUIT);
        const std::string quitText = "Quit";
        float quitTextCenterX = quitButtonRect.x + quitButtonRect.width / 2.0f;
        m_textRenderer.render(quitText, quitTextCenterX, quitButtonRect.y - textYOffset, textScale, Config::COLOR_TEXT_DEFAULT);

        if (m_logoTextureID != 0 && m_logoHeight > 0) {
            float logoDrawHeight = Config::LOGO_TARGET_HEIGHT;
            float scaleFactor = logoDrawHeight / m_logoHeight;
            float logoDrawWidth = m_logoWidth * scaleFactor;
            float logoX = 15.0f;
            float logoY = 15.0f;

            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            Utils::drawTexturedRect(logoX, logoY, logoDrawWidth, logoDrawHeight, m_logoTextureID);
            glDisable(GL_BLEND);

            std::string nameText = Config::COPYRIGHT_NAME;
            float nameTextTargetHeight = logoDrawHeight * 0.6f;
            float nameTextScale = nameTextTargetHeight / m_textRenderer.getTextHeight(1.0f);
            float nameY = logoY + (logoDrawHeight - m_textRenderer.getTextHeight(nameTextScale)) * 0.5f;
            float nameX_start = logoX + logoDrawWidth + Config::LOGO_NAME_SPACING;
            float nameTextWidth = m_textRenderer.getTextWidth(nameText, nameTextScale);
            float nameTextCenterX_for_left_align = nameX_start + nameTextWidth / 2.0f;
            m_textRenderer.render(nameText, nameTextCenterX_for_left_align, nameY, nameTextScale, Config::COLOR_TEXT_DEFAULT);
        } else if (m_logoTextureID == 0) {
            std::string nameText = Config::COPYRIGHT_NAME;
            float nameTextScale = 0.5f; 
            float nameX_start = 15.0f; 
            float nameY = 15.0f;
            float nameTextWidth = m_textRenderer.getTextWidth(nameText, nameTextScale);
            float nameTextCenterX_for_left_align = nameX_start + nameTextWidth / 2.0f;
            m_textRenderer.render(nameText, nameTextCenterX_for_left_align, nameY, nameTextScale, Config::COLOR_TEXT_DEFAULT);
        }
    }

    /**
     * @brief Met à jour la logique du jeu principal (mode GAME).
     * @param dt Delta-temps.
     * @note Gère le lancement de la balle, les mouvements de la raquette, les collisions, la perte de vies et les conditions de victoire/défaite.
     */
    void updateGame(float dt) {
        if (m_inputManager.wasKeyPressed(Config::KEY_MENU_TOGGLE)) {
            m_statusMessage = ""; m_currentState = GameState::MENU; return;
        }

        if (!m_ball.isMoving() && m_lives > 0 && !m_level.areAllBricksCleared() && m_inputManager.wasMouseLeftClicked()) {
             float launchVelX = m_windowWidth * Config::BALL_INITIAL_SPEED_X_FACTOR * ((rand() % 2 == 0) ? 1.0f : -1.0f);
             float launchVelY = m_windowHeight * Config::BALL_INITIAL_SPEED_Y_FACTOR;
            if (m_lives < Config::PLAYER_STARTING_LIVES) {
                Vec2 preserved_speed = m_ball.getPreservedVelocity();
                launchVelX = preserved_speed.x;
                launchVelY = preserved_speed.y;
            }
             m_ball.setVelocity(launchVelX, std::abs(launchVelY));
             m_statusMessage = "";
        }

        if (m_ball.isMoving() || (m_lives > 0 && !m_level.areAllBricksCleared())) {
            m_paddle.update(dt, m_inputManager, static_cast<float>(m_windowWidth));
        }

        if(m_ball.isMoving()){
            m_level.checkCollisions(m_ball);            
            m_ball.checkCollisionPaddle(m_paddle, dt);  
            int hitBrickIndex = m_ball.update(dt, static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight), m_level.m_gridStartX, m_level.m_gridStartX + m_level.m_gridTotalWidth);
            
            if (hitBrickIndex != -1) { 
                int brickOriginalScoreValue = m_level.processBrickHit(hitBrickIndex); 
                if (brickOriginalScoreValue > 0) { 
                    m_score += brickOriginalScoreValue;
                    m_bricksHitInCurrentGame++;
                              
                    float speedMultiplier = 1.0f;
                    if (brickOriginalScoreValue == Config::SCORE_ORANGE && !m_firstOrangeBrickHit) {
                        speedMultiplier *= 1.19f; m_firstOrangeBrickHit = true;
                    }
                    if (brickOriginalScoreValue == Config::SCORE_RED && !m_firstRedBrickHit) {
                        speedMultiplier *= 1.19f; m_firstRedBrickHit = true;
                    }
                    if (m_bricksHitInCurrentGame == 4 && !m_speedBoost4thBrickDone) {
                        speedMultiplier *= 1.19f; m_speedBoost4thBrickDone = true;
                    }
                    if (m_bricksHitInCurrentGame == 12 && !m_speedBoost12thBrickDone) {
                        speedMultiplier *= 1.19f; m_speedBoost12thBrickDone = true;
                    }
                    if (speedMultiplier > 1.001f) { 
                        Vec2 currentVel = m_ball.getVelocity();
                        m_ball.setVelocity(currentVel.x * speedMultiplier, currentVel.y * speedMultiplier);
                    }
                }
            }

            if (m_ball.getY() - m_ball.getRadius() < 0) {
                m_lives--;
                std::cout << "Life lost! Lives remaining: " << m_lives << ". Score: " << m_score << std::endl;
                if (m_lives > 0) {
                    resetAfterLifeLoss();
                } else {
                    m_statusMessage = "Game Over! Score: " + std::to_string(m_score) ;
                    m_ball.stop();
                }
            } else if (m_level.areAllBricksCleared()) {
                m_statusMessage = "You Win! Score: " + std::to_string(m_score) ;
                m_ball.stop();
            }
        } else if (m_lives > 0 && !m_level.areAllBricksCleared()) { // Ball not moving, but game is on
             float ballStartX = m_paddle.getX() + m_paddle.getWidth() / 2.0f;
             float ballStartY = m_paddle.getY() + m_paddle.getHeight() + m_ball.getRadius() + 1.0f;
             m_ball.setPosition(ballStartX, ballStartY);
        }
    }

    /**
     * @brief Effectue le rendu du jeu principal (mode GAME).
     * @note Affiche la raquette, la balle, les briques et l'HUD (score, vies, messages).
     *       Dessine également des bordures si la grille de briques n'occupe pas toute la largeur de la fenetre.
     */
    void renderGame() {
        float gridEndX = m_level.m_gridStartX + m_level.m_gridTotalWidth;

        if (m_level.m_gridStartX > 0.1f) {
            Utils::drawRect(0.0f, 0.0f, m_level.m_gridStartX, static_cast<float>(m_windowHeight), Config::COLOR_BORDER);
        }
        if (gridEndX < m_windowWidth - 0.1f) {
             float rightBorderWidth = static_cast<float>(m_windowWidth) - gridEndX;
             Utils::drawRect(gridEndX, 0.0f, rightBorderWidth, static_cast<float>(m_windowHeight), Config::COLOR_BORDER);
        }
        
        // Rendu des éléments du jeu
        m_paddle.render();
        m_level.render();
        m_ball.render();

        float hudScale = 0.6f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        float hudYPos = m_windowHeight - 35.0f; 
        float hudBottomYPos = 35.0f;
        float hudXMargin = 15.0f;

        std::string scoreText = "Score: " + std::to_string(m_score);
        float scoreTextWidth = m_textRenderer.getTextWidth(scoreText, hudScale);
        float scoreTextCenterX_for_left_align = hudXMargin + scoreTextWidth / 2.0f;
        m_textRenderer.render(scoreText, scoreTextCenterX_for_left_align, hudYPos, hudScale, Config::COLOR_TEXT_DEFAULT);

        std::string livesText = "Lives: " + std::to_string(m_lives);
        float livesTextWidth = m_textRenderer.getTextWidth(livesText, hudScale);
        float livesTextCenterX_for_left_align = hudXMargin + livesTextWidth / 2.0f;
        m_textRenderer.render(livesText, livesTextCenterX_for_left_align, hudBottomYPos, hudScale, Config::COLOR_TEXT_ERROR);


        std::string menuText = "M: Menu";
        float menuTextWidth = m_textRenderer.getTextWidth(menuText, hudScale);
        float menuTextCenterX_for_right_align = (m_windowWidth - hudXMargin) - menuTextWidth / 2.0f;
        m_textRenderer.render(menuText, menuTextCenterX_for_right_align, hudYPos, hudScale, Config::COLOR_TEXT_DEFAULT);

        // Affichage des messages d'état
        if (!m_statusMessage.empty()) {
            if (m_lives <= 0 || m_level.areAllBricksCleared() || !m_ball.isMoving()) {
                float msgScale = 1.2f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
                int numLines = 1;
                for(char ch_c : m_statusMessage) if(ch_c == '\n') numLines++;
                float singleLineHeight = m_textRenderer.getTextHeight(msgScale);
                float totalMsgBlockHeight = singleLineHeight * numLines;

                float msgCenterX = m_windowWidth / 2.0f;
                float msgY_baseline_first_line = m_windowHeight * 0.5f + totalMsgBlockHeight / 2.0f - singleLineHeight * 0.5f;

                Config::Color msgColor = Config::COLOR_TEXT_DEFAULT;
                 if (m_statusMessage.find("Win") != std::string::npos) msgColor = Config::COLOR_TEXT_ACCENT;
                 else if (m_statusMessage.find("Over") != std::string::npos) msgColor = Config::COLOR_TEXT_ERROR;
                 else if (m_statusMessage.find("Click") != std::string::npos) msgColor = Config::COLOR_TEXT_DEFAULT;
                m_textRenderer.render(m_statusMessage, msgCenterX, msgY_baseline_first_line, msgScale, msgColor);
            }
        }
    }

    /**
     * @brief Met à jour la logique de l'éditeur de niveaux.
     * @param dt Delta-temps.
     * @note Gère la sélection/placement/suppression de briques, la sauvegarde du niveau et le retour au menu.
     */
    void updateEditor(float dt) {
        m_level.calculateGridDimensions(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));

        if (m_inputManager.wasKeyPressed(Config::KEY_MENU_TOGGLE)) {
            m_statusMessage = ""; m_currentState = GameState::MENU; return;
        }

        float buttonPaddingX = 30.0f; float buttonH = m_windowHeight * 0.07f;
        float buttonY = m_windowHeight * 0.05f; float buttonSpacingX = m_windowWidth * 0.05f;
        float textScale = 0.7f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        const std::string backText = "Back to Menu (M)"; const std::string saveText = "Save Level";
        float backTextWidth = m_textRenderer.getTextWidth(backText, textScale); float saveTextWidth = m_textRenderer.getTextWidth(saveText, textScale);
        float backButtonW = backTextWidth + buttonPaddingX; float saveButtonW = saveTextWidth + buttonPaddingX;
        float totalButtonWidth = backButtonW + saveButtonW + buttonSpacingX;
        float backButtonX = (m_windowWidth - totalButtonWidth) / 2.0f; float saveButtonX = backButtonX + backButtonW + buttonSpacingX;
        Rect saveButtonRect = { saveButtonX, buttonY, saveButtonW, buttonH };
        Rect backButtonRect = { backButtonX, buttonY, backButtonW, buttonH };

        bool statusNeedsUpdate = false;
        bool saveAttemptedThisFrame = false;
        float mx = m_inputManager.getMouseX(); float my = m_inputManager.getMouseY();

        if (m_inputManager.wasMouseLeftClicked()) {
            if (mx >= saveButtonRect.x && mx <= saveButtonRect.x + saveButtonRect.width && my >= saveButtonRect.y && my <= saveButtonRect.y + saveButtonRect.height) {
                if (m_level.save(Config::LEVEL_FILENAME)) m_statusMessage = "Level saved successfully!";
                else m_statusMessage = "ERROR: Could not save level!";
                statusNeedsUpdate = true;
                saveAttemptedThisFrame = true;
            } else if (mx >= backButtonRect.x && mx <= backButtonRect.x + backButtonRect.width && my >= backButtonRect.y && my <= backButtonRect.y + backButtonRect.height) {
                m_statusMessage = ""; m_currentState = GameState::MENU; return;
            } else {
                int selectionResult = m_level.handleEditorLeftClickSelection(mx, my);
                if (selectionResult == 1) {
                    statusNeedsUpdate = true;
                }
            }
        }
        else if (m_inputManager.wasMouseRightClicked()) {
            bool placed = m_level.handleEditorRightClickPlace(m_inputManager.isShiftDown());
            if (placed) {
                statusNeedsUpdate = true;
            }
        }
        else if (m_inputManager.wasKeyPressed(Config::KEY_EDITOR_SET_EMPTY)) {
            if (m_level.hasEditorSelection()) {
                m_level.clearSelectedBrick();
                statusNeedsUpdate = true;
            }
        }

        // Met à jour le message d'état si nécessaire
        if (statusNeedsUpdate) {
           if (!saveAttemptedThisFrame) { // N'écrase pas le message de sauvegarde/erreur
               if (m_level.hasEditorSelection()) {
                   int currentScore = m_level.getSelectedBrickScore();
                   int r = m_level.getSelectedRow(); int c = m_level.getSelectedCol();
                   std::string scoreStr;
                   if (currentScore == Config::SCORE_INDESTRUCTIBLE) scoreStr = "Indestructible";
                   else if (currentScore > 0) scoreStr = std::to_string(currentScore) + "pts";
                   else scoreStr = "Empty";

                   m_statusMessage = "Selected [" + std::to_string(r) + "," + std::to_string(c) +
                                     "] Type: " + scoreStr + "\n R-Click: Place (Shift+R-C: Indestructible) | Del: Clear";
               } else {
                   m_statusMessage = "L-Click: Select | R-Click: Place (Shift+R-C: Indestructible) \n Del: Clear";
               }
           }
       }
}
    /**
     * @brief Effectue le rendu de l'éditeur de niveaux.
     * @note Affiche la grille d'édition, la surbrillance de la cellule sélectionnée, le titre, les messages et les boutons.
     */
    void renderEditor() {
        // Dessine les bordures latérales si la grille n'occupe pas toute la largeur
        float gridEndX = m_level.m_gridStartX + m_level.m_gridTotalWidth;
        if (m_level.m_gridStartX > 0.1f) {
            Utils::drawRect(0.0f, 0.0f, m_level.m_gridStartX, static_cast<float>(m_windowHeight), Config::COLOR_BORDER);
        }
        if (gridEndX < m_windowWidth - 0.1f) {
             float rightBorderWidth = static_cast<float>(m_windowWidth) - gridEndX;
             Utils::drawRect(gridEndX, 0.0f, rightBorderWidth, static_cast<float>(m_windowHeight), Config::COLOR_BORDER);
        }

        const std::string title = "Level Editor"; 
        float titleScale = 1.1f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        float titleCenterX = m_windowWidth / 2.0f; 
        float titleY = m_windowHeight * 0.95f - 0.35f * m_textRenderer.getTextHeight(titleScale);
        m_textRenderer.render(title, titleCenterX, titleY, titleScale, Config::COLOR_TEXT_DEFAULT);

        // Rendu de la grille et de la surbrillance
        m_level.renderEditorGrid(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
        m_level.highlightEditorCell(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));

        if (!m_statusMessage.empty()) {
            float msgScale = 0.6f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
            
            int numLines = 1;
            for(char ch_c : m_statusMessage) if(ch_c == '\n') numLines++;
            float singleLineHeight = m_textRenderer.getTextHeight(msgScale);
            float totalMsgBlockHeightEst = singleLineHeight * numLines;

            float msgCenterX = m_windowWidth / 2.0f;
            // Positionne le message au-dessus des boutons (environ 18% de la hauteur de l'écran)
            float desiredBlockCenterY = m_windowHeight * 0.18f;
            float msgY_baseline_first_line = desiredBlockCenterY + totalMsgBlockHeightEst / 2.0f - singleLineHeight * 0.5f;


            Config::Color msgColor = Config::COLOR_TEXT_DEFAULT;
            if (m_statusMessage.find("ERROR") != std::string::npos) msgColor = Config::COLOR_TEXT_ERROR;
            else if (m_statusMessage.find("saved") != std::string::npos) msgColor = Config::COLOR_TEXT_ACCENT;
            m_textRenderer.render(m_statusMessage, msgCenterX, msgY_baseline_first_line, msgScale, msgColor);
        }

        // Rendu des boutons de sauvegarde et de retour
        float buttonPaddingX = 30.0f; float buttonH = m_windowHeight * 0.07f;
        float buttonY = m_windowHeight * 0.05f; float buttonSpacingX = m_windowWidth * 0.05f;
        float textScale = 0.7f * (static_cast<float>(m_windowWidth) / Config::DEFAULT_WINDOW_WIDTH);
        float textHeight = m_textRenderer.getTextHeight(textScale);
        float textYOffset = (buttonH - textHeight) / 2.0f;

        const std::string backText = "Back to Menu (M)"; const std::string saveText = "Save Level";
        float backTextWidth = m_textRenderer.getTextWidth(backText, textScale); float saveTextWidth = m_textRenderer.getTextWidth(saveText, textScale);
        float backButtonW = backTextWidth + buttonPaddingX; float saveButtonW = saveTextWidth + buttonPaddingX;
        float totalButtonWidth = backButtonW + saveButtonW + buttonSpacingX;
        float backButtonX_start = (m_windowWidth - totalButtonWidth) / 2.0f; 
        float saveButtonX_start = backButtonX_start + backButtonW + buttonSpacingX;

        // Bouton Sauvegarder
        Rect saveButtonRect = { saveButtonX_start, buttonY, saveButtonW, buttonH };
        Utils::drawRect(saveButtonRect, Config::COLOR_BUTTON_SAVE);
        float saveTextCenterX = saveButtonRect.x + saveButtonRect.width / 2.0f;
        m_textRenderer.render(saveText, saveTextCenterX, saveButtonRect.y - textYOffset, textScale, Config::COLOR_TEXT_DEFAULT);

        // Bouton Retour
        Rect backButtonRect = { backButtonX_start, buttonY, backButtonW, buttonH };
        Utils::drawRect(backButtonRect, Config::COLOR_BUTTON_BACK);
        float backTextCenterX = backButtonRect.x + backButtonRect.width / 2.0f;
        m_textRenderer.render(backText, backTextCenterX, backButtonRect.y - textYOffset, textScale, Config::COLOR_TEXT_DEFAULT);
    }

    /** @brief Callback d'erreur GLFW (statique). */
    static void errorCallback_GLFW(int error, const char* description) { std::cerr << "GLFW Error [" << error << "]: " << description << std::endl; }
    
    /** @brief Callback des touches clavier GLFW (statique). Transmet à l'InputManager de l'instance Game. */
    static void keyCallback_GLFW(GLFWwindow* window, int key, int scancode, int action, int mods) { if(auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window))) game->m_inputManager.keyCallback(key, action); }
    
    /** @brief Callback des boutons de la souris GLFW (statique). Transmet à l'InputManager. */
    static void mouseButtonCallback_GLFW(GLFWwindow* window, int button, int action, int mods) { if(auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window))) game->m_inputManager.mouseButtonCallback(button, action); }
    
    /** @brief Callback de la position du curseur GLFW (statique). Transmet à l'InputManager. */
    static void cursorPosCallback_GLFW(GLFWwindow* window, double xpos, double ypos) { if(auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window))) game->m_inputManager.cursorPosCallback(xpos, ypos); }

    /** @brief Callback de redimensionnement du framebuffer GLFW (statique). Appelle la méthode membre correspondante. */
    static void framebufferSizeCallback_GLFW(GLFWwindow* window, int width, int height) { if(auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window))) game->framebufferSizeCallback(width, height); }

    /**
     * @brief Méthode membre appelée lors du redimensionnement du framebuffer.
     * @param width Nouvelle largeur du framebuffer.
     * @param height Nouvelle hauteur du framebuffer.
     * @note Met à jour les dimensions de la fenêtre, le viewport OpenGL, et recalcule les positions des éléments si nécessaire.
     */
    void framebufferSizeCallback(int width, int height) {
         if (width <= 0 || height <= 0) return;
         m_windowWidth = width; m_windowHeight = height;
         Config::WINDOW_WIDTH = width;
         Config::WINDOW_HEIGHT = height;
         m_inputManager.setWindowHeight(height);
         glViewport(0, 0, width, height);
         std::cout << "Framebuffer resized to " << width << " x " << height << std::endl;
         m_level.calculateGridDimensions(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
         if (m_currentState == GameState::GAME) {
             m_paddle.init(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
             if (!m_ball.isMoving()) {
                 resetBallAndPaddle();
             }
         }
    }
};

/**
* @brief Fonction principale du programme.
* @return 0 si succès, -1 en cas d'échec d'initialisation.
* @note Initialise le générateur de nombres aléatoires, crée et lance le jeu.
*/
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    Game breakoutGame;
    if (!breakoutGame.init()) {
        std::cerr << "FATAL: Game initialization failed. Exiting." << std::endl;
        return -1;
    }
    breakoutGame.run();
    std::cout << "Exiting application" << std::endl;
    return 0;
}