#include <iostream>
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;
//------------------------STRUKTURY------------------------------------//

struct zaba
{
    int y;                  //pozycja y zaby
    int x;                  //pozycja x zaby
    int ostatni_wynik;
    int rekord;
};

struct dane
{
    int Y_PLANSZY;
    int X_PLANSZY;
    int ilosc_aut;
    int predkosc_aut;
};

struct samochod
{
    int wiersz;                 //pozycja y auta
    int kolumna;                //pozycja x auta
    int opoznienie;             //predkosc auta
    int opoznienie_domyslne;    //do przyspieszania i zwalniania
    bool kierunek;              //0=lewo 1=prawo
    int przyjazny;              //0=agresywne; 1=przyjazne 2=neutralne 3=pomocny
    int licznik;                //do opozniania
    int dlugosc;
};

struct wrog
{
    int y;
    int x;
    int y_cel;
    int x_cel;
    int predkosc;
    int licznik;
    int timer;
    chtype znak;
};

//---------------------------------------------------------------------//


//-----------------------OBSLUGA PLIKOW--------------------------------//

void wczytaj_dane(const char *nazwa_pliku, dane* stale)
{
    FILE *file_pointer;
    file_pointer=fopen(nazwa_pliku, "r");
    if(file_pointer==NULL)
    {
        mvprintw(10,10, "BRAK PLIKU KONFIGURACYJNEGO GRY, GRASZ NA DOMYSLNYCH USTAWIENIACH");
        napms(1000);
        refresh();
        return;
    }
    char linia[100];
    while (fgets(linia, sizeof(linia), file_pointer))
    {
        int i = 0;
        while (linia[i] != '\0')
        {
            if (linia[i] == '\n' || linia[i] == '\r')
            {
                linia[i] = '\0';
                break;
            }
            i++;
        }
        sscanf(linia, "WYSOKOSC PLANSZY %d", &stale->Y_PLANSZY);
        sscanf(linia, "SZEROKOSC PLANSZY %d", &stale->X_PLANSZY);
        sscanf(linia, "PREDKOSC AUT %d", &stale->predkosc_aut);
    }

    fclose(file_pointer);
}

void zapisz_dane(const char *nazwa_pliku, dane n_ustawienia)
{
    FILE *file_pointer;
    file_pointer=fopen(nazwa_pliku, "w");
    if (file_pointer == NULL)
    {
        mvprintw(12, 5, "BLAD ZAPISU DO PLIKU KONFIGURACYJNEGO");
        napms(2000);
        refresh();
        return;
    }
    fprintf(file_pointer, "WYSOKOSC PLANSZY %d\n", n_ustawienia.Y_PLANSZY);
    fprintf(file_pointer, "SZEROKOSC PLANSZY %d\n", n_ustawienia.X_PLANSZY);
    fprintf(file_pointer, "PREDKOSC AUT %d\n", n_ustawienia.predkosc_aut);
    fclose(file_pointer);
}

void wczytaj_rekord(zaba *gracz)
{
    FILE *file_pointer;
    file_pointer=fopen("rekord.txt", "r");
    if (file_pointer == NULL)
    {
        mvprintw(12, 5, "BLAD ODCZYTU DO PLIKU KONFIGURACYJNEGO");
        napms(2000);
        refresh();
        return;
    }
    char linia[100];
    while (fgets(linia, sizeof(linia), file_pointer))
    {
        int i = 0;
        while (linia[i] != '\0')
        {
            if (linia[i] == '\n' || linia[i] == '\r')
            {
                linia[i] = '\0';
                break;
            }
            i++;
        }
        sscanf(linia, "REKORD %d", &gracz->rekord);
    }
}

void zapisz_rekord(zaba gracz)
{
    FILE *file_pointer;
    file_pointer=fopen("rekord.txt", "w");
    if (file_pointer == NULL)
    {
        mvprintw(12, 5, "BLAD ZAPISU DO PLIKU KONFIGURACYJNEGO");
        napms(2000);
        refresh();
        return;
    }
    fprintf(file_pointer, "REKORD %d\n", gracz.rekord);
    fclose(file_pointer);
}

//---------------------------------------------------------------------//


//-------------------------KOLORY--------------------------------------//

