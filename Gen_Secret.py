#Gen secret
#Generate random garbage for some set offset
#Write the secret message
#Generate more random garbage

import os
import sys

if len(sys.argv) != 3:
    print("Usage: [offset to secret] [number of trailing bytes]")
    exit()

leading_bin = os.urandom(int(sys.argv[1]))
secret_text = "You got it!".encode()
trailing_bin = os.urandom(int(sys.argv[2]))


with open("Secret", "wb") as file:
    file.write(leading_bin)
    file.write(secret_text)
    file.write(trailing_bin)