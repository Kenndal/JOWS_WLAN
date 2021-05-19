Aktualnie to jest postawione na 802.11ac i chyba po prostu zrobi się osobne pliki na podstawie tego dla innych standardów i dla klas ruchu.
Nie wiem czy jest robienie tego dla różnego ruchu, zastanawiam sie czy za dużo tego nie będzie. Bedziemy widzieć.


Można dawać dowolną liczne stacji IPTV. Każda bedzie dawać ruch 6Mb/s (na jakieś stronie znalazłem że takie jest wymagane dla HD MPEG-4)
Osobno są zrobione stacje do ruchu w tle, też ustawiane w okrędu. Każda będzie nadawać ile sieć pozwala.

Dlaczego tak zrobiłem, a nie odpalam na tych samych stacjach ruchu w tle?
- W sumie to dziwne mi się wydawało to że dekoder IPTV będzie chciał dotawać 200 Mb/s ruchu w tle, dlatego zrobiłem je jako osobne stacje.

Przykład: 10 stacji IPTV, 5 stacji background, ruch VI i BE odpalony
./waf --run "wlan_jows --simTime=10 --nSTA=10 --VI=true --BE=true"

Ogólnie to jakoś tak śmiesznie wywala się przy 14 stacja IPTV. Wtedy ten ruch umiera, a całość przejmuje ruch w tle. 
Nie mam pojęcia czemu tak jest. Moze za dużo ich tam rywalizuje, ale czemy za to zyskuje BE to też nie mam pojecia.
Dziwne to, ale może tak ma być :/
(UPDATE: Mam podejrzenie że wydłużenie czasu symulacji z 2 sekund do więcej może rozwiązać ten problem)

A i trzeba wymyślić jak to przesymulować szybko, bo z tego co widzę to mamy to zrobić dla zwiększającej się liczby stacji IPTV żeby osiągnąć pojemność sieci.