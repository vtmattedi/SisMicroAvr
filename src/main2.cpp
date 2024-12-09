#define __DELAY_BACKWARD_COMPATIBLE__ // for _delay_ms, not sure if needed
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// TELA na PORTB
#define TELA_PORTA 'b'
#define TELA_ENABLE 0
#define TELA_RS 1
#define TELA_DATA_4 4
#define TELA_DATA_5 5
#define TELA_DATA_6 6
#define TELA_DATA_7 7

// TECLADO na PORTC
#define TECLADO_PORTA 'c'
#define TECLADO_LINHA_1 0
#define TECLADO_LINHA_2 1
#define TECLADO_LINHA_3 2
#define TECLADO_COLUNA_1 3
#define TECLADO_COLUNA_2 4
#define TECLADO_COLUNA_3 5

#define PORTA_OUTROS 'd'
#define ALARME 4
#define PORTA 5
static unsigned tentativa = 0;
static unsigned senha = 1234;
unsigned volatile long timer1_segundos = 0;
static bool senha_registrada = 0;
static unsigned long porta_aberta_segundos = 0;
void handle_interrupt()
{
    if (senha_registrada)
    {
        if (senha == 0)
        {
            senha = tentativa;
            escrever_serial("Confirme a senha para registrar\n");
            tentativa = 0;
        }
        else if (senha == tentativa)
        {
            senha_registrada = true;
            escrever_serial("Senha registrada\n");
            salvar_senha(senha);
        }
        else
        {
            tentativa = 0;
            senha = 0;
            escrever_serial("Senha incorreta\n");
        }
    }
    else if (senha == tentativa)
    {
        trancar_porta(true);
    }
}
bool ler_pino(char porta, int pino)
{
    if (porta == 'b')
        return (PINB & (1 << pino)) != 0;
    if (porta == 'c')
        return (PINC & (1 << pino)) != 0;
    if (porta == 'd')
        return (PIND & (1 << pino)) != 0;
}

void salvar_senha(unsigned int valor)
{
    uint8_t endereco = 0;
    escrever_eeprom(endereco, valor >> 24 & 0xFF);
    endereco++;
    escrever_eeprom(endereco, valor >> 16 & 0xFF);
    endereco++;
    escrever_eeprom(endereco, valor >> 8 & 0xFF);
    endereco++;
    escrever_eeprom(endereco, valor & 0xFF);
    endereco++;
    escrever_eeprom(endereco, 0xFF);
}

void escrever_pino(char porta, int pino, bool valor)
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
        // Espera o buffer de transmissao estar vazio
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        // Coloca o char no buffer de transmissao
        UDR0 = texto[i];
    }
}

