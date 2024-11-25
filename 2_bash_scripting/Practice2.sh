#!/bin/bash

read -p "enter first number: " first

read -p "enter the operator: " op

read -p "enter the second number: " second

case $op in
    "+")
	echo "The result is: "$((first + second));;
    "-")
	echo "The result is: " $((first - second));;
    "*")
	echo "The result is: " $((first * second));;
    "/")
	echo "The result is: " $((first / second));;
    *)
	echo "unknown operator";;
esac

