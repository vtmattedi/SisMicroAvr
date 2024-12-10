/*
 * projeto_vitoria.cpp
 *
 * Created: 09-Dec-24 15:44:45
 * Author :
 */

// #define ESCONDER_SENHA // Esconde a senha na tela 
// Clock do microcontrolador
#define F_CPU 16000000UL
// Baud rate da serial
#define BAUD_RATE 9600
// Equacao para o baud rate de acordo com o datasheet
#define CLOCK_SERIAL ((F_CPU / 16 / BAUD_RATE) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// TELA na PORTB
#define TELA_PORTA 'b'
#define TELA_ENABLE 0
#define TELA_RS 1
#define TELA_DATA_4 2
#define TELA_DATA_5 3
#define TELA_DATA_6 4
#define TELA_DATA_7 5

// TECLADO na PORTC
#define TECLADO_PORTA 'c'
#define TECLADO_LINHA_1 0
#define TECLADO_LINHA_2 1
#define TECLADO_LINHA_3 2
#define TECLADO_COLUNA_1 3
#define TECLADO_COLUNA_2 4
#define TECLADO_COLUNA_3 5

// Outros
#define PORTA_OUTROS 'd'
#define PINO_USART_TX 1
#define PINO_USART_RX 0
#define CONFIRMAR 2 // PORTD2 = INT0;
#define ALARME 4
#define FECHADURA 5 // Fechadura da porta
#define INDICADOR 6 // Indicador de porta aberta

// Tamanho MAXIMO da senha
#define TAMANHO_SENHA 10
// Quantas tentativas erradas para ativar o alarme
#define TENTATIVAS_PARA_ALARME 15
// Tempo maximo que a porta pode ficar aberta
#define TEMPO_PORTA_ABERTA 15000

// Numero de tentativas erradas
uint8_t tentativas_erradas = 0;
// Senha cadastrada
uint8_t senha[TAMANHO_SENHA] = {0};
// Tentativa de senha
uint8_t tentativa_senha[TAMANHO_SENHA] = {0};
// Index da tentativa de senha
uint8_t index_tenantiva = 0;
// flag que indica se a senha foi registrada
static bool senha_registrada = false;
// flag que indica se a tela deve ser impressa no loop
static bool imprimir_tela_loop = false;
// mantem o numero de milisegundos desde que o microcontrolador foi ligado
unsigned volatile long timer1_millis = 0;
// indica quando que a porta foi aberta, 0 indica que a porta esta fechada
static unsigned long porta_aberta_ms = 0;
// indica se o alarme esta ativo
bool alarme_ativo = false;
// diz qual foi a ultima vez que o alarme foi ligado ou desligado para fazer o som de alarme
unsigned long alarme_ms = 0;

// Foward declarations
void escrever_eeprom(uint16_t endereco, uint8_t dado);
void escrever_serial(char *texto, bool exibir_horario = false);
void escrever_pino(char porta, int pino, bool valor);
void salvar_senha();
void abrir_porta(bool abrir);

/*############## Funcoes de Maniuplacao da senha/fechadura ################*/

// Zera a tentativa de senha e ajuda o index da tentativa para 0
void zerar_tenativa_senha()
{
    for (int i = 0; i < TAMANHO_SENHA; i++)
    {
        tentativa_senha[i] = 0;
    }
    index_tenantiva = 0;
}

// compara a senha com a tentativa de senha
bool tentar_senha()
{
    for (int i = 0; i < TAMANHO_SENHA; i++)
    {
        if (senha[i] != tentativa_senha[i])
        {
            return false;
        }
    }
    return true;
}

// lida com o botao de confirmacao sendo apertado
void botao_apertado()
{
    // Se não tiver senha cadastrada, cadastra a senha
    if (!senha_registrada)
    {
        // Cadastra a senha e pede para confirmar
        if (senha[0] == 0)
        {
            for (int i = 0; i < index_tenantiva; i++)
            {
                senha[i] = tentativa_senha[i];
            }
            escrever_serial("Para registrar a senha cadastrada, repita a senha\n");
        }
        else
        {
            // Se a senha for a mesma, registra a senha e salva na eeprom
            if (tentar_senha())
            {
                senha_registrada = true;
                salvar_senha();
                escrever_serial("Senha cadastrada\n");
            }
            else
            {
                escrever_serial("Senha repetida incorreta, tente novamente\n");
            }
        }
    }
    // Se ja tiver uma senha registrada
    else
    {
        if (tentar_senha())
        {
            abrir_porta(true);
            tentativas_erradas = 0;
        }
        else
        {
            if (porta_aberta_ms > 0)
                return;

            escrever_serial("Senha incorreta\n", true);
            tentativas_erradas++;
            if (tentativas_erradas >= TENTATIVAS_PARA_ALARME)
            {
                alarme_ativo = true;
                escrever_serial("Alarme ativado\n", true);
            }
        }
    }
}

