// implementaceBinarnihoStromu.cpp: Definuje vstupní bod pro aplikaci.
//
#include <cstdlib>
#include "implementaceBinarnihoStromu.h"
#include<stdio.h>
#include<stdlib.h>
#include <conio.h>
#include <time.h>


//kody klaves
#define key_p  112
#define key_l  108
#define key_plus 43
#define key_minus 45
#define ctrlk 11
#define ctrln 14
#define ctrlo 15
#define ctrlp 16
#define ctrlr 18
#define ctrls 19
#define ctrlt 20
#define ctrlu 21
#define ESC 27

//velikost mezery pri tisku a velikost pole
#define COUNT 10
#define MAX_POLE 1024

//kody zbarveni textu
#define CERVENA     "\x1b[31m"
#define ZELENA   "\x1b[32m"
#define ZLUTA  "\x1b[33m"
#define MODRA    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


//vraci vetsi z cisel
int max(int a, int b) {
	return (a > b) ? a : b;
}



//vytvoreni struktury binarniho stromu - jeden prvek obsahuje hodnotu a dve struktury stejneho typu --> rozvetvi se
typedef struct bstrom {
	int hodnota;
	int vyska;
	struct bstrom* leva, * prava;
}BSTROM;

//vypocte vysku vetve od uzlu dolu
int vyska_vetve(struct bstrom* uzel)
{
	if (uzel == NULL)
		return 0;
	return uzel->vyska;
}

//funkce pro tvorbu noveho uzlu nebo kmene (v pripade noveho stromu)
bstrom* vytvor_uzel(int data)
{
	bstrom* uzel = (bstrom*)malloc(sizeof(bstrom));
	uzel->leva = uzel->prava = NULL;
	uzel->hodnota = data;
	uzel->vyska = 1;
	return uzel;
}


//funkce potrebne pro vyvazovani stromu
//v podstate protoci trojici uzlu tak, aby ten stredne velky byl nad nimi (zjisteni, ktery to je, obstarava jiny kod)
//prava rotace protaci po smeru hodinovych rucicek, leva naopak
struct bstrom* prava_rotace(struct bstrom* y)
{

	struct bstrom *x = y->leva;
	struct bstrom *odlozeni = x->prava;

	x->prava = y;
	y->leva = odlozeni;

	y->vyska = max(vyska_vetve(y->leva), vyska_vetve(y->prava)) + 1;
	x->vyska = max(vyska_vetve(x->leva), vyska_vetve(x->prava)) + 1;

	return x;
}
struct bstrom* leva_rotace(struct bstrom* x)
{
	struct bstrom* y = x->prava;
	struct bstrom* odlozeni = y->leva;

	y->leva = x;
	x->prava = odlozeni;

	x->vyska = max(vyska_vetve(x->leva), vyska_vetve(x->prava)) + 1;
	y->vyska = max(vyska_vetve(y->leva), vyska_vetve(y->prava)) + 1;

	return y;
}


//pokud jsou podstromy stejne dlouhe, vyjde 0 --> cim rozdilnejsi hodnota vyjde, tim je strom nevyvazenejsi
int hodnota_vyvazeni(struct bstrom* uzel)
{
	if (uzel == NULL)
		return 0;
	return (vyska_vetve(uzel->leva) - vyska_vetve(uzel->prava));
}



//funkce pro nalezeni idealniho mista pro novy prvek (pokud budeme pridavat prvky pouze pomoci teto funkce, vytvorime vyvazovany strom
//pokud mame jiz rozpracovany nevyvazovany strom, neumi ho to uplne opravit, ale nove prvky se snazi delat spravne a v jejich okoli strom vyvazi
struct bstrom* vloz_automaticky(struct bstrom* uzel, int cislo)
{
	//kdyz dojde na prazdne misto, vytvori uzel
	if (!uzel)
		return vytvor_uzel(cislo);
	
	//hledani spravneho mista
	if (cislo > uzel->hodnota)
		uzel->prava = vloz_automaticky(uzel->prava, cislo);
	else if (cislo < uzel->hodnota)
		uzel->leva = vloz_automaticky(uzel->leva, cislo);
	else
	return uzel;

