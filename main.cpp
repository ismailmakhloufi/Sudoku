#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <array>
#include <fstream>
#include <map>
#include<cmath>

// États du jeu
enum GameState {
    MENU,
    PLAYING,
    OPTIONS,
    STATS,
    SHOP,
    PAUSED,
    SHOP_PROMPT,
    QUIT
};

class Sudoku;
class Cell {
private:
    int value;
    int solution;
    bool fixed;
    bool selected;
    bool incorrect;
    sf::RectangleShape shape;
    sf::Text text;
    bool notes[9];
    sf::Text notesText[9];
    bool highlighted = false;
    int row;
    int col;
    Sudoku* sudoku;
    bool firstAttempt = true;
    bool firstAttemptCorrect = false;

public:
    Cell(Sudoku* sudokuRef = nullptr, int r = -1, int c = -1) :sudoku(sudokuRef), row(r), col(c), value(0), solution(0), fixed(false), selected(false), incorrect(false) {
        for (int i = 0; i < 9; i++) {
            notes[i] = false;
        }
    }
    int getRow() const { return row; }
    int getCol() const { return col; }
    void setSudokuReference(Sudoku* sudokuRef) { sudoku = sudokuRef; }
    void markAttempt(bool correct) {
        if (firstAttempt) {
            firstAttemptCorrect = correct;
            firstAttempt = false;
        }
    }

    bool getFirstAttemptStatus() const {
        return firstAttemptCorrect;
    }

    void setValue(int val) {
        if (value == 0 && val != 0) {
            bool correct = (val == solution);
            markAttempt(correct);
        }
        value = val;
        incorrect = (val != 0 && val != solution);
    }

    int getValue() const {
        return value;
    }

    void setSolution(int sol) {
        solution = sol;
    }

    int getSolution() const {
        return solution;
    }

    void setFixed(bool fix) {
        fixed = fix;
    }

    bool isFixed() const {
        return fixed;
    }

    void setSelected(bool sel) {
        selected = sel;
    }

    bool isSelected() const {
        return selected;
    }

    void setIncorrect(bool inc) {
        incorrect = inc;
    }

    bool isIncorrect() const {
        return incorrect;
    }

    void setShape(sf::RectangleShape s) {
        shape = s;
    }

    sf::RectangleShape getShape() const {
        return shape;
    }
    void setHighlighted(bool state) { highlighted = state; }
    bool isHighlighted() const { return highlighted; }

    void setText(sf::Text t) {
        text = t;
    }

    sf::Text& getText() {
        return text;
    }

    void toggleNote(int num) {
        if (num >= 1 && num <= 9) {
            notes[num-1] = !notes[num-1];
        }
    }

    bool hasNote(int num) const {
        if (num >= 1 && num <= 9) {
            return notes[num-1];
        }
        return false;
    }

    void clearNotes() {
        for (int i = 0; i < 9; i++) {
            notes[i] = false;
        }
    }

    void initNotesText(sf::Font& font) {
        for (int i = 0; i < 9; i++) {
            notesText[i].setFont(font);
            notesText[i].setCharacterSize(12);
            notesText[i].setFillColor(sf::Color(180, 180, 200));
            notesText[i].setString(std::to_string(i+1));
            int row = i / 3;
            int col = i % 3;
            notesText[i].setPosition(shape.getPosition().x + 5 + col * 16,
                                    shape.getPosition().y + 5 + row * 16);
        }
    }

    void updateText(sf::Font& font) {
        if (value != 0) {
            text.setString(std::to_string(value));
        } else {
            text.setString("");
        }

        // Gestion de la couleur du texte
        if (fixed) {
            text.setFillColor(sf::Color(100, 100, 140));
        } else if (incorrect) {
            text.setFillColor(sf::Color::Red);
        } else {
            text.setFillColor(sf::Color::Blue);
        }

        if (selected) {
            shape.setFillColor(sf::Color(255, 240, 150));
        }
        else if (highlighted) {
            shape.setFillColor(sf::Color(150, 230, 255));
        }
        else if (incorrect) {
            shape.setFillColor(sf::Color(255, 200, 200));
        }
        else {
            shape.setFillColor(sf::Color(245, 245, 255));
        }
        if (notesText[0].getString() == "") {
            initNotesText(font);
        }
    }

    // Méthode pour afficher les notes
    void drawNotes(sf::RenderWindow* window) {
        if (value == 0) {
            for (int i = 0; i < 9; i++) {
                if (notes[i]) {
                    window->draw(notesText[i]);
                }
            }
        }
    }
};
class Menu {
private:
    std::vector<sf::Text> options;
    int selectedItemIndex;
    sf::Font& font;
    int width, height;

public:
    Menu(float width, float height, sf::Font& menuFont) :
        font(menuFont), width(width), height(height), selectedItemIndex(0) {

        const std::vector<std::string> menuTexts = {
            "Start",
            "Options",
            "Stats",
            "Boutique",
            "Quit"
        };

        // Configuration de l'espacement réduit
        float verticalSpacing = 80; // Espacement fixe entre les options
        float startY = (height - (menuTexts.size() * verticalSpacing)) / 2; // Centrage vertical

        for (size_t i = 0; i < menuTexts.size(); ++i) {
            sf::Text text;
            text.setFont(font);
            text.setString(menuTexts[i]);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::White);

            // Positionnement centré
            sf::FloatRect bounds = text.getLocalBounds();
            text.setPosition(
                (width - bounds.width) / 2,
                startY + i * verticalSpacing
            );

            options.push_back(text);
        }