// Funcao para controlar a abertura e fechamento da porta
// O ato de abrir a porta, desativa o alarme
void abrir_porta(bool abrir)
{
    if (abrir)
    {
        escrever_pino(PORTA_OUTROS, FECHADURA, true);
        escrever_pino(PORTA_OUTROS, INDICADOR, true);
        escrever_pino(PORTA_OUTROS, ALARME, false);
        alarme_ativo = false;
        escrever_serial("Porta destrancada\n", true);
        porta_aberta_ms = timer1_millis;
    }
    else
    {
        escrever_pino(PORTA_OUTROS, FECHADURA, false);
        escrever_pino(PORTA_OUTROS, INDICADOR, false);
        escrever_serial("Porta trancada\n", true);
        porta_aberta_ms = 0;
    }
}

/*############## Funcoes da EEPROM ################*/

// Salva um byte em um endereco da eeprom
void escrever_eeprom(uint16_t endereco, uint8_t dado)
{
    // Espera o buffer de transmissao estar vazio
    while ((EECR & (1 << EEPE)))
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

// Salva a senha na eeprom
void salvar_senha()
{
    uint8_t endereco = 0;
    for (int i = 0; i < TAMANHO_SENHA; i++)
    {
        escrever_eeprom(endereco, senha[i]);
        endereco++;
    }
    escrever_eeprom(endereco, 0xFF);
}

// Le o valor em um endereco da eeprom
uint8_t ler_eeprom(uint16_t endereco)
{
    // Espera o buffer de transmissao estar vazio
    while (EECR & (1 << EEPE))
        ;
    // Coloca o dado no buffer de transmissao
    EEARH = endereco >> 8;   // 8 bits mais significativos
    EEARL = endereco & 0xFF; // 8 bits menos significativos
    // Habilita a leitura
    EECR |= (1 << EERE);
    uint8_t dado = EEDR;
    return dado;
}

/*############## Funcoes de IO ################*/

// Ler um pino em uma das portas
bool ler_pino(char porta, int pino)
{
    if (porta == 'b')
        return (PINB & (1 << pino)) != 0;
    if (porta == 'c')
        return (PINC & (1 << pino)) != 0;
    if (porta == 'd')
        return (PIND & (1 << pino)) != 0;
}

// Escreve um valor em um pino em uma das portas
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

// Faz um loop pelas linhas e colunas do teclado
// Se uma tecla for pressionada, retorna o valor da tecla
int ler_teclado()
{
    // Ativa 1 linha por vez
    // Para cada linha ativa, verifica se alguma coluna esta ativa
    // Se estiver, retorna o valor da tecla
    // Se nao, passa para a proxima linha
    // Se nenhuma tecla for pressionada, retorna -1
    for (int linha = 0; linha < 3; linha++)
    {
        escrever_pino(TECLADO_PORTA, TECLADO_LINHA_1, linha == 0);
        escrever_pino(TECLADO_PORTA, TECLADO_LINHA_2, linha == 1);
        escrever_pino(TECLADO_PORTA, TECLADO_LINHA_3, linha == 2);
        for (int coluna = 0; coluna < 3; coluna++)
        {
            if (ler_pino(TECLADO_PORTA, TECLADO_COLUNA_1 + coluna))
            {
                return linha * 3 + coluna + 1;
            }
        }
    }
    return -1;
}

/*############## Funcoes da USART ################*/

// Funcao para escrever um texto na serial, com a possibilidade
// de exibir o horario atual (desde que o sistema foi ligado)
void escrever_serial(char *texto, bool exibir_horario)
{
    if (exibir_horario)
    {
        char buffer_saida[20];
        unsigned long milisegundos = timer1_millis;
        int segs = (milisegundos / 1000UL) % 60;
        int mins = (milisegundos / 1000UL / 60UL) % 60;
        int horas = (milisegundos / 1000UL / 3600UL) % 24;
        snprintf(buffer_saida, 20, "%02d:%02d:%02d - ", horas, mins, segs % 60);
        for (int i = 0; buffer_saida[i] != '\0'; i++)
        {
            while (!(UCSR0A & (1 << UDRE0)))
                ;
            UDR0 = buffer_saida[i];
        }
    }

    for (int i = 0; texto[i] != '\0'; i++)
    {
        // Espera o buffer de transmissao estar vazio
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        // Coloca o char no buffer de transmissao
        UDR0 = texto[i];
    }
}

// Funcao para escrever um byte na serial, usado para escrever um char ao inves de um texto
void escrever_serial_byte(uint8_t dado)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Coloca o char no buffer de transmissao
    UDR0 = dado;
}

