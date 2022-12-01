# Turing Machine

Source code is converted into a Turing Machine with the following features:

- multi-tape: constant number of tapes with a head on each tape, moving independently from each other
- each tape extends to infinity on one side
- tape alphabet and input alphabet consists of 3 symbols: 0, 1, and _ as the blank symbol
- after a head writes to a tape cell, it can either move left, move right, or stay on the same tape cell

It can be shown that this particular style of Turing Machines is equivalent to a more "basic" Turing Machine with:

- one tape
- the tape extends to infinity on one side
- tape alphabet consists of 2 symbols
- after a head writes to a tape cell, it can either move left or move right

It should be noted that there should be an encoding function that converts human input into a Turing Machine input, as well as a decoding function that converts Turing Machine output into human-readable output, and that both should be computable. In this case, we are just converting from base-10 digits into binary and back.
