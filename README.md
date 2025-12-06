# 🐸 Console Crossy Road (C++)

Klasyczna gra zręcznościowa przeniesiona do terminala. Projekt napisany od podstaw w języku C++ z wykorzystaniem biblioteki `ncurses` (Linux) / `PDCurses` (Windows) do obsługi grafiki w trybie tekstowym (TUI).

Gra demonstruje umiejętność niskopoziomowego zarządzania pamięcią, obsługi wskaźników oraz logiki pętli gry (Game Loop).

## 🎮 Rozgrywka

Zadaniem gracza jest przeprowadzenie żaby przez ruchliwą ulicę. Z każdym poziomem samochody poruszają się szybciej.

**Kluczowe mechaniki:**
* **Zróżnicowane pojazdy:** Samochody wrogie, przyjazne (zwalniające), neutralne oraz specjalne "auto przenoszące".
* **Przeciwnik:** "Czapla", która poluje na gracza, wymuszając ciągły ruch.
* **Konfiguracja:** Gra wczytuje parametry mapy i prędkości z zewnętrznego pliku `config.txt`.
* **System zapisu:** Najlepszy wynik jest trwale zapisywany w pliku `rekord.txt`.

## 🚀 Technologie i Rozwiązania

Projekt został zrealizowany w celu pogłębienia wiedzy z zakresu inżynierii oprogramowania:
* **Język:** C++ (wykorzystanie `struct`, `new`/`delete`).
* **Zarządzanie pamięcią:** Ręczna alokacja dynamicznych tablic obiektów (wskaźniki `samochod** auta`, `pasy[]`).
* **Logika:** Implementacja własnego timera i obsługi kolizji bez użycia gotowych silników gier.
* **I/O:** Parsowanie plików tekstowych do konfiguracji gry.

## ⚙️ Jak uruchomić (Linux/macOS)

1. Upewnij się, że masz zainstalowaną bibliotekę `ncurses`:
   ```bash
   sudo apt-get install libncurses5-dev libncursesw5-dev
2. Skompiluj projekt:
   ```bash
    g++ main.cpp -o crossyroad -lncurses
3. Uruchom grę:
   ```bash
    ./crossyroad