        // Couleur initiale jaune pour la sélection
        options[selectedItemIndex].setFillColor(sf::Color::Yellow);
    }

    void draw(sf::RenderWindow& window) {
        // Fond dégradé
        sf::VertexArray background(sf::Quads, 4);
        background[0].position = sf::Vector2f(0, 0);
        background[1].position = sf::Vector2f(width, 0);
        background[2].position = sf::Vector2f(width, height);
        background[3].position = sf::Vector2f(0, height);

        background[0].color = sf::Color(30, 30, 60);
        background[1].color = sf::Color(60, 60, 100);
        background[2].color = sf::Color(30, 30, 60);
        background[3].color = sf::Color(60, 60, 100);

        window.draw(background);

        // Titre "SUDOKU"
        sf::Text titleText;
        titleText.setFont(font);
        titleText.setString("SUDOKU");
        titleText.setCharacterSize(60);
        titleText.setFillColor(sf::Color(173, 216, 230));
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(
            (width - titleText.getLocalBounds().width) / 2,
            30
        );
        window.draw(titleText);

        // Dessiner les options
        for (const auto& option : options) {
            window.draw(option);
        }
    }

    void moveUp() {
        options[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex = (selectedItemIndex - 1 + options.size()) % options.size();
        options[selectedItemIndex].setFillColor(sf::Color::Yellow);
    }

    void moveDown() {
        options[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex = (selectedItemIndex + 1) % options.size();
        options[selectedItemIndex].setFillColor(sf::Color::Yellow);
    }

    int getSelectedItemIndex() const {
        return selectedItemIndex;
    }
};
class Options {
private:
    sf::Text title;
    sf::Text instructions;
    sf::Text backText;
    sf::Font font;

public:
    Options(float width, float height, sf::Font& optionsFont) : font(optionsFont) {
        title.setFont(font);
        title.setString("Comment jouer");
        title.setCharacterSize(50);
        title.setFillColor(sf::Color(108, 69, 128));
        title.setStyle(sf::Text::Bold);
        title.setPosition(sf::Vector2f(130, 20));

        instructions.setFont(font);
        std::string instructionsStr =
        "1. Cliquez sur une case pour la s\xE9lectionner\n\n"
        "2. Tapez un chiffre de 1 \xE0 9 pour le placer\n\n"
        "3. Si le chiffre est incorrect,l'écran deviendra rouge\n\n"
        "4. Remplissez toutes les cases correctement pour gagner\n\n"
        "5. Il faut que le nombre ne se r\xE9p\xE8te pas dans la ligne,la colonne \n\n"
        "et la sous-grille\n\n"
        "6. Appuyez sur N pour activer/d\xE9sactiver le mode notes\n\n"
        "7. En mode notes, les chiffres sont ajout\xE9s comme aide-m\xE9moire\n\n"
        "8. P ou ESPACE pour mettre le jeu en pause\n\n"
        "9. Echap pour revenir au menu";
        instructions.setString(instructionsStr);
        instructions.setCharacterSize(20);
        instructions.setFillColor(sf::Color(108, 69, 128));
        instructions.setPosition(sf::Vector2f(50, 120));

        backText.setFont(font);
        backText.setString("Appuyez sur une touche pour revenir au menu");
        backText.setCharacterSize(20);
        backText.setFillColor(sf::Color(108, 69, 128));
        backText.setPosition(sf::Vector2f(200, 650));
        backText.setStyle(sf::Text::Bold);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(title);
        window.draw(instructions);
        window.draw(backText);
    }
};

class Shop {
private:
    int width, height;
    sf::Font& font;
    int& playerScore;
    std::vector<bool>& unlockedDecorations;
    int selectedItem = 0;

    struct ShopItem {
        std::string name;
        std::string description;
        int price;
        bool isDecoration;
        int index;
    };

    std::vector<ShopItem> items;

public:
    Shop(int w, int h, sf::Font& f, int& score, std::vector<bool>& decorations)
        : width(w), height(h), font(f), playerScore(score), unlockedDecorations(decorations) {

        // Initialiser les articles de la boutique
        items = {
            {"Indice supplémentaire", "Ajoute un indice à utiliser pendant le jeu", 500, false, 1},
            {"Pack de 3 indices", "Ajoute 3 indices à utiliser pendant le jeu", 1200, false, 3},
            {"Confettis dorés", "Confettis dorés pour les animations de victoire", 2000, true, 0},
            {"Thème Galaxie", "Un thème spatial pour la grille de jeu", 3500, true, 1},
            {"Couronnes de victoire", "Affiche une couronne lors d'une victoire sans erreur", 5000, true, 2}
        };
    }

    void draw(sf::RenderWindow& window) {
        // Fond de la boutique
        sf::RectangleShape background(sf::Vector2f(width, height));
        background.setFillColor(sf::Color(240, 240, 255));
        window.draw(background);

        // Titre
        sf::Text titleText;
        titleText.setFont(font);
        titleText.setString("BOUTIQUE");
        titleText.setCharacterSize(40);
        titleText.setFillColor(sf::Color(60, 60, 100));
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(width / 2 - 100, 50);
        window.draw(titleText);

        // Affichage du score actuel
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Points disponibles: " + std::to_string(playerScore));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color(60, 120, 60));
        scoreText.setPosition(width / 2 - 150, 120);
        window.draw(scoreText);

        // Instructions
        sf::Text instructionsText;
        instructionsText.setFont(font);
        instructionsText.setString("Utilisez les flèches haut/bas pour naviguer\nEntrée pour acheter, Échap pour revenir");
        instructionsText.setCharacterSize(18);
        instructionsText.setFillColor(sf::Color(100, 100, 100));
        instructionsText.setPosition(50, height - 70);
        window.draw(instructionsText);

        // Afficher les articles
        for (int i = 0; i < items.size(); i++) {
            sf::RectangleShape itemBg;
            if (i == selectedItem) {
                itemBg.setSize(sf::Vector2f(width - 100, 80));
                itemBg.setFillColor(sf::Color(220, 220, 255));
                itemBg.setOutlineColor(sf::Color(100, 100, 180));
                itemBg.setOutlineThickness(3);
            } else {
                itemBg.setSize(sf::Vector2f(width - 100, 70));
                itemBg.setFillColor(sf::Color(230, 230, 245));
                itemBg.setOutlineColor(sf::Color(180, 180, 220));
                itemBg.setOutlineThickness(1);
            }
            itemBg.setPosition(50, 180 + i * 90);
            window.draw(itemBg);

            // Nom de l'article
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(items[i].name);
            nameText.setCharacterSize(22);
            nameText.setStyle(sf::Text::Bold);
            nameText.setFillColor(sf::Color::Black);
            nameText.setPosition(70, 190 + i * 90);
            window.draw(nameText);

            // Description
            sf::Text descText;
            descText.setFont(font);
            descText.setString(items[i].description);
            descText.setCharacterSize(16);
            descText.setFillColor(sf::Color(80, 80, 80));
            descText.setPosition(70, 220 + i * 90);
            window.draw(descText);

            // Prix
            sf::Text priceText;
            priceText.setFont(font);
            priceText.setString(std::to_string(items[i].price) + " pts");
            priceText.setCharacterSize(20);

            // Si c'est une décoration et qu'elle est déjà débloquée
            if (items[i].isDecoration && unlockedDecorations[items[i].index]) {
                priceText.setString("DÉBLOQUÉ");
                priceText.setFillColor(sf::Color(50, 150, 50));
            }
            // Si le joueur peut se le permettre
            else if (playerScore >= items[i].price) {
                priceText.setFillColor(sf::Color(50, 150, 50));
            }
            // Si le joueur ne peut pas se le permettre
            else {
                priceText.setFillColor(sf::Color(150, 50, 50));
            }

            priceText.setPosition(width - 170, 205 + i * 90);
            window.draw(priceText);
        }
    }

    void moveUp() {
        if (selectedItem > 0) {
            selectedItem--;
        }
    }

    void moveDown() {
        if (selectedItem < items.size() - 1) {
            selectedItem++;
        }
    }
    void updateScore(int newScore) {
    playerScore = newScore;  // Met à jour la référence
}

void updateDecorations(const std::vector<bool>& newDecorations) {
    // Mettre à jour les décorations si nécessaire
    unlockedDecorations = newDecorations;  // Met à jour la référence
}

    bool purchase(int& remainingHints) {
        if (selectedItem >= 0 && selectedItem < items.size()) {
            ShopItem& item = items[selectedItem];

            // Vérifier si l'article est une décoration déjà débloquée
            if (item.isDecoration && unlockedDecorations[item.index]) {
                return false; // Déjà débloqué
            }

            // Vérifier si le joueur a assez de points
            if (playerScore >= item.price) {
                playerScore -= item.price;

                if (item.isDecoration) {
                    unlockedDecorations[item.index] = true;
                } else {
                    remainingHints += item.index;
                }

                return true;
            }
        }
        return false;
    }
      bool purchaseHintPack(int& remainingHints, int& playerScore) {
        const int COST = 240;

        if (playerScore >= COST) {
            remainingHints += 3;
            playerScore -= COST;
            return true;
        }
        return false;
    }
};

class Sudoku {
private:
    static const int SIZE = 9;
    Cell grid[SIZE][SIZE];
    sf::Font font;
    int selectedX, selectedY;
    bool gameWon;
    bool showError;
    sf::Clock errorClock;
    std::string errorMessage;
    char currentDifficulty;
    sf::RectangleShape errorOverlay;
    GameState gameState;
    Menu* menu;
    Options* options;
    sf::RenderWindow* window;
    bool noteMode;
    std::array<int, 9> numberCount;
    sf::Texture checkmarkTexture;
    sf::Sprite checkmarkSprite;
    sf::Clock gameClock;
    sf::Time elapsedTime;
    sf::Text timerText;
    bool isTimerRunning;
    bool isPaused;
    bool showShopPrompt = false;
    bool hintRequested = false;
    int remainingHints = 3;
    sf::Text hintsText;
    sf::Clock hintFlashClock;
    int lastHintX = -1, lastHintY = -1;
    sf::Text pauseText;
    sf::RectangleShape pauseOverlay;
    int highlightedNumber = 0;
    struct GameStats {
        int totalGames = 0;
        int totalWins = 0;
        std::map<char, std::pair<int, float>> difficultyStats;
        int firstTryCorrect = 0;
        int totalFirstAttempts = 0;
        int bestTime[3] = {INT_MAX, INT_MAX, INT_MAX};
        int bestScore[3]={0,0,0};
    } stats;

    bool statsLoaded = false;
    sf::Clock firstTryClock;
    sf::Time pausedTime;
    sf::Time totalElapsedTime;
    bool wasTimerRunning = false;
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer correctBuffer;
    sf::SoundBuffer errorBuffer;
    sf::SoundBuffer winBuffer;
    sf::SoundBuffer pauseBuffer;
    sf::SoundBuffer hintBuffer;
    sf::Sound clickSound;
    sf::Sound correctSound;
    sf::Sound errorSound;
    sf::Sound winSound;
    sf::Sound pauseSound;
    sf::Sound hintSound;
    int lives = 3;
    sf::Texture heartTexture;
    sf::Sprite heartSprite[3];
    sf::Clock heartAnimClock;
    bool isHeartAnimating = false;
    int animatingHeartIndex = -1;
    sf::Clock winAnimClock;
    bool isWinAnimating = false;
    std::vector<sf::CircleShape> confetti;
    sf::Texture soundOnTexture;
    sf::Texture soundOffTexture;
    sf::Sprite soundButton;
    bool soundEnabled;
    sf::SoundBuffer applauseBuffer;
    sf::Sound applauseSound;
    sf::SoundBuffer heartLostBuffer;
    sf::Sound heartLostSound;
    sf::SoundBuffer gameOverBuffer;
    sf::Sound gameOverSound;
     int score;
    int scoreMultiplier;
    int correctStreak;
    sf::Text scoreText;
    bool showScoreAnimation;
    sf::Clock scoreAnimClock;
    int lastScoreChange;
    sf::Text scoreChangeText;
    Shop* shop;
    int scoreshop; // Le score du joueur (déjà présent)
    std::vector<bool> unlockedDecorations; // Pour suivre les décorations débloquées
    bool gameOver = false;

    bool isValid(int row, int col, int num) {
        // Vérifier la ligne
        for (int x = 0; x < SIZE; x++) {
            if (grid[row][x].getValue() == num) {
                return false;
            }
        }

        // Vérifier la colonne
        for (int y = 0; y < SIZE; y++) {
            if (grid[y][col].getValue() == num) {
                return false;
            }
        }

        // Vérifier le bloc 3x3
        int startRow = row - row % 3;
        int startCol = col - col % 3;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (grid[i + startRow][j + startCol].getValue() == num) {
                    return false;
                }
            }
        }

        return true;
    }
 void checkHints() {
        if (remainingHints <= 0) {
            showShopPrompt = true;
            gameState = SHOP_PROMPT;
        }
    }
    bool solveSudoku() {
        int row, col;

        // Chercher une cellule vide
        bool found = false;
        for (row = 0; row < SIZE && !found; row++) {
            for (col = 0; col < SIZE && !found; col++) {
                if (grid[row][col].getValue() == 0) {
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            return true;
        }
        for (int num = 1; num <= 9; num++) {
            if (isValid(row, col, num)) {
                grid[row][col].setValue(num);

                if (solveSudoku()) {
                    return true;
                }


                grid[row][col].setValue(0); // Annuler si ça ne marche pas
            }
        }
        return false;
    }

    void updateNumberCount() {
        numberCount.fill(0);

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                int val = grid[i][j].getValue();
                if (val >= 1 && val <= 9) {
                    numberCount[val-1]++;
                }
            }
        }
    }
    void generatePuzzle(char difficulty) {
    score = 0;
    scoreMultiplier = 1;
    correctStreak = 0;
    scoreText.setString("Score: 0");
        stats.totalGames++;
        stats.difficultyStats[difficulty].first++;
        totalElapsedTime = sf::Time::Zero;
        gameClock.restart();
        isTimerRunning = true;
        lives = 3;
    gameOver = false;
    isWinAnimating = false;
    isHeartAnimating = false;
     for (int i = 0; i < 3; i++) {
        heartSprite[i].setTexture(heartTexture);
        heartSprite[i].setScale(0.5f, 0.5f);
        heartSprite[i].setPosition(150 + i * 40, 20);
    }
        // Réinitialiser la grille
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j].setValue(0);
                grid[i][j].setSolution(0);
                grid[i][j].setFixed(false);
                grid[i][j].setIncorrect(false);
                grid[i][j].clearNotes();
            }
        }

        numberCount.fill(0);

        for (int i = 0; i < 3; i++) {
            int num = rand() % 9 + 1;
            int row = rand() % 9;
            int col = rand() % 9;
            if (isValid(row, col, num)) {
                grid[row][col].setValue(num);
            }
        }
        solveSudoku();
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j].setSolution(grid[i][j].getValue());
            }
        }

        int cellsToRemove;
        switch (difficulty) {
            case 'A': cellsToRemove = 40; break; // Facile
            case 'B': cellsToRemove = 45; break; // Moyen
            case 'C': cellsToRemove = 50; break; // Difficile
            default: cellsToRemove = 45;
        }

        currentDifficulty = difficulty;

        std::vector<std::pair<int, int>> positions;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                positions.push_back(std::make_pair(i, j));
            }
        }

        // Mélanger les positions
        for (int i = 0; i < positions.size(); i++) {
            int j = rand() % positions.size();
            std::swap(positions[i], positions[j]);
        }

        // Enlever les cellules
        for (int i = 0; i < cellsToRemove && i < positions.size(); i++) {
            int row = positions[i].first;
            int col = positions[i].second;
            grid[row][col].setValue(0);
        }

        // Marquer les cellules non vides comme fixes
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j].getValue() != 0) {
                    grid[i][j].setFixed(true);
                }
            }
        }

        // Mettre à jour les compteurs de chiffres
        updateNumberCount();

        gameWon = false;
        showError = false;
        errorMessage = "";
        noteMode = false;
        isPaused = false;

        // Réinitialiser le chronomètre
        gameClock.restart();
        elapsedTime = sf::Time::Zero;
        pausedTime = sf::Time::Zero;

        // Réinitialiser les indices
        remainingHints = 3;
        hintsText.setString("Indices: 3");

        // Sauvegarder les statistiques après la génération
        saveStats();
    }

    bool checkWin() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j].getValue() != grid[i][j].getSolution()) {
                    return false;
                }
            }
        }
        stats.totalWins++;
        float currentTime = elapsedTime.asSeconds();

        // Mettre à jour le temps moyen
        auto& diffStat = stats.difficultyStats[currentDifficulty];
        if (diffStat.second == 0) {
            diffStat.second = currentTime;
        } else {
            diffStat.second = ((diffStat.second * (diffStat.first - 1)) + currentTime) / diffStat.first;
        }

        int diffIndex;
        switch(currentDifficulty) {
            case 'A': diffIndex = 0; break;
            case 'B': diffIndex = 1; break;
            case 'C': diffIndex = 2; break;
            default: diffIndex = 1;
        }
         if (score > stats.bestScore[diffIndex]) {
        stats.bestScore[diffIndex] = score;
         saveStats();
    }
        if (currentTime < stats.bestTime[diffIndex] || stats.bestTime[diffIndex] == INT_MAX) {
    stats.bestTime[diffIndex] = currentTime;
    saveStats();
}

        // Sauvegarder les statistiques
        saveStats();

        return true;
    }
    void applyGalaxyTheme(sf::RenderWindow* window) {
    if (unlockedDecorations[1]) {
        // Fond d'étoiles
        static std::vector<sf::CircleShape> stars;

        // Initialiser les étoiles une seule fois
        if (stars.empty()) {
            for (int i = 0; i < 100; i++) {
                sf::CircleShape star(1 + (rand() % 3) * 0.5f);
                star.setPosition(rand() % window->getSize().x, rand() % window->getSize().y);
                star.setFillColor(sf::Color(200 + rand() % 55, 200 + rand() % 55, 200 + rand() % 55,
                                          100 + rand() % 155));
                stars.push_back(star);
            }
        }

        // Dessiner les étoiles
        for (auto& star : stars) {
            // Faire scintiller les étoiles
            if (rand() % 50 == 0) {
                star.setFillColor(sf::Color(220 + rand() % 35, 220 + rand() % 35, 220 + rand() % 35,
                                          150 + rand() % 105));
            }
            window->draw(star);
        }

        // Background gradient pour la grille
        sf::RectangleShape gridBg(sf::Vector2f(450, 450));
        gridBg.setPosition(50, 50);
        gridBg.setFillColor(sf::Color(20, 20, 40, 200));
        window->draw(gridBg);
    }
}
void drawVictoryCrown() {
    // Variable statique partagée entre toutes les branches
    static bool crownInitialized = false;
    static sf::Clock crownAnimClock;

    if (gameWon && unlockedDecorations[2] && !isWinAnimating) {
        // Dessiner une couronne au-dessus de la grille
        if (!crownInitialized) {
            crownAnimClock.restart();
            crownInitialized = true;
        }
        float time = crownAnimClock.getElapsedTime().asSeconds();
        float hover = sin(time * 2) * 5; // Effet de flottement

        // Dessiner la couronne (utilisant des formes primitives)
        sf::ConvexShape crown;
        crown.setPointCount(7);
        crown.setPoint(0, sf::Vector2f(0, 20));
        crown.setPoint(1, sf::Vector2f(20, 0));
        crown.setPoint(2, sf::Vector2f(40, 20));
        crown.setPoint(3, sf::Vector2f(60, 0));
        crown.setPoint(4, sf::Vector2f(80, 20));
        crown.setPoint(5, sf::Vector2f(80, 40));
        crown.setPoint(6, sf::Vector2f(0, 40));
        crown.setFillColor(sf::Color(255, 215, 0)); // Or
        crown.setOutlineColor(sf::Color(218, 165, 32)); // Or foncé
        crown.setOutlineThickness(2);
        crown.setPosition(230, 10 + hover);
        window->draw(crown);

        // Dessiner les joyaux sur la couronne
        sf::CircleShape gem1(6), gem2(6), gem3(6);
        gem1.setFillColor(sf::Color(255, 0, 0, 200)); // Rubis
        gem2.setFillColor(sf::Color(0, 0, 255, 200)); // Saphir
        gem3.setFillColor(sf::Color(0, 255, 0, 200)); // Émeraude
        gem1.setPosition(240, 20 + hover);
        gem2.setPosition(260, 15 + hover);
        gem3.setPosition(280, 20 + hover);
        window->draw(gem1);
        window->draw(gem2);
        window->draw(gem3);
    } else if (!gameWon) {
        // Réinitialiser l'animation de la couronne quand le jeu n'est plus gagné
        crownInitialized = false;
    }
}

    void loseLife() {
    if (lives > 0) {
        lives--;
        heartLostSound.play();

        // Démarrer l'animation du cœur perdu
        isHeartAnimating = true;
        animatingHeartIndex = lives; // L'index du cœur qui vient d'être perdu
        heartAnimClock.restart();

        if (lives <= 0) {
            gameOver = true;
            gameOverSound.play();
        }
    }
}

