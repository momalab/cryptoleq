cl -EHsc -Ox %1.cpp -DREGS=100 -DREG0=1
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=3
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=4
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=5
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=6
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=7
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=8
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=9
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=10
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=11
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=12
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=13
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=14
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=15
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=16
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=17
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=18
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=19
%1 | sed -n 4,4p >> %1.out

cl -EHsc -Ox %1.cpp -DREGS=20
%1 | sed -n 4,4p >> %1.out

rm %1.obj %1.exe