	//aktualizace hodnoty vyska -> pri vkladani uzlu rucne toto uplne nefunguje a vyvazovani moc nefunguje
	uzel->vyska = 1 + max(vyska_vetve(uzel->leva), vyska_vetve(uzel->prava));

	int vyvazeni = hodnota_vyvazeni(uzel);

	//leva vetev je delsi a hodnota spodniho uzlu je mensi nez hodnotahorniho uzlu
	if (vyvazeni > 1 && cislo < uzel->leva->hodnota)
		return prava_rotace(uzel);

	//prava vetev je delsi a hodnota spodniho uzlu je vetsi nez horniho
	if (vyvazeni < -1 && cislo > uzel->prava->hodnota)
		return leva_rotace(uzel);

	//leva vetev je delsi a ve strome vznikla rada po sobe jdoucich cisel -> je potreba dvakrat protocit
	if (vyvazeni > 1 && cislo > uzel->leva->hodnota) {
		uzel->leva = leva_rotace(uzel->leva);
		return prava_rotace(uzel);
	}
	
	//prava vetev je delsi -//-
	if (vyvazeni < -1 && cislo < uzel->prava->hodnota) {
		uzel->prava = prava_rotace(uzel->prava);
		return leva_rotace(uzel);
	}

	return uzel;
}


//najde nejmensi hodnotu ve strome za predpokladu ze je serazeny (bude uplne nalevo)
struct bstrom* min(struct bstrom* uzel) {
	struct bstrom* nejmensi = uzel;

	while (nejmensi->leva != NULL)
		nejmensi = nejmensi->leva;

	return nejmensi;
}


//smaze ze stromu hodnotu zadanou uzivatelem - pokud je hodnota ve strome dvakrat, smaze jen tu na kterou narazi driv
struct bstrom* smaz_uzel(struct bstrom* uzel, int cislo) {

	if (uzel == NULL)
		return uzel;

	//hleda hodnotu ve strome - pouze pokud je serazeny
	if (cislo < uzel->hodnota)
		uzel->leva = smaz_uzel(uzel->leva, cislo);

	else if (cislo > uzel->hodnota)
		uzel->prava = smaz_uzel(uzel->prava, cislo);

	else {//narazi na hledanou hodnotu

		//pokud za hodnotou neni na jedne strane pokracovani
		if ((uzel->leva == NULL) || (uzel->prava == NULL)) {

			struct bstrom* nahradni;
			//kdyz existuje leva vetev
			if (uzel->leva)
			{
				nahradni = uzel->leva;
			}
			else
			{
				nahradni = uzel->prava;
			}
			//struct bstrom* nahradni = uzel->leva ? uzel->leva : uzel->prava;

			//pokud nahodou nebyla ani jedna z vetvi proste smaze uzel i nahradni promennou
			if (nahradni == NULL) {
				nahradni = uzel;
				uzel = NULL;
			}
			else //jinak posadi nahradni uzel na misto uzle ktery chceme smazat a smaze nahradni promennou
				*uzel = *nahradni;
			free(nahradni);
		}
		else { //pokud ma uzel na obou stranach hodnoty
			struct bstrom* nahradni = min(uzel->prava);

			uzel->hodnota = nahradni->hodnota;

			uzel->prava = smaz_uzel(uzel->prava, nahradni->hodnota);
		}
	}

	if (uzel == NULL)
		return uzel;


	//nakonec se strom dovyvazi - kontroluje se take vyvazeni nizsiho uzlu
	uzel->vyska = 1 + max(vyska_vetve(uzel->leva),vyska_vetve(uzel->prava));

	int vyvazeni = hodnota_vyvazeni(uzel);
	if (vyvazeni > 1 && hodnota_vyvazeni(uzel->leva) >= 0)
		return prava_rotace(uzel);

	if (vyvazeni > 1 && hodnota_vyvazeni(uzel->leva) < 0) {
		uzel->leva = leva_rotace(uzel->leva);
		return prava_rotace(uzel);
	}

	if (vyvazeni < -1 && hodnota_vyvazeni(uzel->prava) <= 0)
		return leva_rotace(uzel);

	if (vyvazeni < -1 && hodnota_vyvazeni(uzel->prava) > 0) {
		uzel->prava = prava_rotace(uzel->prava);
		return leva_rotace(uzel);
	}