// Ajouter une méthode pour animer les confettis de victoire
void startWinAnimation() {
    if (!isWinAnimating) {
        isWinAnimating = true;
        winAnimClock.restart();
        applauseSound.play();

        // Positionner les confettis pour l'animation
        for (auto& particle : confetti) {
            // Position aléatoire en haut de l'écran
            particle.setPosition(50 + rand() % 450, -10 - rand() % 50);

            // Utiliser des confettis dorés si la décoration est débloquée
            if (unlockedDecorations[0]) {
                // Couleurs dorées
                int g = 150 + rand() % 105;
                particle.setFillColor(sf::Color(255, g, 0));
            } else {
                // Couleurs vives normales
                int r = 100 + rand() % 155;
                int g = 100 + rand() % 155;
                int b = 100 + rand() % 155;
                particle.setFillColor(sf::Color(r, g, b));
            }
        }
    }
}

public:
    Sudoku(sf::RenderWindow* win) : selectedX(-1), selectedY(-1), gameWon(false), showError(false),
                                  currentDifficulty('B'), gameState(MENU), window(win), noteMode(false),
                                  isPaused(false), score(0), scoreMultiplier(1),
                              correctStreak(0), showScoreAnimation(false), lastScoreChange(0) {
        if (!clickBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\clic.wav")) {
    std::cerr << "Erreur: Son click.wav introuvable!" << std::endl;
}
if (!correctBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\correct.wav")) {
    std::cerr << "Erreur: Son correct.wav introuvable!" << std::endl;
}
if (!errorBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\wrong.wav")) {
    std::cerr << "Erreur: Son error.wav introuvable!" << std::endl;
}

clickSound.setBuffer(clickBuffer);
correctSound.setBuffer(correctBuffer);
errorSound.setBuffer(errorBuffer);
        // Initialiser le compteur de chiffres
        numberCount.fill(0);
          scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color(60, 60, 60));
    scoreText.setPosition(320, 20); // Position en haut au centre
    scoreText.setString("Score: 0");

    // Initialiser le texte d'animation du score
    scoreChangeText.setFont(font);
    scoreChangeText.setStyle(sf::Text::Bold);
    scoreChangeText.setCharacterSize(24);
    scoreChangeText.setPosition(320, 50);
   try {
        unlockedDecorations = {false, false, false};
        loadStats();
        shop = new Shop(window->getSize().x, window->getSize().y, font, score, unlockedDecorations);
        loadDecorations();
    } catch (std::exception& e) {
        std::cerr << "Exception dans le constructeur Sudoku: " << e.what() << std::endl;
        // Assurer que shop est valide même en cas d'erreur
        if (shop == nullptr) {
            shop = new Shop(window->getSize().x, window->getSize().y, font, score, unlockedDecorations);
        }
    }
          if (!heartTexture.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\heart.png")) {
        std::cerr << "Erreur: Image heart.png introuvable!" << std::endl;
    }
     for (int i = 0; i < 3; i++) {
        heartSprite[i].setTexture(heartTexture);
        heartSprite[i].setScale(0.5f, 0.5f); // Ajuster selon la taille de votre image
        heartSprite[i].setPosition(150 + i * 40, 20);
    }
     if (!applauseBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\applause.wav")) {
        std::cerr << "Erreur: Son applause.wav introuvable!" << std::endl;
    }
    if (!heartLostBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\heartlost.wav")) {
        std::cerr << "Erreur: Son heartlost.wav introuvable!" << std::endl;
    }
    if (!gameOverBuffer.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\gameover.wav")) {
        std::cerr << "Erreur: Son gameover.wav introuvable!" << std::endl;
    }

    applauseSound.setBuffer(applauseBuffer);
    heartLostSound.setBuffer(heartLostBuffer);
    gameOverSound.setBuffer(gameOverBuffer);
    for (int i = 0; i < 100; i++) {
        sf::CircleShape particle(2 + rand() % 5); // Taille aléatoire entre 2 et 6
        particle.setPosition(-100, -100); // Hors écran initialement

        // Couleurs vives aléatoires
        int r = 100 + rand() % 155;
        int g = 100 + rand() % 155;
        int b = 100 + rand() % 155;
        particle.setFillColor(sf::Color(r, g, b));

        confetti.push_back(particle);
    }
        if (!checkmarkTexture.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\tik.jpg")) {
            std::cerr << "Erreur: Image checkmark.png introuvable!" << std::endl;
        }
        checkmarkSprite.setTexture(checkmarkTexture);

        // Ajustements initiaux
        checkmarkSprite.setScale(0.5f, 0.5f); // Échelle selon besoin

        // Charger la police
        if (!font.loadFromFile("C:\\Users\\ismai\\OneDrive\\Bureau\\jeu\\arial\\Aileron-Light.otf")) {
            std::cerr << "Erreur lors du chargement de la police" << std::endl;
            // Essayer avec une autre police
            if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
                std::cerr << "Impossible de charger la police alternative" << std::endl;
                // Essayer une autre police
                if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                    std::cerr << "Aucune police n'a pu être chargée" << std::endl;
                }
            }
        }

        // Initialisation du timer et des indices
        timerText.setFont(font);
        timerText.setCharacterSize(24);
        timerText.setFillColor(sf::Color(60, 60, 60));
        timerText.setPosition(520, 20); // Position en haut à droite
        isTimerRunning = false;

        hintsText.setFont(font);
        hintsText.setString("Indices: 3");
        hintsText.setCharacterSize(20);
        hintsText.setFillColor(sf::Color(80, 80, 80));
        hintsText.setPosition(20, 20);

        // Initialiser les éléments de pause
        pauseOverlay.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
        pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent noir

        pauseText.setFont(font);
        pauseText.setString("PAUSE\n\nAppuyez sur P ou Espace pour continuer\nEchap pour revenir au menu");
        pauseText.setCharacterSize(30);
        pauseText.setFillColor(sf::Color::White);
        // Centrer le texte
        sf::FloatRect pauseTextRect = pauseText.getLocalBounds();
        pauseText.setOrigin(pauseTextRect.left + pauseTextRect.width/2.0f, pauseTextRect.top + pauseTextRect.height/2.0f);
        pauseText.setPosition(window->getSize().x/2.0f, window->getSize().y/2.0f);

        // Initialiser les cellules
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j] = Cell(this, i, j);
                grid[i][j].setValue(0);
                grid[i][j].setSolution(0);
                grid[i][j].setFixed(false);
                grid[i][j].setSelected(false);
                grid[i][j].setIncorrect(false);

                sf::RectangleShape shape(sf::Vector2f(50, 50));
                shape.setPosition(j * 50 + 50, i * 50 + 50);
                shape.setFillColor(sf::Color::White);
                shape.setOutlineThickness(1);
                shape.setOutlineColor(sf::Color::Black);
                grid[i][j].setShape(shape);

                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(28); // Plus grande taille pour meilleure visibilité
                text.setFillColor(sf::Color::Black);
                text.setPosition(j * 50 + 65, i * 50 + 55);
                grid[i][j].setText(text);

                // Initialiser les notes (sera fait dans updateText)
            }
        }

        // Initialiser l'overlay d'erreur (pour l'écran rouge)
        errorOverlay.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
        errorOverlay.setFillColor(sf::Color(255, 200, 200, 80)); // Rouge semi-transparent

        // Initialiser le menu
        menu = new Menu(window->getSize().x, window->getSize().y, font);

        // Initialiser les options
        options = new Options(window->getSize().x, window->getSize().y, font);

        // Générer une grille avec une difficulté moyenne par défaut
        generatePuzzle('B');
        loadStats();
    }
    // Méthode pour mettre à jour le score