/*############## Funcoes de interrupcao ################*/

// Interrupcao para o botao de confirmacao
ISR(INT0_vect)
{
    // Se a porta estiver aberta, fecha a porta
    if (porta_aberta_ms > 0)
    {
        abrir_porta(false);
    }
    // Se a porta tiver fechada, verifica a senha
    else
    {
        botao_apertado();
        zerar_tenativa_senha();
    }
    // Liga a flag de atualizacao da tela
    imprimir_tela_loop = true;
}

// Interrupcao para o timer
ISR(TIMER1_COMPA_vect)
{
    // Incrementa o contador de milisegundos
    // caso o timer1 seja igual ao valor de ticks nescessario para 1 ms
    timer1_millis++;
}

/*############## Funcoes para a tela ################*/

// pulsa o pino enable da tela
void tela_mandar_sinal()
{
    escrever_pino(TELA_PORTA, TELA_ENABLE, true);
    _delay_ms(1);
    escrever_pino(TELA_PORTA, TELA_ENABLE, false);
    _delay_ms(1);
}

// Manda um byte para a tela, com ou sem o pino RS
void escrever_tela(uint8_t dado, bool pino_rs)
{
    escrever_pino(TELA_PORTA, TELA_RS, pino_rs);
    escrever_pino(TELA_PORTA, TELA_DATA_4, dado >> 4 & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_5, dado >> 5 & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_6, dado >> 6 & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_7, dado >> 7 & 0x01);
    tela_mandar_sinal();
    _delay_ms(5);
    escrever_pino(TELA_PORTA, TELA_DATA_4, dado & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_5, dado >> 1 & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_6, dado >> 2 & 0x01);
    escrever_pino(TELA_PORTA, TELA_DATA_7, dado >> 3 & 0x01);
    tela_mandar_sinal();
    _delay_ms(5);
}

// Escreve um texto na tela, caractere por caractere
void escrever_texto_tela(char *texto)
{

    for (int i = 0; texto[i] != '\0'; i++)
    {
        escrever_tela(texto[i], true);
    }
}

