totalnum=8
correctnum=0
cd testdata

for ((i = 1; i <= 8; i++))
do
    rm -f test$1.out
    ../heu -f test$i < input$i > out
   diff test$i.out input$i.out > compare
    if [[ -s compare ]] ; then
	      echo "[test] input$i wrong"
        cat compare
    else
	      echo "[test] input$i correct"
	      ((correctnum += 1))
    fi;
done
rm -f out compare

if [[ $correctnum -eq $totalnum ]] ; then
    echo "[test] all test passed!"
else
    echo "[test] NOT all test passed!"
    exit 1
fi;