void updateScore(int points) {
    int actualPoints = points * scoreMultiplier;
    score += actualPoints;
    if (score < 0) score = 0; // Éviter les scores négatifs

    // Mise à jour de l'affichage
    scoreText.setString("Score: " + std::to_string(score));

    // Animation du changement de score
    showScoreAnimation = true;
    scoreAnimClock.restart();
    lastScoreChange = actualPoints;

    if (actualPoints > 0) {
        scoreChangeText.setString("+" + std::to_string(actualPoints));
        scoreChangeText.setFillColor(sf::Color(50, 200, 50)); // Vert pour les points gagnés
    } else {
        scoreChangeText.setString(std::to_string(actualPoints));
        scoreChangeText.setFillColor(sf::Color(200, 50, 50)); // Rouge pour les points perdus
    }
}

// Méthode pour gérer les réponses correctes
void handleCorrectAnswer() {
    correctStreak++;

    // Augmenter le multiplicateur tous les 3 placements corrects
    if (correctStreak % 3 == 0) {
        scoreMultiplier++;

        // Afficher un message pour le multiplicateur
        showError = true;
        errorMessage = "Multiplicateur x" + std::to_string(scoreMultiplier) + "!";
        errorClock.restart();
    }

    // Points de base selon la difficulté
    int basePoints;
    switch (currentDifficulty) {
        case 'A': basePoints = 10; break; // Facile
        case 'B': basePoints = 20; break; // Moyen
        case 'C': basePoints = 30; break; // Difficile
        default: basePoints = 20;
    }

    // Bonus de rapidité si moins de 3 secondes depuis la dernière action
    if (gameClock.getElapsedTime().asSeconds() < 3.0f) {
        basePoints += 5;
    }

    updateScore(basePoints);
}

