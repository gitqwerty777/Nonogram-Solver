make clean; make mode=release
for ((i = 1; i <= 8; i = i + 1))
do
    echo "input$i"
    ./heu -f input$i < input$i >/dev/null
done
