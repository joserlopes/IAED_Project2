/* Projeto realizado por:
    José António Lopes LEIC-A 1ºano nº103938
*/

/* Import das várias bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definição de constantes */
#define MAX_FLIGHT 30000
#define MAX_AEROPORT 40
#define MAX_ID 4
#define MAX_COUNTRY 31
#define MAX_CITY 55
#define MAX_CODE_FLIGHT 11
#define MAX_RESERVE 65536
#define IN 1
#define OUT 0

#define less_strings(a, b) (strcmp(a, b) < 0)
#define more_strings(a, b) (strcmp(a, b) > 0)

/* vetor no qual o índice + 1 corresponde a um mês do ano e o conteúdo é o número de dias desse mês */
int days_of_month[12] = {31, 28, 31 ,30 , 31, 30, 31, 31, 30, 31, 30, 31};

/* Definição da estrutura das horas*/
typedef struct time
{
    int hours;
    int minutes;
} Time;

/* Definição da estrutura da data */
typedef struct date
{
    int day;
    int month;
    int year;
} Date;

/* Definição da estrutura dos aeroportos */
typedef struct aeroport
{
    char ID[MAX_ID];
    char country[MAX_COUNTRY];
    char city[MAX_CITY]; /* Tamanho maior que 51 para contar com espaços */
    int num_flights;
} Aeroport;

/* Definição da estrutura dos voos */
typedef struct flight
{
    char code[MAX_CODE_FLIGHT];
    char aeroport_out_ID[MAX_ID];
    char aeroport_arrive_ID[MAX_ID];
    Date date;
    Time hour;
    Time duration;
    int flight_capacity;
} Flight;

typedef struct reserve
{
    char flight_ID[MAX_CODE_FLIGHT];
    Date date;
    char *reserve_ID;
    int num_passengers;
} Reserve;

struct reserve_node
{
    Reserve value;
    struct reserve_node *next;
};

typedef struct reserve_node Node;
typedef struct reserve_node* link;

/* Definição de variáveis globais */
int i_a = 0, i_id = 0, i_v = 0, i_c = 0, i_p = 0, i_r = 0;
Aeroport aeroports[MAX_AEROPORT], temp_aeroport;
Flight temp_flight;
Flight flights[MAX_FLIGHT], flights_aux_p[MAX_FLIGHT], flights_aux_c[MAX_FLIGHT];
Date date = {1, 1, 2022}, new_date;
Reserve reserve;    
link head = NULL;

/* Função que copia o aeroport_1 para o aeroport_2 */
Aeroport copy_aeroport(Aeroport aeroport_1, Aeroport aeroport_2)
{
    strcpy(aeroport_2.ID, aeroport_1.ID);
    strcpy(aeroport_2.country, aeroport_1.country);
    strcpy(aeroport_2.city, aeroport_1.city);
    aeroport_2.num_flights = aeroport_1.num_flights;

    return aeroport_2;
}

/* Função que copia o flight_1 para o flight_2 */
Flight copy_flight(Flight flight_1, Flight flight_2)
{
    strcpy(flight_2.code, flight_1.code);
    strcpy(flight_2.aeroport_out_ID, flight_1.aeroport_out_ID);
    strcpy(flight_2.aeroport_arrive_ID, flight_1.aeroport_arrive_ID); 
    flight_2.date.day = flight_1.date.day;
    flight_2.date.month = flight_1.date.month;
    flight_2.date.year = flight_1.date.year;
    flight_2.hour.hours = flight_1.hour.hours;
    flight_2.hour.minutes = flight_1.hour.minutes;
    flight_2.duration.hours = flight_1.duration.hours;
    flight_2.duration.minutes = flight_1.duration.minutes;
    flight_2.flight_capacity = flight_1.flight_capacity;

    return flight_2;
}

/* Função que dadas duas horas retorna a hora relativa à sua soma */
Time add_time(Time time1, Time time2)
{
    Time sum;

    sum.minutes = time1.minutes + time2.minutes;
    sum.hours = time1.hours + time2.hours;
    sum.hours += sum.minutes / 60;
    sum.minutes %= 60;

    return sum;
}

int invalid_capacity(Flight flight)
{
    if (flight.flight_capacity >= 10)
        return 0;
    return 1;
}


/* Função que verifica a validade da duração de um voo */
int high_fligth_duration(Time duration)
{
    if (duration.hours == 12 && duration.minutes > 0)
        return 1;
    else if (duration.hours > 12)
        return 1;
    return 0;
}

