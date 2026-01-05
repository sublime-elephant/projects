from collections import Counter
import string
import random
import math

alphabet = [chr(x) for x in range(65,92) if (chr(x).isascii() and chr(x).isalpha())]

#need to construct the alphabets

list1 = []
vignereAlphabets = []
for x in range(0,26):
    for y in range (0,26):
        list1.insert(y,alphabet[(0+y+x)%26])
    vignereAlphabets.append(list1[:])
    list1.clear()

preplaintext = ("Barry and Sally Childs-Helton are a husband-and-wife duo of filk performers based in Indianapolis, IN")
plaintext=""

#input check

for x in preplaintext:
    if x.isalpha():
        plaintext += x.upper()
        
      
key = []
for i in range(0,3):
    key.append(alphabet[random.randrange(0,26)])
key = ''.join(key)
print(key)
ciphertext=[]

# integer-ize it 

intedPlaintext = [alphabet.index(x) for x in plaintext]
intedKey = [alphabet.index(x) for x in key]

#horizontal for plaintext character, vertical column (vignereAlphabets) for vertical

for m in range(0, len(plaintext)):
    ciphertext.append(vignereAlphabets[intedKey[m % len(key)]][intedPlaintext[m]])

print(plaintext)
print("-"*len(plaintext))
for l in range(0, len(plaintext)):
    print(key[l % len(key)],end='')
print()
print("-"*len(plaintext))   
print(''.join(ciphertext))


# kasiski test attempt

#enter proposed key length

keylength = 3
holder_list=[]
indexAlphabets=[]
frequencies = {
'E':11.16, 'M':3.01,
'A':8.50, 'H':3.00,
'R':7.58, 'G':2.47,
'I':7.54,'B':2.07,
'O':7.16,'F':1.81,
'T':6.95,'Y':1.78,
'N':6.65,'W':1.29,
'S':5.74,'K':1.10,
'L':5.49,'V':1.01,
'C':4.54,'X':0.29,
'U':3.63,'Z':0.27,
'D':3.38,'J':0.20,
'P':3.17,'Q':0.20
}


# need to split up the ciphertext into (keylength) ciphertexts...

for y in range(0, keylength):
    holder_list.append(ciphertext[y::keylength])
    indexAlphabets.append(holder_list[0])
    holder_list.clear()
   
#holder list 2 is the coset list
f_list=[]
chilist=[]
for m in range(0,len(indexAlphabets)):   
    print("index {} alphabet:\n{}".format(m+1, indexAlphabets[m]))
    count=Counter(indexAlphabets[m])
    for i in count:
        print("{},{}".format(i, count[i]), end=" ")
    print()

print()
sumlist=[]
minlist=[]
for x in range(0, len(indexAlphabets)):
    shiftedCiphertexts=[]
    frequencyLetters=[]
    total=0
    for w in range(0,26):
        for y in range(0, len(indexAlphabets[x])):
            shiftedCiphertexts.append(alphabet[((alphabet.index(indexAlphabets[x][y]))-w)%26])
        print("index {}: iteration {}: {}".format(x,w,shiftedCiphertexts))
        total=len(shiftedCiphertexts)
        count=Counter(shiftedCiphertexts)
        for j in count:
            expectation=total*(frequencies[j]/100)
            chisquare=((count[j]-expectation)**2)/expectation
            print(total, j, count[j], expectation, chisquare)
            frequencyLetters.append(chisquare)
        print("Chi-square is {}\n".format(sum(frequencyLetters)))
        sumlist.append(sum(frequencyLetters))
        frequencyLetters.clear()
        shiftedCiphertexts.clear()
        total=0
    for i in range(0,len(sumlist)):
        
        minlist.append(min(sumlist))
        minlist.append(sumlist.index(min(sumlist)))
        minlist.append(x)
        print(len(sumlist))
        sumlist[(sumlist.index(min(sumlist)))]=99999
        print(sumlist)
    sumlist.clear()
#need to find out the resulting ciphertext from the indicated shift. Then need to find what A would equal to get a part of the key.
print(minlist)
for x in range(0, len(indexAlphabets)):
    for a in range(0,5): #increase upper bound to show more results. 26 shows every chi-score for every indexalphabet
        score = minlist[0::3][a+(26*x)]
        shift = minlist[1::3][a+(26*x)]
        whichciphertext = minlist[2::3][a+(26*x)]
        print("[%d] shift %s for %s, yielded %s [%s]" %(a, shift,whichciphertext, score, alphabet[alphabet.index(indexAlphabets[x][0])-shift % 26]))
        # BestShift.append("Shift %s, for %s" % (shift, whichciphertext))
        # BestShift.append([alphabet[alphabet.index(w)-shift % 26] for w in indexAlphabets[x]])

print(''.join(ciphertext))

