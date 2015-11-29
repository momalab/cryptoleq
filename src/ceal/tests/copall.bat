set PL=msc
::set PL=unx

cd ..
make clean 
make PLAT=msc -j 12 & make PLAT=msc & make PLAT=msc
::make PLAT=unx -j 12 & make PLAT=unx & make PLAT=unx
cd tests

call scop PLAT %PL% clean
call scop PLAT %PL% all

call show_html.bat