/* Função que verifica se date_1 é no passado relativamente a date_2 */
int past_date(Date date_1, Date date_2)
{
    if (date_1.year < date_2.year)
        return 1;
    if ((date_1.year == date_2.year) && (date_1.month < date_2.month))
        return 1;
    if ((date_1.year == date_2.year) && (date_1.month == date_2.month) && (date_1.day < date_2.day))
        return 1;
    return 0;
}

/* Função que verifica se date_1 é depois de um ano relativamente a date_2 */
int future_date(Date date_1, Date date_2)
{
    if (date_1.year > 2023)
        return 1;
    if (date_1.year - date_2.year > 1)
        return 1;
    if ((date_1.year - date_2.year == 1) && (date_1.month > date_2.month))
        return 1;
    else if ((date_1.year - date_2.year == 1) && (date_1.month == date_2.month) && date_1.day > date_2.day)
        return 1;
    return 0;
}

/* Função que verifica se date_1 já existe num vetor de voos de tamanho max */
int existing_day(Date date_1, Flight flight_1[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (date_1.day == flight_1[i].date.day && date_1.month == flight_1[i].date.month && date_1.year == flight_1[i].date.year)
            return 1;
    }
    return 0;
}

/* Função que verifica se o aeroporto com ID "IDA" já existe */
int in_aeroport(char IDA[], Aeroport aeroport[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (strcmp(IDA, aeroport[i].ID) == 0)
            return 1;
    }
    return 0;
}

/* Função que verifica se o voo com ID "flight_ID" já existe */
int in_flight(char flight_ID[], Flight flights_in[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (strcmp(flight_ID, flights_in[i].code) == 0)
            return 1;
    }
    return 0;
}

/* Função de sort "bubblesort" para strings */
void bubble_sort_strings(Aeroport aeroport_sort[], int max)
{
    Aeroport aux;
    int i, j;

    for (i = 0; i < max - 1; i++)
    {
        for (j = 0; j < max - 1 - i; j++)
        {
            if (strcmp(aeroport_sort[j].ID, aeroport_sort[j + 1].ID) > 0)
            {
                aux = aeroport_sort[j];
                aeroport_sort[j] = aeroport_sort[j + 1];
                aeroport_sort[j + 1] = aux;
            }
        }
    }
}

/* Função que verifica se duas datas, date_1 e date_2 são no mesmo dia */
int same_day(Date date_1, Date date_2)
{
    if ((date_1.day == date_2.day) && (date_1.month == date_2.month) && (date_1.year == date_2.year))
        return 1;

    return 0;
}

/* Função que verifica se um voo com o mesmo código já existe no mesmo dia */
int same_day_flight(Flight flight, Flight flights_same[], int max)
{
    int i;

    for (i = 0; i < max; i++)
    {
        if (strcmp(flight.code, flights_same[i].code) == 0)
        {
            if (same_day(flights_same[i].date, flight.date))
                return 1;
        }
    }

    return 0;
}

/* Função que verifica se time_2 é no futuro relativamente a time_1 */
int past_hour(Time time_1, Time time_2)
{
    if (time_1.hours < time_2.hours)
        return 1;
    if ((time_1.hours == time_2.hours) && (time_1.minutes < time_2.minutes))
        return 1;
    return 0;
}

/* Função de sort "bubblesort" para datas. O algoritmo utilizado foi o bubblesort por ser estável */
void bubble_sort_dates(Flight flights_sort[], int max)
{
    Flight aux;
    int i, j;

    for (i = 0; i < max - 1; i++)
    {
        for (j = 0; j < max - 1 - i; j++)
        {
            if (past_date(flights_sort[j + 1].date, flights_sort[j].date))
            {
                aux = flights_sort[j];
                flights_sort[j] = flights_sort[j + 1];
                flights_sort[j + 1] = aux;
            }
        }
    }
}

/* Função de sort "bubblesort" para datas. O algoritmo utilizado foi o bubblesort por ser estável*/
void bubble_sort_time(Flight flights_sort[], int max)
{
    Flight aux;
    int i, j;

    for (i = 0; i < max - 1; i++)
    {
        for (j = 0; j < max - 1 - i; j++)
        {
            if ((past_hour(flights_sort[j + 1].hour, flights_sort[j].hour)) && (same_day(flights_sort[j + 1].date, flights_sort[j].date)))
            {
                aux = flights_sort[j];
                flights_sort[j] = flights_sort[j + 1];
                flights_sort[j + 1] = aux;
            }
        }
    }
}

