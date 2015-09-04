totalnum=8
correctnum=0
cd testdata

for ((i = 1; i <= 8; i++ ))
do
    ../heu -f test$i < input$i > out
    diff test$i.out input$i.out > compare
    if [[ -s compare ]] ; then
	echo "test inputn$i wrong"
    else
	echo "test input$i correct"
	((correctnum += 1))
    fi;
done

if [[ $correctnum -eq $totalnum ]] ; then
    echo "all test passed!"
else
    echo "NOT all test passed!"
    exit 1
fi;
    
    
