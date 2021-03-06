#include "GameStateMainMenu.h"
#include "DefaultSettings.h"
#include "StateManager.h"
#include "GameStatePlayingLevel.h"
#include "GameConfiguration.h"
#include "GameStateTransition.h"
#include "StateOutroEffects.h"
#include "StateIntroEffects.h"

#include <memory>
#include <functional>
#include <vector>

using std::make_shared;
using std::vector;
using std::shared_ptr;
using std::function;

GameStateMainMenu::GameStateMainMenu(sf::RenderWindow &window, StateManager &stateManager) :
    GameState(false),
    playGame(DEFAULT_SCREEN_RESOLUTION, "button.png", DEFAULT_SCREEN_RESOLUTION / 2.f)
    {
        //create the click function for the mouse
        setupButtonFunctions(window, stateManager);
    }

void GameStateMainMenu::handleInputEvents(sf::Event &event, sf::RenderWindow &window, StateManager &StateManager) {

    playGame.handleInputEvents(event, window);

    if(event.type == sf::Event::Resized) {

        sf::Vector2f newScreenResolution(event.size.width, event.size.height);
        playGame.handleScreenResize(newScreenResolution);
    }
}

void GameStateMainMenu::handleStateEvents(sf::RenderWindow &window, StateManager &stateManager) {

    playGame.handleStateEvents(window);
}

void GameStateMainMenu::update(float deltaTime, sf::RenderWindow &window, StateManager &stateManager) {

    //nothing for now
}

void GameStateMainMenu::draw(sf::RenderWindow &window, StateManager &stateManager) {

    playGame.draw(window);
}

void GameStateMainMenu::setupButtonFunctions(sf::RenderWindow &window, StateManager &stateManager) {

    //do the required things to begin playing the game
    function<void()> onPlayButtonClick(
        [&]{

            GameConfiguration configuration;
            configuration.load();

            shared_ptr<Player> player1 = createPlayer(glm::vec2(0, 0), PlayerNumber::PLAYER_1, configuration);

            vector<shared_ptr<Player> > players;
            players.push_back(player1);

            //begin playing from level 1
            //insert outro effect and make it so it starts at the first level
            shared_ptr<GameState> playingState = make_shared<GameStatePlayingLevel>("world1", players, window);

            //create a level intro that plays before the level begins
            shared_ptr<GameStateTransition> levelIntro = make_shared<GameStateTransition>(window, true, false);
            shared_ptr<StateTransitionEffect> introEffect = make_shared<BlackCoverWipeRightIntro>();
            levelIntro->setTransitionEffect(introEffect);

            shared_ptr<GameStateTransition> outro = make_shared<GameStateTransition>(window, true, false);
            outro->insertFollowingState(playingState);
            outro->insertFollowingState(levelIntro);

            //create the transition effect
            shared_ptr<StateTransitionEffect> transitionEffect = make_shared<BlackCoverWipeRightOutro>();
            outro->setTransitionEffect(transitionEffect);

            stateManager.push(outro);
        }
    );

    playGame.setOnClickFunction(onPlayButtonClick);
}
