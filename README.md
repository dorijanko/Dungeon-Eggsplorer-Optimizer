This program maximizes your gains in the Dungeon Eggsplorer event. It needs to know the complete dungeon layout(from dungeon.txt), the amount of torches you have and what value you give to each reward(from resourceValues.txt). Here's what resourceValues.txt should look like:

[number] total torches
  
DI(diamonds): [numerical value of diamonds]
  
PL(pet levels; don't include the fact that they give torches into the value): [numerical value of pet levels]
  
SP(skill points): [numerical value of skill points]

SR(skill tree reset token): [numerical value of skill tree reset tokens]
  
DU(dust): [numerical value of dust]
  
SH(shards): [numerical value of crafting shards]
  
EA(event avatar): [numerical value of the event avatar]
  
CE(common equipment): [numerical value of common equipment]

RE(rare equipment): [numerical value of rare equipment]
  
EE(event equipment): [numerical value of event equipment]
  
LE(legendary equipment): [numerical value of legendary equipment]
  
PK(perk): [numerical value of perks]
  
FS(firestones): [numerical value of firestones]
  
I don't recommend setting any of the values above 1000000 or between 0 and 0.00001. The program will try to maximize the total value of resources you get on the path; for example, if you set the value of diamonds as 10 and the value of dust as 16 and you get 200 diamonds and 300 dust, the total value will be 2000+4800=6800.

Here's what dungeon.txt should look like:

[the number of floors]

Then, for each floor:

[number of rows] [number of columns]

For each row, you write something like this [number of columns] times:

[2-letter code][quantity, an integer]

11 of the 2-letter codes are explained above, but there are 7 others:

ST means start; quantity doesn't matter

ED means end; quantity doesn't matter

XX means inaccessible; quantity doesn't matter

SK means silver key; quantities other than 1 are not supported

GK means gold key; quantities other than 1 are not supported

SL means silver lock; quantities other than 1 are not supported

GL means gold lock; quantities other than 1 are not supported

After running the program, it will write the result in result.txt. An example of dungeon.txt and resourceValues.txt, as well as what result.txt should look like after running the program with the example dungeon.txt and resourceValues.txt files is given.

If you already spent some torches and want to optimize spending the rest, you can do that by setting the tiles that you opened in the dungeon as something like 1000 diamonds to force the optimizer to get them.