void kolory()
{
    init_color(11, 150,88,3);           //brazowy
    init_color(12, 180, 180, 180);      //szary

    init_pair(1, COLOR_BLACK, COLOR_WHITE);     //kolor dla menu
    init_pair(2, COLOR_BLUE, COLOR_BLUE);       //start/meta
    init_pair(3, COLOR_GREEN, COLOR_GREEN);     //zaba
    init_pair(4, 11, 11);                       //drzewa
    init_pair(5, 12, 12);                       //chodnik

    init_pair(6, COLOR_RED, COLOR_RED);         //wrogie auta
    init_pair(7, COLOR_YELLOW, COLOR_YELLOW);   //przyjazne auta
    init_pair(8, COLOR_MAGENTA, COLOR_MAGENTA); //neutralne auta
    init_pair(9, COLOR_CYAN, COLOR_CYAN);       //auto przenoszace
    init_pair(10, COLOR_WHITE, COLOR_WHITE);     //czapla
}

//---------------------------------------------------------------------//


//--------------------------MENU---------------------------------------//

void ruch_menu(int ruch, int& y, int& operacja)
{
    curs_set(0);
    operacja=0;
    mvprintw(y,1," ");
    if(ruch==KEY_UP)
    {
        y=(y==1)?y:y-1;
    }
    else if(ruch==KEY_DOWN)
    {
        y=(y==3)?y:y+1;
    }
    attron(COLOR_PAIR(1));
    mvprintw(y,1,">");
    attroff(COLOR_PAIR(1));
    if(ruch==KEY_ENTER || ruch=='\n' || ruch=='\r')
    {
        if(y==1)
        {
            operacja=1;
        }
        else if(y==2)
        {
            operacja=2;
        }
        else if(y==3)
        {
            operacja=3;
        }
    }
    return;
}

void wyswietl_menu(int y, char imie[])
{
    curs_set(0);
    mvprintw(0,3,"GRACZ: %s",imie);
    mvprintw(1,3,"GRAJ");
    mvprintw(2,3,"STATYSTYKI");
    mvprintw(3,3,"USTAWIENIA");
    attron(COLOR_PAIR(1));
    mvprintw(y,1,">");
    attroff(COLOR_PAIR(1));

}

void statystyki(zaba gracz)
{
    clear();
    mvprintw(5, 5, "STATYSTYKI:");
    mvprintw(7, 5, "Ostatni wynik: %d", gracz.ostatni_wynik);
    mvprintw(8, 5, "Najlepszy wynik: %d",gracz.rekord);
    mvprintw(10, 5, "Nacisnij dowolny klawisz, aby wrocic do menu.");
    refresh();
    if(getch()==27)
        return;
}

void ustawienia(const char* nazwa_pliku)
{
    int max_y, max_x;
    getmaxyx(stdscr,max_y,max_x);
    dane n_ust;
    clear();
    mvprintw(5, 5, "USTAWIENIA - ZMIANA WARTOSCI:");

    mvprintw(7, 5, "Podaj WYSOKOSC PLANSZY: ");
    echo();
    scanw("%d", &n_ust.Y_PLANSZY);
    if(n_ust.Y_PLANSZY<3)
    {
        mvprintw(7, 40, "Podana wartosc jest za mala, ustawiam minimalna wysokosc");
        n_ust.Y_PLANSZY=3;
    }
    else if(n_ust.Y_PLANSZY>max_y)
    {
        mvprintw(7, 40, "Podana wartosc jest za duza, ustawiam minimalna wysokosc");
        n_ust.Y_PLANSZY=max_y;
    }

    mvprintw(8, 5, "Podaj SZEROKOSC PLANSZY: ");
    scanw("%d", &n_ust.X_PLANSZY);
    if(n_ust.X_PLANSZY<3)
    {
        mvprintw(8, 40, "Podana wartosc jest za mala, ustawiam minimalna szerokosc");
        n_ust.X_PLANSZY=3;
    }
    else if(n_ust.X_PLANSZY>max_x)
    {
        mvprintw(8, 40, "Podana wartosc jest za duza, ustawiam minimalna wysokosc");
        n_ust.X_PLANSZY=max_x;
    }

    mvprintw(9, 5, "Podaj PREDKOSC AUT: ");
    scanw("%d", &n_ust.predkosc_aut);
    noecho();

    zapisz_dane(nazwa_pliku, n_ust);

    mvprintw(12, 5, "USTAWIENIA ZOSTALY ZAPISANE!");
    refresh();
    napms(2000);
}

//---------------------------------------------------------------------//


//----------------------FUNKCJE TWORZACE-------------------------------//

