Aktualnie to jest postawione na 802.11ac i chyba po prostu zrobi się osobne pliki na podstawie tego dla innych standardów i dla klas ruchu.
Nie wiem czy jest robienie tego dla różnego ruchu, zastanawiam sie czy za dużo tego nie będzie. Bedziemy widzieć.


Można dawać dowolną liczne stacji IPTV. Każda bedzie dawać ruch 6Mb/s (na jakieś stronie znalazłem że takie jest wymagane dla HD MPEG-4)
Osobno są zrobione stacje do ruchu w tle, też ustawiane w okrędu. Każda będzie nadawać ile sieć pozwala.

Dlaczego tak zrobiłem, a nie odpalam na tych samych stacjach ruchu w tle?
- W sumie to dziwne mi się wydawało to że dekoder IPTV będzie chciał dotawać 200 Mb/s ruchu w tle, dlatego zrobiłem je jako osobne stacje.

Przykład: 10 stacji IPTV, 5 stacji background, ruch VI i BE odpalony
./waf --run "wlan_jows --simTime=10 --nSTA=1 --VI=true --BE=true"

Ogólnie to jakoś tak śmiesznie wywala się przy 14 stacja IPTV. Wtedy ten ruch umiera, a całość przejmuje ruch w tle. 
Nie mam pojęcia czemu tak jest. Moze za dużo ich tam rywalizuje, ale czemy za to zyskuje BE to też nie mam pojecia.
Dziwne to, ale może tak ma być :/
(UPDATE: Mam podejrzenie że wydłużenie czasu symulacji z 2 sekund do więcej może rozwiązać ten problem)

A i trzeba wymyślić jak to przesymulować szybko, bo z tego co widzę to mamy to zrobić dla zwiększającej się liczby stacji IPTV żeby osiągnąć pojemność sieci.


UPDATE:

- Możliwość wyboru czy ruch video ma być traktowany jako BE czy VI
- Możliwość wyboru standardu ac i n jest do wyboru
- Zapisuje do csv w miłej formie, zeby łatwiej było wyniki analizować. (Jak zobaczyłem tego xml którego automatycznie generuje to aż się zaśmiałem jaki byłby to rak do parsowania xD)
	Format zapisu:
	"wlan_project-"+standard+"-vi_sta-"+std::to_string(nSTA)+"-"+vi_resolution+"-VI_as_BE-"+std::to_string(vi_as_BE)+"-BE_traffic-"+std::to_string(BE)+".csv"
	Przykład:
	wlan_project-n-vi_sta-10-HD-VI_as_BE-1-BE_traffic-1.csv
	wlan_project-n-vi_sta-10-HD-VI_as_BE-0-BE_traffic-1.csv


Wszystko działa ładnie. Sprawdziłem pcap'y i leci wszystko tak jak należy w tym samym kanale i na tym samym standardzie, czyli wpływają na siebie.
Spradziłem zachowanie jak video jest traktowane jako BE i mamy straty pakietów i większe opóźnienie wiec jest w pyte.

Można to odpalać w pętli po liczbie stacji IPTV.



Przykładowe odpalenia które testowałem:

802.11ac:

./waf --run "wlan_jows --simTime=5 --nSTA=1 --nSTA_background=1 --VI=true --BE=true"
./waf --run "wlan_jows --simTime=5 --nSTA=1 --nSTA_background=1 --VI=true --BE=true --vi_as_BE=true"
./waf --run "wlan_jows --simTime=5 --nSTA=1 --nSTA_background=1 --VI=true --BE=true --vi_resolution=UHD"
./waf --run "wlan_jows --simTime=5 --nSTA=1 --nSTA_background=1 --VI=true --BE=true --vi_resolution=UHD --vi_as_BE=true"


./waf --run "wlan_jows --simTime=15 --nSTA=10 --nSTA_background=4 --VI=true --BE=true"
./waf --run "wlan_jows --simTime=15 --nSTA=10 --nSTA_background=4 --VI=true --BE=true --vi_as_BE=true"

802.11n:

./waf --run "wlan_jows --simTime=5 --nSTA=1 --nSTA_background=1 --VI=true --BE=true --standard=n"

./waf --run "wlan_jows --simTime=15 --nSTA=10 --nSTA_background=4 --VI=true --BE=true --standard=n"
./waf --run "wlan_jows --simTime=15 --nSTA=10 --nSTA_background=4 --VI=true --BE=true --standard=n --vi_as_BE=true"


