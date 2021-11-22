/*	Author: sspon001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 11  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../header/bit.h"
#include "../header/keypad.h"
#include "../header/scheduler.h"
#include "../header/timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char input, output ;

enum keypad_SMStates { startK, keypad_input } ;

int keypadSMTick(int state){
	input = GetKeypadKey() ;
	switch(state){
		case startK:
			state = keypad_input ;
			break ;
		case keypad_input:
			state = keypad_input ;
			break ;
		default:
			state = startK ;
			break ;
	}
	switch(state){

		case startK:
			break ;
		case keypad_input:
	switch(input){
		case '\0':
			output = 0x1F ;
			break ;
		case '0':
			output = 0x00 ;
			break ;
		case '1':
			output = 0x01 ;
			break ;
		case '2':
			output = 0x02 ;
			break ;
		case '3':
			output = 0x03 ;
			break ;
		case '4':
			output = 0x04 ;
			break ;
		case '5':
			output = 0x05 ;
			break ;
		case '6':
			output = 0x06 ;
			break ;
		case '7':
			output = 0x07 ;
			break ;
		case '8':
			output = 0x08 ;
			break ;
		case '9':
			output = 0x09 ;
			break ;
		case 'A':
			output = 0x0A ;
			break ;
		case 'B':
			output = 0x0B ;
			break ;
		case 'C':
			output = 0x0C ;
			break ;
		case 'D':
			output = 0x0D ;
			break ;
		case '#':
			output = 0x0F ;
			break ;
		default:
			output = 0x1B ;
			break ;
	}
	PORTB = output ;
	break ;
		default: 
	break ;
	}
	return state ;
}
enum LCD_SMStates { startL, scroll } ;
unsigned char msg[] = "CS120B is Legend... wait for it DARY!     " ;
unsigned char msg_sz = 37 + 5 ;
unsigned char output_msg[16] ;
unsigned char i = 0 ;

int LCDSMTick(int state){
	switch(state){
		case startL:
			state = scroll ;
			break ;
		case scroll:
			state = scroll ;
			break ;
		default:
			state = startL ;
			break ;
	}
	switch(state){
		case startL:
			break ;
		case scroll:
			for(int j = 0 ; j < 16 ; j++) output_msg[j] = msg[(j + i) % msg_sz] ; 	
			i = (i + 1) % msg_sz ;
			LCD_DisplayString(1, output_msg) ;
			break ;
		default:
			break ;
	}
	return state ;
}


int main(){
	DDRB = 0xFF ; PORTB = 0x00 ;
	DDRC = 0xF0 ; PORTC = 0x0F ;
	DDRA = 0xFF ; PORTA = 0x00 ;
	DDRD = 0xFF ; PORTD = 0x00 ;

	static task task1, task2 ;
	task *tasks[] = { &task1, &task2 } ; //&task2, &task3, &task4 } ;
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*) ;

	const char start = -1 ;

	task1.state = start ;
	task1.period = 50 ;
	task1.elapsedTime = task1.period ;
	task1.TickFct = &keypadSMTick ;

	task2.state = start ;
	task2.period = 500 ;
	task2.elapsedTime = task2.period ;
	task2.TickFct = &LCDSMTick ;
/*
	task3.state = start ;
	task3.period = 1000 ;
	task3.elapsedTime = task3.period ;
	task3.TickFct = &toggleLED1SMTick ;

	task4.state = start ;
	task4.period = 10 ;
	task4.elapsedTime = task4.period ;
	task4.TickFct = &displaySMTick ;
*/
	unsigned long GCD = tasks[0]->period ;
	unsigned short i ;
	for(i = 1 ; i < numTasks ; i++) GCD = findGCD(GCD, tasks[i]->period) ;

	TimerSet(GCD) ;
	TimerOn() ;

	while(1){
		for(i = 0 ; i < numTasks ; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state) ;
				tasks[i]->elapsedTime = 0 ;
			}
			tasks[i]->elapsedTime += GCD ;
		}
		while(!TimerFlag) {} 
		TimerFlag = 0 ;

	}
	return 1;
}
