echo "cleaning doxygen folder for the website..."
rm -rf ./doxygen/*
echo "copying lattest doxygen output to it..."
cp -rf ../Doxygen/out/html/* ./doxygen/
echo "done!"