	return uzel;
}


//funkce pomoci ktere se muzeme pohybovat po strome
//v kazdem kroku nam napise na jakem uzlu se nachazime a vyzve nas ke kroku doprava nebo doleva
//pokud dojdeme na prazdne pole, muzeme primo v teto funkci pridat do tohoto pole hodnotu
//v kazdem kroku take muzeme uzel smazat, cimz smazeme celou nasledujici vetev
//muzeme takto smazat i cely strom, pokud smazeme prvni uzel
void pohyb(bstrom* uzel)
{
	int cislo;
	printf("\nNachazite se na uzlu %i", uzel->hodnota);
	printf("\nChceteli jit doprava, stisknete P, pokud doleva L");
	printf("\nStisknutim jine klavesy se vratite na zacatek.\n\n");
	int volba = _getch();
	switch (volba)
	{
	case key_p:
		if (uzel->prava)
		{
			pohyb(uzel->prava);
		}
		else
		{
			printf("Neni tam hodnota. Muzete ji pridat (+).");
			int volbaa = _getch();
			switch (volbaa)
			{
			case key_plus:
				printf("Zadejte cislo: ");
				scanf("%i", &cislo);
				uzel->prava = vytvor_uzel(cislo);
				break;

			default:
				break;
			}
		}

		break;

	case key_l:
		if (uzel->leva)
		{
			pohyb(uzel->leva);
		}
		else
		{
			printf("Neni tam hodnota. Muzete ji pridat (+), nebo se vratit na zacatek (Z).");
			int volbaa = _getch();
			switch (volbaa)
			{
			case key_plus:
				printf("Zadejte cislo: ");
				scanf("%i", &cislo);
				uzel->leva = vytvor_uzel(cislo);
				break;

			default:
				break;
			}

		}
		break;

	default:
		return;
		break;


	}

	return;


}


//funkce pro tisk stromu
void tisk_2(struct bstrom* uzel,int mezera)
{
	if (uzel == NULL)
		return;
	//mezera se takto zvetsi s kazdym posunem na nizsi vetev
	mezera += COUNT;

	//nejdrive se vytisknou prave prvky -> jsou pak v konzoli vyse
	tisk_2(uzel->prava, mezera);


	printf("\n");

	//vypise se prislusny pocet volnych mist
	for (int i = COUNT; i < mezera; i++)
		printf(" ");
	printf("%d\n", uzel->hodnota);


	tisk_2(uzel->leva, mezera);

}
void tisk(struct bstrom* uzel)
{
	//pri volani funkce tisk_2 primo se nezvetsovaly mezery..
		tisk_2(uzel,0);
}


//funkce pro tisk stromu do souboru funguje stejne jako tisk do konzole
void tiskSoubor_2(struct bstrom* uzel, int space, FILE* tisteny_strom)
{
	if (uzel == NULL)
		return;		
	space += COUNT;

	tiskSoubor_2(uzel->prava, space, tisteny_strom);

	fprintf(tisteny_strom, "\n");
	for (int i = COUNT; i < space; i++)
		fprintf(tisteny_strom, " ");
	fprintf(tisteny_strom, "%d\n", uzel->hodnota);
	tiskSoubor_2(uzel->leva, space, tisteny_strom);
}
void tiskSoubor(struct bstrom* uzel)
{
	FILE* tisteny_strom;
	fopen_s(&tisteny_strom, "tisteny_strom.txt", "w");
	tiskSoubor_2(uzel, 0, tisteny_strom);
	fclose(tisteny_strom);
	printf("Strom byl uspesne vytisten do souboru tisteny_strom.txt!");
}


