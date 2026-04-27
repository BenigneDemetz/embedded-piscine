#include "includes/leds.h"
#include "avr/io.h"
#include "util/twi.h"
#include "util/delay.h"
#include "includes/leds.h"

volatile uint8_t game_state;

//game_state = 0 : game will start
//game_state = 1 : 3 lights on starting the game
//game_state = 2 : red light on
//game_state = 3 : yellow light on
//game_state = 4 : green light on
//game_state = 10 : game ended, ask for retry

void timer_init() {
    TCCR1B |= (1 << WGM12); // set timer to CTC mode, so it resets when it reaches the value in OCR1A p142
    TCCR1B |= (1 << CS12); // p143 set prescaler at 256
    OCR1A = F_CPU / (uint16_t) (256 * (uint16_t) 1) - 1; //    F_CPU / (256 * 2 * 1) - 1
                                            //hz voulu
    TIMSK1 |= (1 << OCIE1A); //enable compare with OCR1A
    TIFR1 &= ~(1 << OCF1A); //set interrupt flag to 0
    SREG |= (1 << 7); // flag for Global Interrupt
    TCNT1 = 0; // initialize counter to 0
}

void init() {
    TWBR = 72; // Set TWI clock to 100kHz (assuming 16MHz CPU) --> 16 + 2(72)*1 = 160. 16MHz/160 = 100kHz
    TWSR = 0; // on est directement à 100kHz, pas de prédivision
    SREG |= (1 << 7); // Enable global interrupts
}

void init_slave() {
    uint8_t slave_addr = SLAVE_ADDR; // Base address for slaves
    TWAR = SLAVE_ADDR; // Set slave address (shifted left for TWAR)
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // Enable TWI, ACK generation, and INTERRUPT
    SREG |= (1 << 7); // Enable global interrupts
    while (1);
}

void init_master() {
    game_state = 10;
    timer_init(); //init timer for the game
    leds_init(); //initialize communications with leds
}

void master_race() {
    if (MASTER) {
        init_master();
    } 
    else {
        init_slave();
    }
}

void start_timer() { //init timer and switch oon all leds
    expander_set_leds(EXPANDER01_ADDR, 0b00000000);
    expander_set_leds(EXPANDER02_ADDR, 0b00000000);
    game_state = 1;
    TCNT1 = 0;
}

void start_game() { //initialize the game
    OCR1A = F_CPU / (uint16_t) (256 * (uint16_t) 1) - 1; // Set timer for 0.5s intervals
    game_state = 0;
    start_timer();
}

__attribute__((signal))
void TIMER1_COMPA_vect() {
    if (game_state == 1) { //if 1s passed, light up red leds
        expander_set_leds(EXPANDER01_ADDR, 0b00000110);
        expander_set_leds(EXPANDER02_ADDR, 0b00000110);
        game_state = 2; 
    }
    else if (game_state == 2) { //if 2s passed, light up yellow leds
        expander_set_leds(EXPANDER01_ADDR, 0b00001010);
        expander_set_leds(EXPANDER02_ADDR, 0b00001010);
        game_state = 3; 
    }
    else if (game_state == 3) { //if 3s passed, light up green leds
        expander_set_leds(EXPANDER01_ADDR, 0b00001100);
        expander_set_leds(EXPANDER02_ADDR, 0b00001100);
        game_state = 4;
    }
}

uint8_t expander_read_switch(uint8_t expander_adr) { //read expander switches
    uint8_t state = (expander_read_PORT0(expander_adr) >> 0) & 1;
    return state;
}