/* Função que adiciona um dia a date caso as horas de time forem iguais ou superiores a 24 */
Date add_day(Date date, Time time)
{

    if (time.hours >= 24)
    {
        time.hours -= 24;
        date.day += 1;
    }

    if (date.day > days_of_month[date.month - 1])
    {
        date.day = 1;
        date.month += 1;
    }

    if (date.month == 13)
    {
        date.month = 1;
        date.year++;
    }

    return date;
}

/* Função adicona um aeroport ao sistema se este cumprir os requisitos */
void add_aeroport()
{
    int state_aeroport = OUT, i_aeroport;

    scanf("%s%s ", temp_aeroport.ID, temp_aeroport.country);

    fgets(temp_aeroport.city, MAX_CITY, stdin);

    temp_aeroport.city[strlen(temp_aeroport.city) - 1] = '\0';

    /* Se o IDA do aeroporto tiver menos que 3 caracteres é inválido */
    if (strlen(temp_aeroport.ID) < 3)
                state_aeroport = IN;

    /* Se os caracteres do IDA do aeroporto não forem todos letras maíusculas este é inválido */
    for (i_aeroport = 0; state_aeroport == OUT && i_aeroport < 3; i_aeroport++)
    {
        if (temp_aeroport.ID[i_aeroport] < 'A' || temp_aeroport.ID[i_aeroport] > 'Z')
            state_aeroport = IN;
    }

    /*------------------------------- ERROS ------------------------------*/

    /* Se o IDA do aeroporto for inválido */
    if (state_aeroport)
    {
        puts("invalid airport ID");
    }
    /* Se já tiverem sido criados 40 aeroportos */
    else if (i_a >= MAX_AEROPORT)
    {
        puts("too many airports");
    }
    /* Se o aeroporto já existir */
    else if (in_aeroport(temp_aeroport.ID, aeroports, i_a))
    {
        puts("duplicate airport");
    }
    /* Se nenhum erro se verificar, o aeroporto é adicionado ao sistema */
    else
    {
        aeroports[i_a] = copy_aeroport(temp_aeroport, aeroports[i_a]);
        printf("airport %s\n", temp_aeroport.ID);
        i_a++;
    }
}

/* Função que lista os aeroportos do sistema por ordem alfabética de IDA */
void list_aeroports_alone()
{
    int i;

    for (i = 0; i < i_a; i++)
    {                
        printf("%s %s %s %d\n", aeroports[i].ID, aeroports[i].city, aeroports[i].country, aeroports[i].num_flights);
    }
}

/* Função que lista os aeroportos do sistema correspondentes aos IDA's lidos como argumentos do comando l, por ordem de leitura*/
void list_set_aeroports()
{
    int i;
    char IDA[MAX_ID];
    scanf("%s", IDA);

    if (in_aeroport(IDA, aeroports, i_a))
    {
        for (i = 0; i < i_a; i++)
        {
            if (strcmp(IDA, aeroports[i].ID) == 0)
            {
                printf("%s %s %s %d\n", aeroports[i].ID, aeroports[i].city, aeroports[i].country, aeroports[i].num_flights); 
            }
        }
    }   
    else
        printf("%s: no such airport ID\n", IDA);

}

/* Função que verifica se um dado caracter "c" é um dígito */
int is_digit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

/* Função que verifica se um dado caracter "c" é uma letra maiúscula */
int is_upper_letter(char c)
{
    if (c >= 'A' && c <= 'Z')
        return 1;
    return 0;
}

int is_lower_letter(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    return 0;
}

