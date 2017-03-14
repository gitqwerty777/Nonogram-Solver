./heu -t -n 1 -s 15 < testdata/tour15
gprof ./heu | gprof2dot -w -s | dot -Tpng -o output.png
gnome-open output.png

# 每個方框內部顯示函數名稱，函數整體包括內部子函數佔用時間%比，函數自身，不包括內部子函數佔用時間%比，函數執行次數。邊表示父函數調用該子函數佔用的時間%比，調用次數
