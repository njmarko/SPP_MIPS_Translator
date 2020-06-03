#ifndef __CONSTANTS__
#define __CONSTANTS__

/***
 * State symbol definitions
 */
const int IDLE_STATE = 0;
const int START_STATE = 1;
const int INVALID_STATE = -2;

/***
 * Number of states in FSM
 */
const int NUM_STATES = 55;

/***
 * Number of supported characters
 */
const int NUM_OF_CHARACTERS = 47;

/***
 * Use this when instruction interference to other instruction.
 */
const int __INTERFERENCE__ = 1;

/***
 * Use this when instruction is no interference to other instruction.
 */
const int __EMPTY__ = 0;

/***
 * Number of regs in processor.
 */
const int __REG_NUMBER__ = 2;

/***
 * Use these to print liveness analysis dump.
 */
const int __DUMPS__ = 1;
const int __NO_DUMPS__ = 0;

/***
 * Alignment definitions for nice printing
 */
const int LEFT_ALIGN = 20;
const int RIGHT_ALIGN = 25;

/***
* Size of the identificator for zerobytes files
*/
const int ZB_ID_SIZE = 10;

/***
* Value that signals that hexadecimal code will be used in zeroBytes program
*/
const int __HEX__ = 1;

/***
* Value that signals that binary code will be used in zeroBytes program
*/
const int __BIN__ = 0;

#endif