// Méthode pour gérer les réponses incorrectes
void handleIncorrectAnswer() {
    correctStreak = 0; // Réinitialiser la série de réponses correctes
    scoreMultiplier = 1; // Réinitialiser le multiplicateur

    // Pénalité selon la difficulté
    int penalty;
    switch (currentDifficulty) {
        case 'A': penalty = -5; break;  // Facile
        case 'B': penalty = -10; break; // Moyen
        case 'C': penalty = -15; break; // Difficile
        default: penalty = -10;
    }

    updateScore(penalty);
}

// Méthode pour gérer l'utilisation d'un indice
void handleHintUsed() {
    updateScore(-20); // Forte pénalité pour l'utilisation d'un indice
}
    void togglePause() {
    if (gameState == PLAYING) {
        isPaused = !isPaused;

        if (isPaused) {
            // Entrer en pause
            pausedTime += gameClock.getElapsedTime();
            isTimerRunning = false;
            gameState = PAUSED;  // Synchroniser avec l'état du jeu
        } else {
            // Sortir de pause
            gameClock.restart();
            isTimerRunning = true;
            gameState = PLAYING; // Synchroniser avec l'état du jeu
        }
    } else if (gameState == PAUSED) {
        // Sortir de pause
        isPaused = false;
        gameClock.restart();
        isTimerRunning = true;
        gameState = PLAYING;
    }
}

    void drawStatsMenu() {
        window->clear(sf::Color(234, 234, 255)); // Fond clair

        sf::Text statsText;
        statsText.setFont(font);
        statsText.setCharacterSize(24);
        statsText.setFillColor(sf::Color::Black);
        statsText.setPosition(50, 50);

        std::string statsStr = "STATISTIQUES\n\n";
        statsStr += "Parties jouees: " + std::to_string(stats.totalGames) + "\n";
        statsStr += "Victoires: " + std::to_string(stats.totalWins) + "\n\n";

        statsStr += "Temps moyen:\n";
        statsStr += "- Facile: " + std::to_string(stats.difficultyStats['A'].second) + "s\n";
        statsStr += "- Moyen: " + std::to_string(stats.difficultyStats['B'].second) + "s\n";
        statsStr += "- Difficile: " + std::to_string(stats.difficultyStats['C'].second) + "s\n\n";

        // Ajouter les meilleurs temps
      statsStr += "Meilleur temps:\n";
    statsStr += "- Facile: " + (stats.bestTime[0] == INT_MAX ? "N/A" : std::to_string(stats.bestTime[0])) + "s\n";
    statsStr += "- Moyen: " + (stats.bestTime[1] == INT_MAX ? "N/A" : std::to_string(stats.bestTime[1])) + "s\n";
    statsStr += "- Difficile: " + (stats.bestTime[2] == INT_MAX ? "N/A" : std::to_string(stats.bestTime[2])) + "s\n\n";

    statsStr += "Meilleur score:\n";
    statsStr += "- Facile: " + std::to_string(stats.bestScore[0]) + "\n";
    statsStr += "- Moyen: " + std::to_string(stats.bestScore[1]) + "\n";
    statsStr += "- Difficile: " + std::to_string(stats.bestScore[2]) + "\n";

        statsText.setString(statsStr);
        window->draw(statsText);

        // Bouton retour
        sf::Text backText;
        backText.setString("Retour (ECHAP)");
        backText.setFont(font);
         backText.setColor(sf::Color(255,120,255));

        backText.setPosition(250, 600);
        window->draw(backText);
    }

    ~Sudoku() {
        saveStats();
        delete menu;
        delete options;
        delete shop;
    }
