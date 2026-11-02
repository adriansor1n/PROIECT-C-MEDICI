#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX 100

#define RESET      "\033[0m"
#define RED        "\033[31m"
#define GREEN      "\033[32m"
#define YELLOW     "\033[33m"
#define CYAN       "\033[36m"
#define BOLD       "\033[1m"
#define MAGENTA    "\033[35m"
#define BLUE       "\033[34m"
#define BG_BLUE    "\033[44m"
#define BG_CYAN    "\033[46m"
#define WHITE      "\033[37m"
#define DIM        "\033[2m"

typedef struct
{
    char nume[50];
    char specialitate[50];
    char program[50];
} Medic;

typedef struct
{
    char pacient[50];
    char medic[50];
    char data[20];
    char ora[10];
} Programare;

void clearscreen()
{
    system("cls||clear");
}

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void asteaptaenter()
{
    printf("\n" DIM "Apasa ENTER pentru a reveni la meniu..." RESET);
    clearBuffer();
    getchar();
    clearscreen();
}

Medic medici[MAX];
int nr_medici = 0;

Programare programari[MAX];
int nr_programari = 0;

void strToLower(const char *src, char *dst)
{
    int i;
    for (i = 0; src[i]; i++)
        dst[i] = tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

int esteNumeValid(const char *s)
{
    if (strlen(s) < 2) return 0;
    for (int i = 0; s[i]; i++)
        if (!isalpha((unsigned char)s[i]) && s[i] != '-' && s[i] != ' ')
            return 0;
    return 1;
}

int esteDataValida(const char *d)
{
    if (strlen(d) != 10) return 0;
    if (d[2] != '-' || d[5] != '-') return 0;
    for (int i = 0; i < 10; i++)
        if (i != 2 && i != 5 && !isdigit((unsigned char)d[i])) return 0;
    int zi   = atoi(d);
    int luna = atoi(d + 3);
    int an   = atoi(d + 6);
    if (zi < 1 || zi > 31)    return 0;
    if (luna < 1 || luna > 12) return 0;
    if (an < 2024 || an > 2100) return 0;

    time_t t = time(NULL);
    struct tm *azi = localtime(&t);
    int an_azi   = azi->tm_year + 1900;
    int luna_azi = azi->tm_mon + 1;
    int zi_azi   = azi->tm_mday;

    if (an < an_azi) return 0;
    if (an == an_azi && luna < luna_azi) return 0;
    if (an == an_azi && luna == luna_azi && zi < zi_azi) return 0;

    return 1;
}

int esteOraValida(const char *o)
{
    if (strlen(o) != 5) return 0;
    if (o[2] != ':') return 0;
    for (int i = 0; i < 5; i++)
        if (i != 2 && !isdigit((unsigned char)o[i])) return 0;
    int h = atoi(o);
    int m = atoi(o + 3);
    if (h < 0 || h > 23) return 0;
    if (m < 0 || m > 59) return 0;
    return 1;
}

int esteOraInProgram(const char *ora, const char *program)
{
    int ora_h      = atoi(ora);
    int prog_start = atoi(program);
    int prog_stop  = atoi(program + 3);
    return (ora_h >= prog_start && ora_h < prog_stop);
}

int esteProgramValid(const char *p)
{
    if (strlen(p) != 5) return 0;
    if (p[2] != '-') return 0;
    for (int i = 0; i < 5; i++)
        if (i != 2 && !isdigit((unsigned char)p[i])) return 0;
    int start = atoi(p);
    int stop  = atoi(p + 3);
    if (start < 0 || start > 23) return 0;
    if (stop  < 0 || stop  > 23) return 0;
    if (stop <= start) return 0;
    return 1;
}

void citesteNumeCuSpatiu(char *buf, int maxlen)
{
    fgets(buf, maxlen, stdin);
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
}

void scrieRaportProgramari()
{
    FILE *f = fopen("raport_programari.txt", "w");
    if (f == NULL)
    {
        printf(RED "EROARE: Nu s-a putut crea raport_programari.txt!\n" RESET);
        return;
    }
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", tm_info);

    fprintf(f, "========================================\n");
    fprintf(f, "       RAPORT PROGRAMARI CONSULTATII    \n");
    fprintf(f, "========================================\n");
    fprintf(f, "Generat la: %s\n", timestamp);
    fprintf(f, "----------------------------------------\n\n");

    if (nr_programari == 0)
    {
        fprintf(f, "Nu exista programari inregistrate.\n");
    }
    else
    {
        fprintf(f, "Total programari: %d\n\n", nr_programari);
        fprintf(f, "%-20s %-20s %-15s %-10s\n", "Pacient", "Medic", "Data", "Ora");
        fprintf(f, "------------------------------------------------------------\n");
        for (int i = 0; i < nr_programari; i++)
            fprintf(f, "%-20s %-20s %-15s %-10s\n",
                    programari[i].pacient,
                    programari[i].medic,
                    programari[i].data,
                    programari[i].ora);
    }
    fprintf(f, "\n========================================\n");
    fclose(f);
}

void salveazaProgramari()
{
    FILE *f = fopen("programari.txt", "w");
    if (f == NULL)
    {
        printf(RED "EROARE: Nu s-a putut salva fisierul programari.txt!\n" RESET);
        return;
    }
    fprintf(f, "%-20s|%-20s|%-15s|%-10s\n", "Pacient", "Medic", "Data", "Ora");
    fprintf(f, "------------------------------------------------------------\n");
    for (int i = 0; i < nr_programari; i++)
        fprintf(f, "%-20s|%-20s|%-15s|%-10s\n",
                programari[i].pacient,
                programari[i].medic,
                programari[i].data,
                programari[i].ora);
    fclose(f);
}

void incarcaProgramari()
{
    FILE *f = fopen("programari.txt", "r");
    if (f == NULL) return;
    nr_programari = 0;
    char linie[120];
    fgets(linie, sizeof(linie), f);
    fgets(linie, sizeof(linie), f);
    while (nr_programari < MAX && fgets(linie, sizeof(linie), f))
    {
        char pacient[50], medic[50], data[20], ora[10];
        if (sscanf(linie, "%49[^|]|%49[^|]|%19[^|]|%9[^\n]",
                   pacient, medic, data, ora) == 4)
        {
            int j;
            for (j = strlen(pacient)-1; j >= 0 && pacient[j]==' '; j--) pacient[j]='\0';
            for (j = strlen(medic)-1;   j >= 0 && medic[j]==' ';   j--) medic[j]='\0';
            for (j = strlen(data)-1;    j >= 0 && data[j]==' ';    j--) data[j]='\0';
            for (j = strlen(ora)-1;     j >= 0 && ora[j]==' ';     j--) ora[j]='\0';
            strcpy(programari[nr_programari].pacient, pacient);
            strcpy(programari[nr_programari].medic,   medic);
            strcpy(programari[nr_programari].data,    data);
            strcpy(programari[nr_programari].ora,     ora);
            nr_programari++;
        }
    }
    fclose(f);
}

void incarcaMedici()
{
    FILE *f = fopen("medici.txt", "r");
    if (f == NULL)
    {
        printf(RED "EROARE: Fisierul medici.txt nu a fost gasit!\n" RESET);
        exit(1);
    }
    nr_medici = 0;
    while (nr_medici < MAX &&
           fscanf(f, "%49s %49s %19s",
                  medici[nr_medici].nume,
                  medici[nr_medici].specialitate,
                  medici[nr_medici].program) == 3)
    {
        nr_medici++;
    }
    fclose(f);
}

void salveazaMedici()
{
    FILE *f = fopen("medici.txt", "w");
    if (f == NULL)
    {
        printf(RED "EROARE: Nu s-a putut salva fisierul medici.txt!\n" RESET);
        return;
    }
    for (int i = 0; i < nr_medici; i++)
        fprintf(f, "%s %s %s\n",
                medici[i].nume,
                medici[i].specialitate,
                medici[i].program);
    fclose(f);
}

void printSeparator()
{
    printf(DIM CYAN "  ========================================\n" RESET);
}

void printHeader(const char *titlu)
{
    clearscreen();
    printf("\n");
    printf(BOLD CYAN "  +======================================+\n");
    printf(BOLD CYAN "  |  " BOLD WHITE "%-36s" CYAN "  |\n", titlu);
    printf(BOLD CYAN "  +======================================+\n" RESET);
    printf("\n");
}

void afisaremedici()
{
    printHeader("LISTA MEDICI");
    if (nr_medici == 0)
    {
        printf(RED "  Nu exista medici inregistrati.\n" RESET);
    }
    else
    {
        printf(BOLD "  %-15s  %-15s  %s\n" RESET, "Nume", "Specialitate", "Program");
        printSeparator();
        for (int i = 0; i < nr_medici; i++)
            printf("  " CYAN "%-15s" RESET "  " YELLOW "%-15s" RESET "  " GREEN "%s" RESET "\n",
                   medici[i].nume, medici[i].specialitate, medici[i].program);
    }
    asteaptaenter();
}

void cautamedicnume()
{
    printHeader("CAUTARE MEDIC DUPA NUME");
    printf(DIM "  (0 = inapoi)\n\n" RESET);
    char nume[50];
    int gasit = 0;

    clearBuffer();
    do {
        printf(BOLD "  Numele medicului: " RESET);
        citesteNumeCuSpatiu(nume, sizeof(nume));
        if (strcmp(nume, "0") == 0) { clearscreen(); return; }
        if (!esteNumeValid(nume))
            printf(RED "  X Nume invalid. Foloseste doar litere si spatii.\n" RESET);
    } while (!esteNumeValid(nume));

    char numeL[50], medicL[50];
    strToLower(nume, numeL);

    for (int i = 0; i < nr_medici; i++)
    {
        strToLower(medici[i].nume, medicL);
        if (strcmp(medicL, numeL) == 0)
        {
            printf("\n  " CYAN "%-15s" RESET "  " YELLOW "%-15s" RESET "  " GREEN "%s" RESET "\n",
                   medici[i].nume, medici[i].specialitate, medici[i].program);
            gasit = 1;
        }
    }
    if (!gasit)
        printf(RED "\n  Medicul nu a fost gasit.\n" RESET);
    asteaptaenter();
}

void cautamedicspecialitate()
{
    printHeader("CAUTARE MEDIC DUPA SPECIALITATE");
    printf(DIM "  (0 = inapoi)\n\n" RESET);
    char spec[50];
    int gasit = 0;

    clearBuffer();
    do {
        printf(BOLD "  Specialitatea: " RESET);
        citesteNumeCuSpatiu(spec, sizeof(spec));
        if (strcmp(spec, "0") == 0) { clearscreen(); return; }
        if (!esteNumeValid(spec))
            printf(RED "  X Specialitate invalida. Foloseste doar litere.\n" RESET);
    } while (!esteNumeValid(spec));

    char specL[50], medicSpecL[50];
    strToLower(spec, specL);

    for (int i = 0; i < nr_medici; i++)
    {
        strToLower(medici[i].specialitate, medicSpecL);
        if (strcmp(medicSpecL, specL) == 0)
        {
            printf("\n  " CYAN "%-15s" RESET "  " GREEN "%s" RESET "\n",
                   medici[i].nume, medici[i].program);
            gasit = 1;
        }
    }
    if (!gasit)
        printf(RED "\n  Nu exista medici cu aceasta specialitate.\n" RESET);
    asteaptaenter();
}

void cautamedicprogram()
{
    printHeader("CAUTARE MEDIC DUPA PROGRAM");
    printf(DIM "  (0 = inapoi)\n\n" RESET);
    char prog[20];
    int gasit = 0;

    do {
        printf(BOLD "  Program (hh-hh, ex: 08-14): " RESET);
        scanf("%19s", prog);
        if (strcmp(prog, "0") == 0) { clearscreen(); return; }
        if (!esteProgramValid(prog))
            printf(RED "  X Format invalid. Foloseste hh-hh.\n" RESET);
    } while (!esteProgramValid(prog));

    for (int i = 0; i < nr_medici; i++)
        if (strcmp(medici[i].program, prog) == 0)
        {
            printf("\n  " CYAN "%-15s" RESET "  " YELLOW "%s" RESET "\n",
                   medici[i].nume, medici[i].specialitate);
            gasit = 1;
        }
    if (!gasit)
        printf(RED "\n  Nu exista medici cu acest program.\n" RESET);
    asteaptaenter();
}

void adaugaMedic()
{
    printHeader("ADAUGARE MEDIC");
    printf(DIM "  (0 la orice camp = inapoi)\n\n" RESET);

    if (nr_medici >= MAX)
    {
        printf(RED "  Lista de medici este plina!\n" RESET);
        asteaptaenter();
        return;
    }
    char tmp[50];

    clearBuffer();
    do {
        printf(BOLD "  Nume medic (litere/spatiu, min 2 caractere): " RESET);
        citesteNumeCuSpatiu(tmp, sizeof(tmp));
        if (strcmp(tmp, "0") == 0) { clearscreen(); return; }
        if (!esteNumeValid(tmp))
            printf(RED "  X Nume invalid.\n" RESET);
    } while (!esteNumeValid(tmp));
    strcpy(medici[nr_medici].nume, tmp);

    do {
        printf(BOLD "  Specialitate (litere, min 2 caractere): " RESET);
        citesteNumeCuSpatiu(tmp, sizeof(tmp));
        if (strcmp(tmp, "0") == 0) { clearscreen(); return; }
        if (!esteNumeValid(tmp))
            printf(RED "  X Specialitate invalida.\n" RESET);
    } while (!esteNumeValid(tmp));
    strcpy(medici[nr_medici].specialitate, tmp);

    do {
        printf(BOLD "  Program (hh-hh, ex: 08-14): " RESET);
        scanf("%19s", tmp);
        clearBuffer();
        if (strcmp(tmp, "0") == 0) { clearscreen(); return; }
        if (!esteProgramValid(tmp))
            printf(RED "  X Format invalid.\n" RESET);
    } while (!esteProgramValid(tmp));
    strcpy(medici[nr_medici].program, tmp);

    nr_medici++;
    salveazaMedici();
    printf(GREEN "\n  OK Medicul " RESET CYAN "%s" RESET GREEN " a fost adaugat cu succes!\n" RESET,
           medici[nr_medici - 1].nume);
    asteaptaenter();
}

void stergeMedic()
{
    printHeader("STERGERE MEDIC");
    printf(DIM "  (0 = inapoi)\n\n" RESET);

    if (nr_medici == 0)
    {
        printf(RED "  Nu exista medici de sters.\n" RESET);
        asteaptaenter();
        return;
    }
    for (int i = 0; i < nr_medici; i++)
        printf("  " BOLD MAGENTA "%2d." RESET " " CYAN "%-15s" RESET " - " YELLOW "%-15s" RESET " - " GREEN "%s" RESET "\n",
               i + 1, medici[i].nume, medici[i].specialitate, medici[i].program);

    int nr;
    do {
        printf("\n  Numarul medicului de sters (1-%d, 0 = inapoi): ", nr_medici);
        if (scanf("%d", &nr) != 1) { clearBuffer(); nr = -1; }
        if (nr == 0) { clearscreen(); return; }
        if (nr < 1 || nr > nr_medici)
            printf(RED "  X Numar invalid.\n" RESET);
    } while (nr < 1 || nr > nr_medici);

    char numeStters[50];
    strcpy(numeStters, medici[nr - 1].nume);
    for (int i = nr - 1; i < nr_medici - 1; i++)
        medici[i] = medici[i + 1];
    nr_medici--;
    salveazaMedici();
    printf(GREEN "\n  OK Medicul " RESET CYAN "%s" RESET GREEN " a fost sters cu succes!\n" RESET, numeStters);
    asteaptaenter();
}

void programareConsultatie()
{
    printHeader("PROGRAMARE CONSULTATIE");
    printf(DIM "  (0 = inapoi)\n\n" RESET);

    printf(BOLD "  Medici disponibili:\n" RESET);
    printSeparator();
    for (int i = 0; i < nr_medici; i++)
        printf("  " BOLD MAGENTA "%2d." RESET " " CYAN "%-15s" RESET " - " YELLOW "%-15s" RESET " - " GREEN "%s" RESET "\n",
               i + 1, medici[i].nume, medici[i].specialitate, medici[i].program);

    if (nr_programari >= MAX)
    {
        printf(RED "\n  Nu se mai pot adauga programari (lista plina).\n" RESET);
        asteaptaenter();
        return;
    }

    int alegere;
    do {
        printf("\n  Numarul medicului ales (1-%d, 0 = inapoi): ", nr_medici);
        if (scanf("%d", &alegere) != 1) { clearBuffer(); alegere = -1; }
        if (alegere == 0) { clearscreen(); return; }
        if (alegere < 1 || alegere > nr_medici)
            printf(RED "  X Alegere invalida.\n" RESET);
    } while (alegere < 1 || alegere > nr_medici);
    alegere--;

    char pacient[50], data[20], ora[10];

    clearBuffer();
    do {
        printf(BOLD "  Numele pacientului (litere/spatiu, 0 = inapoi): " RESET);
        citesteNumeCuSpatiu(pacient, sizeof(pacient));
        if (strcmp(pacient, "0") == 0) { clearscreen(); return; }
        if (!esteNumeValid(pacient))
            printf(RED "  X Nume invalid.\n" RESET);
    } while (!esteNumeValid(pacient));

    do {
        printf(BOLD "  Data consultatiei (zz-ll-aaaa, ex: 25-06-2025): " RESET);
        scanf("%19s", data);
        if (strcmp(data, "0") == 0) { clearscreen(); return; }
        if (!esteDataValida(data))
            printf(RED "  X Data invalida sau in trecut. Foloseste formatul zz-ll-aaaa.\n" RESET);
    } while (!esteDataValida(data));

    do {
        printf(BOLD "  Ora consultatiei (hh:mm) " RESET "[medicul lucreaza " GREEN "%s" RESET "]: ",
               medici[alegere].program);
        scanf("%9s", ora);
        if (strcmp(ora, "0") == 0) { clearscreen(); return; }
        if (!esteOraValida(ora))
            printf(RED "  X Ora invalida.\n" RESET);
        else if (!esteOraInProgram(ora, medici[alegere].program))
            printf(RED "  X Ora %s este in afara programului medicului (%s).\n" RESET,
                   ora, medici[alegere].program);
    } while (!esteOraValida(ora) || !esteOraInProgram(ora, medici[alegere].program));

    strcpy(programari[nr_programari].pacient, pacient);
    strcpy(programari[nr_programari].medic,   medici[alegere].nume);
    strcpy(programari[nr_programari].data,     data);
    strcpy(programari[nr_programari].ora,      ora);
    nr_programari++;
    salveazaProgramari();
    scrieRaportProgramari();

    printf(GREEN "\n  OK Programare inregistrata cu succes!\n" RESET);
    printf("  Pacient: " CYAN "%s" RESET " | Medic: " CYAN "%s" RESET
           " | Data: " YELLOW "%s" RESET " | Ora: " YELLOW "%s" RESET "\n",
           pacient, medici[alegere].nume, data, ora);
    asteaptaenter();
}

void afisareProgramari()
{
    printHeader("LISTA PROGRAMARI");

    if (nr_programari == 0)
    {
        printf(RED "  Nu exista programari inregistrate.\n" RESET);
        asteaptaenter();
        return;
    }
    printf(BOLD "  %-20s %-15s %-15s %-10s\n" RESET, "Pacient", "Medic", "Data", "Ora");
    printSeparator();
    for (int i = 0; i < nr_programari; i++)
        printf("  " CYAN "%-20s" RESET " " CYAN "%-15s" RESET " " YELLOW "%-15s" RESET " " GREEN "%-10s" RESET "\n",
               programari[i].pacient, programari[i].medic,
               programari[i].data, programari[i].ora);
    asteaptaenter();
}

void anulareProgramare()
{
    printHeader("ANULARE PROGRAMARE");
    printf(DIM "  (0 = inapoi)\n\n" RESET);

    if (nr_programari == 0)
    {
        printf(RED "  Nu exista programari de anulat.\n" RESET);
        asteaptaenter();
        return;
    }
    printf(BOLD "  %-5s %-20s %-15s %-15s %-10s\n" RESET, "Nr.", "Pacient", "Medic", "Data", "Ora");
    printSeparator();
    for (int i = 0; i < nr_programari; i++)
        printf("  " MAGENTA "%-5d" RESET " " CYAN "%-20s" RESET " " CYAN "%-15s" RESET
               " " YELLOW "%-15s" RESET " " GREEN "%-10s" RESET "\n",
               i + 1, programari[i].pacient, programari[i].medic,
               programari[i].data, programari[i].ora);

    int nr;
    do {
        printf("\n  Numarul programarii de anulat (1-%d, 0 = inapoi): ", nr_programari);
        if (scanf("%d", &nr) != 1) { clearBuffer(); nr = -1; }
        if (nr == 0) { clearscreen(); return; }
        if (nr < 1 || nr > nr_programari)
            printf(RED "  X Numar invalid.\n" RESET);
    } while (nr < 1 || nr > nr_programari);

    for (int i = nr - 1; i < nr_programari - 1; i++)
        programari[i] = programari[i + 1];
    nr_programari--;
    salveazaProgramari();
    scrieRaportProgramari();
    printf(GREEN "\n  OK Programarea a fost anulata cu succes!\n" RESET);
    asteaptaenter();
}

void modificareProgramare()
{
    printHeader("MODIFICARE PROGRAMARE");
    printf(DIM "  (0 = inapoi)\n\n" RESET);

    if (nr_programari == 0)
    {
        printf(RED "  Nu exista programari de modificat.\n" RESET);
        asteaptaenter();
        return;
    }

    printf(BOLD "  %-5s %-20s %-15s %-15s %-10s\n" RESET, "Nr.", "Pacient", "Medic", "Data", "Ora");
    printSeparator();
    for (int i = 0; i < nr_programari; i++)
        printf("  " MAGENTA "%-5d" RESET " " CYAN "%-20s" RESET " " CYAN "%-15s" RESET
               " " YELLOW "%-15s" RESET " " GREEN "%-10s" RESET "\n",
               i + 1, programari[i].pacient, programari[i].medic,
               programari[i].data, programari[i].ora);

    int nr;
    do {
        printf("\n  Numarul programarii de modificat (1-%d, 0 = inapoi): ", nr_programari);
        if (scanf("%d", &nr) != 1) { clearBuffer(); nr = -1; }
        if (nr == 0) { clearscreen(); return; }
        if (nr < 1 || nr > nr_programari)
            printf(RED "  X Numar invalid.\n" RESET);
    } while (nr < 1 || nr > nr_programari);

    int idx = nr - 1;
    int idxMedic = -1;
    for (int i = 0; i < nr_medici; i++)
        if (strcmp(medici[i].nume, programari[idx].medic) == 0)
        {
            idxMedic = i;
            break;
        }

    int continua = 1;
    while (continua)
    {
        clearscreen();
        printf("\n");
        printf(BOLD CYAN "  +======================================+\n");
        printf(BOLD CYAN "  |  " BOLD WHITE "%-36s" CYAN "  |\n", "MODIFICARE PROGRAMARE");
        printf(BOLD CYAN "  +======================================+\n" RESET "\n");

        printf("  Programarea selectata:\n");
        printf("  Pacient : " CYAN "%s" RESET "\n", programari[idx].pacient);
        printf("  Medic   : " CYAN "%s" RESET "\n", programari[idx].medic);
        printf("  Data    : " YELLOW "%s" RESET "\n", programari[idx].data);
        printf("  Ora     : " GREEN "%s" RESET "\n\n", programari[idx].ora);

        printSeparator();
        printf("  " MAGENTA BOLD "1." RESET " Modifica pacient\n");
        printf("  " MAGENTA BOLD "2." RESET " Modifica medic\n");
        printf("  " MAGENTA BOLD "3." RESET " Modifica data\n");
        printf("  " MAGENTA BOLD "4." RESET " Modifica ora\n");
        printf("  " GREEN  BOLD "5." RESET " Salveaza si iesi\n");
        printf("  " RED    BOLD "0." RESET " Iesi fara salvare\n");
        printSeparator();

        int opt;
        printf("  Optiune: ");
        if (scanf("%d", &opt) != 1) { clearBuffer(); continue; }

        char tmp[50];

        switch (opt)
        {
            case 1:
                clearBuffer();
                printf("\n  Nume curent: " CYAN "%s" RESET "\n", programari[idx].pacient);
                do {
                    printf("  Noul nume (litere/spatiu, 0 = anuleaza): ");
                    citesteNumeCuSpatiu(tmp, sizeof(tmp));
                    if (strcmp(tmp, "0") == 0) break;
                    if (!esteNumeValid(tmp))
                        printf(RED "  X Nume invalid.\n" RESET);
                } while (!esteNumeValid(tmp));
                if (strcmp(tmp, "0") != 0)
                {
                    strcpy(programari[idx].pacient, tmp);
                    printf(GREEN "  OK Pacient actualizat.\n" RESET);
                }
                break;

            case 2:
                printf("\n  Medic curent: " CYAN "%s" RESET "\n\n", programari[idx].medic);
                for (int i = 0; i < nr_medici; i++)
                    printf("  " MAGENTA "%2d." RESET " " CYAN "%-15s" RESET " - " YELLOW "%-15s" RESET
                           " - " GREEN "%s" RESET "\n",
                           i + 1, medici[i].nume, medici[i].specialitate, medici[i].program);

                int alegere;
                do {
                    printf("\n  Noul medic (1-%d, 0 = anuleaza): ", nr_medici);
                    if (scanf("%d", &alegere) != 1) { clearBuffer(); alegere = -1; }
                    if (alegere == 0) break;
                    if (alegere < 1 || alegere > nr_medici)
                        printf(RED "  X Alegere invalida.\n" RESET);
                } while (alegere < 1 || alegere > nr_medici);

                if (alegere != 0)
                {
                    idxMedic = alegere - 1;
                    strcpy(programari[idx].medic, medici[idxMedic].nume);
                    printf(GREEN "  OK Medic actualizat: %s\n" RESET, medici[idxMedic].nume);
                    if (!esteOraInProgram(programari[idx].ora, medici[idxMedic].program))
                        printf(YELLOW "  ! Atentie: ora actuala nu se incadreaza in programul noului medic (%s).\n" RESET,
                               medici[idxMedic].program);
                }
                break;

            case 3:
                printf("\n  Data curenta: " YELLOW "%s" RESET "\n", programari[idx].data);
                do {
                    printf("  Noua data (zz-ll-aaaa, 0 = anuleaza): ");
                    scanf("%19s", tmp);
                    if (strcmp(tmp, "0") == 0) break;
                    if (!esteDataValida(tmp))
                        printf(RED "  X Data invalida sau in trecut.\n" RESET);
                } while (!esteDataValida(tmp));
                if (strcmp(tmp, "0") != 0)
                {
                    strcpy(programari[idx].data, tmp);
                    printf(GREEN "  OK Data actualizata.\n" RESET);
                }
                break;

            case 4:
                printf("\n  Ora curenta: " GREEN "%s" RESET "\n", programari[idx].ora);
                if (idxMedic >= 0)
                    printf("  Program medic " CYAN "%s" RESET ": " GREEN "%s" RESET "\n",
                           medici[idxMedic].nume, medici[idxMedic].program);
                else
                    printf(YELLOW "  ! Medicul nu a fost gasit in lista curenta.\n" RESET);

                do {
                    printf("  Noua ora (hh:mm, 0 = anuleaza): ");
                    scanf("%9s", tmp);
                    if (strcmp(tmp, "0") == 0) break;
                    if (!esteOraValida(tmp))
                        printf(RED "  X Ora invalida.\n" RESET);
                    else if (idxMedic >= 0 && !esteOraInProgram(tmp, medici[idxMedic].program))
                        printf(RED "  X Ora %s este in afara programului (%s).\n" RESET,
                               tmp, medici[idxMedic].program);
                } while (!esteOraValida(tmp) ||
                         (idxMedic >= 0 && !esteOraInProgram(tmp, medici[idxMedic].program)));

                if (strcmp(tmp, "0") != 0)
                {
                    strcpy(programari[idx].ora, tmp);
                    printf(GREEN "  OK Ora actualizata.\n" RESET);
                }
                break;

            case 5:
                salveazaProgramari();
                scrieRaportProgramari();
                printf(GREEN "\n  OK Modificarile au fost salvate cu succes!\n" RESET);
                asteaptaenter();
                continua = 0;
                break;

            case 0:
                incarcaProgramari();
                clearscreen();
                printf(YELLOW "  Modificarile nu au fost salvate.\n" RESET);
                asteaptaenter();
                continua = 0;
                break;

            default:
                printf(RED "  X Optiune invalida.\n" RESET);
                break;
        }

        if (continua && opt >= 1 && opt <= 4)
        {
            printf("\n" DIM "  Apasa ENTER pentru a continua..." RESET);
            clearBuffer();
            getchar();
        }
    }
}

void submeniuMedici()
{
    int opt;
    do {
        clearscreen();
        printf("\n");
        printf(BOLD BLUE "  +======================================+\n");
        printf(BOLD BLUE "  |  " BOLD WHITE "%-36s" BLUE "  |\n", "GESTIONARE MEDICI");
        printf(BOLD BLUE "  +======================================+\n" RESET "\n");

        printf("  " CYAN BOLD "1." RESET " Afisare toti medicii\n");
        printf("  " CYAN BOLD "2." RESET " Cautare dupa nume\n");
        printf("  " CYAN BOLD "3." RESET " Cautare dupa specialitate\n");
        printf("  " CYAN BOLD "4." RESET " Cautare dupa program\n");
        printf("  " GREEN BOLD "5." RESET " Adaugare medic\n");
        printf("  " RED   BOLD "6." RESET " Stergere medic\n");
        printf("  " DIM   BOLD "0." RESET " Inapoi la meniu principal\n");
        printf("\n");
        printSeparator();
        printf("  Optiune: ");
        if (scanf("%d", &opt) != 1) { clearBuffer(); opt = -1; }

        switch (opt)
        {
            case 1: afisaremedici();          break;
            case 2: cautamedicnume();         break;
            case 3: cautamedicspecialitate(); break;
            case 4: cautamedicprogram();      break;
            case 5: adaugaMedic();            break;
            case 6: stergeMedic();            break;
            case 0: clearscreen(); break;
            default:
                clearscreen();
                printf(RED "  Optiune invalida!\n\n" RESET);
                break;
        }
    } while (opt != 0);
}

void submeniuProgramari()
{
    int opt;
    do {
        clearscreen();
        printf("\n");
        printf(BOLD MAGENTA "  +======================================+\n");
        printf(BOLD MAGENTA "  |  " BOLD WHITE "%-36s" MAGENTA "  |\n", "GESTIONARE PROGRAMARI");
        printf(BOLD MAGENTA "  +======================================+\n" RESET "\n");

        printf("  " CYAN   BOLD "1." RESET " Afisare programari\n");
        printf("  " GREEN  BOLD "2." RESET " Adauga programare\n");
        printf("  " YELLOW BOLD "3." RESET " Modifica programare\n");
        printf("  " RED    BOLD "4." RESET " Anuleaza programare\n");
        printf("  " DIM    BOLD "0." RESET " Inapoi la meniu principal\n");
        printf("\n");
        printSeparator();
        printf("  Optiune: ");
        if (scanf("%d", &opt) != 1) { clearBuffer(); opt = -1; }

        switch (opt)
        {
            case 1: afisareProgramari();      break;
            case 2: programareConsultatie();  break;
            case 3: modificareProgramare();   break;
            case 4: anulareProgramare();      break;
            case 0: clearscreen(); break;
            default:
                clearscreen();
                printf(RED "  Optiune invalida!\n\n" RESET);
                break;
        }
    } while (opt != 0);
}

void meniuPrincipal()
{
    clearscreen();
    printf("\n");
    printf(BOLD GREEN "  +======================================+\n");
    printf(BOLD GREEN "  |                                      |\n");
    printf(BOLD GREEN "  |   " BOLD WHITE "SISTEM PROGRAMARI MEDICALE" GREEN "        |\n");
    printf(BOLD GREEN "  |                                      |\n");
    printf(BOLD GREEN "  +======================================+\n" RESET "\n");

    printf("  " BLUE   BOLD "1." RESET " " BOLD "Medici" RESET "         " DIM "(afisare, cautare, adaugare, stergere)" RESET "\n");
    printf("  " MAGENTA BOLD "2." RESET " " BOLD "Programari" RESET "     " DIM "(afisare, adaugare, modificare, anulare)" RESET "\n");
    printf("  " RED    BOLD "0." RESET " Iesire\n");
    printf("\n");
    printSeparator();
    printf("  Optiune: ");
}

int main()
{
    incarcaMedici();
    incarcaProgramari();

    int opt;
    do
    {
        meniuPrincipal();
        if (scanf("%d", &opt) != 1) { clearBuffer(); opt = -1; }

        switch (opt)
        {
            case 1: submeniuMedici();     break;
            case 2: submeniuProgramari(); break;
            case 0:
                clearscreen();
                printf(GREEN "  Program inchis. La revedere!\n\n" RESET);
                break;
            default:
                clearscreen();
                printf(RED "  Optiune invalida! Incearca din nou.\n\n" RESET);
                break;
        }
    } while (opt != 0);

    return 0;
}
