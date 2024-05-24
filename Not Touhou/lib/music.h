/*        Your Name & E-mail: Nathan Ha    nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Final Project

 *         Description: Music notes and songs go here

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 */
#ifndef MUSIC_H
#define MUSIC_H



// piano notes and their frequencies
// 0 octave was too low
const short C1 = 33;
const short CSHARP1_DFLAT1 = 35;
const short D1 = 37;
const short DSHARP1_EFLAT1 = 39;
const short E1 = 41;
const short F1 = 44;
const short FSHARP1_GFLAT1 = 46;
const short G1 = 49;
const short GSHARP1_AFLAT1 = 52;
const short A1 = 55;
const short ASHARP1_BFLAT1 = 58;
const short B1 = 62;

const short C2 = 65;
const short CSHARP2_DFLAT2 = 69;
const short D2 = 73;
const short DSHARP2_EFLAT2 = 78;
const short E2 = 82;
const short F2 = 87;
const short FSHARP2_GFLAT2 = 93;
const short G2 = 98;
const short GSHARP2_AFLAT2 = 104;
const short A2 = 110;
const short ASHARP2_BFLAT2 = 117;
const short B2 = 123;

const short C3 = 131;
const short CSHARP3_DFLAT3 = 139;
const short D3 = 147;
const short DSHARP3_EFLAT3 = 156;
const short E3 = 165;
const short F3 = 175;
const short FSHARP3_GFLAT3 = 185;
const short G3 = 196;
const short GSHARP3_AFLAT3 = 208;
const short A3 = 220;
const short ASHARP3_BFLAT3 = 233;
const short B3 = 247;

const short C4 = 262;
const short CSHARP4_DFLAT4 = 277;
const short D4 = 294;
const short DSHARP4_EFLAT4 = 311;
const short E4 = 330;
const short F4 = 349;
const short FSHARP4_GFLAT4 = 370;
const short G4 = 392;
const short GSHARP4_AFLAT4 = 415;
const short A4 = 440;
const short ASHARP4_BFLAT4 = 466;
const short B4 = 494;

const short C5 = 523;
const short CSHARP5_DFLAT5 = 554;
const short D5 = 587;
const short DSHARP5_EFLAT5 = 622;
const short E5 = 659;
const short F5 = 698;
const short FSHARP5_GFLAT5 = 740;
const short G5 = 784;
const short GSHARP5_AFLAT5 = 831;
const short A5 = 880;
const short ASHARP5_BFLAT5 = 932;
const short B5 = 988;

const short C6 = 1047;
const short CSHARP6_DFLAT6 = 1109;
const short D6 = 1175;
const short DSHARP6_EFLAT6 = 1245;
const short E6 = 1319;
const short F6 = 1397;
const short FSHARP6_GFLAT6 = 1480;
const short G6 = 1568;
const short GSHARP6_AFLAT6 = 1661;
const short A6 = 1760;
const short ASHARP6_BFLAT6 = 1865;
const short B6 = 1976;

const short C7 = 2093;
const short CSHARP7_DFLAT7 = 2217;
const short D7 = 2349;
const short DSHARP7_EFLAT7 = 2489;
const short E7 = 2637;
const short F7 = 2794;
const short FSHARP7_GFLAT7 = 2960;
const short G7 = 3136;
const short GSHARP7_AFLAT7 = 3322;
const short A7 = 3520;
const short ASHARP7_BFLAT7 = 3729;
const short B7 = 3951;

// anything past this is TOO high

short night_of_knights[] = { 
                             D4, F4, E4, D4, C4, A4, E4, C4,  // intro
                            //  D4, F4, E4, D4, C4, A4, E4, C4, C4,
                            //  D4, F4, E4, D4, C4, A4, E4, C4, C4,
                             D5, F5, E5, D5, C5, A5, E5, C5, C5,
                             D4, F4, E4, D4, C4, A4, E4, C4, C4,
                            //  D4, F4, E4, D4, C4, A5, E5, C5, C5,
                             D4, F4, E4, D4, C4, A5, E5, C5, C5,
                             D4, B4, C5, E5,
                             D4, A4, G4, E4, F4, A4, G4, F4, E4, D4, //chorus
                            //  A4, G4, A4, E4, F4, A4, C4, C4,
                            //  D4, A4, G4, E4, F4, A4, G4, F4, E4, D4,
                            //  A4, G4, A4, E4, F4, A4, C4, C4,
                            //  D4, A4, G4, E4, F4, A4, G4, F4, E4, D4,
                             A4, G4, A4, E4, F4, A4, C4, C4,
                             D5, A5, G5, E5, F5, A5, G5, F5, E5, D5,
                             A5, G5, A5, E5, F5, A5, C5, C5,
                             D5, A5, G5, E5, F5, A5, G5, F5, E5, D5,
                             A5, G5, A5, E5, F5, A5, C5, C5, 
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5, // juice
                             C6, A5, G5, A5,
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5, 
                             C6, A5, G5, A5,
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5,
                             C6, A5, G5, A5,
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5,
                             C6, A5, F4, F4,
                             G4, G4, G4, A4, A3, A4, A3, A4, C5, B5, C5, B5, C6, C6, // intermission ? idk what to call it
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5,
                             C6, A5, G5, A5,
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5,
                             C6, A5, G5, A5,
                             A5, F5, E5, F5, E4, G5, F4, E5, F4, E5, C5, D5, A6, G5, A5,
                             C6, A5, G5, A5,
                             G4, G4, G4, A4, G4, F4, E4, D4, G4, G4, F4, G4, F4, D4, C4
                            };
const int night_of_knight_length = (sizeof(night_of_knights)/sizeof(night_of_knights[0]));

#endif // piano_notes.h
