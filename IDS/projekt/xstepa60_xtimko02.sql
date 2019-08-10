drop table Hrac_Trenink;
drop table Hrac_Zapas;
drop table Zapas_Tym;
drop table Trenink_Sportoviste;
drop table Trener_Tym;
drop table Hrac_Tym;
drop table Trener_Trenink;
drop table Vybaveni;
drop table Trenink;
drop table Trener;
drop table Tym;
drop table Zapas;
drop table Hrac;
drop table Sportoviste;

drop sequence hrac_seq;

create table Hrac
(
  ID              NUMBER PRIMARY KEY        NOT NULL,
  jmeno           varchar(45)               NOT NULL,
  prijmeni        varchar(45)               NOT NULL,
  adresa_bydliste varchar(45)               NOT NULL,
  datum_narozeni  date                      NOT NULL,
  vyska           NUMBER                    NOT NULL,
  vaha            NUMBER                    NOT NULL,
  rodne_cislo     NUMBER(10)                NOT NULL,
  CONSTRAINT CHCK_vyska CHECK (vyska > 0),
  CONSTRAINT CHCK_vaha CHECK (vaha > 0)
);


create table Trener
(
  ID             INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  jmeno          varchar(80)               NOT NULL,
  prijmeni       varchar(80)               NOT NULL,
  adresa         varchar(80)               NOT NULL,
  datum_narozeni date                      NOT NULL
);

create table Tym
(
  kategorie varchar(45) NOT NULL PRIMARY KEY
);

create table Vybaveni
(
  ID        INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  typ       varchar(80)               NOT NULL,
  cena      NUMBER                    NOT NULL,
  HracID    INT REFERENCES Hrac (ID),
  TrenerID  INT REFERENCES Trener (ID),
  kategorie VARCHAR(45) REFERENCES Tym (kategorie),
  CONSTRAINT CHK_cena CHECK (cena >= 0)
);

create table Trenink
(
  ID           INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  pravidelnost char(1)                   NOT NULL,
  datum_a_cas  timestamp                 NOT NULL,
  TrenerID     INT REFERENCES Trener (ID),
  kategorie    varchar(45) REFERENCES Tym (kategorie)
);

create table Sportoviste
(
  ID              INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  typ             varchar(45)               NOT NULL,
  nazev           varchar(80)               NOT NULL,
  adresa          varchar(80)               NOT NULL,
  kapacita_divaku int,
  kapacita_hracu  int,
  vlastnictvi     char(1)                   NOT NULL,
  CONSTRAINT CHCK_kapacita_divaku CHECK (kapacita_divaku > 0),
  CONSTRAINT CHCK_kapacita_hracu CHECK (kapacita_hracu > 0)
);

create table Zapas
(
  ID            INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  datum_a_cas   timestamp                 NOT NULL,
  adresa        varchar(80),
  SportovisteID INT REFERENCES Sportoviste (ID)
);

create table Hrac_Zapas
(
  HracID  NUMBER NOT NULL,
  ZapasID NUMBER NOT NULL,
  FOREIGN KEY (HracID) REFERENCES Hrac (ID),
  FOREIGN KEY (ZapasID) REFERENCES Zapas (ID),
  UNIQUE (HracID, ZapasID)
);

create table Hrac_Trenink
(
  HracID    NUMBER NOT NULL,
  TreninkID NUMBER NOT NULL,
  FOREIGN KEY (HracID) REFERENCES Hrac (ID),
  FOREIGN KEY (TreninkID) REFERENCES Trenink (ID)
);

create table Zapas_Tym
(
  ZapasID     NUMBER      NOT NULL,
  Kategorie   varchar(45) NOT NULL,
  goly_domaci INT         NOT NULL,
  goly_hoste  INT         NOT NULL,
  FOREIGN KEY (ZapasID) REFERENCES Zapas (ID),
  FOREIGN KEY (Kategorie) REFERENCES Tym (kategorie)

);

create table Trenink_Sportoviste
(
  TreninkID     NUMBER NOT NULL,
  SportovisteID NUMBER NOT NULL,
  FOREIGN KEY (TreninkID) REFERENCES Trenink (ID),
  FOREIGN KEY (SportovisteID) REFERENCES Sportoviste (ID)
);

create table Trener_Tym
(
  TrenerID NUMBER      NOT NULL,
  TymID    varchar(45) NOT NULL,
  FOREIGN KEY (TrenerID) REFERENCES Trener (ID),
  FOREIGN KEY (TymID) REFERENCES Tym (kategorie),
  funkce   varchar(45) NOT NULL
);