void saveDecorations() {
    std::ofstream file("sudoku_decorations.dat", std::ios::trunc);
    if (!file) {
        std::cerr << "Impossible d'ouvrir le fichier des décorations!" << std::endl;
        return;
    }

    file << score << std::endl;  // Sauvegarder le score actuel

    // Sauvegarder les décorations débloquées
    for (bool decoration : unlockedDecorations) {
        file << (decoration ? '1' : '0');
    }
    file << std::endl;

    file.close();
}
void loadDecorations() {
    std::ifstream file("sudoku_decorations.dat");
    if (!file) {
        std::cerr << "Fichier de décorations non trouvé." << std::endl;
        // Initialiser avec des valeurs par défaut
        score = 0;
        unlockedDecorations = {false, false, false};
        saveDecorations();
        return;
    }

    file >> score;  // Charger le score

    // Charger les décorations
    std::string decorStr;
    file >> decorStr;

    for (size_t i = 0; i < decorStr.length() && i < unlockedDecorations.size(); i++) {
        unlockedDecorations[i] = (decorStr[i] == '1');
    }

    file.close();
}
    bool isHintFlashActive(int row, int col) const {
        // Implémentation vérifiant si la cellule doit clignoter
        return (this->lastHintX == col &&
                this->lastHintY == row &&
                hintFlashClock.getElapsedTime().asSeconds() < 1.0f);
    }

    void saveStats() {
    std::ofstream file("sudoku_stats1.dat", std::ios::trunc);  // Écraser le fichier existant
    if (!file) {
        std::cerr << "Impossible d'ouvrir le fichier de statistiques pour écriture!" << std::endl;
        return;
    }

    // Écrire les stats globales avec une mise en forme claire
    file << stats.totalGames << " "
         << stats.totalWins << " "
         << stats.firstTryCorrect << " "
         << stats.totalFirstAttempts << std::endl;

    // Écrire les stats par difficulté (toujours A, B, C dans cet ordre)
    char difficulties[] = {'A', 'B', 'C'};
    for (char diff : difficulties) {
        auto it = stats.difficultyStats.find(diff);
        if (it != stats.difficultyStats.end()) {
            file << diff << " " << it->second.first << " " << it->second.second << std::endl;
        } else {
            // Écrire des valeurs par défaut si cette difficulté n'existe pas encore
            file << diff << " 0 0.0" << std::endl;
        }
    }

    // Écrire les meilleurs temps
    file << stats.bestTime[0] << " " << stats.bestTime[1] << " " << stats.bestTime[2] << std::endl;

    // Écrire les meilleurs scores
    file << stats.bestScore[0] << " " << stats.bestScore[1] << " " << stats.bestScore[2] << std::endl;

    file.close();
    std::cout << "Statistiques sauvegardées!" << std::endl;
}

   void loadStats() {
    std::ifstream file("sudoku_stats1.dat");
    if (!file) {
        std::cerr << "Fichier de statistiques non trouvé, création d'un nouveau fichier." << std::endl;
        // Initialiser avec des valeurs par défaut, puis sauvegarder
        stats = GameStats();  // Remise à zéro des stats
        saveStats();
        return;
    }

    try {
        // Lire les stats globales
        file >> stats.totalGames >> stats.totalWins
             >> stats.firstTryCorrect >> stats.totalFirstAttempts;

        // Lire les stats par difficulté
        char diff;
        int count;
        float avgTime;
        stats.difficultyStats.clear();  // Nettoyer avant de charger

        // Lire jusqu'à 3 difficultés (A, B, C)
        for (int i = 0; i < 3; i++) {
            if (file >> diff >> count >> avgTime) {
                if (diff == 'A' || diff == 'B' || diff == 'C') {
                    stats.difficultyStats[diff] = {count, avgTime};
                }
            }
        }

        // Lire les meilleurs temps et scores
        file >> stats.bestTime[0] >> stats.bestTime[1] >> stats.bestTime[2];
        // Convertir les 0 en INT_MAX
for (int i = 0; i < 3; i++) {
    if (stats.bestTime[i] == 0) {
        stats.bestTime[i] = INT_MAX;
    }}
        file >> stats.bestScore[0] >> stats.bestScore[1] >> stats.bestScore[2];

        statsLoaded = true;
        std::cout << "Statistiques chargées avec succès!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement des statistiques: " << e.what() << std::endl;
        // En cas d'erreur, réinitialiser et sauvegarder
        stats = GameStats();
        saveStats();
    }

    file.close();
}

  void updateTimer() {
    if (gameState == PLAYING && !gameOver) {
        if (!isTimerRunning) {
            gameClock.restart();
            isTimerRunning = true;
        }
        elapsedTime = totalElapsedTime + gameClock.getElapsedTime();
    } else {
        if (isTimerRunning) {
            totalElapsedTime += gameClock.getElapsedTime();
            isTimerRunning = false;
        }
    }
}

    void drawTimer() {
        // Convertir le temps en minutes:secondes
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        // Formater le texte
        std::string timeString =
            (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + std::to_string(seconds);

        timerText.setString(timeString);
        window->draw(timerText);
    }


   void draw() {
    window->clear(sf::Color(234, 234, 255)); // Fond gris très clair
    updateTimer();

    // Afficher en fonction de l'état du jeu
    switch (gameState) {
          case SHOP:
    try {
        if (shop != nullptr) {
            shop->draw(*window);
        } else {
            // Si shop est null, revenir au menu
            gameState = MENU;
            showError = true;
            errorMessage = "Erreur: Boutique non initialisée";
            errorClock.restart();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception lors de l'affichage de la boutique: " << e.what() << std::endl;
        gameState = MENU;
    }
    break;
        case STATS:
            drawStatsMenu();
            break;
        case MENU:
            menu->draw(*window);
            break;
        case OPTIONS:
            options->draw(*window);
            break;
        case PLAYING:
             window->draw(scoreText);
if (unlockedDecorations[1]) {
                applyGalaxyTheme(window);
            }
            if (unlockedDecorations[2] && gameWon) {
                drawVictoryCrown();
            }
if (showScoreAnimation) {
    float time = scoreAnimClock.getElapsedTime().asSeconds();

    if (time < 2.0f) {
        float progress = time / 2.0f;
        float alpha = 255 * (1.0f - progress * progress);
        float yOffset = -50 * (1.0f - cos(progress * 3.14159f));

        // Configuration du texte
        scoreChangeText.setCharacterSize(24 + 10 * (1.0f - progress)); // Réduction progressive de la taille
        scoreChangeText.setPosition(320, 50 + yOffset);

        // Ombre portée pour meilleure visibilité
        scoreChangeText.setOutlineThickness(1);
        scoreChangeText.setOutlineColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha * 0.7f)));

        // Application de la transparence
        sf::Color color = scoreChangeText.getFillColor();
        scoreChangeText.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(alpha)));

        window->draw(scoreChangeText);
    } else {
        showScoreAnimation = false;
    }
}
            // Dessiner les cellules avec un fond blanc
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    sf::RectangleShape cellBg(sf::Vector2f(50, 50));
                    cellBg.setPosition(j * 50 + 50, i * 50 + 50);
        if (grid[i][j].isHighlighted()) {
            cellBg.setFillColor(sf::Color(255, 255, 150)); // Bleu clair pour la surbrillance
        } else {
            cellBg.setFillColor(sf::Color::White);
        }
        window->draw(cellBg);
    }
}

            // Dessiner les lignes fines pour toutes les cellules
            for (int i = 0; i <= SIZE; i++) {
                sf::RectangleShape hLine(sf::Vector2f(450, 1));
                hLine.setPosition(50, i * 50 + 50);
                hLine.setFillColor(sf::Color(220, 220, 240)); // Gris clair
                window->draw(hLine);

                sf::RectangleShape vLine(sf::Vector2f(1, 450));
                vLine.setPosition(i * 50 + 50, 50);
                vLine.setFillColor(sf::Color(220, 220, 240)); // Gris clair
                window->draw(vLine);
            }

            // Dessiner les lignes épaisses pour délimiter les blocs 3x3
            for (int i = 0; i <= 3; i++) {
                sf::RectangleShape hLine(sf::Vector2f(450, 4)); // Lignes plus épaisses (4px)
                hLine.setPosition(50, i * 150 + 50);
                hLine.setFillColor(sf::Color(150, 150, 180)); // Noir pour les lignes de bloc
                window->draw(hLine);

                sf::RectangleShape vLine(sf::Vector2f(4, 450)); // Lignes plus épaisses (4px)
                vLine.setPosition(i * 150 + 50, 50);
                vLine.setFillColor(sf::Color(150, 150, 180)); // Noir pour les lignes de bloc
                window->draw(vLine);
            }

            // Surligner la cellule sélectionnée
            if (selectedX != -1 && selectedY != -1) {
                sf::RectangleShape highlight(sf::Vector2f(50, 50));
                highlight.setPosition(selectedX * 50 + 50, selectedY * 50 + 50);
                highlight.setFillColor(sf::Color(173, 216, 230)); // Bleu clair
                window->draw(highlight);
            }

            // Dessiner les cellules et leurs valeurs
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    // Mettre à jour le texte
                    grid[i][j].updateText(font);
                    // Dessiner le texte
                    if (grid[i][j].getValue() != 0) {
                        window->draw(grid[i][j].getText());
                    }
                }
            }

            // Dessiner les notes
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    grid[i][j].drawNotes(window);
                }
            }

            // Dessiner la liste des chiffres avec indicateurs de complétion
            sf::RectangleShape numberListBg(sf::Vector2f(150, 450));
            numberListBg.setPosition(520, 50);
            numberListBg.setFillColor(sf::Color(245, 245, 255));
            numberListBg.setOutlineColor(sf::Color(200, 200, 220));
            numberListBg.setOutlineThickness(2);
            window->draw(numberListBg);

            sf::Text numberListTitle;
            numberListTitle.setFont(font);
            numberListTitle.setString("Chiffres");
            numberListTitle.setCharacterSize(20);
            numberListTitle.setFillColor(sf::Color::Black);
            numberListTitle.setStyle(sf::Text::Bold);
            numberListTitle.setPosition(555, 60);
            window->draw(numberListTitle);

            // Mettre à jour les compteurs de chiffres
            updateNumberCount();

            // Dessiner chaque numéro avec son état
            for (int i = 0; i < 9; i++) {
                sf::Text numberText;
                numberText.setFont(font);
                numberText.setString(std::to_string(i + 1));
                numberText.setCharacterSize(24);
                numberText.setFillColor(sf::Color::Black);
                numberText.setPosition(540, 100 + i * 40);
                window->draw(numberText);

                // Afficher le compteur pour chaque chiffre
                sf::Text countText;
                countText.setFont(font);
                countText.setString(std::to_string(numberCount[i]) + "/9");
                countText.setCharacterSize(18);
                countText.setFillColor(sf::Color(100, 100, 100));
                countText.setPosition(580, 105 + i * 40);
                window->draw(countText);

                if (numberCount[i] == 9) {
                    checkmarkSprite.setPosition(620, 110 + i * 40);
                    checkmarkSprite.setScale(0.5f, 0.5f); // Ajuster selon la taille de l'image
                    window->draw(checkmarkSprite);
                }
            }

            // Afficher un message si le jeu est gagné
            if (gameWon) {
                // Fond bleu pour le message de victoire
                sf::RectangleShape messageBg(sf::Vector2f(450, 60));
                messageBg.setPosition(50, 245); // Milieu de l'écran
                messageBg.setFillColor(sf::Color(100, 150, 100, 200)); // Bleu foncé
                window->draw(messageBg);

                sf::Text winText;
                winText.setFont(font);
                winText.setString("F\xE9licitations! Vous avez r\xE9solu ce sudoku!");
                winText.setCharacterSize(20);
                winText.setFillColor(sf::Color(240, 255, 240));
                winText.setPosition(80, 265);
                window->draw(winText);
            }

            // Afficher un message d'erreur si nécessaire
            if (showError) {
                if (errorClock.getElapsedTime().asSeconds() > 1) {
                    showError = false;
                }
                sf::Text errorText;
                errorText.setFont(font);
                errorText.setString(errorMessage);
                errorText.setCharacterSize(20);
                errorText.setFillColor(sf::Color(200, 80, 80));
                errorText.setPosition(80, 550);
                window->draw(errorText);
            }

            // Afficher le niveau de difficulté actuel
            sf::Text difficultyText;
            difficultyText.setFont(font);
            std::string difficultyLabel;
            switch (currentDifficulty) {
                case 'A': difficultyLabel = "Difficult\xE9: A (Facile)"; break;
                case 'B': difficultyLabel = "Difficult\xE9: B (Moyen)"; break;
                case 'C': difficultyLabel = "Difficult\xE9: C (Difficile)"; break;
                default: difficultyLabel = "Difficult\xE9: B (Moyen)";
            }
            difficultyText.setString(difficultyLabel);
            difficultyText.setCharacterSize(16);
            difficultyText.setFillColor(sf::Color(120, 120, 150));
            difficultyText.setPosition(50, 530);
            window->draw(difficultyText);

            // Afficher l'indicateur de mode note
            sf::Text noteModeText;
            noteModeText.setFont(font);
            if (noteMode) {
                noteModeText.setString("Mode Notes: activ\xE9");
                noteModeText.setFillColor(sf::Color(100, 150, 100));
            } else {
                noteModeText.setString("Mode Notes: d\xE9sactiv\xE9 (N)");
                noteModeText.setFillColor(sf::Color(150, 150, 180));
            }
            noteModeText.setCharacterSize(16);
            noteModeText.setPosition(350, 530);
            window->draw(noteModeText);

            // Dessiner les instructions
            sf::Text instructions;
            instructions.setFont(font);
            instructions.setString("R: Nouvelle grille | A, B, C: Difficult\xE9 | N: Mode notes | \xC9sc: Menu | p: pause"); // Difficulté, Ésc
            instructions.setCharacterSize(16);
            instructions.setFillColor(sf::Color(120, 120, 150));
            instructions.setPosition(50, 510);
            window->draw(instructions);

            // Dessiner le timer
            drawTimer();

            // Dessiner le texte des indices
            window->draw(hintsText);

            // Dessiner les coeurs uniquement en mode PLAYING
            for (int i = 0; i < 3; i++) {
                if (i < lives) {
                    heartSprite[i].setColor(sf::Color::White);
                } else {
                    heartSprite[i].setColor(sf::Color(100, 100, 100, 150));
                }
                if (isHeartAnimating && i == animatingHeartIndex) {
                    float time = heartAnimClock.getElapsedTime().asSeconds();
                    if (time < 0.6f) {
                        float scale = 0.15f * (1.0f + sin(time * 15) * (1.0f - time / 0.6f));
                        heartSprite[i].setScale(scale, scale);
                        int alpha = 255 * (1.0f - time / 0.6f);
                        heartSprite[i].setColor(sf::Color(255, 0, 0, alpha));
                    } else {
                        isHeartAnimating = false;
                        heartSprite[i].setScale(0.15f, 0.15f);
                    }
                }
                window->draw(heartSprite[i]);
            }

            // Afficher l'overlay de pause si le jeu est en pause
            if (isPaused || gameState == PAUSED) {
                window->draw(pauseOverlay);
                window->draw(pauseText);
            }
            break;
    }

    // Animation de victoire
    if (isWinAnimating) {
        float time = winAnimClock.getElapsedTime().asSeconds();
        // Faire tomber les confettis pendant 3 secondes
        if (time < 3.0f) {
            for (auto& particle : confetti) {
                // Mouvement de chute avec légère oscillation
                float x = particle.getPosition().x + sin(time * 3 + particle.getPosition().x * 0.01) * 2;
                float y = particle.getPosition().y + (50 + rand() % 20) * time * 0.016f;
                // Replacer les particules qui sortent de l'écran
                if (y > 550) {
                    y = -10;
                    x = 50 + rand() % 450;
                }
                particle.setPosition(x, y);
                window->draw(particle);
            }
        } else {
            isWinAnimating = false;
        }
    }

    // Afficher un écran de Game Over si nécessaire
    if (gameOver) {
        sf::RectangleShape gameOverBg(sf::Vector2f(450, 100));
        gameOverBg.setPosition(50, 225);
        gameOverBg.setFillColor(sf::Color(150, 50, 50, 220));
        window->draw(gameOverBg);

        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER!\nVous avez épuisé vos 3 vies.");
        gameOverText.setCharacterSize(28);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setPosition(120, 219);
        window->draw(gameOverText);

        sf::Text restartText;
        restartText.setFont(font);
        restartText.setString("Appuyez sur R pour recommencer");
        restartText.setCharacterSize(20);
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition(150, 290);
        window->draw(restartText);
    }
    if (showShopPrompt) {
        // Fond semi-transparent
        sf::RectangleShape overlay(sf::Vector2f(window->getSize().x, window->getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window->draw(overlay);

        // Boîte de dialogue
        sf::RectangleShape box(sf::Vector2f(400, 200));
        box.setFillColor(sf::Color(50, 50, 80));
        box.setPosition(125, 250);
        window->draw(box);

        // Texte
        sf::Text promptText;
        promptText.setFont(font);
        promptText.setString("Plus d'indices !\nVoulez-vous en acheter ?\n(O)uvrir boutique\n(N) Continuer");
        promptText.setCharacterSize(24);
        promptText.setPosition(150, 300);
        window->draw(promptText);
    }

    window->display();
}
void giveHint() {
    if (remainingHints > 0 && selectedX != -1 && selectedY != -1) {
        Cell& cell = grid[selectedY][selectedX];

        if (cell.getValue() == 0 && !cell.isFixed()) {
            // Révéler la solution et mettre à jour l'interface
            cell.setValue(cell.getSolution());
            cell.setIncorrect(false);
            handleHintUsed();
            // Déclencher l'animation
            lastHintX = selectedX;
            lastHintY = selectedY;
            hintFlashClock.restart();

            // Mettre à jour le compteur
            remainingHints--;
            hintsText.setString("Indices: " + std::to_string(remainingHints));

            // Vérifier la victoire
            if (checkWin()) gameWon = true;
        }
    }
    else{
        hintRequested = true;
            checkHints();
    }
}
  void handleClick(int x, int y) {
    if (gameState == PLAYING) {
        clickSound.play();
        // Réinitialiser toutes les surbrillances
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j].setHighlighted(false);
            }
        }

        // Vérifier si le clic est dans la grille
        if (x >= 50 && x <= 500 && y >= 50 && y <= 500) {
            selectedX = (x - 50) / 50;
            selectedY = (y - 50) / 50;
            Cell& clickedCell = grid[selectedY][selectedX];
            int targetValue = clickedCell.getValue();

            if (targetValue != 0) { // Ne pas surligner si la case est vide
                highlightedNumber = targetValue; // Stocker le numéro sélectionné

                // Surligner la ligne
                for (int col = 0; col < SIZE; col++) {
                    grid[selectedY][col].setHighlighted(true);
                }

                // Surligner la colonne
                for (int row = 0; row < SIZE; row++) {
                    grid[row][selectedX].setHighlighted(true);
                }

                // Surligner la sous-grille 3x3
                int startRow = (selectedY / 3) * 3;
                int startCol = (selectedX / 3) * 3;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        grid[startRow + i][startCol + j].setHighlighted(true);
                    }
                }

                // Surligner toutes les occurrences du même numéro
                for (int i = 0; i < SIZE; i++) {
                    for (int j = 0; j < SIZE; j++) {
                        if (grid[i][j].getValue() == targetValue) {
                            grid[i][j].setHighlighted(true);
                        }
                    }
                }
            } else {
                highlightedNumber = 0;
            }
        }
    }
}
void handleKeypress(sf::Keyboard::Key key) {
    switch (gameState) {
              case MENU:
            if (key == sf::Keyboard::S) gameState = STATS;
            if (key == sf::Keyboard::Up) {
                menu->moveUp();
            } else if (key == sf::Keyboard::Down) {
                menu->moveDown();
            } else if (key == sf::Keyboard::Return) {
                switch (menu->getSelectedItemIndex()) {
                    case 0: // Start
                        gameState = PLAYING;
                        generatePuzzle(currentDifficulty);
                        break;
                    case 1: // Options
                        gameState = OPTIONS;
                        break;
                    case 2: // Stats
                        gameState = STATS;
                        break;
                    case 4: // Quit
                        gameState = QUIT;
                        window->close();
                        break;
                    case 3: // Boutique
    try {
        if (shop == nullptr) {
            shop = new Shop(window->getSize().x, window->getSize().y, font, score, unlockedDecorations);
        }
        gameState = SHOP;
    } catch (std::exception& e) {
        std::cerr << "Exception lors de l'ouverture de la boutique: " << e.what() << std::endl;
        showError = true;
        errorMessage = "Erreur lors de l'ouverture de la boutique";
        errorClock.restart();
    }
    break;
                }
            }
            break;

        case SHOP:
            if (key == sf::Keyboard::Escape) {
                gameState = MENU;
                saveDecorations(); // Sauvegarder avant de quitter
            }
            if (key == sf::Keyboard::Up) {
                if (shop != nullptr) shop->moveUp();
            }
            if (key == sf::Keyboard::Down) {
                if (shop != nullptr) shop->moveDown();
            }
            if (key == sf::Keyboard::Return) {
                if (shop != nullptr && shop->purchase(remainingHints)) {
                    // Achat réussi
                    hintsText.setString("Indices: " + std::to_string(remainingHints));
                    showError = true;
                    errorMessage = "Achat effectué avec succès!";
                    errorClock.restart();
                    correctSound.play();
                    saveDecorations(); // Sauvegarder après un achat
                } else {
                    // Achat échoué
                    showError = true;
                    errorMessage = "Achat impossible!";
                    errorClock.restart();
                    errorSound.play();
                }
            }
            break;
        case STATS:
            if (key == sf::Keyboard::Escape) gameState = MENU;
            break;

        case OPTIONS:
            // N'importe quelle touche pour revenir au menu
            gameState = MENU;
            break;
          case SHOP_PROMPT:
        if (key == sf::Keyboard::O) {
            gameState = SHOP;
            showShopPrompt = false;
        }
        else if (key == sf::Keyboard::N) {
            gameState = PLAYING;
            showShopPrompt = false;
        }
        break;
        case PAUSED:
            // Reprendre le jeu avec Escape ou P
            if (key == sf::Keyboard::Escape || key == sf::Keyboard::P) {
              togglePause();
            }
            // Retour au menu depuis la pause
            if (key == sf::Keyboard::M) {
                gameState = MENU;
            }
            break;

        case PLAYING:
            if (checkWin()) {
                if (key == sf::Keyboard::Escape) {
                    gameOver = false;
                    gameState = MENU;
                      applauseSound.stop();
                    return; // Quitter directement pour éviter les autres traitements
                }}
              if (gameOver) {
                if (key == sf::Keyboard::Escape) {
                    gameOver = false;
                    gameState = MENU;
                      gameOverSound.stop();
                    return; // Quitter directement pour éviter les autres traitements
                }}
            // Mettre le jeu en pause
            if (key == sf::Keyboard::P) {
                gameState = PAUSED;
            }
            // Accéder directement aux statistiques
            if (key == sf::Keyboard::S) {
                gameState = STATS;
            }

            // Touche N pour activer/désactiver le mode note
            if (key == sf::Keyboard::N) {
                noteMode = !noteMode;

                // Afficher un message indiquant le changement de mode
                showError = true;
                if (noteMode) {
                    errorMessage = "Mode notes activ\xE9";
                } else {
                    errorMessage = "Mode notes d\xE9sactiv\xE9";
                }
                errorClock.restart();
            }

            if (key == sf::Keyboard::H) {
                giveHint();
            }

           if (selectedX != -1 && selectedY != -1 && !grid[selectedY][selectedX].isFixed() && !gameOver) {
    if ((key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num9) ||
        (key >= sf::Keyboard::Numpad1 && key <= sf::Keyboard::Numpad9)) {

        int number;
        if (key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num9) {
            number = key - sf::Keyboard::Num0;
        } else {
            number = key - sf::Keyboard::Numpad0;
        }

        if (noteMode) {
            // En mode note, on ajoute/retire la note
            grid[selectedY][selectedX].toggleNote(number);
            clickSound.play();
        } else {
            // Mode normal: placer un nombre
            grid[selectedY][selectedX].setValue(number);
            grid[selectedY][selectedX].clearNotes(); // Effacer les notes quand on place un nombre

            // Vérifier si le nombre est correct
            if (number != grid[selectedY][selectedX].getSolution()) {
                grid[selectedY][selectedX].setIncorrect(true);
                handleIncorrectAnswer();
                showError = true;
                errorMessage = "Erreur! Ce nombre n'est pas correct.";
                errorClock.restart();
                errorSound.play();

                // Perdre une vie
                loseLife();
            } else {
                grid[selectedY][selectedX].setIncorrect(false);
                correctSound.play();
                handleCorrectAnswer();
                if (checkWin()) {
                    gameWon = true;
                    startWinAnimation();
                     int winBonus = 100 * (currentDifficulty - 'A' + 1);
        updateScore(winBonus);
                }
            }
        }
    }
             else if (key >= sf::Keyboard::Numpad1 && key <= sf::Keyboard::Numpad9) {
                    int number = key - sf::Keyboard::Numpad0;

                    if (noteMode) {
                        // En mode note, on ajoute/retire la note
                        grid[selectedY][selectedX].toggleNote(number);
                    } else {
                        // Mode normal: placer un nombre
                        grid[selectedY][selectedX].setValue(number);
                        grid[selectedY][selectedX].clearNotes(); // Effacer les notes quand on place un nombre

                        // Vérifier si le nombre est correct
                        if (number != grid[selectedY][selectedX].getSolution()) {
                            grid[selectedY][selectedX].setIncorrect(true);
                            showError = true;
                            errorMessage = "Erreur! Ce nombre n'est pas correct.";
                            errorClock.restart();
                            errorSound.play();
                        } else {
                            grid[selectedY][selectedX].setIncorrect(false);
                            correctSound.play();
                            if (checkWin()) {
                                gameWon = true;
                            }
                        }
                    }
                } else if (key == sf::Keyboard::Delete || key == sf::Keyboard::BackSpace) {
                    grid[selectedY][selectedX].setValue(0);
                    grid[selectedY][selectedX].setIncorrect(false);
                    // On conserve les notes lors de la suppression
                }
            }

            // Générer une nouvelle grille
            if (key == sf::Keyboard::R) {
                if(gameOver){
                    gameOverSound.stop();
                    gameOver=false;
                    lives=3;
                    generatePuzzle(currentDifficulty);
                    selectedX=selectedY=-1;
                }
                else{
                    generatePuzzle(currentDifficulty);
                    selectedX=selectedY=-1;
                }

            }

            // Changer la difficulté
            if (key == sf::Keyboard::A) {
                generatePuzzle('A'); // Facile
                selectedX = selectedY = -1;
            }
            if (key == sf::Keyboard::B) {
                generatePuzzle('B'); // Moyen
                selectedX = selectedY = -1;
            }
            if (key == sf::Keyboard::C) {
                generatePuzzle('C'); // Difficile
                selectedX = selectedY = -1;
            }

            // Retour au menu
            if (key == sf::Keyboard::Escape) {
                gameState = MENU;
            }
            break;

        default:
            break;
    }
}

    GameState getGameState() const {
        return gameState;
 }   };

int main() {
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Créer la fenêtre
    sf::RenderWindow window(sf::VideoMode(650, 700), "Sudoku");
    window.setFramerateLimit(60);

    // Créer le jeu
    Sudoku game(&window);

    // Boucle principale
    while (window.isOpen() && game.getGameState() != QUIT) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game.saveStats();
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.handleClick(event.mouseButton.x, event.mouseButton.y);
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                game.handleKeypress(event.key.code);
            }
        }

        game.draw();
    }

    return 0;}
