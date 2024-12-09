#define __DELAY_BACKWARD_COMPATIBLE__ // for _delay_ms, not sure if needed
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

//TELA na PORTB
#define TELA_PORTA 'b'
#define TELA_ENABLE 0
#define TELA_RS 1
#define TELA_DATA_4 4
#define TELA_DATA_5 5
#define TELA_DATA_6 6
#define TELA_DATA_7 7

//TECLADO na PORTC
#define TECLADO_PORTA 'c' 
#define TECLADO_LINHA_1 0
#define TECLADO_LINHA_2 1
#define TECLADO_LINHA_3 2
#define TECLADO_COLUNA_1 3
#define TECLADO_COLUNA_2 4
#define TECLADO_COLUNA_3 5

#define ALARME 4
#define PORTA 5
static int tentativa = 0;
static int senha = 1234;
bool ler_pino(char porta, int pino)
{
    if (porta == 'b')
        return (PINB & (1 << pino)) != 0;
    if (porta == 'c')
        return (PINC & (1 << pino)) != 0;
    if (porta == 'd')
        return (PIND & (1 << pino)) != 0;
}

void escrever_pino (char porta, int pino, bool valor)
{
    if (porta == 'b')
    {
        if (valor)
            PORTB |= (1 << pino);
        else
            PORTB &= ~(1 << pino);
    }
    if (porta == 'c')
    {
        if (valor)
            PORTC |= (1 << pino);
        else
            PORTC &= ~(1 << pino);
    }
    if (porta == 'd')
    {
        if (valor)
            PORTD |= (1 << pino);
        else
            PORTD &= ~(1 << pino);
    }
}

int ler_teclado()
{
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_1, true);
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_1))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_1, false);
        return 1;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_2))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_1, false);
        return 4;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_3))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_1, false);
        return 7;
    }
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_1, false);
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_2, true);
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_1))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_2, false);
        return 2;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_2))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_2, false);
        return 5;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_3))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_2, false);
        return 8;
    }
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_2, false);
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_3, true);
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_1))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_3, false);
        return 3;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_2))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_3, false);
        return 6;
    }
    if (ler_pino(TECLADO_PORTA, TECLADO_LINHA_3))
    {
        escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_3, false);
        return 9;
    }
    escrever_pino(TECLADO_PORTA, TECLADO_COLUNA_3, false);

    return -1;
}

void escrever_serial(char *texto)
{
    for (int i = 0; texto[i] != '\0'; i++)
    {
        //Espera o buffer de transmissao estar vazio
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        //Coloca o char no buffer de transmissao
        UDR0 = texto[i];
    }
}

ISR(INT0_vect)
{
    if (tentativa == senha)
    {
        escrever_serial("Senha correta\n");
    }
    else
    {
        escrever_serial("Senha incorreta\n");
    }
    tentativa = 0;
}	

int main(void)
{
    //cofigura as portas
    DDRB = 0xFF; //PORTB como saida
    //0000 0111
    // Colunas como saida e linhas como entrada
    DDRC = 0x07; //PORTC como entrada
    // 0011 0000
    DDRD = 0x30; //PORTD como saida
    sei();//habilita interrupcoes

    //loop do programa
    while (true)
    {
        int tecla = ler_teclado();
        if (tecla == -1)
            continue;
        escrever_serial("Tecla: ");
        char tecla_char[] = {(tecla + '0'), '\0'};
        escrever_serial(&tecla_char[0]);
        tentativa = tentativa * 10 + tecla;
    }

    return 1;
}
