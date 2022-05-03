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

ME(mythic equipment): [numerical value of mythic equipment]
  
PS(Power of Swiping): [numerical value of the Power of Swiping perk]

AR(Adrenaline Rush): [numerical value of the Adrenaline Rush Perk]

MR(Make it Rain): [numerical value of the Make it Rain Perk]

MP(Mana Potion): [numerical value of the Mana Potion Perk]

DO(Doom): [numerical value of the Doom Perk]
  
FS(firestones): [numerical value of firestones]

FW(fortune hero weapon): [numerical value of fortune hero weapons]
  
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

In addition to writing the result into result.txt, it will also make intoSheet1.txt, and it might make intoSheet2.txt. intoSheet1.txt shows a file that you can use by making a copy of the "5.16 Dungeon Eggsplorer Event map" sheet(https://docs.google.com/spreadsheets/d/1hbrPgt0tHw-fJSjUZ6PY56TLY6uEIxX-lImWFQnKDos/edit#gid=0), going to one of the Opti sheets, delete everything from C14 to L342, paste the entire content of that file into a useless cell with lots of space below it(for this example I'll choose AY15), then in C14 put "=ARRAYFORMULA(IFERROR(SPLIT(AY15:AY, ";")))"(change AY15 and AY to something different you chose a different cell to paste the file into) and you'll see a nice visualisation of the path you should take. If it's optimal to buy some useless pets from the shop to get more torches, intoSheet2.txt will show you the best strategy if you want to do that; you use it in the same way as intoSheet1.txt.

If you already spent some torches and want to optimize spending the rest, you can do that by setting the tiles that you opened in the dungeon as something like 1000 diamonds to force the optimizer to get them.