int main() {
    init();
    master_race();
    uint8_t SW3 = expander_read_switch(EXPANDER01_ADDR); //read les 2 switchs
    uint8_t SW4 = expander_read_switch(EXPANDER02_ADDR);
    uint8_t retry1 = 0;
    uint8_t retry2 = 0;

    expander_set_leds(EXPANDER01_ADDR, 0b00001010); //allumer leds orange sur les 2
    expander_set_leds(EXPANDER02_ADDR, 0b00001010);
    while (1) {
        uint8_t SW3_now = expander_read_switch(EXPANDER01_ADDR); // Read the state of SW3
        uint8_t SW4_now = expander_read_switch(EXPANDER02_ADDR); // Read the state of SW4
        if (SW3 == 1 && SW3_now == 0) {
            if (game_state == 4) { //win en cliquant en 1er
                expander_set_leds(EXPANDER01_ADDR, 0b11110000);
                expander_set_leds(EXPANDER02_ADDR, 0b11110110);
                game_state = 10;
            }
            else if (game_state == 10) { //retry ask
                expander_set_leds(EXPANDER01_ADDR, 0b00001100);
                expander_set_leds(EXPANDER02_ADDR, 0b00001010);
                retry1 = 1;
            }
            else { //defaite, appuie trop tot
                expander_set_leds(EXPANDER01_ADDR, 0b11110110);
                expander_set_leds(EXPANDER02_ADDR, 0b11110000);
                game_state = 10;
            }
        }
        if (SW4 == 1 && SW4_now == 0) {
            if (game_state == 4) {
                expander_set_leds(EXPANDER01_ADDR, 0b11110110);
                expander_set_leds(EXPANDER02_ADDR, 0b11110000);
                game_state = 10;
            }
            else if (game_state == 10) { //ask retry
                expander_set_leds(EXPANDER02_ADDR, 0b00001100);
                expander_set_leds(EXPANDER01_ADDR, 0b00001010);
                retry2 = 1;
            }
            else {
                expander_set_leds(EXPANDER01_ADDR, 0b11110000);
                expander_set_leds(EXPANDER02_ADDR, 0b11110110);
                game_state = 10;
            }
        }

        SW3 = SW3_now;
        SW4 = SW4_now;
        if (game_state == 0 || (game_state == 10 && retry1 == 1 && retry2 == 1)) {
            retry1 = 0;
            retry2 = 0;
            
            start_game();
        }
        _delay_ms(20);
    }
}

/*
Chapitre III
Partie obligatoire
Exercice : 00
Howdy cowboy
Dossier de rendu : ex00/
Fichiers à rendre : Makefile, *.c, *.h
Fonctions Autorisées : avr/io.h, util/delay.h, util/twi.h
III.1 Objectif
L’objectif de cet exercice est de créer un jeu de rapidité où 2 microcontrolleurs communiquent ensemble. Vous devez utiliser 2 de vos devkits et les connecter avec le câble
fourni. Le but de la manoeuvre est de vous montrer comment la communication entre 2
MCU peut fonctionner.
Faites bon usage de votre matériel ! n’essayez PAS de brancher un
cable là où il ne rentre pas.
3
Piscine embarquée Rush 00 : Multiplayer
III.2 Pré-requis
Vous n’avez pas a implémenter le protocole I2C pour ce rush.
Vous devez seulement remplir les pré-requis.
• Connectez the 2 microcontrolleurs avec les câble fourni sur l’interface I2C interface
des 2 cartes.
• Le microcontroller AVR ATmega328P a 1 periphérique I2C que vous devez utiliser
pour communiquer avec l’autre microcontrolleur.
• L’I2C du MCU doit être configuré avec une fréquence de 100kHz.
• Vous devez décider quel microcontrolleur est le master et le slave.
• Chacun des microcontrolleurs doit avoir le même code flashé.
• Les MCUs doivent pouvoir communiquer même si l’un d’eux redémarre. Vous
pouvez décider de recommencer la partie.
III.3 Règles du jeu
Le jeu est plutôt simple :
• Quand les 2 joueurs ont appuyé sur le bouton le jeu commence.
• Un compte à rebours s’affiche sur les leds.
• Le joueur le plus rapide à avoir appuyé sur le bouton à la fin du compte à rebours
gagne la partie.
• Une autre partie peut maintenant commencer.
Si le joueur appuie sur le bouton avant la fin du compte à rebours
alors il perd la partie. Nous vous laissons créer un signalement
lumineux indiquant le vainqueur et le perdant. N’oubliez pas
de mettre les règles en commentaire dans le code source pour la
correction !
*/