// Atualiza a tela com o texto correto.
// de acordo com o modo atual.
void imprimir_tela(int modo)
{
    // 0 - Configurar senha
    // 1 - Repetir senha
    // 2 - Digitar senha
    // 3 - Porta aberta

    // Guarda o ultimo modo para evitar limpar a tela desnecessariamente
    static int modo_anterior = -1;

    if (modo != modo_anterior)
    {
        modo_anterior = modo;
        // limpa a tela e retorna para home
        escrever_tela(0x01, false);
        escrever_tela(0x02, false);

        // imprime o texto base
        if (modo == 0)
        {
            escrever_texto_tela("Configurar Senha:");
        }
        else if (modo == 1)
        {
            escrever_texto_tela("Repita a Senha:");
        }
        else if (modo == 2)
        {
            escrever_texto_tela("Senha:");
        }
        if (modo == 3)
        {
            escrever_texto_tela("Porta Aberta!");
            // Se a porta estiver aberta nao precisa atualizar a tela
            return;
        }
    }

    // Nos modos de registrar a senha, pulamos a segunda linha
    // e escrevemos a senha na tela
    if (modo == 0 || modo == 1)
        escrever_tela(168, false); // Ajusta o cursor para a segunda linha, primeira coluna
    else if (modo == 2)
    {
        // no modo de digitar a senha, escrevemos a quantidade de tentativas restantes
        // caso tenha errado a senha pelo menos 1 vez

        if (tentativas_erradas == 1)
        {
            // escreve "Tentativas: " apenas 1 vez na tela
            escrever_tela(168, false);
            escrever_texto_tela("Tentativas: ");
        }
        if (tentativas_erradas >= 1)
        {
            // calcaula as tentativas restantes mas limita a 0;
            uint8_t tentativas_restantes = (TENTATIVAS_PARA_ALARME - tentativas_erradas) > 0 ? (TENTATIVAS_PARA_ALARME - tentativas_erradas) : 0;
            escrever_tela(180, false); // Segunda linha, coluna 12
            // escreve o numero de tentativas restantes na tela
            if (tentativas_restantes < 10)
            {
                // imprime um espaco, caso TENTATIVAS_PARA_ALARME > 10 para sobreescrever o 1ro digito
                escrever_tela(' ', true); 
                escrever_tela(tentativas_restantes + '0', true);
            }
            else
            {
                // imprime o 1ro digito
                escrever_tela(tentativas_restantes / 10 + '0', true);
                // imprime o 2do digito
                escrever_tela(tentativas_restantes % 10 + '0', true);
            }
        }
        // aponta o cursor para a primeira linha, coluna 6, onde a senha sera escrita
        escrever_tela(0x86, false); // Primeira linha, coluna 6
    }
    // imprime a tentiva atual na tela
    for (int i = 0; i < 16; i++)
    {
        // percorre toda linha, atualizando a senha atual na tela
        // mesmo no modo 2 em que é impresso na primeira linha
        // a tela internamente tem mais de 16 caracteres e portanto não tem problema

        if (tentativa_senha[i] != 0 && i < index_tenantiva)
        {
#ifdef ESCONDER_SENHA
            escrever_tela('*', true);
#else
            escrever_tela(tentativa_senha[i] + '0', true);
#endif
        }
        else
        {
            // se a senha for menor que 16 caracteres, preenche com espacos
            // sobreescrevendo o que estava anteriormente na tela
            escrever_tela(' ', true);
        }
    }
}

// Imprime a tentativa atual na serial (apenas para debug)
void imprimir_tentativa()
{
    for (int i = 0; i < index_tenantiva; i++)
    {
        if (tentativa_senha[i] != 0)
        {
            escrever_serial_byte(tentativa_senha[i] + '0');
        }
    }
}