void stworz_auto(samochod* auta[], int pasy[], dane stale)
{
    bool czy_ist_pomoc=0;
    for(int i=0, j=0; j<stale.Y_PLANSZY && i<stale.ilosc_aut; j++)
    {
        if(pasy[j]!=0)
        {
            auta[i] = new samochod;
            auta[i]->opoznienie = (rand()%21+7);
            auta[i]->opoznienie_domyslne=auta[i]->opoznienie;
            auta[i]->wiersz = j+1;
            auta[i]->kolumna= rand()%stale.X_PLANSZY;
            auta[i]->kierunek=(pasy[j]==-1)?0:1;
            auta[i]->licznik=0;
            auta[i]->dlugosc=rand()%(stale.X_PLANSZY/2)+1;
            auta[i]->przyjazny=rand()%3;
            if(auta[i]->przyjazny==1)
            {
                if(rand()%2==1 && czy_ist_pomoc==0)
                {
                    auta[i]->przyjazny=3;
                    czy_ist_pomoc=1;
                }
            }
            i++;
        }
    }
}

void mapuj_pasy(int pasy[], dane stale)
{
    int pasy_wzor[5]= {1,1,-1,-1,0};
    pasy[stale.Y_PLANSZY-1]=0;
    pasy[stale.Y_PLANSZY-2]=0;
    for(int i=stale.Y_PLANSZY-2; i>0; i--)
    {
        pasy[i]=pasy_wzor[i%5];
    }
}

void modyfikuj_auto(samochod* auta, dane stale)
{
    auta->dlugosc=rand()%(stale.X_PLANSZY/2)+1;
    auta->opoznienie=(rand()%21+7);
    auta->opoznienie_domyslne=auta->opoznienie;
    auta->przyjazny=rand()%4;
}

void stworz_czaple(wrog*& czapla, dane stale)
{
    czapla=new wrog;
    czapla->predkosc=50;
    czapla->y=stale.Y_PLANSZY-1;
    czapla->x=stale.X_PLANSZY-1;
    czapla->y_cel=stale.Y_PLANSZY-1;
    czapla->x_cel=stale.X_PLANSZY-1;
    czapla->timer=7;
    czapla->licznik=0;
    czapla->znak='-';
}

//---------------------------------------------------------------------//


//---------------------FUNKCJE WYSWIETLAJACE---------------------------//

void rys_plansze(dane stale, char imie[], int pasy[])
{
    for(int y=0; y<stale.Y_PLANSZY; y++)
    {
        for(int x=0; x<stale.X_PLANSZY; x++)
        {
            if(pasy[y-1]==0)
            {
                attron(COLOR_PAIR(5));
                mvprintw(y,x,"-");
                attroff(COLOR_PAIR(5));
            }
            else if(y==stale.Y_PLANSZY-1 || y==0)
            {
                attron(COLOR_PAIR(2));
                mvprintw(y,x,"-");
                attroff(COLOR_PAIR(2));
            }
            else
                mvprintw(y,x,"-");
        }
    }
    mvprintw(0,stale.X_PLANSZY+10,"GRACZ: %s", imie);
    mvprintw(stale.Y_PLANSZY+5, 0, "Dominik Szwed s203731");
    refresh();
}

void wyswietl_kolor(samochod* auta, int i, int kolor)
{
    attron(COLOR_PAIR(kolor));
    mvprintw(auta->wiersz, auta->kolumna+i,"#");
    attroff(COLOR_PAIR(kolor));
}

void wyswietl_auto(samochod* auta, dane stale)
{
    for(int i=0; i<auta->dlugosc; i++)
    {
        if(auta->kierunek==0)
        {
            if(auta->kolumna+i>=0 && auta->kolumna+i<stale.X_PLANSZY)
            {
                if(auta->przyjazny==0)
                {
                    wyswietl_kolor(auta,i,6);

                }
                else if(auta->przyjazny==1)
                {
                    wyswietl_kolor(auta,i,7);

                }
                else if(auta->przyjazny==2)
                {
                    wyswietl_kolor(auta,i,8);

                }
                else if(auta->przyjazny==3)
                {
                    wyswietl_kolor(auta,i,9);
                }
            }

        }
        else
        {
            if(auta->kolumna-i>=0 && auta->kolumna-i<stale.X_PLANSZY)
            {
                if(auta->przyjazny==0)
                {
                    wyswietl_kolor(auta,-i,6);
                }
                else if(auta->przyjazny==1)
                {
                    wyswietl_kolor(auta,-i,7);
                }
                else if(auta->przyjazny==2)
                {
                    wyswietl_kolor(auta,-i,8);
                }
                else if(auta->przyjazny==3)
                {
                    wyswietl_kolor(auta,-i,9);
                }
            }
        }
    }
}

