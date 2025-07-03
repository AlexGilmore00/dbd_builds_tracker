# dbd_builds_tracker

dbd builds tracker is a way to create, store, and keep track of your dead by daylight builds for both killer and survivor.
it includes:  
    - build creation with perks verified from a list of all perks currently in the game  
    - the generation of random builds and ability to store them  
    - a way to automatically update the list of all perks using deadbydaylight.wiki.gg  

HOW TO DOWNLOAD AND RUN  
for windows systems:  
    - download and unzip the dbd_builds_tracker_win32.7z folder  
    - run the dbd_builds_win32.exe to open the program  
for linux:  
    - download and unzip the dbd_builds_tracker_linux.tar.gz folder  
    - run ./dbd_builds from the same directory the file is located in to open the program  
    - if permisions are denied, run 'chmod 111 ./dbd_builds' from the terminal while in the same directory as the file  

this project was made as a way for me to learn the basics of c and so doesnt use any libraries that must be downloaded from external sources. this means the UI is fairly simple and some things may not be done as efficiently as possible however its meant I've had to focus more on how things work and solving problems myself.  

dependancies for python script:  
    - requests             > pip install requests  
    - beautifulsoup4       > pip install beautifulsoup4  
    - Unidecode            > pip install Unidecode  
a virtual environment is packaged with the compiled windows and linux downloads, these are only necessary to know is downloading the source files