//vytiskne velmi simple navigacni panel
void UI()
{
	printf("\n ____  _            __           __      _                       ");
	printf("\n|  _ \\(_)          /_/          /_/     | |");
	printf("\n| |_) |_ __ __  __ _ _ __ _ __  _    ___| |_ _ __ ___  _ __  ___");
	printf("\n|  _ <| | '_  \\/ _` | '__| '_ \\| |  / __| __| '__/ _ \\| '_ ` _  \\ ");
	printf("\n| |_) | | | | | (_| | |  | | | | |  \\__ \\ |_| | | (_) | | | | | |");
	printf("\n|____/|_|_| |_|\\__,_|_|  |_| |_|_|  |___/\\__|_|  \\___/|_| |_| |_| ");
	printf("\n------------------------------------------------------------------------------------------------------------------------\n");
	printf(" O programu (Napoveda): Ctrl+o | Smazat konzoli: Ctrl+s | Konec aplikace: ESC\n");
	printf("\n");
	printf(" Vystup: Ctrl+r | Vytisknout strom: Ctrl+t | Pridat prvek: + | Odebrat prvek: - | Pohybovat se: Ctrl+p\n");
	printf("\n");
//	printf(" Vystup: Ctrl+? | Vytisknout strom: Ctrl+? | Pridat prvek: Ctrl+? | Pohybovat se: Ctrl+?\n");
//	printf("\n");
	printf(" Nacist strom ze souboru: Ctrl+n | Ulozit strom do souboru: Ctrl+u | Vyvazit strom: Ctrl+v\n");
	printf("------------------------------------------------------------------------------------------------------------------------\n");


}

//funkce pro zobrazeni napovedy/o programu
//vytiskne taky prikladovy strom, aby bylo uzivateli jasne jak se tiskne
void napoveda()
{
	printf("\nVitejte v aplikaci Binarni strom.");
	printf("\n");
	printf("\nPokud si chcete prohlednout strom ulozeny v pameti, zvolte moznost VYTISKNOUT STROM.");
	printf("\nV pameti muze byt naraz jen jeden strom. Pro praci s vice stromy pouzijte ukladani do souboru.");
	printf("\n");
	printf("\nV pameti aplikace jsou vytvoreny nektere ukazkove stromy, ke kterym se dostanete pres moznost");
	printf("\nNACIST STROM ZE SOUBORU, kde zadate nazev jednoho ze stromu:");
	printf("\nBezny strom: jablon.dat");
	printf("\nStrom s hodnotami od 1 do 30 (v souboru ulozeno vzestupne): baobab.dat");
	printf("\nStrom se zapornymi cisly: liliovnik.dat");
	printf("\n");
	printf("\nDo stromu v pameti muzete pridavat nebo z nej odebirat hodnoty pomoci funkci PRIDAT/ODEBRAT PRVEK.");
	printf("\nMuzete take spustit specialni rezim prohlidky stromu pomoci funkce POHYBOVAT SE");
	printf("\nkde se pomoci prikazu pohybujete doprava/doleva a aktualni uzel muzete vzdy zmenit (popripade pridat).");
	printf("\nStrom muzete vytisknout do TXT souboru pomoci moznosti Vystup.");
	printf("\nPri tisku stromu se vytiskne koren doleva, prave vetve smerem nahoru a leve dolu. Viz ukazka nize.");
	printf("\nUdaje ze stromu muzete ulozit pomoci funkce ULOZIT STROM DO SOUBORU.");
	printf("\nZ techto souboru muzete strom take znovu nacist (NACIST STROM ZE SOUBORU)");
	printf("\n");
	printf("\nUkazka tisku:");
	printf("\n");
	printf("\n\t\t\t");
	printf("\n\t\t\tprava druha vetev");
	printf("\n");
	printf("\n\t\tprava prvni vetev");
	printf("\n");
	printf("\n\t\t\tleva druha vetev");
	printf("\n");
	printf("\n\tkoren");
	printf("\n");
	printf("\n\t\t\tprava druha vetev");
	printf("\n");
	printf("\n\t\tleva prvni vetev");
	printf("\n");
	printf("\n\t\t\tleva druha vetev");
	printf("\n");
}




//ulozeni pole do souboru
void ulozeni_2(int cislo, char nazev[])
{
	FILE* vystup;

	char buffer[256];
	vystup = fopen(nazev, "a");
	if (vystup == NULL) {
		return;
	}	

	snprintf(buffer, 256, "%d\n", cislo);

	size_t bytes_wrote = fwrite(buffer, sizeof(char), strlen(buffer), vystup);
	fclose(vystup);

	if (bytes_wrote != strlen(buffer)) {
		return;
	}

}
void ulozeni(int pole[])
{
	char nazev[20];
	printf("Zadejte nazev noveho nebo existujiciho souboru (ve formatu nazev.dat): ");
	scanf("%s", &nazev);
	system("cls");
	UI();

	FILE* vystup;
	vystup = fopen(nazev, "w");
	if (vystup == NULL) {
		return;
	}
	fprintf(vystup, "");
	fclose(vystup);

	int i = 0;
	int cislo = 0;
	while (pole[i] != NULL)
	{
		cislo = pole[i];
		ulozeni_2(cislo, nazev);
		i++;
	}
}


