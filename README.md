Opis Projektu
Rozszerzona wersja klasycznej gry Asteroids, zaimplementowana z wykorzystaniem zasad programowania obiektowego (OOP) w C++ z użyciem biblioteki Raylib.

Dodane Funkcjonalności
1. System Punktacji
Różne wartości punktowe dla różnych typów asteroid:

Trójkąt: 15 punktów

Kwadrat: 25 punktów

Pięciokąt: 40 punktów

Gwiazda: 60 punktów

Punkty są mnożone przez rozmiar asteroidy (mała, średnia, duża)

Wyświetlanie aktualnego wyniku na ekranie

2. Efekty Wizualne
Eksplozje przy zniszczeniu asteroid:

Różne kolory w zależności od rozmiaru asteroidy![Zrzut ekranu 2025-05-14 214841](https://github.com/user-attachments/assets/86b77102-0696-45b3-8181-ec06b3a2f684)
![Zrzut ekranu 2025-05-14 214802](https://github.com/user-attachments/assets/9f5584b1-3faf-4a88-b7a0-22d70d9fd1b3)


Efekt rozszerzającego się okręgu z fade-outem

Kolorowe asteroidy (każdy typ ma inny kolor)

Efekt migania przy zmianie poziomu

3. System Ulepszeń
Power-upy pojawiają się losowo (20% szans) po zniszczeniu asteroidy:

Regeneracja zdrowia (+25 HP)

Ulepszenie broni (zwiększenie szybkostrzelności i prędkości pocisków)

Wizualna reprezentacja power-upów:

Zielone koło z "+" dla zdrowia

Niebieskie koło z "W" dla ulepszenia broni

4. System Poziomów
Progresja poziomów po zniszczeniu określonej liczby asteroid

Każdy poziom zwiększa trudność (szybsze asteroidy)

Wymagana liczba asteroid do następnego poziomu rośnie z każdym poziomem

Powiadomienie o zbliżającym się awansie poziomu

Efekt wizualny przy awansie poziomu

5. Interfejs Gracza
Pasek zdrowia nad statkiem

Rozbudowany HUD wyświetlający:

Aktualne zdrowie (HP)

Wynik (Score)

Poziom (Level)

Czas gry (Time)

Aktywną broń

Ekran końca gry z:

Podsumowaniem wyniku

Możliwością restartu (klawisz R)

Informacje o sterowaniu na dole ekranu

6. Ulepszenia Rozgrywki
Ograniczenie ruchu statku - nie może opuścić obszaru gry

Balans broni:

Laser: mocniejszy, wolniejszy

Pociski: słabsze, szybsze

Możliwość wyboru kształtu asteroid (klawisze 1-5)

Przełączanie broni (klawisz TAB)

7. Optymalizacje
Rezerwacja pamięci dla kontenerów

Efektywne zarządzanie pamięcią przez smart pointery

System czyszczenia nieużywanych obiektów (asteroidy, pociski, efekty)

Wymagania
Kompilator C++17

Biblioteka Raylib

Sterowanie
WASD - poruszanie statkiem

SPACJA - strzał

TAB - zmiana broni

1-5 - wybór kształtu asteroid

R - restart po śmierci
