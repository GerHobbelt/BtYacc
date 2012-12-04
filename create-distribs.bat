rem
rem create distribution files for www.hebbut.net
rem
                          
set name=btyacc-3.0.5.i_a-v1
                                                       
mkdir .\distrib

pushd .\distrib

del *.bak
del *.exe 
del *.tar
del *.bz2

popd

                                                       
rem create rar files for the source dists, etc.

"c:\Program Files\WinRAR\Rar.exe" a -sfx -av -m5 -mdG -r -s -x@create-distribs.exclude .\distrib\%name%.full-src.exe *.* 
"c:\Program Files\WinRAR\Rar.exe" a -sfx -av -m5 -mdG -r -s -x@create-distribs.exclude .\distrib\%name%.bin-win32.exe *.exe ChangeLog* README* !README* *.1
                                                          
                                                          
rem use the tar from UnxUtils: unix utils for win32 at sourgeforce; NO cygwin s***!

pushd .\distrib
mkdir tmp
cd tmp
"c:\Program Files\WinRAR\Rar.exe" x -y ..\%name%.full-src.exe *.* 

tar -cvvvvvvvvvvvvvvpf ../%name%.full-src.tar * 

cd ..
echo Y | rmdir tmp /s




mkdir tmp
cd tmp
"c:\Program Files\WinRAR\Rar.exe" x -y ..\%name%.bin-win32.exe *.* 

tar -cvvvvvvvvvvvvvvpf ../%name%.bin-win32.tar * 

cd ..
echo Y | rmdir tmp /s

popd

                                         
                                         
pushd .\distrib

for %%f in ( *.tar ) do bzip2 -z -9 "%%f"

popd
