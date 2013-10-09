cd npa0
make
cd ..

cd npx0
make
cd ..

cd npc0
../npa0/npa asm NPC.npa NPC.npx
cd ..

cd npx1
make
cd ..
