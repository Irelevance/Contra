#ifndef GAMESTATEPLAYINGLEVEL_H_INCLUDED
#define GAMESTATEPLAYINGLEVEL_H_INCLUDED

#include "GameStateBase.h"
#include "GameWorld.h"
#include "SFML/Graphics.hpp"
#include "PlayerStatsUI.h"

#include <string>
#include <vector>
#include <memory>

class Player;

class GameStatePlayingLevel : public GameState {

    public:

        //loads the given level and uses the given players instead of creating new players
        //this is done so that the gameplaying state doesn't have to woryr about creating the correct number of players
        GameStatePlayingLevel(const std::string &levelToLoad, std::vector<std::shared_ptr<Player> > &players, sf::RenderWindow &window);

        virtual ~GameStatePlayingLevel() {

        }

        virtual void handleInputEvents(sf::Event &event, sf::RenderWindow &window, StateManager& stateManager);
        virtual void handleStateEvents(sf::RenderWindow &window, StateManager& stateManager);
        virtual void update(float deltaTime, sf::RenderWindow &window, StateManager &stateManager);
        virtual void draw(sf::RenderWindow &window, StateManager &stateManager);

    private:

        bool allBossesDefeated();
        void goToNextLevel(sf::RenderWindow &window, StateManager &stateManager);

        GameWorld world;

        std::string levelName;

        PlayerStatsUI playerStats;
};

#endif // GAMESTATEPLAYINGLEVEL_H_INCLUDED
