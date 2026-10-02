#ifndef BUZZER_CONFIG_H
#define BUZZER_CONFIG_H

#include <stddef.h>

#include "buzzer_pwm.h"
#include "esp_err.h"

#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  261
#define NOTE_D4  293
#define NOTE_E4  329
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_B4  494
#define NOTE_A4  440
#define NOTE_C5  532
#define NOTE_CS7 2217
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_C7  2093
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

#define PP {NOTE_B7, 20}, {NOTE_C7, 20}

typedef struct {
    uint16_t freq;
    float period;
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

static const note_t skoczna_fanfara_c_dur[] = {
    /* A: chaos, skoki tryton/septymy: 16 x 45 = 720 ms */
    {NOTE_C7, 45},  {NOTE_FS7, 45}, {NOTE_D7, 45},  {NOTE_GS7, 45},
    {NOTE_E7, 45},  {NOTE_B7, 45},  {NOTE_F7, 45},  {NOTE_AS7, 45},
    {NOTE_C7, 45},  {NOTE_G7, 45},  {NOTE_DS7, 45}, {NOTE_A7, 45},
    {NOTE_D7, 45},  {NOTE_GS7, 45}, {NOTE_FS7, 45}, {NOTE_C8, 45},

    /* B: dive-bomb C8 -> C7 (chromatycznie): 12 x 30 + 40 = 400 ms */
    {NOTE_C8, 30},  {NOTE_B7, 30},  {NOTE_AS7, 30}, {NOTE_A7, 30},
    {NOTE_GS7, 30}, {NOTE_G7, 30},  {NOTE_FS7, 30}, {NOTE_F7, 30},
    {NOTE_E7, 30},  {NOTE_DS7, 30}, {NOTE_D7, 30},  {NOTE_CS7, 30},
    {NOTE_C7, 40},

    /* C: cisza przed burzą: 60 ms */
    {REST, 60},

    /* D: glissando C7 -> B7 (każdy półton 2 x 15 ms): 24 x 15 = 360 ms */
    {NOTE_C7, 15},  {NOTE_C7, 15},  {NOTE_CS7, 15}, {NOTE_CS7, 15},
    {NOTE_D7, 15},  {NOTE_D7, 15},  {NOTE_DS7, 15}, {NOTE_DS7, 15},
    {NOTE_E7, 15},  {NOTE_E7, 15},  {NOTE_F7, 15},  {NOTE_F7, 15},
    {NOTE_FS7, 15}, {NOTE_FS7, 15}, {NOTE_G7, 15},  {NOTE_G7, 15},
    {NOTE_GS7, 15}, {NOTE_GS7, 15}, {NOTE_A7, 15},  {NOTE_A7, 15},
    {NOTE_AS7, 15}, {NOTE_AS7, 15}, {NOTE_B7, 15},  {NOTE_B7, 15},

    /* E: laser ping-pong: 20 x 20 = 400 ms */
    PP, PP, PP, PP, PP, PP, PP, PP, PP, PP,

    /* F: fałszywy finał: 4 x 80 = 320 ms */
    {NOTE_C7, 80}, {NOTE_E7, 80}, {NOTE_G7, 80}, {NOTE_C8, 80},

    /* G: cisza: 140 ms */
    {REST, 140},

    /* H: dwa uderzenia: 2 x (60+40) = 200 ms */
    {NOTE_C8, 60}, {REST, 40}, {NOTE_C8, 60}, {REST, 40},

    /* I: cios: 150 + 250 = 400 ms */
    {NOTE_C7, 150}, {NOTE_C8, 250}
};

static const note_t hava_nagila[] = {
    {NOTE_E4, 428.57}, {NOTE_E4, 428.57}, {NOTE_E4, 214.28}, {NOTE_GS4, 214.28}, {NOTE_F4, 214.28}, {NOTE_E4, 214.28},
    {NOTE_GS4, 428.57}, {NOTE_GS4, 428.57}, {NOTE_GS4, 214.28}, {NOTE_B4, 214.28}, {NOTE_A4, 214.28}, {NOTE_GS4, 214.28},
    {NOTE_A4, 428.57}, {NOTE_A4, 428.57}, {NOTE_A4, 214.28}, {NOTE_C5, 214.28}, {NOTE_B4, 214.28}, {NOTE_A4, 214.28},
    {NOTE_GS4, 428.57}, {NOTE_F4, 214.28}, {NOTE_E4, 214.28}, {NOTE_E4, 214.28}, {NOTE_GS4, 857.14},
    {NOTE_GS4, 214.28}, {NOTE_GS4, 428.57}, {NOTE_F4, 214.28}, {NOTE_E4, 214.28}, {NOTE_E4, 214.28}, {NOTE_E4, 428.57},
    {NOTE_F4, 214.28}, {NOTE_F4, 428.57}, {NOTE_E4, 214.28}, {NOTE_D4, 214.28}, {NOTE_D4, 214.28}, {NOTE_D4, 428.57},
    {NOTE_D4, 428.57}, {NOTE_F4, 321.42}, {NOTE_E4, 107.14}, {NOTE_D4, 214.28}, {NOTE_D4, 214.28}, {NOTE_A4, 428.57}
};

static const note_t krakowiaczek[] = {
    {NOTE_B3, 450}, {NOTE_D4, 150}, {NOTE_F4, 300}, {NOTE_A4, 300},
    {NOTE_G4, 300}, {NOTE_E4, 600}, {REST, 600},
    {NOTE_G4, 450}, {NOTE_G4, 150}, {NOTE_F4, 300}, {NOTE_D4, 300},
    {NOTE_C4, 300}, {NOTE_E4, 600}, {REST, 600},
    {NOTE_B3, 450}, {NOTE_D4, 150}, {NOTE_F4, 300}, {NOTE_A4, 300},
    {NOTE_G4, 300}, {NOTE_E4, 600}, {REST, 600},
    {NOTE_G4, 450}, {NOTE_G4, 150}, {NOTE_F4, 300}, {NOTE_D4, 300},
    {NOTE_C4, 600}, {NOTE_C4, 300}, {REST, 600}
};

bool buzzer_play_notes(const note_t *notes, size_t num_notes);

esp_err_t start_recovery_music(void);

#endif