void wyswietl_w_auta(dane stale, samochod* auta[])
{
    for(int i=0; i<stale.ilosc_aut; i++)
    {
        wyswietl_auto(auta[i], stale);
    }
}

void wyswietl_zabe(zaba gracz)
{
    attron(COLOR_PAIR(3));
    mvprintw(gracz.y,gracz.x,"^");
    attroff(COLOR_PAIR(3));
}

void rys_przeszkody(int pasy[], dane stale)
{
    attron(COLOR_PAIR(4));
    for(int i=1; i<stale.Y_PLANSZY-1; i++)
    {
        if(pasy[i]==0)
        {
            int dl_przeszkody=rand()%(stale.X_PLANSZY/3);
            if(dl_przeszkody>0)
            {
                int ile_przeszkod=rand()%2+1;
                if(ile_przeszkod==1)
                {
                    int mod=stale.X_PLANSZY-dl_przeszkody;
                    int pocz_przeszkody=rand()%mod-1;
                    for(int j=0; j<dl_przeszkody; j++)
                    {
                        mvprintw(i+1,pocz_przeszkody+j,"|");
                    }
                }
                else if(ile_przeszkod==2)
                {
                    int mod=stale.X_PLANSZY-dl_przeszkody;
                    int pocz_przeszkody=(rand()%mod)/2;
                    for(int j=0; j<dl_przeszkody; j++)
                    {
                        mvprintw(i+1,pocz_przeszkody+j,"|");
                        mvprintw(i+1,stale.X_PLANSZY-pocz_przeszkody-j-1,"|");
                    }
                }
            }
        }
    }
    attroff(COLOR_PAIR(4));
    refresh();
}

void legenda()
{
    clear();
    attron(COLOR_PAIR(3));
    mvprintw(1,1,"#");
    attroff(COLOR_PAIR(3));
    mvprintw(1,3,"ZABA");

    attron(COLOR_PAIR(4));
    mvprintw(2,1,"#");
    attroff(COLOR_PAIR(4));
    mvprintw(2,3,"PRZESZKODY");

    attron(COLOR_PAIR(5));
    mvprintw(3,1,"#");
    attroff(COLOR_PAIR(5));
    mvprintw(3,3,"CHODNIK");

    attron(COLOR_PAIR(8));
    mvprintw(4,1,"#");
    attroff(COLOR_PAIR(8));
    mvprintw(4,3,"AUTA NEUTRALNE");

    attron(COLOR_PAIR(6));
    mvprintw(5,1,"#");
    attroff(COLOR_PAIR(6));
    mvprintw(5,3,"AUTA WROGIE");

    attron(COLOR_PAIR(7));
    mvprintw(6,1,"#");
    attroff(COLOR_PAIR(7));
    mvprintw(6,3,"AUTA PRZYJAZNE");

    attron(COLOR_PAIR(9));
    mvprintw(7,1,"#");
    attroff(COLOR_PAIR(9));
    mvprintw(7,3,"AUTO PRZENOSZACE");

    attron(COLOR_PAIR(10));
    mvprintw(8,1,"#");
    attroff(COLOR_PAIR(10));
    mvprintw(8,3,"CZAPLA");

    refresh();
    int ch=getch();
    while(ch!=KEY_ENTER && ch!='\n' && ch!='\r')
    {
        ch=getch();
    }
    clear();
    return;
}

int timer(time_t start_time, dane stale, zaba gracz)
{
    time_t akt_time=time(NULL);
    double d_time=difftime(akt_time,start_time);
    mvprintw(2,stale.X_PLANSZY+10,"Czas: %.0f s", d_time);
    move(gracz.y,gracz.x);
    refresh();
    return d_time;
}

void wyswietl_czaple(wrog* czapla)
{
    attron(COLOR_PAIR(10));
    mvprintw(czapla->y, czapla->x, "&");
    attroff(COLOR_PAIR(10));
}