/* Função que adiciona um voo ao sistema se este cumprir os requisitos */
void add_flight()
{
    int state_flight = OUT, i = 0;
    long unsigned int i_flight_unsigned = 0;

    scanf("%s %s %s %d-%d-%d %d:%d %d:%d %d", temp_flight.code, temp_flight.aeroport_out_ID, temp_flight.aeroport_arrive_ID, &temp_flight.date.day, &temp_flight.date.month, &temp_flight.date.year, &temp_flight.hour.hours, &temp_flight.hour.minutes, &temp_flight.duration.hours, &temp_flight.duration.minutes, &temp_flight.flight_capacity);

    /* Se o tamanho do código do voo for menor que 3 ele automaticamente é inválido */
    if (strlen(temp_flight.code) < 3)
    {
        state_flight = IN;
    }        

    if (strlen(temp_flight.code) < 4 && state_flight == OUT)
    {
        if (is_digit(temp_flight.code[1]))
            state_flight = IN;

        if (is_upper_letter(temp_flight.code[2]) || is_lower_letter(temp_flight.code[2]))
            state_flight = IN;

        else if (temp_flight.code[2] == '0' && state_flight == OUT)
            state_flight = IN;
    }

    if (strlen(temp_flight.code) < 5 && state_flight == OUT)
    {
        if (is_upper_letter(temp_flight.code[3]) || is_lower_letter(temp_flight.code[3]))
        {
            state_flight = IN;
        }
            

        else if (temp_flight.code[3] == '0' && state_flight == OUT)
        {

            state_flight = IN;
        }
            
    }

    /* dois for's para verificar a validade do código do voo */

    /* Se os dois primeiros caracteres não forem letras maiúsculas */
    /* Se os restantes caracteres do não forem dígitos */
    for (i_flight_unsigned = 4; state_flight == OUT && i_flight_unsigned < strlen(temp_flight.code); i_flight_unsigned++)
    {
        if (!is_digit(temp_flight.code[i_flight_unsigned]))
            state_flight = IN;
    }

    /*------------------------------- ERROS ------------------------------*/

    /* Se o código do voo for inválido */
    if (state_flight)
        puts("invalid flight code");
    /* Se um voo com o mesmo código no mesmo dia já existir */
    else if (in_flight(temp_flight.code, flights, i_v) && same_day_flight(temp_flight, flights, i_v))
    {
        puts("flight already exists");
    }
    /* Se o aeroporto de partida correspondente ao voo não exitir */
    else if (!in_aeroport(temp_flight.aeroport_out_ID, aeroports, i_a))
        printf("%s: no such airport ID\n", temp_flight.aeroport_out_ID);
    /* Se o aeroporto de chegada correspondente ao voo não exitir */
    else if (!in_aeroport(temp_flight.aeroport_arrive_ID, aeroports, i_a))
        printf("%s: no such airport ID\n", temp_flight.aeroport_arrive_ID);
    /* Se já tiverem sido criados 30000 voos */
    else if (i_v >= MAX_FLIGHT)
    {
        puts("too many flights");
        i_v--;
    }
    /* Se a data do voo for inválida */
    else if ((past_date(temp_flight.date, date)) || future_date(temp_flight.date, date))
        puts("invalid date");
    /* Se a duração do voo for inválida */
    else if (high_fligth_duration(temp_flight.duration))
        puts("invalid duration");
    /* Se a capacidade do voo for inválida */
    else if (invalid_capacity(temp_flight))
        puts("invalid capacity");
    /* Se nenhum erro se verificar então o voo é adicionado ao sistema */
    else
    {
        flights[i_v] = copy_flight(temp_flight, flights[i_v]);
        for (i = 0; i < i_a; i++)
        {
            if ((strcmp(flights[i_v].aeroport_out_ID, aeroports[i].ID)) == 0)
                aeroports[i].num_flights++;
        }
        i_v++;
    }
}

/* Função que lista os voos do sistema por ordem de criação */
void list_flights()
{
    int i_flight = 0;

    for (i_flight = 0; i_flight < i_v; i_flight++)
    {
        printf("%s %s %s %02d-%02d-%04d %02d:%02d\n", flights[i_flight].code, flights[i_flight].aeroport_out_ID, flights[i_flight].aeroport_arrive_ID, flights[i_flight].date.day, flights[i_flight].date.month, flights[i_flight].date.year, flights[i_flight].hour.hours, flights[i_flight].hour.minutes);
    }
}

/* Função que adiciona um voo a um vetor auxiliar de voos que contém os voos correspondentes ao IDA do aeroporto de partida lido */
void add_flight_p()
{
    char IDA[MAX_ID];
    int i;
    scanf("%s", IDA);

    if (in_aeroport(IDA, aeroports, i_a))
    {
        for (i = 0; i < i_v; i++)
        {
            if (strcmp(flights[i].aeroport_out_ID, IDA) == 0)
            {
                flights_aux_p[i_p] = copy_flight(flights[i], flights_aux_p[i_p]);
                i_p++;
            }
        }
    }
    else
        printf("%s: no such airport ID\n", IDA);
}

