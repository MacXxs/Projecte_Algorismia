#!/bin/sh
echo "S'executarà l'escript amb la configuració següent:"
echo "Tipus de graf: $1"
echo "n: $2"
echo "p: $3"
echo "r: $4"
echo "q: $5"

for i in {1..5}
do
    sleep 1
    echo -ne "."
done
for i in {1..100}
do
	./program_script $1 $2 $3 $4 $5
    echo $i
done