void usun_czaple(wrog* czaple, int pasy[], dane stale)
{
    if(czaple->znak=='|')
    {
        attron(COLOR_PAIR(4));
        mvprintw(czaple->y, czaple->x, "|");
        attroff(COLOR_PAIR(4));
    }
    else if(czaple->znak=='-' && pasy[czaple->y-1]==0)
    {
        attron(COLOR_PAIR(5));
        mvprintw(czaple->y,czaple->x,"-");
        attroff(COLOR_PAIR(5));
    }
    else if(czaple->y==stale.Y_PLANSZY-1)
    {
        attron(COLOR_PAIR(2));
        mvprintw(czaple->y, czaple->x, "-");
        attroff(COLOR_PAIR(2));
    }
    else
    {
        mvprintw(czaple->y, czaple->x, "-");
    }
}

void wyswietl_poczatek_gry(dane stale, char imie[], int pasy[], zaba gracz, wrog* czapla, samochod* auta[])
{
    legenda();
    rys_plansze(stale, imie, pasy);
    rys_przeszkody(pasy,stale);
    wyswietl_zabe(gracz);
    wyswietl_w_auta(stale, auta);
    wyswietl_czaple(czapla);
}

//---------------------------------------------------------------------//


//-----------------FUKCJE ODPOWIADAJACE ZA RUCH------------------------//

void usun_auto(samochod* auta, dane stale)
{
    for(int i=0; i<auta->dlugosc; i++)
    {
        if(auta->kierunek==0)
        {
            if(auta->kolumna+i>=0 && auta->kolumna+i<stale.X_PLANSZY)
                mvprintw(auta->wiersz, auta->kolumna+i,"-");
        }
        else
        {
            if(auta->kolumna-i>=0 && auta->kolumna-i<stale.X_PLANSZY)
                mvprintw(auta->wiersz, auta->kolumna-i,"-");
        }
    }
}

void ruch_zaby(zaba& gracz, int ch, dane stale, int pasy[])
{
    if(pasy[gracz.y-1]==0)
    {
        attron(COLOR_PAIR(5));
        mvprintw(gracz.y,gracz.x,"-");
        attroff(COLOR_PAIR(5));
    }
    else if(gracz.y==0 || gracz.y==stale.Y_PLANSZY-1)
    {
        attron(COLOR_PAIR(2));
        mvprintw(gracz.y,gracz.x,"-");
        attroff(COLOR_PAIR(2));
    }
    else
        mvprintw(gracz.y,gracz.x,"-");
    if(ch==KEY_UP)
    {
        char czy_przeszkoda=mvinch(gracz.y-1,gracz.x);
        if(czy_przeszkoda!='|')
            gracz.y=(gracz.y<=0)?gracz.y: gracz.y-1;
    }
    else if(ch==KEY_DOWN)
    {
        char czy_przeszkoda=mvinch(gracz.y+1,gracz.x);
        if(czy_przeszkoda!='|')
            gracz.y=(gracz.y>=stale.Y_PLANSZY-1)?gracz.y: gracz.y+1;
    }
    else if(ch==KEY_LEFT)
    {
        char czy_przeszkoda=mvinch(gracz.y,gracz.x-1);
        if(czy_przeszkoda!='|')
            gracz.x=(gracz.x<=0)?gracz.x: gracz.x-1;
    }
    else if(ch==KEY_RIGHT)
    {
        char czy_przeszkoda=mvinch(gracz.y,gracz.x+1);
        if(czy_przeszkoda!='|')
            gracz.x=(gracz.x>=stale.X_PLANSZY-1)?gracz.x: gracz.x+1;;
    }
    wyswietl_zabe(gracz);
    refresh();
}

void ruch_auta_wrogiego(samochod* auta, zaba gracz)
{
    auta->opoznienie=auta->opoznienie_domyslne;
    int przyspieszenie=2;
    if(gracz.y-auta->wiersz ==1 || gracz.y-auta->wiersz==0)
    {
        auta->opoznienie-=przyspieszenie;
    }
    else if(auta->opoznienie_domyslne!=auta->opoznienie)
    {
        auta->opoznienie=auta->opoznienie_domyslne;
    }
}

void ruch_auta_pomocnego(samochod* auta[], zaba& gracz, int pasy[], int ch, dane stale)
{
    for(int i=0; i<stale.ilosc_aut; i++)
    {
        if(auta[i]->przyjazny==3)
        {
            if((gracz.y-auta[i]->wiersz==0 && gracz.x-auta[i]->kolumna==1 && auta[i]->kierunek==1)||(gracz.y-auta[i]->wiersz==0 && gracz.x-auta[i]->kolumna==-1 && auta[i]->kierunek==0))
            {

                int pas_bezpieczny=gracz.y-1;
                while(pasy[pas_bezpieczny]!=0 && pas_bezpieczny>0)
                {
                    pas_bezpieczny--;
                }
                mvprintw(gracz.y,gracz.x,"-");
                if(pas_bezpieczny==0)
                {
                    gracz.y=0;
                }
                else
                {
                    gracz.y=pas_bezpieczny+1;
                }
            }
        }
    }
}