/* Função que lista os voos com partida de um certo aeroporto por ordem de data e hora de partida, da mais antiga para a mais recente*/
void list_flights_p()
{
    int i;
    for (i = 0; i < i_p; i++)
        printf("%s %s %02d-%02d-%04d %02d:%02d\n", flights_aux_p[i].code, flights_aux_p[i].aeroport_arrive_ID, flights_aux_p[i].date.day, flights_aux_p[i].date.month, flights_aux_p[i].date.year, flights_aux_p[i].hour.hours, flights_aux_p[i].hour.minutes);

    i_p = 0;
}


/* Função que adiciona um voo a um vetor auxiliar de voos que contém os voos correspondentes ao IDA do aeroporto de chegada lido */
void add_flight_c()
{
    char IDA[MAX_ID];
    int i;

    scanf("%s", IDA);

    if (in_aeroport(IDA, aeroports, i_a))
    {
        for (i = 0; i < i_v; i++)
        {
            if (strcmp(flights[i].aeroport_arrive_ID, IDA) == 0)
            {
            
                flights_aux_c[i_c] = copy_flight(flights[i], flights_aux_c[i_c]);                                                                                                   
                flights_aux_c[i_c].hour = add_time(flights_aux_c[i_c].hour, flights_aux_c[i_c].duration);

                if (flights_aux_c[i_c].hour.hours >= 24)
                {
                    flights_aux_c[i_c].date.day += 1;
                    flights_aux_c[i_c].hour.hours -= 24;
                }

                flights_aux_c[i_c].date = add_day(flights_aux_c[i_c].date, flights_aux_c[i_c].hour);

                i_c++;
            }
        }
    }
    else
        printf("%s: no such airport ID\n", IDA);
}

/* Função que lista os voos com partida de um certo aeroporto por ordem de data e hora de chegada, da mais antiga para a mais recente*/
void list_flights_c()
{
    int i;

    for (i = 0; i < i_c; i++)
    {
        if (!past_date(flights_aux_c[i].date, date))
            printf("%s %s %02d-%02d-%04d %02d:%02d\n", flights_aux_c[i].code, flights_aux_c[i].aeroport_out_ID, flights_aux_c[i].date.day, flights_aux_c[i].date.month, flights_aux_c[i].date.year, flights_aux_c[i].hour.hours, flights_aux_c[i].hour.minutes);
    }
    
    i_c = 0;
}

/* Função que altera a data do sistema se esta for válida */
void change_date()
{
    scanf("%d-%d-%d", &new_date.day, &new_date.month, &new_date.year);


    /* Se a data for no passado ou mais de 1 ano no futuro é inválida */
    if (past_date(new_date, date) || future_date(new_date, date))
        puts("invalid date");
    else
    {
        printf("%02d-%02d-%04d\n", new_date.day, new_date.month, new_date.year);

        date.day = new_date.day;
        date.month = new_date.month;
        date.year = new_date.year;
    }
}

/* Função que diminui o número de voos a sair do aeroporto com ID flight_ID */
void delete_flight_num_from_airport(char flight_ID[])
{
    int i = 0, i_a_aux = 0;
    
    char aeroport_ID[MAX_ID];

    for (i = 0; i < i_v; i++)
    {
        if (strcmp(flights[i].code, flight_ID) == 0)
        {
            strcpy(aeroport_ID, flights[i].aeroport_out_ID);
            for (i_a_aux = 0; i_a_aux < i_a; i_a_aux++)
            {
                if (strcmp(aeroport_ID, aeroports[i_a_aux].ID) == 0)
                    aeroports[i_a_aux].num_flights--;
            }
        }
    }
}

/* Função que liberta a memória referente a um nó da lista de reservas */
void free_node (link node)
{
    free(node->value.reserve_ID);
    free(node);
}

/* Função que liberta a memória referente a toda a lista de reservas */
void free_list()
{
    link x;

    if (head == NULL)
        return;

    while (head != NULL)
    {
        x = head;
        head = head->next;
        free_node(x);
    }
}

/* Função que cria um novo nó de reservas, alocando a memória necessária */
link NEW(Reserve reserve)
{
    link x = (link) malloc(sizeof(Node));

    if (x == NULL)
    {
        puts("no memory");
        free(x);
        free_list(head);
        exit(1);
    }

        x->value.reserve_ID = (char *)malloc(sizeof(char) * (strlen(reserve.reserve_ID) + 1));

        if (x->value.reserve_ID == NULL)
        {
            puts("no memory");
            free(x->value.reserve_ID);
            free_list(head);
            exit(1);
        }

            x->value.date.day = reserve.date.day;
            x->value.date.month = reserve.date.month;
            x->value.date.year = reserve.date.year;
            strcpy(x->value.flight_ID, reserve.flight_ID);
            strcpy(x->value.reserve_ID, reserve.reserve_ID);
            x->value.num_passengers = reserve.num_passengers;
            x->next = NULL;

    return x;
}

