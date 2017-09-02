#include "TM4C123.h"                    // Device header
#define LCD_DATA GPIOB 
#define LCD_CTRL GPIOA  
#define RS 0x20 /* PORTA BIT5 mask */
#define RW 0x40 /* PORTA BIT6 mask */
#define EN 0x80 /* PORTA BIT7 mask */
void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);

void keypad_init(void);
unsigned char keypad_kbhit(void);

int main(void)
{
    unsigned char key;
    
    keypad_init();
    LCD_init();

    while(1)
    {
        LCD_command(0x80);      /* LCD cursor location */
        delayMs(500);
        if (keypad_kbhit() != 0) /* if a key is pressed */
            LCD_data('P');      /* display 'P' */
        else
            LCD_data('R');      /* display 'R' */

        delayMs(10);        /* wait for a while */
    }
}

#define KEYPAD_ROW GPIOE
#define KEYPAD_COL GPIOC 

/* this function initializes the ports connected to the keypad */
void keypad_init(void)
{
    SYSCTL->RCGCGPIO |= 0x04;   /* enable clock to GPIOC */
    SYSCTL->RCGCGPIO |= 0x10;   /* enable clock to GPIOE */
    
    KEYPAD_ROW->DIR |= 0x0F;     /* set row pins 3-0 as output */
    KEYPAD_ROW->DEN |= 0x0F;     /* set row pins 3-0 as digital pins */
    KEYPAD_ROW->ODR |= 0x0F;     /* set row pins 3-0 as open drain */
    
    KEYPAD_COL->DIR &= ~0xF0;    /* set column pin 7-4 as input */
    KEYPAD_COL->DEN |= 0xF0;     /* set column pin 7-4 as digital pins */
    KEYPAD_COL->PUR |= 0xF0;     /* enable pull-ups for pin 7-4*/
}

/* This is a non-blocking function. */
/* If a key is pressed, it returns 1. */
/* Otherwise, it returns a 0 (not ASCII 0).*/
unsigned char keypad_kbhit(void)
{
   int col;

    /* check to see any key pressed */
    KEYPAD_ROW->DATA = 0;               /* enable all rows */
    col = KEYPAD_COL->DATA & 0xF0;      /* read all columns */
    if (col == 0xF0) 
        return 0;   /* no key pressed */
    else
        return 1;   /* a key is pressed */
}
void LCD_init(void)
{ 
    SYSCTL->RCGCGPIO |= 0x01;  /* enable clock to GPIOA */
    SYSCTL->RCGCGPIO |= 0x02;  /* enable clock to GPIOB */
    
    LCD_CTRL->DIR |= 0xE0;     /* set PORTA pin 7-5 as output for control */
    LCD_CTRL->DEN |= 0xE0;     /* set PORTA pin 7-5 as digital pins */
    LCD_DATA->DIR = 0xFF;      /* set all PORTB pins as output for data */
    LCD_DATA->DEN = 0xFF;      /* set all PORTB pins as digital pins */

    delayMs(20);            /* initialization sequence */
    LCD_command(0x30);
    delayMs(5);
    LCD_command(0x30);
    delayUs(100);
    LCD_command(0x30);
    
    LCD_command(0x38);      /* set 8-bit data, 2-line, 5x7 font */
    LCD_command(0x06);      /* move cursor right */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

void LCD_command(unsigned char command)
{
    LCD_CTRL->DATA = 0;     /* RS = 0, R/W = 0 */
    LCD_DATA->DATA = command;
    LCD_CTRL->DATA = EN;    /* pulse E */
    delayUs(0);
    LCD_CTRL->DATA = 0;
    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayUs(40);        /* all others 40 us */
}

void LCD_data(unsigned char data)
{
    LCD_CTRL->DATA = RS;    /* RS = 1, R/W = 0 */
    LCD_DATA->DATA = data;
    LCD_CTRL->DATA = EN | RS;   /* pulse E */
    delayUs(0);
    LCD_CTRL->DATA = 0;
    delayUs(40);
}

/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}  /* do nothing for 1 ms */
}

/* delay n microseconds (16 MHz CPU clock) */
void delayUs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3; j++)
            {}  /* do nothing for 1 us */
}
