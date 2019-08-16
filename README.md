# Rainbow Tables Lab

- ######  Hashing Scheme: An n-bit password p is padded to the left with 128 − n zeros and used as an AES-128 key to encrypt the all-0 plaintext. 
  - ###### _The result is the hashed password [h(p)]_
- ###### The attack should use a rainbow table with 2^n/2 chains of 2^n/2 length each.

## [Program Overview]

### GenTable 
###### Corresponds to the preprocessing phase in which a rainbow table will be generated. 
- - -

Execution Sequence:
- Define structure used to write on the binary file rainbow
- Getting the maximum number of entries ("lines") on rainbow file, so it satisfies the size restriction
- Picking key with lowest value that haven't seen before
- Receives a 32 bit key and return a 128 bit key left padding from n to 128 bit (as we clear those bits)
- Reduce 128 bit key to n bit key, then take input given from arguments
- Check the input, and create the file; as we will create keys bitmap by calculating the size of it and keeping track of all generated keys to avoid any collisions
- Keys will be calculated and stored on rainbow file
- If the # of first keys on a chain reach the maximum number of password allowed, break the loop, even before reaching the maximum size on rainbow
- When the key bitmap is full, check if there is still space on the rainbow file if there is space, copy the bitmap into key bitmap
- As we keep track of the keys on the chain, AES will then allow us to encrypt, reduce, and check if the key is already taken
- If it hasn’t been taken already, then take it; otherwise if the key is already on the key bitmap, break the chain
- After the chain finishes, store the first key (in the data struct before putting it on the binary file rainbow)
- Write to the rainbow doc, and calculate the max number of times AES is evaluated

### Crack
###### Corresponds to the on-line phase in which you are given the hashed password [h(p)] and need to recover p.
- - -

Execution Sequence:
- Define a similar struct to write on the binary file
- Calculate the size of the bitmap and create the keys bitmap; as we will keep track of generated keys to avoid collisions
- Go through the rainbow file looking for a match on the ciphertext; Return true if there is a match, otherwise false
- If the password is found on the rainbow, we will perform the AES algorithm by reducing the chain until reaching the hash entered as input; as we will count the times it takes to reach that hash