void ruch_auta_przyjaznego(samochod* auta, zaba gracz)
{
    int spowolnienie=10000;

    if((gracz.y-auta->wiersz==0 || gracz.y-auta->wiersz==1) && ((auta->kierunek==0 && (auta->kolumna-gracz.x==2 || auta->kolumna-gracz.x==1)) || (auta->kierunek==1 && (gracz.x-auta->kolumna==2 || gracz.x-auta->kolumna==1))))
    {
        auta->opoznienie+=spowolnienie;
    }
    else if(auta->opoznienie_domyslne!=auta->opoznienie)
    {
        auta->opoznienie=auta->opoznienie_domyslne;
    }
}

void ruch_auta(samochod* auta[], zaba& gracz, dane stale, int pasy[], int ch)
{
    for(int i=0; i<stale.ilosc_aut; i++)
    {
        if(auta[i]->przyjazny==0)
        {
            ruch_auta_wrogiego(auta[i],gracz);
        }
        else if(auta[i]->przyjazny==1 || auta[i]->przyjazny==3)
        {
            ruch_auta_przyjaznego(auta[i],gracz);
        }

        auta[i]->licznik++;
        if(auta[i]->licznik>=(auta[i]->opoznienie)/stale.predkosc_aut)
        {
            usun_auto(auta[i], stale);

            if(auta[i]->kierunek==1)
            {
                auta[i]->kolumna++;
                if(auta[i]->kolumna-auta[i]->dlugosc>=stale.X_PLANSZY)
                {
                    bool czy_mod=rand()%2;
                    if(czy_mod)
                    {
                        modyfikuj_auto(auta[i],stale);
                    }
                    auta[i]->kolumna=-1;
                }
            }
            else
            {
                auta[i]->kolumna--;
                if(auta[i]->kolumna+auta[i]->dlugosc<0)
                {
                    bool czy_mod=rand()%2;
                    if(czy_mod)
                    {
                        modyfikuj_auto(auta[i],stale);
                    }
                    auta[i]->kolumna=stale.X_PLANSZY;
                }
            }
            wyswietl_auto(auta[i], stale);
            move(gracz.y,gracz.x);
            auta[i]->licznik=0;
        }
    }
    refresh();
}

void ruch_czapli(wrog* czapla, zaba gracz, int pasy[], dane stale)
{
    czapla->y_cel=gracz.y;
    czapla->x_cel=gracz.x;
    czapla->licznik++;
    if(czapla->licznik>=czapla->predkosc)
    {
        usun_czaple(czapla, pasy, stale);

        czapla->znak=mvinch(czapla->y, czapla->x);

        if(czapla->y_cel>czapla->y)
            czapla->y++;
        else if(czapla->y_cel<czapla->y)
            czapla->y--;
        if(czapla->x_cel>czapla->x)
            czapla->x++;
        else if(czapla->x_cel<czapla->x)
            czapla->x--;
        czapla->licznik=0;
    }
    wyswietl_czaple(czapla);
    refresh();
}

//---------------------------------------------------------------------//


//-------------------FUNKCJE KONCA GRY---------------------------------//

bool kolizja(zaba gracz, samochod* auta[], dane stale, wrog* czapla)
{
    if(czapla->y==gracz.y && czapla->x==gracz.x)
        return true;
    for(int i=0; i<stale.ilosc_aut; i++)
    {
        for(int j=0; j<auta[i]->dlugosc; j++)
        {
            int x_sprawdzania;
            if(auta[i]->kierunek==1)
            {
                x_sprawdzania=auta[i]->kolumna-j;
            }
            else
            {
                x_sprawdzania=auta[i]->kolumna+j;
            }
            if(gracz.y==auta[i]->wiersz && gracz.x==x_sprawdzania)
            {
                return true;
            }
        }
    }
    return false;
}

bool wygrana(zaba gracz)
{
    if(gracz.y==0)
        return 1;
    return 0;
}