create table Hrac_Tym
(
  HracID     NUMBER      NOT NULL,
  TymID      varchar(45) NOT NULL,
  FOREIGN KEY (HracID) REFERENCES Hrac (ID),
  FOREIGN KEY (TymID) REFERENCES Tym (kategorie),
  post       varchar(45) NOT NULL,
  cisloDresu int         NOT NULL,
  CONSTRAINT over_kategorii_tymu CHECK (TymID IN ('Dorost', 'Muzi', 'Zeny'))
);

create table Trener_Trenink
(
  TrenerID  NUMBER NOT NULL,
  TreninkID NUMBER NOT NULL,
  FOREIGN KEY (TrenerID) REFERENCES Trener (ID),
  FOREIGN KEY (TreninkID) REFERENCES Trenink (ID)
);

-- VKLADANI DO TABULKY HRAC

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (1,'Eden', 'Hazard', 'Trafalgar Square 8', TO_DATE('7/1/1991','DD/MM/YYYY'), 173, 76, 9101075321);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (2,'Lionel', 'Messi', 'Barcelona 3', TO_DATE('24/6/1987','DD/MM/YYYY'), 170, 70, 8706245893);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (3,'Gonzalo', 'Higuaín', 'Trafalgar Square 10', TO_DATE('10/12/1987','DD/MM/YYYY'), 186, 89, 8712104925);


INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (4,'Elena', 'Pudlova', 'Brno', TO_DATE('12/05/1993','DD/MM/YYYY'), 160, 61, 9355122843);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (5,'Jana', 'Kvapilová', 'Brno1', TO_DATE('3/07/1997','DD/MM/YYYY'), 164, 63, 9757031893);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (6,'Tereza', 'Mokrá', 'Brno2', TO_DATE('10/04/1990','DD/MM/YYYY'), 172, 69, 9054107932);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (7,'Adam', 'Nový', 'Brno3', TO_DATE('11/02/1999','DD/MM/YYYY'), 187, 82, 9902113847);