//nacteni pole ze souboru
void nacteni(int prazdne[],char nazev[])
{

	int i = 0;
	FILE* soubor;
	if (fopen_s(&soubor, nazev, "r")==0)
	{
		if (soubor = fopen(nazev, "r"))
		{
			while (fscanf(soubor, "%d", &prazdne[i]) != EOF)
			{
				i++;
			}
			fclose(soubor);
			prazdne[i] = '\0';
		}
	}
	else
	{
		printf("Zadan neplatny nazev, nacitam vychozi strom! \n");
		if (soubor = fopen("vychozi.dat", "r"))
		{
			while (fscanf(soubor, "%d", &prazdne[i]) != EOF)
			{
				i++;
			}
			fclose(soubor);
			prazdne[i] = '\0';
		}
	}



	return;
}


//vygeneruje z pole vyvazeny strom
struct bstrom* strom_z_pole(int pole[])
{
	int i = 1;
	bstrom* uzel = vytvor_uzel(pole[0]);
	while (pole[i] != NULL)
	{
		uzel = vloz_automaticky(uzel, pole[i]);		
		i++;
	}
	return uzel;
}

//strom vlozi do pole
void pole_ze_stromu(int pole[], struct bstrom* uzel)
{
	int i = 0;
	if (uzel == NULL)
		return;
	while (pole[i]!=NULL)
	{
		i++;
	}
	pole[i] = uzel->hodnota;
	pole_ze_stromu(pole, uzel->prava);	
	pole_ze_stromu(pole, uzel->leva);
	return;
}




int main()
{	
	char nazev[20] = { "vychozi.dat" };
	int prazdne[MAX_POLE];
	int cislo;
	int pole[MAX_POLE] = {};
	bstrom* uzel = strom_z_pole(pole);

	system("cls");
	UI();

	volebni:
	int volba = _getch();

	switch (volba)
	{
	default:
		break;

		//vytiskne strom do konzoly
	case ctrlt:
		system("cls");
		UI();
		tisk(uzel);
		break;

		//smaze obrazovku
	case ctrls:
		system("cls");
		UI();
		break;

		//spusti rezim pohybu
	case ctrlp:
		pohyb(uzel);
		break;

		//zobrazi napovedu
	case ctrlo:
		system("cls");
		UI();
		napoveda();
		break;

		//pridani hodnoty do stromu
	case key_plus:
		system("cls");
		UI();
		printf("Zadejte hodnotu, kterou chcete do stromu pridat: ");
		scanf_s("%d", &cislo);
		vloz_automaticky(uzel, cislo);
		system("cls");
		UI();
		break;

		//tisk stromu do souboru v citelne podobe
	case ctrlr:
		system("cls");
		UI();
		tiskSoubor(uzel);
		break;

		//ukonci aplikaci
	case ESC:
		exit(42);
		break;

		//odebere hodnotu ze stromu
	case key_minus:
		system("cls");
		UI();
		printf("Zadejte hodnotu, kterou chcete smazat ze stromu: ");
		scanf_s("%d", &cislo);
		smaz_uzel(uzel, cislo);
		system("cls");
		UI();
		break;

		//nacte strom ze souboru
	case ctrln:
		printf("Z jakeho souboru chcete nacist strom (ve formatu nazev.dat): ");
		scanf("%s",&nazev);
		nacteni(prazdne,nazev);
		uzel = strom_z_pole(prazdne);
		system("cls");
		UI();
		break;

		//ulozi strom do souboru
	case ctrlu:		
		pole_ze_stromu(pole,uzel);
		ulozeni(pole);
		system("cls");
		UI();
		break;
	}
	goto volebni;


	return 0;
}
