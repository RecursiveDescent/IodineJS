if [ "`md5sum IodineJS/*`" != "`cat sums`" ]; then
	cd IodineJS

	clang++ -std=c++17 -g -c api.cpp -o api.so

	clang++ -std=c++17 -g main.cpp api.so -o ../main

	cd ..
fi

./main

rm sums

md5sum IodineJS/* >> sums