/* Função que insere ordenadamente um reserva no sistema */
link insert_sorted(Reserve reserve_aux)
{
    link x = head, prev = NULL;
    link aux = NEW(reserve_aux);
    /* Se a lista estiver vazia, o conteúdo pasa a ser aux */
    if (head == NULL)
        return aux;
    /* Como a lista está sempre ordenada, se o elemento a adicionar for menor que a cabeça, ele passerá a ser a nova cabeça */
    if (less_strings(aux->value.reserve_ID, head->value.reserve_ID))
    {
        aux->next = head;
        return aux;
    }
    /* Enquanto o elemento a adicionar for alfabéticamente superior ao elemento presente, avança-se na lista */
    while (x != NULL && more_strings(aux->value.reserve_ID, x->value.reserve_ID))
    {
        prev = x;
        x = x->next;
    }
    /* Adicionar o elemento antes do primeiro valor alfabéticamente maior que ele (lista já está ordenada) */
    prev->next = aux;
    aux->next = x;

    return head;
}

/* Função que apaga a reserva da lista de reservas com ID "reserve_ID" */
link delete_reserve(char reserve_ID[])
{
    link t, prev;
    for (t = head, prev = NULL; t != NULL; prev = t, t = t->next)
    {
        if(strcmp(t->value.reserve_ID, reserve_ID) == 0)
        {
            if (t == head)
                head = t->next;
            else
                prev->next = t->next;
            free_node(t);
            i_r--;
            break;
        }
    }

    return head;
}

/* Função que apaga da lista de reservas todas as reservas referentes ao voo com ID "flight_ID" */
link delete_reserve_flight(char flight_ID[])
{
    link t, prev;
    for (t = head, prev = NULL; t != NULL; prev = t, t = t->next)
    {
        if(strcmp(t->value.flight_ID, flight_ID) == 0)
        {
            if (t == head)
                head = t->next;
            else
                prev->next = t->next;
            free_node(t);
            i_r--;
            break;
        }
    }

    return head;
}

/* Função que dá print às reservas referentes ao voo com ID "flight_ID" na data "date_aux" */
void print(char flight_ID[], Date date_aux)
{
    link t = head;

    if (t == NULL)
        return;

    for (t = head; t != NULL; t = t->next)
    {
        if (same_day(t->value.date, date_aux) && strcmp(flight_ID, t->value.flight_ID) == 0)
            printf("%s %d\n", t->value.reserve_ID, t->value.num_passengers);
    }
}

/* Função que verifica se a reserva com ID "reserve_ID" já existe */
int in_reserve(char reserve_ID[])
{
    link x = head;
    if (head == NULL)
        return 0;

    while (x != NULL)
    {   
        if (strcmp(x->value.reserve_ID, reserve_ID) == 0)
            return 1;
        x = x->next;
    }
    return 0;
}

/* Função que retira passageiras do voo referente á reserva "reserve_aux" */
int subtract_passengers(Reserve reserve_aux, int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (strcmp(flights[i].code, reserve_aux.flight_ID) == 0 && same_day
        (flights[i].date, reserve_aux.date))
        {
            /* Se o número de passageiros de uma reserva é menor do que 0 ele é inválido portanto volta-se a retornar simplesmente o valor original */
            if (reserve_aux.num_passengers < 0)
                return flights[i].flight_capacity;
            else
                flights[i].flight_capacity -= reserve_aux.num_passengers;
            return flights[i].flight_capacity;
        } 
    }

    return 0;
}

/* Função que adiciona passageiras do voo referente á reserva "reserve_aux" */
void add_passengers(Reserve reserve_aux, int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (strcmp(flights[i].code, reserve_aux.flight_ID) == 0 && same_day(flights[i].date, reserve_aux.date))        
            flights[i].flight_capacity += reserve_aux.num_passengers;
                   
    }
}

/* Função que verifica se um dado voo de ID "flight_ID" existe na data "date_aux" */
int in_flight_reserve(char flight_ID[], Date date_aux, int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (strcmp(flights[i].code, flight_ID) == 0 && same_day(flights[i].date, date_aux))
            return 1;
    }
    return 0;
}

