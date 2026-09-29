#ifndef BUZZER_CONFIG_H
#define BUZZER_CONFIG_H

#include <stddef.h>
#include "buzzer_pwm.h"

#define NOTE_A3  220
#define NOTE_C4  261
#define NOTE_D4  293
#define NOTE_E4  329
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define REST 0

#define BPM 120

#define QUARTER_NOTE (60000 / BPM)
#define WHOLE_NOTE (QUARTER_NOTE * 4)
#define HALF_NOTE (QUARTER_NOTE * 2)
#define HALF_NOTE_DOT (QUARTER_NOTE * 3)
#define SIXTEENTH_NOTE (QUARTER_NOTE / 4)
#define EIGHTH_NOTE (QUARTER_NOTE / 2)
#define EIGHTH_NOTE_DOT (SIXTEENTH_NOTE * 3)

typedef struct {
    uint16_t freq;
    uint16_t period;
} note_t;

static const note_t dlugosc_dzwieku_samotnosci[] = {
    {NOTE_C4, EIGHTH_NOTE},
    {NOTE_A4, EIGHTH_NOTE}, {NOTE_G4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE},{NOTE_F4, EIGHTH_NOTE},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, HALF_NOTE_DOT}, {REST, QUARTER_NOTE}, {NOTE_F4, QUARTER_NOTE}, {NOTE_F4, SIXTEENTH_NOTE}, {NOTE_F4, EIGHTH_NOTE_DOT},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, HALF_NOTE_DOT}, {REST, EIGHTH_NOTE}, {NOTE_E4, EIGHTH_NOTE}, {NOTE_E4, SIXTEENTH_NOTE}, {NOTE_F4, EIGHTH_NOTE_DOT},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_D4, HALF_NOTE_DOT}, {REST, QUARTER_NOTE}, {REST, EIGHTH_NOTE}, {NOTE_A3, EIGHTH_NOTE},
    {NOTE_A4, EIGHTH_NOTE}, {NOTE_G4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE}, {REST, EIGHTH_NOTE}, {NOTE_F4, EIGHTH_NOTE},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, HALF_NOTE_DOT}, {REST, QUARTER_NOTE}, {NOTE_F4, SIXTEENTH_NOTE}, {NOTE_F4, EIGHTH_NOTE_DOT},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_E4, HALF_NOTE_DOT}, {REST, EIGHTH_NOTE}, {NOTE_E4, EIGHTH_NOTE}, {NOTE_E4, SIXTEENTH_NOTE}, {NOTE_F4, EIGHTH_NOTE_DOT},
    {NOTE_F4, EIGHTH_NOTE}, {NOTE_D4, HALF_NOTE_DOT}
};

/* Łącznie dokładnie 3000 ms */
static const note_t skoczna_fanfara_c_dur[] = {
    /* A: chaos, losowe skoki tryton/septymy: 16 x 45 = 720 ms */
    {NOTE_C5, 45}, {NOTE_FS5, 45}, {NOTE_A4, 45}, {NOTE_DS6, 45},
    {NOTE_G5, 45}, {NOTE_B6, 45}, {NOTE_E5, 45}, {NOTE_CS7, 45},
    {NOTE_A5, 45}, {NOTE_F6, 45}, {NOTE_DS5, 45}, {NOTE_GS6, 45},
    {NOTE_C6, 45}, {NOTE_FS6, 45}, {NOTE_B4, 45}, {NOTE_D7, 45},

    /* B: dive-bomb w dół: 20 x 20 = 400 ms */
    {NOTE_D7, 20}, {NOTE_C7, 20}, {NOTE_AS6, 20}, {NOTE_GS6, 20}, {NOTE_FS6, 20},
    {NOTE_E6, 20}, {NOTE_D6, 20}, {NOTE_C6, 20}, {NOTE_AS5, 20}, {NOTE_GS5, 20},
    {NOTE_FS5, 20}, {NOTE_E5, 20}, {NOTE_D5, 20}, {NOTE_C5, 20}, {NOTE_AS4, 20},
    {NOTE_GS4, 20}, {NOTE_FS4, 20}, {NOTE_E4, 20}, {NOTE_D4, 20}, {NOTE_C4, 20},

    /* C: cisza przed burzą: 60 ms */
    {REST, 60},

    /* D: glissando chromatyczne w górę: 24 x 15 = 360 ms */
    {NOTE_C5, 15}, {NOTE_CS5, 15}, {NOTE_D5, 15}, {NOTE_DS5, 15}, {NOTE_E5, 15}, {NOTE_F5, 15},
    {NOTE_FS5, 15}, {NOTE_G5, 15}, {NOTE_GS5, 15}, {NOTE_A5, 15}, {NOTE_AS5, 15}, {NOTE_B5, 15},
    {NOTE_C6, 15}, {NOTE_CS6, 15}, {NOTE_D6, 15}, {NOTE_DS6, 15}, {NOTE_E6, 15}, {NOTE_F6, 15},
    {NOTE_FS6, 15}, {NOTE_G6, 15}, {NOTE_GS6, 15}, {NOTE_A6, 15}, {NOTE_AS6, 15}, {NOTE_B6, 15},

    /* E: laser ping-pong: 20 x 20 = 400 ms */
    {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20},
    {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20},
    {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20}, {NOTE_B6, 20}, {NOTE_C5, 20},
    {NOTE_B6, 20}, {NOTE_C5, 20},

    /* F: fałszywy finał: 4 x 80 = 320 ms */
    {NOTE_C6, 80}, {NOTE_E6, 80}, {NOTE_G6, 80}, {NOTE_C7, 80},

    /* G: cisza: 140 ms */
    {REST, 140},

    /* H: dwa uderzenia: 2 x (60+40) = 200 ms */
    {NOTE_C7, 60}, {REST, 40}, {NOTE_C7, 60}, {REST, 40},

    /* I: cios z dołu na sam szczyt: 150 + 250 = 400 ms */
    {NOTE_C4, 150}, {NOTE_C7, 250}
};

bool buzzer_play_notes(const note_t *notes, size_t num_notes);

#endif
