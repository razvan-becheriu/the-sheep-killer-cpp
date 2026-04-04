#include "StateUnit.hpp"

TEventListener* ActiveStatus = nullptr;

void ChangeListener(TEventListener* newState) {
    if (ActiveStatus != nullptr) {
        ActiveStatus->DoExit();
        ActiveStatus->active = false;
    }
    
    if (newState != nullptr) {
        newState->active = true;
        ActiveStatus = newState;
        newState->DoEnter();
        // Note: Connecting the loop would happen here or in the caller
        // depending on how the loop structure is handled in main.cpp
    }
}