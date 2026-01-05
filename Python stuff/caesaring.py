import string

alphabet = list(string.ascii_lowercase)
numbers = [x for x in range(1,27)] ##reminder a = 1, z = 26, but in list a[0]=1 not a[1]=1
dictionary = {alphabet[x]:numbers[x] for x in range (0,26)}

plaintext=input("enter plaintext: ")
plaintext=list(plaintext)
ciphertext = []
for i in range(0,26):
    for x in range(0,len(plaintext)):
        if (plaintext[x] == ' '):
            ciphertext.append(' ')
            continue
        ciphertext.append(dictionary.get(plaintext[x]))
    for x in range(0, len(plaintext)):
        if (ciphertext[x] == ' '):
            continue
        ciphertext[x] = (ciphertext[x]+i)%26
    keys = list(dictionary)
    #reverse key lookup
    for x in range(0, len(plaintext)):
        if (ciphertext[x] == ' '):
            continue
        ciphertext[x]=keys[ciphertext[x]]

        
    print ('rot {}:       '.format(i+1)+''.join(ciphertext))
    ciphertext.clear()