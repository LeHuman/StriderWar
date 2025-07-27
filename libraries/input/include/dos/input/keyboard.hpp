#pragma once

#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "interface.hpp"

#ifndef interrupt
// FOR IDE LINTING
#define interrupt
#endif

namespace DOS {
namespace Input {
    namespace Keyboard {

        extern void interrupt (*old_isr)();

        struct State : Interface {
            bool up;
            bool down;
            bool left;
            bool right;
            State() : up(false), down(false), left(false), right(false), Interface() {}
        };

        extern State playerA;
        extern State playerB;
        extern bool exit_requested;
        extern bool save_requested;
        extern bool reset_requested;
        extern bool bot_requested;

        void interrupt new_isr();

        inline void initialize() {
            playerA = State();
            playerB = State();
            exit_requested = false;
            save_requested = false;
            reset_requested = false;
            bot_requested = false;
            old_isr = _dos_getvect(0x09); // Backup original ISR
            _dos_setvect(0x09, new_isr);  // Set our custom handler
        }

        inline void update() {
            playerA.x = (-30 * playerA.left) + (30 * playerA.right);
            playerA.y = (-30 * playerA.up) + (30 * playerA.down);

            playerB.x = (-30 * playerB.left) + (30 * playerB.right);
            playerB.y = (-30 * playerB.up) + (30 * playerB.down);
        }

        inline void shutdown() {
            _dos_setvect(0x09, old_isr); // Restore original ISR
        }

    } // namespace Keyboard
} // namespace Input
} // namespace DOS
