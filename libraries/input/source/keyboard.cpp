#include "dos/input/keyboard.hpp"

#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <stdio.h>

#define KEYBOARD_PORT 0x60
#define STATUS_PORT 0x64

namespace DOS {
namespace Input {
    namespace Keyboard {

        State playerA;
        State playerB;
        bool exit_requested = false;
        bool save_requested = false;
        bool bot_requested = false;
        bool reset_requested = false;
        void interrupt (*old_isr)();

        void interrupt new_isr() {
            uint8_t scan = inp(0x60);
            bool is_release = scan & 0x80;
            uint8_t code = scan & 0x7F;

            switch (code) {
                case 0x11:
                    playerA.up = !is_release;
                    break;
                case 0x1E:
                    playerA.left = !is_release;
                    break;
                case 0x1F:
                    playerA.down = !is_release;
                    break;
                case 0x20:
                    playerA.right = !is_release;
                    break;
                case 0x39:
                    playerA.fire = !is_release;
                    break;
                case 0x1D:
                    playerA.alt = !is_release;
                    break;
                case 0x17:
                    playerB.up = !is_release;
                    break;
                case 0x24:
                    playerB.left = !is_release;
                    break;
                case 0x25:
                    playerB.down = !is_release;
                    break;
                case 0x26:
                    playerB.right = !is_release;
                    break;
                case 0x35:
                    playerB.fire = !is_release;
                    break;
                case 0x23:
                    playerB.alt = !is_release;
                    break;
                case 0x10: // Q
                case 0x2D: // X
                    exit_requested = !is_release;
                    break;
                case 0x15: // Y
                    save_requested = !is_release;
                    break;
                case 0x13: // R
                    reset_requested = !is_release;
                    break;
                case 0x30: // B
                    bot_requested = !is_release;
                    break;
                default:
                    break;
            }

            // Must acknowledge the PIC (Programmable Interrupt Controller)
            outp(0x20, 0x20);
        }

    } // namespace Keyboard
} // namespace Input
} // namespace DOS
