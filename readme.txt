Proiect: Electron
Membrii echipei: Iulian Oleniuc, Ciobotaru Mihai (Grupa B3)

Prezentare:
-----------
https://www.youtube.com/watch?v=-tOELG5cpg0

Explicații (Desenarea legăturilor):
-----------------------------------
- BFS pe grid

- Menținem trei set-uri:
    - usedGridEdges: Muchiile folosite din grid.
    - usedGridNodesForNodes/ForEdges: Intersecțiile folosite din grid. O variantă pentru noduri și alta pentru legături, deoarece fiecare nod are două hitbox-uri diferite: Unul pentru noduri și altul pentru legături, căci legăturile trebuie să aibă acces la endpoint-urile unui nod, pe când un alt nod n-are voie să le atingă.

- La orice operație de rotate/create node/move/etc, se testează mai întâi dacă aceasta poate fi efectuată fără să ducă la dispariția altor legături (din lipsă de spațiu), iar dacă este OK, se efectuează, iar apoi toate legăturile implicate sunt redesenate.

- Fiecare legătură are asociată o permutare de ordin 4, care reprezintă ordinea în care sunt parcurși vecinii intersecțiilor în cadrul construirii legăturii. Astfel, când rotim o muchie, ce facem de fapt este să permutăm ordinea acelor vecini.

Aspecte interesante:
--------------------
1. Meniurile, în special acelea care apar la poziția mouse-ului, deoarece acestea se asigură ca popup-ul să nu iasă niciodată din cadrul ferestrei.
2. File Manager-ul și capacitatea sa de importare a fișierelor prin copierea lor în folderul Saves. Acest detaliu este realizat printr-o scanare a tuturor fișierelor din folder. Dacă un fișier are extensia .circuit, este considerat o salvare.
3. Easter Egg-ul (jocul Snake), care permite și salvarea scorului maxim.
4. Opțiunea de a schimba tema/limba și capacitatea programului de a reține setările într-un fișier. Astfel, la fiecare rulare a programului, acesta se va deschide cu ultimele setări salvate.

Împărțirea sarcinilor:
----------------------
Iulian: tot ce ține de circuit (50%)
Mihai: meniuri, teme, limbi, file manager, snake (50%)