void wygrana_napis(zaba gracz, dane stale, double d_time, int wynik)
{
    clear();
    int x,y;
    getmaxyx(stdscr,y,x);
    x/=2;
    y/=2;
    mvprintw(y-1,x-10,"********************");
    mvprintw(y,x-10," ->   WYGRALES   <- ");
    mvprintw(y+1,x-10,"********************");
    mvprintw(y+5,x-10,"********************");
    mvprintw(y+6,x-10," -> Twoj czas:   %.0f s", d_time);
    mvprintw(y+7,x-10," -> Twoj wynik: %d",wynik);
    mvprintw(y+8,x-10,"********************");
    curs_set(0);
    refresh();
    int ch=getch();
    while(ch!=KEY_ENTER && ch!='\n' && ch!='\r')
    {
        ch=getch();
    }
    clear();
    return;
}

void rys_przegrana(zaba gracz, double d_time, int wynik)
{
    clear();
    int x,y;
    getmaxyx(stdscr,y,x);
    x/=2;
    y/=2;
    mvprintw(y-1,x-10,"********************");
    mvprintw(y,x-10," ->  PRZEGRALES  <- ");
    mvprintw(y+1,x-10,"********************");

    mvprintw(y+5,x-10,"********************");
    mvprintw(y+6,x-10," -> Twoj czas:   %.0f s", d_time);
    mvprintw(y+7,x-10," -> Twoj wynik:  %d",wynik);
    mvprintw(y+8,x-10,"********************");
    curs_set(0);
    refresh();
    nodelay(stdscr,FALSE);
    while(getch()!=27)
    {

    }
    return;
}

void this_is_the_end(zaba gracz, dane stale, int wynik)
{
    clear();
    int x,y;
    getmaxyx(stdscr,y,x);
    x/=2;
    y/=2;
    mvprintw(y-1,x-10,"*********************");
    mvprintw(y,x-10,  " ->BRAWO UKONCZYLES<-");
    mvprintw(y+1,x-10," -> GRE CROSSYROAD <-");
    mvprintw(y+2,x-10,"*********************");
    mvprintw(y+5,x-10,"*************************");
    mvprintw(y+7,x-10,"->Twoj wynik koncowy: %d", wynik);
    mvprintw(y+9,x-10,"*************************");
    mvprintw(y+11,x-10,"AUTOR: DOMINIK SZWED s203731");
    curs_set(0);
    refresh();
    int ch=getch();
    while(ch!=KEY_ENTER && ch!='\n' && ch!='\r')
    {
        ch=getch();
    }
    clear();
    return;
}
//---------------------------------------------------------------------//


//------------------------FUNKCJE LICZACE------------------------------//

int ile_pasow(int pasy[], dane stale)
{
    int ile=0;
    for(int i=0; i<stale.Y_PLANSZY; i++)
    {
        if(pasy[i]!=0)
            ile++;
    }
    return --ile;
}

int ranking(double d_time, int ile_ruchow, zaba gracz, dane stale)
{
    int wynik=10000;
    int pokonane_drogi= stale.Y_PLANSZY - gracz.y;
    int pkt_ujemne_ruch=0, pkt_ujemne_czas=0;

    if(pokonane_drogi<stale.Y_PLANSZY)
    {
        wynik/=2;
        wynik=wynik*pokonane_drogi/stale.Y_PLANSZY;
    }
    if(ile_ruchow>stale.Y_PLANSZY+3)
    {
        pkt_ujemne_ruch=175*(ile_ruchow-stale.Y_PLANSZY+3);
    }
    if(d_time>15.0)
    {
        pkt_ujemne_czas=95*(d_time-stale.Y_PLANSZY);
    }
    wynik=wynik-pkt_ujemne_czas-pkt_ujemne_ruch;
    if(gracz.y==stale.Y_PLANSZY-1)
        wynik=0;
    if(wynik<0)
        wynik=0;
    return wynik;
}

//---------------------------------------------------------------------//


//------------------------FUNKCJE USUWAJACE----------------------------//

void zwolnij_pamiec(dane stale, samochod* auta[], int pasy[], wrog* czapla)
{
    delete czapla;
    for(int i=0; i<stale.ilosc_aut; i++)
        delete auta[i];
    delete[] auta;
    delete[] pasy;
}

//---------------------------------------------------------------------//


