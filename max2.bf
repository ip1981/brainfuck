# Show the maximum number

# == Preparations ==

# input the 1st number into the 1st cell
, # |a|0|0|0|0|0|

# copy 1st cell into the 2nd and the 5th cells
[->+>>>+<<<<] # |0|a|0|0|a|0|


# input the 2nd number into the 1st cell
, # |b|a|0|0|a|0|

# copy 1st cell into the 3nd and the 6th cells
[->>+>>>+<<<<<] # |0|a|b|0|a|b|

> # move to the 2nd cell



# == Main loop ==

# decrement both 2nd and 3rd cells by 1,
# the final position depends on which number is bigger
[[->]<<]


# == Result ==
# go to the answer
>>>>

# show the answer
.