/* Função que adiciona reservas ao sistema ou lista as reservas correspondentes aos comandos */
void add_or_list_reserves()
{
    char next_char, c, reserve_buffer[MAX_RESERVE];
    int i = 0, is_number = 0, is_upper = 0;

    scanf("%s %d-%d-%d", reserve.flight_ID, &reserve.date.day, &reserve.date.month, &reserve.date.year);

    /* Caso seja para adicionar uma reserva ao sistema */
    if ((next_char = getchar()) != '\n')
    {
        while ((c = getchar()) != ' ')
        {
            /* Verificar se cada caracter do ID da reserva é uma letra maiúscula ou um dígito */
            if ((c < 'A' || c > 'Z') && (c < '0' || c > '9') && is_upper == 0)
                is_upper = 1;
            if ((c < '0' || c > '9') && (c < 'A' || c > 'Z') && is_number == 0)
                is_number = 1;
            reserve_buffer[i] = c;
            i++;
        }
        reserve_buffer[i] = '\0';

        reserve.reserve_ID = (char *)malloc(sizeof(char) * i + 1);
        
        /* Se já não houver memória suficiente */
        if (reserve.reserve_ID == NULL)
        {
            puts("no memory");
            free(reserve.reserve_ID);
            free_list(head);
            exit(1);
        }

        strcpy(reserve.reserve_ID, reserve_buffer);

        scanf("%d", &reserve.num_passengers);

        /*------------------------------- ERROS ------------------------------*/

        /* Se o ID da reserva tiver menos de 10 caracteres */
        if (i < 10)
        {
            puts("invalid reservation code");
            free(reserve.reserve_ID);
        }
        /* Se o ID da reserva não for composto só por letras maísculas e dígitos */
        else if (is_number == 1 || is_upper == 1)
        {
            puts("invalid reservation code");
            free(reserve.reserve_ID);
        }
        /* Se não existir um voo com o ID na data indicada */
        else if (!in_flight_reserve(reserve.flight_ID, reserve.date, i_v))
        {
            printf("%s: flight does not exist\n", reserve.flight_ID);
            free(reserve.reserve_ID);
        }
        /* Se já existir uma reserva com o "reserve_ID" indicado */
        else if (in_reserve(reserve.reserve_ID))
        {
            printf("%s: flight reservation already used\n", reserve.reserve_ID);
            free(reserve.reserve_ID);
        }
        /* Se a reserva, ao ser criada, exceder a capadicade do voo */
        else if (subtract_passengers(reserve, i_v) < 0)
        {
            puts("too many reservations");
            add_passengers(reserve, i_v);
            free(reserve.reserve_ID);
        }
        /* Se a data for inválida */
        else if ((past_date(reserve.date, date)) || future_date(reserve.date, date))
        {
            puts("invalid date");
            free(reserve.reserve_ID);
        }
        /* Se o número de passageiros foi inválido */
        else if (reserve.num_passengers <= 0)
        {
            puts("invalid passenger number");
            free(reserve.reserve_ID);
        }
        /* Se nenhum dos erros se verificar, adicionar a reserva ao sistema */
        else
        {
            head = insert_sorted(reserve);
            i_r++;
            free(reserve.reserve_ID);
        }                
    }   
    /* Caso seja para listar reservas */

    /* Se não existir um voo com o ID na data indicada */
    else if (!in_flight_reserve(reserve.flight_ID, reserve.date, i_v))
        printf("%s: flight does not exist\n", reserve.flight_ID);
    /* Se a data for inválida */
    else if ((past_date(reserve.date, date)) || future_date(reserve.date, date))
        puts("invalid date");
    /* Se nenhum dos erros se verificar, listar todas as reservas relativas ao voo da reserva na data correspondente */
    else
        print(reserve.flight_ID, reserve.date);
}

