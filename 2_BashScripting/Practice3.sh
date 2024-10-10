#!/bin/bash

read -p "Enter the temperature: " tem

if [ $tem -lt 0 ]; then
    echo "The Weather is freezing"
elif [ $tem -lt 30 ]; then
    echo  "The  Weather is cool"
else 
    echo  "The Weather is hot"
fi



zarib=1.8
temp_f=$(echo "$tem * $zarib" | bc)
firen=$(echo "$temp_f + 32" | bc)
echo "The temperature in F is $firen"