//--------------------------ROZGRYWKA----------------------------------//
void rozgrywka(dane stale, zaba gracz, int& wynik, int pasy[], samochod* auta[], time_t start_time, int ile_ruchow, int ch, int last_ch, wrog* czapla, bool& czy_wygrana, bool czy_czapla)
{
    while(true)
    {
        ch=getch();
        ruch_auta(auta, gracz, stale, pasy, ch);

        //timer
        double d_time=timer(start_time,stale,gracz);

        if(ch!=ERR && ch!=last_ch)
        {
            if(ch=='q')
            {
                wynik=0;
                break;
            }
            if(ch==' ')
            {
                ruch_auta_pomocnego(auta,gracz,pasy,ch,stale);
            }
            napms(50);
            ile_ruchow++;
            ruch_zaby(gracz,ch, stale, pasy);
            last_ch=ch;
        }
        else if(ch==ERR)
        {
            last_ch=ERR;
        }
        if(kolizja(gracz, auta,stale, czapla))
        {
            wynik+=ranking(d_time,ile_ruchow,gracz,stale);
            rys_przegrana(gracz, d_time, wynik);
            break;
        }
        czy_wygrana=wygrana(gracz);
        if(czy_wygrana)
        {
            wynik=ranking(d_time,ile_ruchow,gracz,stale);
            wygrana_napis(gracz, stale, d_time, wynik);
            break;
        }
        if(d_time>=czapla->timer && czy_czapla==true)
        {
            ruch_czapli(czapla,gracz,pasy, stale);
        }
        napms(20);
    }
}

void gra(dane& stale, char imie[], zaba gracz, int& wynik, wrog* czapla)
{
    nodelay(stdscr,TRUE);
    int* pasy = new int[stale.Y_PLANSZY];
    mapuj_pasy(pasy, stale);

    gracz= {stale.Y_PLANSZY-1, stale.X_PLANSZY/2};
    stale.ilosc_aut=ile_pasow(pasy, stale);

    samochod** auta =new samochod* [stale.ilosc_aut];
    stworz_auto(auta, pasy, stale);
    stworz_czaple(czapla,stale);

    //wyswietlanie
    wyswietl_poczatek_gry(stale, imie, pasy, gracz, czapla, auta);

    int ch, last_ch=ERR, ile_ruchow=0, i=0;
    bool czy_wygrana=true, czy_czapla=false;

    while(i<3 && czy_wygrana==true)
    {
        time_t start_time=time(NULL);
        czy_czapla=(i==0)?false:true;
        stale.predkosc_aut+=2*i;
        wyswietl_poczatek_gry(stale, imie, pasy, gracz, czapla, auta);
        rozgrywka(stale, gracz, wynik, pasy, auta, start_time, ile_ruchow, ch, last_ch, czapla, czy_wygrana, czy_czapla);
        i++;
    }
    if(i==2)
        this_is_the_end(gracz,stale,wynik);
    zwolnij_pamiec(stale,auta,pasy, czapla);
}

int main()
{
    const char *n_pliku="config.txt";
    srand(time(NULL));
    initscr();
    raw();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    kolory();

    dane stale= {20,15,15,1};
    int ch, wynik=0;
    int y=1, operacja=0;

    mvprintw(5,5,"PODAJ SWOJE IMIE: ");
    refresh();
    char imie[50];
    int index=0;
    zaba gracz= {0,0,0};
    wrog* czapla;

    while(true)
    {
        ch=getch();
        if(ch==KEY_ENTER || ch=='\n' || ch=='\r')
        {
            imie[index]='\0';
            break;
        }
        if(index<49)
        {
            imie[index]=ch;
            mvaddch(5,25+index,ch);
            index++;
        }
        refresh();
    }

    while(true)
    {
        nodelay(stdscr,FALSE);
        clear();
        wyswietl_menu(y, imie);
        while(true)
        {
            ch=getch();
            if(ch!=ERR)
            {
                ruch_menu(ch, y, operacja);
                refresh();
            }
            if(operacja!=0)
                break;
        }
        if(operacja==1)
        {
            clear();

            wczytaj_dane(n_pliku, &stale);
            wczytaj_rekord(&gracz);

            gra(stale, imie, gracz, wynik, czapla);

            gracz.ostatni_wynik=wynik;

            if(gracz.ostatni_wynik>gracz.rekord)
                gracz.rekord=gracz.ostatni_wynik;

            zapisz_rekord(gracz);
        }
        else if(operacja==2)
        {
            wczytaj_rekord(&gracz);
            statystyki(gracz);
        }
        else if(operacja==3)
        {
            ustawienia(n_pliku);
        }

    }
    endwin();
    return 0;
}
