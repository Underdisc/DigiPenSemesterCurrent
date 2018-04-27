@echo off

set num_tests=1000

FOR /L %%A IN (1, 1, %num_tests%) DO (
  echo -
  gcc0.exe 3
)
echo -