/* Função que remove uma reserva ou um voo com todas as suas reservas correspondentes e as demais implicações */
void delete_reserve_or_flight()
{
    char reserve_buffer[MAX_RESERVE];
    int i = 0, i_v_aux = 0, i_r_aux = 0, i_current = 0, state_flight = OUT;

    i_v_aux = i_v;
    i_r_aux = i_r;

    scanf("%s", reserve_buffer);

    /* Caso seja para eliminar um voo */
    if (strlen(reserve_buffer) < 10)
    {
        /* Se o voo não existir */
        if (!in_flight(reserve_buffer, flights, i_v))
        {
            puts("not found");
            state_flight = IN;
        }
        else                    
        {
            delete_flight_num_from_airport(reserve_buffer);
            for (i = 0; i < i_r_aux; i++)
            {
                if (past_date(add_day(flights[i].date, add_time(flights[i].hour, flights[i].duration)), date))                
                    head = delete_reserve_flight(reserve_buffer);
                    
            }                            

            i = 0;
            
            while (i < i_v_aux)
            {
                /* "Puxar" os elementos para trás se um igual for encontrado */
                if (strcmp(reserve_buffer, flights[i].code) != 0 || !past_date(add_day(flights[i].date, add_time(flights[i].hour, flights[i].duration)), date))
                {
                    flights[i_current] = copy_flight(flights[i], flights[i_current]);
                    i_current++;
                }        
                else                 
                    i_v--; /* Decrementar o contador de voos se um for eliminado */
                i++;
            }

            if (i_v == i_v_aux && state_flight == OUT)
                puts("not found");
        }
    }            
    /* Se a reserva não existir */
    else if (!in_reserve(reserve_buffer))
        puts("not found");                
    else
    {
        /* Voltar a adcionar os passageiros ao voo, porque este foi eliminado */
        add_passengers(reserve, i_v);
        head = delete_reserve(reserve_buffer);
    }

}

/* Função main */
int main()
{
    /* Definição de variáveis */
    char command, next_char;

    /* ciclo que lê o input do stdinput correspondente aos comandos do sistema.
    Utiliza-se um ciclo do while, para garantir que este é executado pelo menos uma vez. */
    do
    {
        command = getchar();

        /* comando 'a' */
        if (command == 'a')
        {        
            /* Adiciona um novo aeroporto ao sistema */
            add_aeroport();
        }
        /* comando 'l' */
        else if (command == 'l')
        {
            /* Organiza os aeroportos por ordem alfabética de IDA */
            bubble_sort_strings(aeroports, i_a);

            /* Lê o próximo caracter para saber se o comando foi invocado com argumentos */
            next_char = getchar();

            /* Se sim, lista todos os aeroportos por ordem alfabética de IDA */
            if (next_char == '\n')
            {
                list_aeroports_alone();
            }

            /* Se sim, listar todos os aeroportos do sistema correspondentes aos IDA's lidos como argumentos do comando, por ordem de leitura*/
            else
            {
                while (next_char != '\n')
                {                 
                    list_set_aeroports();

                    next_char = getchar();
                }
            }
        }
        /* comando 'v' */
        else if (command == 'v')    
        {
            /* Adiciona um novo voo ao sitema */
            if ((next_char = getchar()) != '\n')
            {
                add_flight();
            }
            /* Se não, lista os voos por ordem de criação */
            else
            {
                list_flights();
            }
        }
        /* comando 'p' */
        else if (command == 'p')
        {
            /* Adiciona voos ao vetor auxiliar "fligts_aux_p" */
            add_flight_p();            

            /* Organiza esses voos por data e hora (do mais antigo para o mais recente) */
            bubble_sort_dates(flights_aux_p, i_p);

            bubble_sort_time(flights_aux_p, i_p);

            /* E listá-los */
            list_flights_p();

        }
        /* comando 'c' */
        else if (command == 'c')
        {
            /* Adiciona voos ao vetor auxiliar "fligts_aux_c" */
            add_flight_c();

            /* Organiza esses voos por data e hora (do mais antigo para o mais recente) */
            bubble_sort_dates(flights_aux_c, i_c);

            bubble_sort_time(flights_aux_c, i_c);

            /* E listá-los */
            list_flights_c();
        }
        /* comando 't' */
        else if (command == 't')
        {
            /* Altera a data do sistema */
            change_date();
        }
        /* comando 'r' */
        else if (command == 'r')
        {
            /* Adiciona reservas ao sistema ou lista as reservas correspondentes aos comandos */
            add_or_list_reserves();
        }
        /* comando 'e' */
        else if (command == 'e')
        {
            /* Remove uma reserva ou um voo com todas as suas reservas correspondentes */
            delete_reserve_or_flight();
        }
    /* Se o comando lido for 'q' ou se se chaegar ao fim do ficheiro, então o programa é terminado */
    } while (command != EOF && command != 'q');

    /* Libertar toda a memória da lista de reservas */
    free_list(head);

    return 0;
}