void trancar_porta(bool trancada)
{
    if (trancada)
    {
        escrever_pino(PORTA_OUTROS, ALARME, false);
        escrever_serial("Porta destrancada\n");
    }
    else
    {
        escrever_pino(PORTA_OUTROS, ALARME, true);
        escrever_serial("Porta trancada\n");
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
ISR(TIMER1_COMPA_vect)
{
    timer1_segundos++;
}
void tela_mandar_sinal()
{
    escrever_pino(TELA_PORTA, TELA_ENABLE, true);
    _delay_ms(1);
    escrever_pino(TELA_PORTA, TELA_ENABLE, false);
    _delay_ms(1);
}
void escrever_tela(uint8_t dado, bool pino_rs)
{
    escrever_pino(TELA_PORTA, TELA_RS, pino_rs);
    escrever_pino(TELA_PORTA, TELA_DATA_4, (dado & 0x10) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_5, (dado & 0x20) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_6, (dado & 0x40) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_7, (dado & 0x80) != 0);
    tela_mandar_sinal();
    escrever_pino(TELA_PORTA, TELA_DATA_4, (dado & 0x01) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_5, (dado & 0x02) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_6, (dado & 0x04) != 0);
    escrever_pino(TELA_PORTA, TELA_DATA_7, (dado & 0x08) != 0);
    tela_mandar_sinal();
}

void escrever_texto_tela(char *texto)
{
    for (int i = 0; texto[i] != '\0'; i++)
    {
        escrever_tela(texto[i], true);
    }
}

void escrever_eeprom(uint16_t endereco, uint8_t dado)
{
    // Espera o buffer de transmissao estar vazio
    while (!(EECR & (1 << EEPE)))
        ;
    // Coloca o dado no buffer de transmissao
    EEARH = endereco >> 8;   // 8 bits mais significativos
    EEARL = endereco & 0xFF; // 8 bits menos significativos
    // Escreve o dado no registrador de dados do eeprom
    EEDR = dado;
    // Habilita a escrita
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}

uint8_t ler_eeprom(uint16_t endereco)
{
    // Espera o buffer de transmissao estar vazio
    while (!(EECR & (1 << EEPE)))
        ;
    // Coloca o dado no buffer de transmissao
    EEARH = endereco >> 8;   // 8 bits mais significativos
    EEARL = endereco & 0xFF; // 8 bits menos significativos
    // Habilita a leitura
    EECR |= (1 << EERE);
    return EEDR;
}

void imprimir_tela()
{
    //limpa a tela e retorna para home
    escrever_tela(0x01, false);
    escrever_tela(0x02, false);
    //imprime o texto
    if (senha_registrada)
    {
        if (senha == 0)
        {
            escrever_texto_tela("Configurar Senha:");
        }
        else
        {
            escrever_texto_tela("Repita a Senha:");
        }
    }
    else
    {
        escrever_texto_tela("Senha:");
    }
    //aponta o cursor para o incio da segunda linha
    //0x80 + 0x40
    escrever_tela(0xC0,false)
    char senha_texto[12];
    vsnprintf(senha_texto,12,"%u",senha);
    escrever_texto_tela(senha_texto);
}

void horario(char* horario_texto)
{
    unsigned long segundos = timer1_segundos;
    int hora = (segundos/(24*60*60)%24);
    int minutos = (segundos/3600%60);
    int seg = segundos%60;
    vsprintf(horario_texto,"%02d:%02d:%02d",hora,minutos,segundos);
}

int main(void)
{
    // cofigura as portas
    DDRB = 0xFF; // PORTB como saida
    // 0000 0111 = 0 7
    //  Colunas como saida e linhas como entrada
    DDRC = 0x07; // PORTC como entrada
    // 0011 0000
    DDRD |= (1 << PORTA) | (1 << ALARME); // PORTD como saida
    // ler a senha salva da eeprom
    uint8_t eeprom = ler_eeprom(0);
    while (eeprom != 0xFF)
    {
        senha = senha << 8 + eeprom;
    }

    unsigned long ticks_por_segundo;
    ticks_por_segundo = (F_CPU/(1000/1000)/ 8);
    //habilita o modo CTC e prescaler 8
    TCCR1B |= (1 << WGM12) | (1 << CS11);
    //
    OCR1AH = (ticks_por_segundo >> 8);
    OCR1AL = ticks_por_segundo;
    //habilita interrupcoes no timer1 por comparacao com o registrador OCR1A
    TIMSK1 |= (1 << OCIE1A);

    sei(); // habilita interrupcoes

    // incia a tela
    // Ativa modo 4 bits
    // manda [0] 0010 0000 para a tela, habilitando o modo 4 bits
    escrever_pino(TELA_PORTA, TELA_RS, false);
    escrever_pino(TELA_PORTA, TELA_DATA_4, false);
    escrever_pino(TELA_PORTA, TELA_DATA_5, true);
    escrever_pino(TELA_PORTA, TELA_DATA_6, false);
    escrever_pino(TELA_PORTA, TELA_DATA_7, false);
    tela_mandar_sinal();
    // limpa a tela e retorna o cursor para o inicio
    escrever_tela(0x01, false);
    escrever_tela(0x02, false);

    if (senha != 0)
    {
        senha_registrada = true;

    }

    // loop do programa
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
