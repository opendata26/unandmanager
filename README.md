# Nand-Manager  
A cross platform tool to backup rednand from your sdcard.  

### Running
Run ~~~~ sudo ./unandmgr 8G/32G (your nand size) read/write (read from sd to file of write from file to sd) input (either the sdcards block device in read mode or the path to the backup in write mode) output (either the location wou want to backup youyr sd to in read mode or the sdcards block device in write mode) ~~~~

#### Example
~~~~ sudo ./unandmgr 32G read /dev/sdg ~/rednand.img ~~~~ 
to read the rednand to ~/rednand.img on a 32GB wiiu

~~~~ sudo ./unandmgr 32G write ~/rednand.img /dev/sdg ~~~~ 
to write ~/rednand.img onto the sd on a 32GB wiiu 

### Screenshot
![Screenshot: ](http://i.imgur.com/PqxTAij.png)

### Compiling    
Run ~~~~ gcc -o unandmgr main.c -lm ~~~~ in the projects directory.  

### Support   
Windows : No  
Linux : Yes  
MacOS: Yes

### Format
The format is a raw copy of sector 2 to the  beggining sector of the first fat32 partion
