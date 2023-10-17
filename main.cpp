#include <iostream>

#include <hagame/core/hg.h>
#include "src/game.h"

#include <hagame/structures/binaryHeap.h>
#include <hagame/utils/timer.h>
#include <hagame/utils/behaviorTree.h>
#include <set>

// The main function simply initializes the game and starts running it

// The main entrypoint of the game is found in src/game.h
// For global configurables, checkout src/constants.h

#if HEADLESS
Game game(GAME_NAME);
#else
Game game(GAME_NAME, GAME_SIZE);
#endif

void emscripten_tick() {
    game.tick();
}

int main() {
#ifdef __EMSCRIPTEN__
    game.initialize();
    emscripten_set_main_loop(emscripten_tick, 0, false);
    // clickVenture.destroy();
#else

    struct State {
        int clock = 0;
    };

    struct RunA : public hg::utils::bt::Node<State> {
    protected:

        void init(State* state, hg::utils::bt::data_context_t* ctx) override {
            std::cout << "Init A\n";
        }

        hg::utils::bt::Status process(double dt, State* state, hg::utils::bt::data_context_t* ctx) override {
            std::cout << "RUN A\n";
            return hg::utils::bt::Status::Success;
        }
    };

    struct RunB : public hg::utils::bt::Node<State> {
    protected:

        void init(State* state, hg::utils::bt::data_context_t* ctx) override {
            std::cout << "Init B\n";
            if (ctx->find("count") == ctx->end()) {
                ctx->insert(std::make_pair("count", 0));
            } else {
                ctx->at("count").emplace<int>(0);
            }

        }

        hg::utils::bt::Status process(double dt, State* state, hg::utils::bt::data_context_t* ctx) override {
            int count = std::get<int>(ctx->at("count"));
            if (count < 5) {
                ctx->at("count").emplace<int>(count + 1);
                std::cout << "COUNT = " << count << "\n";
                return hg::utils::bt::Status::Running;
            }
            return hg::utils::bt::Status::Success;
        }
    };

    struct RunC : public hg::utils::bt::Node<State> {
    protected:

        void init(State* state, hg::utils::bt::data_context_t* ctx) override {
            std::cout << "Init C\n";
        }

        hg::utils::bt::Status process(double dt, State* state, hg::utils::bt::data_context_t* ctx) override {
            std::cout << "RUN C\n";
            return hg::utils::bt::Status::Success;
        }
    };

    /*

    State state;

    auto tree = std::make_shared<hg::utils::BehaviorTree<State>>();

    tree->setRoot<hg::utils::bt::Sequence<State>>();

    tree->addChild<RunA>(tree->root());
    tree->addChild<RunB>(tree->root());
    tree->addChild<RunC>(tree->root());

    for (int i = 0; i < 10; i++) {
        std::cout << (int) tree->tick(0, &state) << "\n";
        if (tree->getCurrent()) {
            std::cout << tree->getCurrent() << "\n";
        }
    }

     */

    hg::HG::Run(&game);
#endif
    return 0;
}