int main(void)
{
    //  Cofigura as portas do PORTC e PORTD
    //  o PORTB é configurado junto com a tela.
    //  xx00 0111 = 0 7
    //  Colunas como saida e linhas como entrada
    DDRC = 0x07; // PORTC como entrada
    // x111 x010 = 7 2
    // Habilita INT0 como entrada e o resto como saida
    DDRD = 0x72; // PORTD como saida

    // configura o timer1
    unsigned long ticks_por_ms;
    // numero de ticks por segundo / 1000 (ms) / 8 (prescaler)
    ticks_por_ms = (F_CPU / 1000 / 8);
    // habilita o modo CTC e prescaler 8
    TCCR1B |= (1 << WGM12) | (1 << CS11);
    OCR1AH = (ticks_por_ms >> 8);
    OCR1AL = ticks_por_ms;
    // habilita interrupcoes no timer1 por comparacao com o registrador OCR1A
    TIMSK1 |= (1 << OCIE1A);
    //  configura a serial
    // 9600 baud rate

    UBRR0H = (CLOCK_SERIAL >> 8);
    UBRR0L = CLOCK_SERIAL;
    UCSR0B = (1 << TXEN0);
    // Configura o formato da transmissao
    // 8 bits de dados, 2 bits de parada
    // configuracao sugerida pelo datasheet
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);

    // Configura a interrupção externa INT0
    EICRA |= (1 << ISC01); // Configura para disparar na borda de descida
    EIMSK |= (1 << INT0);  // Habilita a interrupção externa INT0
    sei();                 // habilita interrupcoes

    // ler a senha salva da eeprom
    for (int i = 0; i < TAMANHO_SENHA; i++)
    {
        uint8_t eeprom = ler_eeprom(i);
        if (eeprom != 0xFF)
        {
            senha[i] = eeprom;
            // se pelo menos um byte for diferente de 0xFF, a senha foi registrada
            senha_registrada = true;
        }
        // debug
        //  char buffer_saida[50];
        //  snprintf(buffer_saida, 50, "eeprom [%d]->%d\n",i, eeprom);
        //  escrever_serial(buffer_saida);
    }

    // incia a tela
    DDRB = 0x3F;  // configura os pinos da tela como saida (0011 1111) da portb
    PORTB = 0x00; // desliga todos os pinos da tela
    // Ativa modo 4 bits de acordo com o datasheet
    // manda [0] 0010 0000 para a tela, habilitando o modo 4 bits
    _delay_ms(2);
    escrever_pino(TELA_PORTA, TELA_RS, false);
    escrever_pino(TELA_PORTA, TELA_DATA_4, false);
    escrever_pino(TELA_PORTA, TELA_DATA_5, true);
    escrever_pino(TELA_PORTA, TELA_DATA_6, false);
    escrever_pino(TELA_PORTA, TELA_DATA_7, false);
    tela_mandar_sinal();
    _delay_ms(5);
    // limpa a tela e retorna o cursor para o inicio
    escrever_tela(0x01, false); // limpa a tela
    escrever_tela(0x02, false); // retorna o cursor para o inicio
    escrever_tela(0x0C, false); // liga a tela

    // indicar que o sistema foi iniciado
    escrever_texto_tela("Sistema Iniciado\n");
    escrever_serial("Sistema Iniciado\n");

    // incia o loop do programa com a flag de imprimir a tela
    //  e sem tecla pressionada
    int ultima_tecla = -1;
    imprimir_tela_loop = true;
    // loop do programa
    while (true)
    {
        // se precisar imprimir a tela, imprime
        if (imprimir_tela_loop)
        {
            // indentifica em que estagio estamos.
            int modo = 2;          // por padrao estamos no modo de digitar a senha
            if (!senha_registrada) // caso nao tenha senha registrada
            {
                // se a senha for 0, estamos no modo de cadastrar a senha
                // se a senha for diferente de 0, estamos no modo de confirmar a senha
                modo = senha[0] == 0 ? 0 : 1;
            }
            else if (porta_aberta_ms > 0)
            {
                // se a porta estiver aberta, estamos no modo 3
                modo = 3;
            }
            // atualiza a tela com o modo atual
            imprimir_tela(modo);
            // reseta a flag de imprimir a tela
            imprimir_tela_loop = false;
        }

        // se a porta estiver aberta por muito tempo, fecha a porta
        if (porta_aberta_ms > 0 && timer1_millis - porta_aberta_ms > TEMPO_PORTA_ABERTA)
        {
            abrir_porta(false);
            imprimir_tela_loop = true;
        }

        // se o alarme estiver ativo, pisca o led do alarme e emite um som
        if (alarme_ativo)
        {
            unsigned long agora = timer1_millis;
            bool estado = ler_pino(PORTA_OUTROS, ALARME);
            if (estado && agora - alarme_ms > 500)
            {
                escrever_pino(PORTA_OUTROS, ALARME, !estado);
                alarme_ms = agora;
            }
            else if (!estado && agora - alarme_ms > 300)
            {
                escrever_pino(PORTA_OUTROS, ALARME, !estado);
                alarme_ms = agora;
            }
        }

        // ler se tem alguma tecla pressionada
        int tecla = ler_teclado();
        // caso a tecla seja diferente da ultima tecla pressionada
        // (segurar a tecla por muito tempo nao conta como pressionar varias vezes)
        if (tecla != ultima_tecla)
        {
            // se a porta estiver aberta, ignora as teclas
            if (tecla == -1 || porta_aberta_ms > 0)
            {
                ultima_tecla = -1;
                continue;
            }
            // se ainda tiver espaço para digitar a senha
            // adiciona a tecla na tentativa
            if (index_tenantiva < TAMANHO_SENHA)
            {
                tentativa_senha[index_tenantiva] = tecla;
                index_tenantiva++;
                imprimir_tela_loop = true;
            }

            // debug
            // char buffer_saida[50];
            // snprintf(buffer_saida, 50, ": tecla: %d tentativa: ", tecla);
            // escrever_serial(buffer_saida, true);
            // imprimir_tentativa();
            // escrever_serial("\n");

            ultima_tecla = tecla;
        }
    }

    return 1;
}
