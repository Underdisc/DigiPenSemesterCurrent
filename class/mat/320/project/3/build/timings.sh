mkdir complexvalues

./genrandoms.exe $1 > complexvalues/$1.txt
time ./dft.exe $1  complexvalues/$1.txt > complexvalues/dft_$1_output.txt
echo ^--- dft ---^
time ./fft.exe $1  complexvalues/$1.txt > complexvalues/fft_$1_output.txt
echo ^--- fft ---^
time ./fft2.exe $1 complexvalues/$1.txt > complexvalues/fft2_$1_output.txt
echo ^--- fft2 ---^

rm -rf complexvalues