INSERT INTO Hrac
  (ID, jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (8,'Petr', 'Soukup', 'Brno4', TO_DATE('06/05/2000','DD/MM/YYYY'), 169, 70, 0005065732);

INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (9,'Jakub', 'Novotný', 'Brno5', TO_DATE('01/04/1999','DD/MM/YYYY'), 178, 75, 9904013469);


-- VKLADANI DO TABULKY TRENER

INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('Maurizio', 'Sarri', 'Mill Lane 51', TO_DATE('10/1/1959','DD/MM/YYYY'));

INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('José', 'Mourinho', 'Thorton St 72', TO_DATE('26/1/1963','DD/MM/YYYY'));


INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('Filip', 'Sepiak', 'Celakovskeho 2', TO_DATE('30/5/1969','DD/MM/YYYY'));

INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('Tomáš', 'Chladný', 'Celakovskeho 5', TO_DATE('11/2/1978','DD/MM/YYYY'));

INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('Vladimír', 'Hrubý', 'Celakovskeho 7', TO_DATE('24/4/1970','DD/MM/YYYY'));

INSERT INTO Trener
  (jmeno, prijmeni, adresa, datum_narozeni
)
VALUES ('Daniel', 'Sobota', 'Celakovskeho 9', TO_DATE('2/5/1964','DD/MM/YYYY'));


-- VKLADANI DO TABULKY SPORTOVISTE

INSERT INTO Sportoviste
  (typ, nazev, adresa, kapacita_divaku, kapacita_hracu, vlastnictvi
)
VALUES ('Fotbalove hriste', 'Stamford Bridge', 'Horni 962', 40000, 40, 'a');

INSERT INTO Sportoviste
  (typ, nazev, adresa, kapacita_hracu, vlastnictvi
)
VALUES ('Posilovna', 'U Hulka', 'Ceska 10', 20, 'a');

INSERT INTO Sportoviste
  (typ, nazev, adresa, kapacita_hracu, vlastnictvi
)
VALUES ('Posilovna', 'Průvan', 'Ceska 12', 20, 'n');


-- VKLADANI DO TABULKY TYM

INSERT INTO Tym
  (kategorie
)
VALUES ('Dorost');

INSERT INTO Tym
  (kategorie
)
VALUES ('Muzi');

INSERT INTO Tym
  (kategorie
)
VALUES ('Zeny');

-- VKLADANI DO TABULKY VYBAVENI

INSERT INTO Vybaveni
  (typ, cena, HracID
)
VALUES ('Fotbalovy mic', 2400, 1);

INSERT INTO Vybaveni
  (typ, cena, kategorie
)
VALUES ('Posilovaci stojan', 39000, 'Dorost');

INSERT INTO Vybaveni
  (typ, cena, kategorie
)
VALUES ('Kuzely', 2500, 'Zeny');

INSERT INTO Vybaveni
  (typ, cena, HracID
)
VALUES ('Cinky', 5000, 2);

INSERT INTO Vybaveni
  (typ, cena, HracID
)
VALUES ('Svihadlo', 800, 4);

INSERT INTO Vybaveni
  (typ, cena, HracID
)
VALUES ('Kopacky', 2000, 3);

-- VKLADANI DO TABULKY Hrac_Tym

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (1, 'Muzi', 'Utocnik', 10);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (2, 'Muzi', 'Utocnik', 9);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (3, 'Muzi', 'Obrance', 13);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (4, 'Zeny', 'Obrance', 20);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (5, 'Zeny', 'Brankar', 1);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (6, 'Zeny', 'Utocnik', 7);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (7, 'Dorost', 'Obrance', 11);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (8, 'Dorost', 'Brankar', 2);

INSERT INTO Hrac_Tym
  (HracID, TymID, post, cisloDresu
)
VALUES (9, 'Dorost', 'Utocnik', 8);

-- VKLADANI DO TABULKY Trener_Tym

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (3, 'Dorost', 'Hlavni trener');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (1, 'Muzi', 'Hlavni trener');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (2, 'Muzi', 'Asistent trenera');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (2, 'Dorost', 'Asistent trenera');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (4, 'Dorost', 'Asistent trenera');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (5, 'Zeny', 'Hlavni trener');

INSERT INTO Trener_Tym
  (TrenerID, TymID, funkce
)
VALUES (6, 'Zeny', 'Asistent trenera');


-- VKLADANI DO TABULKY Trenink

INSERT INTO Trenink
  (pravidelnost, datum_a_cas, TrenerID, kategorie
)
VALUES ('a', TO_TIMESTAMP('2019-04-15 15:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 1, 'Muzi');

INSERT INTO Trenink
  (pravidelnost, datum_a_cas, TrenerID, kategorie
)
VALUES ('n', TO_TIMESTAMP('2019-04-17 18:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 2, 'Dorost');

INSERT INTO Trenink
  (pravidelnost, datum_a_cas, TrenerID, kategorie
)
VALUES ('a', TO_TIMESTAMP('2019-04-14 12:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 3, 'Zeny');

-- VKLADANI DO TABULKY ZAPAS
INSERT INTO Zapas (datum_a_cas, adresa
)
VALUES (TO_TIMESTAMP('2019-04-24 13:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 'Cernopolni 53');

INSERT INTO Zapas (datum_a_cas, SportovisteID
)
VALUES (TO_TIMESTAMP('2019-04-30 19:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 1);

INSERT INTO Zapas (datum_a_cas, adresa
)
VALUES (TO_TIMESTAMP('2019-05-02 21:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 'Cernopolni 53');

INSERT INTO Zapas (datum_a_cas, adresa
)
VALUES (TO_TIMESTAMP('2019-05-14 10:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF'), 'Cernopolni 53');



-- VKLADANI DO TABULKY Zapas_Tym
INSERT INTO Zapas_Tym (ZapasID, Kategorie, goly_domaci, goly_hoste
)
VALUES (1, 'Dorost', 3, 5);

INSERT INTO Zapas_Tym (ZapasID, Kategorie, goly_domaci, goly_hoste
)
VALUES (2, 'Muzi', 3, 1);

INSERT INTO Zapas_Tym (ZapasID, Kategorie, goly_domaci, goly_hoste
)
VALUES (3, 'Zeny', 6, 2);

INSERT INTO Zapas_Tym (ZapasID, Kategorie, goly_domaci, goly_hoste
)
VALUES (4, 'Muzi', 1, 2);



-- VKLADANI DO TABULKY Hrac_Trenink
INSERT INTO Hrac_Trenink (HracID, TreninkID
)
VALUES (1, 1);

INSERT INTO Hrac_Trenink (HracID, TreninkID
)
VALUES (2, 1);

INSERT INTO Hrac_Trenink (HracID, TreninkID
)
VALUES (7, 2);

INSERT INTO Hrac_Trenink (HracID, TreninkID
)
VALUES (5, 3);

-- VKLADANI DO TABULKY Trener_Trenink (Trener se ucastni treninku)
INSERT INTO Trener_Trenink (TrenerID, TreninkID
)
VALUES (2, 1);

INSERT INTO Trener_Trenink (TrenerID, TreninkID
)
VALUES (1, 1);

INSERT INTO Trener_Trenink (TrenerID, TreninkID
)
VALUES (3, 2);

INSERT INTO Trener_Trenink (TrenerID, TreninkID
)
VALUES (5, 3);

INSERT INTO Trener_Trenink (TrenerID, TreninkID
)
VALUES (4, 2);

-- VKLADANI DO TABULKY Trenink_Sportoviste
INSERT INTO Trenink_Sportoviste (TreninkID, SportovisteID
)
VALUES (1, 1);

INSERT INTO Trenink_Sportoviste (TreninkID, SportovisteID
)
VALUES (2, 2);

INSERT INTO Trenink_Sportoviste (TreninkID, SportovisteID
)
VALUES (3, 3);

-- VKLADANI DO TABULKY Hrac_Zapas
INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (7, 1);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (8, 1);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (9, 1);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (1, 2);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (2, 2);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (3, 2);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (4, 3);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (5, 3);

INSERT INTO Hrac_Zapas (HracID, ZapasID
)
VALUES (6, 3);


SET serveroutput ON ;

---------- Alespoň dva dotazy využívající spojení dvou tabulek
-- Prikaz vypise jmeno, prijmeni a post vsech hracu, kteri patri do tymu Muzi
--SELECT jmeno, prijmeni, post FROM Hrac_Tym, Hrac, Tym WHERE Hrac.ID = Hrac_Tym.HracID AND Hrac_Tym.TymID = Tym.kategorie and Tym.kategorie = 'Muzi' ORDER BY Hrac.prijmeni;
-- Prikaz vypise trenera, ktery se porada trenink v den 17.04.2019
--SELECT DISTINCT jmeno, prijmeni FROM Trenink, Trener, Trener_Trenink WHERE Trenink.datum_a_cas = TO_TIMESTAMP('2019-04-17 18:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF') AND Trenink.TrenerID = Trener.ID;

---------- Jeden dotaz využívající spojení tří tabulek
-- Prikaz vypise kteri z hracu v zapase ze dne 30.4.2019 v 19:00 ma pujcene nejake vybaveni a jake
--SELECT jmeno, prijmeni, typ FROM Hrac INNER JOIN Hrac_Zapas HZ ON Hrac.ID = HZ.HracID INNER JOIN Zapas ON HZ.ZapasID = Zapas.ID INNER JOIN Vybaveni ON Hrac.ID = Vybaveni.HracID WHERE Zapas.datum_a_cas = TO_TIMESTAMP('2019-04-30 19:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF') ORDER BY Hrac.prijmeni;

---------- Dva dotazy s klauzulí GROUP BY a agregační funkcí
-- Prikaz vypise, jaky byl celkovy pocet branek za rok 2019
--SELECT SUM((SUM(Zapas_Tym.goly_domaci)+SUM(Zapas_Tym.goly_hoste))) AS Pocet_golu FROM Zapas JOIN Zapas_Tym ON Zapas.ID = Zapas_Tym.ZapasID WHERE Zapas.datum_a_cas BETWEEN TO_TIMESTAMP('2019-01-01 00:00:00.00', 'YYYY-MM-DD HH24:MI:SS.FF') AND TO_TIMESTAMP('2019-12-31 23:59:59.59', 'YYYY-MM-DD HH24:MI:SS.FF') GROUP BY Zapas_Tym.goly_domaci;
-- Prikaz vypise, jaka je prumerna vaha vsech hracu v kazdem tymu
--SELECT ROUND(AVG(Hrac.vaha),0) AS Prumerna_vaha, Tym.kategorie FROM Hrac JOIN Hrac_Tym ON Hrac.ID = Hrac_Tym.HracID JOIN Tym ON Hrac_Tym.TymID = Tym.kategorie GROUP BY Tym.kategorie;

---------- Jeden dotaz obsahující predikát EXISTS
-- Prikaz vypise zapasy, pokud v nich padlo vice nez pet golu
--SELECT * FROM Zapas WHERE EXISTS(SELECT * FROM Zapas_Tym WHERE Zapas_Tym.goly_hoste+Zapas_Tym.goly_domaci > 5 AND Zapas.ID = Zapas_Tym.ZapasID);

---------- Jeden dotaz s predikátem IN s vnořeným selectem (nikoliv IN s množinou konstantních dat).
-- Kteri treneri trenuji pouze tym dorost
--SELECT jmeno, prijmeni FROM Trener JOIN Trener_Tym ON Trener.ID = Trener_Tym.TrenerID JOIN Tym ON Trener_Tym.TymID = Tym.kategorie WHERE TrenerID NOT IN (SELECT TrenerID FROM Trener_Tym JOIN Tym ON Trener_Tym.TymID = Tym.kategorie WHERE Trener_Tym.TymID <> 'Dorost') ORDER BY Trener.prijmeni;



---------- dva netrivialni databazove triggery, vc. jejich predvedeni
-- jeden trigger automaticky generuje hodnoty primarniho klice nejake tabulky ze sekvence (napr. pokud bude pri vkladani zaznamu do dane tabulky hodnota primarniho klice nedefinovana, TJ. NULL)

--Triger na automaticke generovani primarniho klice do tabulkz Hrac
CREATE SEQUENCE hrac_seq
  START WITH 20
  INCREMENT BY 1;

CREATE OR REPLACE TRIGGER trigger_hrac_id
  BEFORE INSERT ON Hrac
  FOR EACH ROW

BEGIN
     :NEW.ID := hrac_seq.nextval;
END ;
/


-- Ukazka funkcnosti trigeru na generovani promarniho klice
INSERT INTO Hrac
  (jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES ('John', 'Snow', 'Winterfell', TO_DATE('02/08/1989','DD/MM/YYYY'), 180, 75, 8908026479);



-- Trigger na kontrolovani rodneho cisla
CREATE OR REPLACE TRIGGER trigger_rodne_cislo
AFTER INSERT OR UPDATE ON Hrac
FOR EACH ROW
DECLARE
  -- Deklarovani promennych, 2 mista pro den, mesic a rok
    rodne_cislo NUMBER;
    den NUMBER(2);
    mesic NUMBER(2);
    rok NUMBER(2);
    datum DATE;
BEGIN
   -- Ziskame vzdy konkretni dvojcisli, modulo 100 nam vrati posledni dve cislice po deleni
    rodne_cislo := :NEW.rodne_cislo;
    rok   := rodne_cislo / 100000000;
    mesic := MOD((rodne_cislo / 1000000), 100);
    den   := MOD((RODNE_CISLO / 10000), 100);

    -- Zeny maji v mesici hodnotu vetsi o 50 nez muzi
    IF (mesic > 50) THEN
          mesic := mesic - 50;
    END IF;

    -- Kontrola na delitelnost rodneho cisla cislem 11
    IF (MOD(rodne_cislo, 11) <> 0) THEN
          Raise_Application_Error (-20000, 'Spatny format rodneho cisla! Musi byt delitelne cislem 11!');
    END IF;

    -- Dosazeni do promenne typu DATE, pokud format neni v poradku nastane vyjimka
    BEGIN
          datum := den||'.'||mesic||'.'||rok;
    EXCEPTION
      WHEN OTHERS THEN
           Raise_Application_Error (-20000, 'Spatny format rodneho cisla!');
    END;
END;
/

-- Ukazka funkcnosti triggeru - dojde k chybe
INSERT INTO Hrac
  (ID,jmeno, prijmeni, adresa_bydliste, datum_narozeni, vyska, vaha, rodne_cislo
)
VALUES (10,'Pavel', 'Modry', 'Brno-Lesna', TO_DATE('02/05/1999','DD/MM/YYYY'), 178, 75, 9905993469);


-- Procedura
CREATE OR REPLACE PROCEDURE kolik_je_v_kazdem_tymu_hracu
IS
   CURSOR hrac_tym IS SELECT * FROM Hrac_Tym;
   zaznam hrac_tym%ROWTYPE;
   pocet_muzi NUMBER;
   pocet_zeny NUMBER;
   pocet_dorost NUMBER;
BEGIN
  pocet_muzi := 0;
  pocet_zeny := 0;
  pocet_dorost := 0;
  OPEN hrac_tym;
  LOOP
    FETCH hrac_tym into zaznam;
    EXIT WHEN hrac_tym%NOTFOUND;
      IF (zaznam.TymID = 'Muzi')
        THEN
          pocet_muzi := pocet_muzi + 1;
      END IF;

      IF (zaznam.TymID = 'Zeny')
        THEN
          pocet_zeny := pocet_zeny + 1;
      END IF;

      IF (zaznam.TymID = 'Dorost')
        THEN
          pocet_dorost := pocet_dorost + 1;
      END IF;
  END LOOP;
  CLOSE hrac_tym;
    DBMS_OUTPUT.enable();
    dbms_output.put_line('Pocet hracu za muze je: ' || pocet_muzi);
    dbms_output.put_line('Pocet hracu za zeny je: ' || pocet_zeny);
    dbms_output.put_line('Pocet hracu za dorost je: ' || pocet_dorost);
    DBMS_OUTPUT.get_lines(DBMS_OUTPUT.CHARARR,1000);
  EXCEPTION
  WHEN OTHERS THEN
    raise_application_error(-20001, 'Chyba pri ziskavani statistik o hracich v tymech!');
END;
/


